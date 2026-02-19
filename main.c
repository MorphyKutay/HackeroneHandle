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

int main() {

    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("Curl init failed\n");
        return 1;
    }

    FILE *f = fopen("output.json", "w");
    if (!f) {
        printf("Dosya açılamadı\n");
        return 1;
    }

    char url[1024] = "https://api.hackerone.com/v1/hackers/programs";

    curl_easy_setopt(curl, CURLOPT_USERPWD,
        "<username>:<apikey>");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    int page = 1;

    while (url[0] != '\0') {

        printf("Sayfa %d alınıyor...\n", page++);

        struct Memory chunk = {0};

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            printf("Request failed: %s\n", curl_easy_strerror(res));
            free(chunk.data);
            break;
        }

        cJSON *json = cJSON_Parse(chunk.data);
        if (!json) {
            printf("JSON parse error\n");
            free(chunk.data);
            break;
        }

        // JSON'u dosyaya yaz
        char *pretty = cJSON_Print(json);
        fprintf(f, "%s\n", pretty);
        free(pretty);

        // links.next al
        cJSON *links = cJSON_GetObjectItem(json, "links");
        cJSON *next = cJSON_GetObjectItem(links, "next");

        if (next && cJSON_IsString(next) && next->valuestring) {
            strncpy(url, next->valuestring, sizeof(url) - 1);
            url[sizeof(url) - 1] = '\0';
        } else {
            url[0] = '\0'; // bitti
        }

        cJSON_Delete(json);
        free(chunk.data);
    }

    fclose(f);
    curl_easy_cleanup(curl);

    printf("Tüm sayfalar indirildi ✅\n");

    return 0;
}
