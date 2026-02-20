#include <stdio.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>

struct Memory {
    char *data;
    size_t size;
};

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    struct Memory *mem = (struct Memory *)userdata;
    size_t total = size * nmemb;

    char *tmp = realloc(mem->data, mem->size + total + 1);
    if (!tmp) return 0;

    mem->data = tmp;
    memcpy(mem->data + mem->size, ptr, total);

    mem->size += total;
    mem->data[mem->size] = '\0';

    return total;
}

//////////////////////////////////////////////////////////////
// Scope extractor (JSON → asset_identifier list)
//////////////////////////////////////////////////////////////

void extract_scopes(const char *json_data, FILE *out) {

    cJSON *json = cJSON_Parse(json_data);
    if (!json) return;

    cJSON *data = cJSON_GetObjectItem(json, "data");

    for (int i = 0; i < cJSON_GetArraySize(data); i++) {

        cJSON *item = cJSON_GetArrayItem(data, i);
        cJSON *attr = cJSON_GetObjectItem(item, "attributes");

        cJSON *asset = cJSON_GetObjectItem(attr, "asset_identifier");
        cJSON *bounty = cJSON_GetObjectItem(attr, "eligible_for_bounty");
        cJSON *submit = cJSON_GetObjectItem(attr, "eligible_for_submission");

        if (cJSON_IsString(asset) &&
            cJSON_IsBool(bounty) &&
            cJSON_IsBool(submit) &&
            cJSON_IsTrue(bounty) &&
            cJSON_IsTrue(submit)) {

            fprintf(out, "%s\n", asset->valuestring);
        }
    }

    cJSON_Delete(json);
}

//////////////////////////////////////////////////////////////
// Fetch scopes for one program handle
//////////////////////////////////////////////////////////////

void fetch_scopes(CURL *curl, const char *handle, FILE *out) {

    char url[512];
    sprintf(url,
        "https://api.hackerone.com/v1/hackers/programs/%s/structured_scopes",
        handle);

    struct Memory chunk = {0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK && chunk.data) {
        printf("Scopes fetched: %s\n", handle);
        extract_scopes(chunk.data, out);
    } else {
        printf("Failed to fetch scopes: %s\n", handle);
    }

    free(chunk.data);
}

//////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////

int main() {

    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("Curl init failed\n");
        return 1;
    }

    FILE *out = fopen("scopes.txt", "w");
    if (!out) {
        printf("Output file error\n");
        return 1;
    }

    // 🔑 HackerOne API credentials
    curl_easy_setopt(curl, CURLOPT_USERPWD,
        "<username>:<apikey>");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    char url[1024] = "https://api.hackerone.com/v1/hackers/programs";

    int page = 1;

    while (url[0] != '\0') {

        printf("Fetching programs page %d...\n", page++);

        struct Memory chunk = {0};

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK || !chunk.data) {
            printf("Request failed\n");
            free(chunk.data);
            break;
        }

        cJSON *json = cJSON_Parse(chunk.data);
        if (!json) {
            printf("JSON parse error\n");
            free(chunk.data);
            break;
        }

        cJSON *data = cJSON_GetObjectItem(json, "data");

        // 🔥 Program handle'larını al
        for (int i = 0; i < cJSON_GetArraySize(data); i++) {

            cJSON *item = cJSON_GetArrayItem(data, i);
            cJSON *attr = cJSON_GetObjectItem(item, "attributes");
            cJSON *handle = cJSON_GetObjectItem(attr, "handle");

            if (cJSON_IsString(handle)) {
                fetch_scopes(curl, handle->valuestring, out);
            }
        }

        // 🔁 Pagination → links.next
        cJSON *links = cJSON_GetObjectItem(json, "links");
        cJSON *next = cJSON_GetObjectItem(links, "next");

        if (next && cJSON_IsString(next) && next->valuestring) {
            strncpy(url, next->valuestring, sizeof(url) - 1);
            url[sizeof(url) - 1] = '\0';
        } else {
            url[0] = '\0';
        }

        cJSON_Delete(json);
        free(chunk.data);
    }

    fclose(out);
    curl_easy_cleanup(curl);

    printf("\nAll scopes saved to scopes.txt ✅\n");

    return 0;
}