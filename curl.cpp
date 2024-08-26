#include <curl/curl.h>

#include <chrono>
#include <iostream>
#include <string>

// This function is called by libcurl as soon as there is data received that needs to be saved.
// It appends the received data to the std::string passed via the user pointer (readBuffer).

// brief Callback function to handle the response data.
// \param contents Pointer to the delivered data.
// \param size Size of each data chunk.
// \param nmemb Number of data chunks.
// \param userp Pointer to user-defined data (in this case, a std::string*).
// \return size_t Total size of the data processed.
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    FILE* filePointer = (FILE*) userp;
    fwrite(contents, size, nmemb, filePointer);
    // std::string* str = (std::string*)userp;
    // str->append((char*)contents, totalSize);
    return totalSize;
}

// brief Progress callback function to report the progress of a data transfer.
// \param clientp Pointer to user-defined data (not used in this case).
// \param dltotal Total number of bytes expected to be downloaded.
// \param dlnow Number of bytes downloaded so far.
// \param ultotal Total number of bytes expected to be uploaded.
// \param ulnow Number of bytes uploaded so far.
// \return int Return 0 to continue the transfer, non-zero to abort.
int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,
                     curl_off_t ulnow)
{
    if (dltotal > 0)
    {
        double progress = (double) dlnow / (double) dltotal * 100.0;
        std::cout << "Progress: " << progress << "% (" << dlnow << " / " << dltotal << " bytes)"
                  << std::endl;
    }
    return 0;  // Return 0 to continue the transfer, non-zero to abort
}

class CurlDownloader
{
   public:
    CurlDownloader()
    {
        // Initializes a CURL easy session and returns a handle to be used in subsequent libcurl
        // function calls.
        curl = curl_easy_init();
    }

    ~CurlDownloader()
    {
        // Clean up
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    std::string download(const CurlDownloader &curlDownloader, const std::string &url, const FILE* filePointer)
    {
        if (!curlDownloader.curl)
        {
            return "Failed to initialize curl";
        }

        // Set the URL for the GET request
        curl_easy_setopt(curlDownloader.curl, CURLOPT_URL, url.c_str());

        // Set the callback function to handle the response data
        curl_easy_setopt(curlDownloader.curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Pass the string to the callback function
        // curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curlDownloader.curl, CURLOPT_WRITEDATA, filePointer);

        // Set the progress callback function
        curl_easy_setopt(curlDownloader.curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
        curl_easy_setopt(curlDownloader.curl, CURLOPT_NOPROGRESS, 0L);  // Enable progress meter

        // Set additional options (optional)
        curl_easy_setopt(curlDownloader.curl, CURLOPT_FOLLOWLOCATION, 1L);  // Follow redirects
        curl_easy_setopt(curlDownloader.curl, CURLOPT_TIMEOUT,
                         10L);  // Timeout after 10 seconds

        // Perform the request
        CURLcode curl_result = curl_easy_perform(curlDownloader.curl);

        if (curl_result != CURLE_OK)
        {
            return std::string(curl_easy_strerror(curl_result));
        }

        return "SUCCESS";
    }
    CURL* curl;
};

int main()
{
    // CURLcode res;
    FILE* filePointer;
    // std::string readBuffer;

    const char* homeDir = getenv("HOME");
    if (!homeDir)
    {
        std::cerr << "Failed to get home directory" << std::endl;
        return 1;
    }

    std::string outputFileNameStr = std::string(homeDir) + "/Downloads/downloaded_data.txt";
    const char* outputFileName = outputFileNameStr.c_str();

    // Initializes the libcurl library globally. This is typically done at the beginning of a
    // program that uses libcurl to ensure that all required resources are set up.
    curl_global_init(CURL_GLOBAL_DEFAULT);

    {
        CurlDownloader curlDownloader;

        filePointer = fopen(outputFileName, "wb");
        if (!filePointer)
        {
            std::cerr << "Failed to open file for writing: " << outputFileName << std::endl;
            return 1;
        }

        std::string url =
            "http://movie.basnetbd.com/Data/TV%20Series/12%20Monkeys/Season%2001/"
            "S01E04%20-%20Atari.eng.srt";
        std::string result = curlDownloader.download(curlDownloader, url, filePointer);

        // Check for errors
        if (result != "SUCCESS")
        {
            std::cerr << "download failed: " << result << std::endl;
        }
        else
        {
            // Print the response
            // std::cout << "Response data: " << readBuffer << std::endl;
            std::cout << "Download successful, data saved to: " << outputFileName << std::endl;
        }

        fclose(filePointer);
    }

    // Global cleanup
    curl_global_cleanup();

    return 0;
}
