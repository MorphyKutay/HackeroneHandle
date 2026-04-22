#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson/cJSON.h"

struct Memory {
    char *data;
    size_t size;
};

void free_response(char *response) {
    if (response) free(response);
}

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->data, mem->size + total + 1);
    if (!ptr) return 0;

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, total);
    mem->size += total;
    mem->data[mem->size] = 0;

    return total;
}

char* http_get(const char *url, const char *api) {
    CURL *curl = curl_easy_init();
    CURLcode res;

    struct Memory *chunk = malloc(sizeof(struct Memory));
    chunk->data = malloc(1);
    chunk->size = 0;

    struct curl_slist *headers = NULL;
    char auth[512];

    snprintf(auth, sizeof(auth), "Authorization: Bearer %s", api);
    headers = curl_slist_append(headers, auth);
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, chunk);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        printf("Request failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        free(chunk->data);
        free(chunk);
        return NULL;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    char *result = chunk->data;
    free(chunk);

    return result;
}

static cJSON *find_version_id(cJSON *root) {
    cJSON *domains = cJSON_GetObjectItem(root, "domains");
    if (!domains) return NULL;

    cJSON *versionId = cJSON_GetObjectItem(domains, "versionId");
    if (versionId && cJSON_IsString(versionId)) return versionId;

    cJSON *currentVersionId = cJSON_GetObjectItem(domains, "currentVersionId");
    if (currentVersionId && cJSON_IsString(currentVersionId)) return currentVersionId;

    cJSON *latestVersionId = cJSON_GetObjectItem(domains, "latestVersionId");
    if (latestVersionId && cJSON_IsString(latestVersionId)) return latestVersionId;

    cJSON *version = cJSON_GetObjectItem(domains, "version");
    if (version) {
        cJSON *id = cJSON_GetObjectItem(version, "id");
        if (id && cJSON_IsString(id)) return id;
    }

    return NULL;
}

static cJSON *get_programs_array(cJSON *root) {
    if (cJSON_IsArray(root)) return root;

    cJSON *content = cJSON_GetObjectItem(root, "content");
    if (content && cJSON_IsArray(content)) return content;

    cJSON *programs = cJSON_GetObjectItem(root, "programs");
    if (programs && cJSON_IsArray(programs)) return programs;

    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data && cJSON_IsArray(data)) return data;

    cJSON *records = cJSON_GetObjectItem(root, "records");
    if (records && cJSON_IsArray(records)) return records;

    return NULL;
}

static int is_wildcard_endpoint(const char *endpoint) {
    return endpoint && strchr(endpoint, '*') != NULL;
}

static int print_domains_from_content(cJSON *content, FILE *out, int wildcard_only) {
    int printed = 0;
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, content) {
        cJSON *endpoint = cJSON_GetObjectItem(item, "endpoint");
        if (endpoint && cJSON_IsString(endpoint)) {
            if (wildcard_only && !is_wildcard_endpoint(endpoint->valuestring)) {
                continue;
            }
            printf("   -> %s\n", endpoint->valuestring);
            if (out) {
                fprintf(out, "   -> %s\n", endpoint->valuestring);
            }
            printed++;
        }
    }

    return printed;
}

int get_domains(const char *programId, const char *api, FILE *out, int wildcard_only) {
    char url[512];
    snprintf(url, sizeof(url),
             "https://api.intigriti.com/external/researcher/v1/programs/%s",
             programId);

    char *programDetailResponse = http_get(url, api);
    if (!programDetailResponse) return 0;

    cJSON *detailRoot = cJSON_Parse(programDetailResponse);
    if (!detailRoot) {
        free_response(programDetailResponse);
        return 0;
    }

    cJSON *domains = cJSON_GetObjectItem(detailRoot, "domains");
    cJSON *content = domains ? cJSON_GetObjectItem(domains, "content") : NULL;
    if (content && cJSON_IsArray(content)) {
        int count = print_domains_from_content(content, out, wildcard_only);
        cJSON_Delete(detailRoot);
        free_response(programDetailResponse);
        return count;
    }

    char versionId[128] = {0};
    cJSON *version = find_version_id(detailRoot);
    if (version && cJSON_IsString(version)) {
        snprintf(versionId, sizeof(versionId), "%s", version->valuestring);
    } else if (domains) {
        cJSON *id = cJSON_GetObjectItem(domains, "id");
        if (id && cJSON_IsString(id)) {
            snprintf(versionId, sizeof(versionId), "%s", id->valuestring);
        }
    }

    cJSON_Delete(detailRoot);
    free_response(programDetailResponse);

    if (strlen(versionId) == 0) {
        printf("   -> Domain bilgisi bulunamadi.\n");
        if (out) fprintf(out, "   -> Domain bilgisi bulunamadi.\n");
        return 0;
    }

    snprintf(url, sizeof(url),
             "https://api.intigriti.com/external/researcher/v1/programs/%s/domains/%s",
             programId, versionId);

    char *response = http_get(url, api);
    if (!response) return 0;

    cJSON *root = cJSON_Parse(response);
    if (!root) {
        free_response(response);
        return 0;
    }

    int printed = 0;
    cJSON *fallbackContent = cJSON_GetObjectItem(root, "content");
    if (fallbackContent && cJSON_IsArray(fallbackContent)) {
        printed = print_domains_from_content(fallbackContent, out, wildcard_only);
    } else {
        printf("   -> Domain listesi bulunamadi.\n");
        if (out) fprintf(out, "   -> Domain listesi bulunamadi.\n");
    }

    cJSON_Delete(root);
    free_response(response);
    return printed;
}

int main(int argc, char *argv[]) {
    char api[600];
    int wildcard_only = (argc > 1 && strcmp(argv[1], "-w") == 0);
    const char *output_filename = wildcard_only ? "wildcard.txt" : "domains.txt";
    FILE *out = fopen(output_filename, "w");
    if (!out) {
        printf("%s acilamadi.\n", output_filename);
        return 1;
    }

    printf("Enter API key: ");
    scanf("%s", api);

    char *response = http_get(
        "https://api.intigriti.com/external/researcher/v1/programs?limit=500",
        api
    );

    if (!response) {
        fclose(out);
        return 1;
    }

    cJSON *root = cJSON_Parse(response);
    if (!root) {
        free(response);
        printf("Program listesi parse edilemedi.\n");
        fclose(out);
        return 1;
    }

    cJSON *programs = get_programs_array(root);
    if (!programs) {
        cJSON *message = cJSON_GetObjectItem(root, "message");
        if (message && cJSON_IsString(message)) {
            printf("API mesajı: %s\n", message->valuestring);
        } else {
            printf("Program listesi beklenen formatta donmedi.\n");
        }

        cJSON_Delete(root);
        free(response);
        fclose(out);
        return 1;
    }

    cJSON *item = NULL;
    int program_count = 0;

    cJSON_ArrayForEach(item, programs) {
        cJSON *id = cJSON_GetObjectItem(item, "id");
        cJSON *name = cJSON_GetObjectItem(item, "name");

        if (id && name && cJSON_IsString(id)) {
            program_count++;
            printf("\n[%s]\n", name->valuestring);
            fprintf(out, "\n[%s]\n", name->valuestring);
            get_domains(id->valuestring, api, out, wildcard_only);
        }
    }

    if (program_count == 0) {
        printf("Erisilebilir program bulunamadi.\n");
    }

    cJSON_Delete(root);
    free(response);
    fclose(out);

    return 0;
}
