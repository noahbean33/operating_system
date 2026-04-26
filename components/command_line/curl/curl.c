// Simple HTTP client using libcurl
// Fetches weather data from Open-Meteo API and displays it
#include <stdio.h>
#include <curl/curl.h>

// Callback function that libcurl calls when data is received
// Parameters: buffer - received data, size - always 1, nmemb - number of bytes
// Returns: number of bytes processed (must return nmemb for success)
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    char c;
    // Iterate through received bytes and print each character
    for(int i=0; i<nmemb; i++)
    {
        c = ((char *) buffer)[i];
        printf("%c", c);
    }
    // Return number of bytes processed (signals success to libcurl)
    return nmemb; 
}

int main()
{
    printf("Welcome to the Ultimate C HTTP Application\n");
    
    // Initialize libcurl globally (must be called before any curl operations)
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Create a curl easy handle for making HTTP requests
    CURL *handle = curl_easy_init();
    // Set the URL to fetch (Open-Meteo weather API for Berlin coordinates)
    curl_easy_setopt(handle, CURLOPT_URL, "https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current=temperature_2m,wind_speed_10m&hourly=temperature_2m,relative_humidity_2m,wind_speed_10m");
    // Set custom callback function to handle received data
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    
    // Perform the HTTP request (blocks until complete)
    CURLcode response = curl_easy_perform(handle);
    // Check if request was successful (CURLE_OK = 0)
    if (response == 0)
    {
        printf("Success.\n");
    }
    else
    {
        printf("Failed request. Code: %d\n", response);
    }
}

