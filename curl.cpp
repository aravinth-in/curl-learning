#include <iostream>
#include <curl/curl.h>
#include <string>

// This function is called by libcurl as soon as there is data received that needs to be saved.
// It appends the received data to the std::string passed via the user pointer (readBuffer).

// brief Callback function to handle the response data.
// \param contents Pointer to the delivered data.
// \param size Size of each data chunk.
// \param nmemb Number of data chunks.
// \param userp Pointer to user-defined data (in this case, a std::string*).
// \return size_t Total size of the data processed.
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = (std::string*)userp;
    str->append((char*)contents, totalSize);
    return totalSize;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Initializes the libcurl library globally. This is typically done at the beginning of a program that 
    //uses libcurl to ensure that all required resources are set up.
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Initializes a CURL easy session and returns a handle to be used in subsequent libcurl function calls.
    curl = curl_easy_init();

    if(curl) {
        // Set the URL for the GET request
        curl_easy_setopt(curl, CURLOPT_URL, "http://movie.basnetbd.com/Data/TV%20Series/12%20Monkeys/Season%2001/S01E04%20-%20Atari.eng.srt");

        // Set the callback function to handle the response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Pass the string to the callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Set additional options (optional)
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Timeout after 10 seconds

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Print the response
            std::cout << "Response data: " << readBuffer << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Global cleanup
    curl_global_cleanup();

    return 0;
}