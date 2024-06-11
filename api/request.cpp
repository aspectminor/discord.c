#include "request.h"

#include <stdio.h>
#include <curl/curl.h>

using namespace std;
string fetch(const string& method, const string& path) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        if (method == "GET") {
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); /*Follow redirection if necessary*/
        } else if (method == "POST") {

        } else if (method == "DELETE") {

        } else {
            /*TODO: ERROR Handler*/
        }
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("%d\n", res);
        }
        curl_easy_cleanup(curl);
    }

    //parse json
    return "";
}