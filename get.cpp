#include"CAPIheader.h"
// ���ڴ洢��Ӧ���ݵĽṹ  
struct string_writer {
    std::string* s;

    string_writer(std::string* p) : s(p) {}

    // libcurl д�����ݵĻص�����  
    size_t writefunction(void* ptr, size_t size, size_t nmemb, std::string* userdata) {
        ((string_writer*)userdata)->s->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }
};

std::string get(std::string url, bool download, std::string filepath, std::string path) {
     
    namespace fs = std::filesystem;
    std::string returnv="-1";

    if (download) {
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }
        returnv=std::to_string(download_file(url, filepath));
    }
    else {
        CURL* curl;
        CURLcode res;
        std::string response;

        // ��ʼ�� libcurl  
        curl_global_init(CURL_GLOBAL_ALL);

        // ����һ���µ� curl �Ự  
        curl = curl_easy_init();
        if (curl) {
            // ���� URL  
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // ��������أ�������д�뺯���Բ�����Ӧ  
                string_writer writer(&response);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &string_writer::writefunction);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writer);

            // ִ�� GET ����  
            res = curl_easy_perform(curl);

            // ����Ƿ��д�����  
            if (res != CURLE_OK) {
                //std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                returnv = "4003";
            }

            // ����  
            curl_easy_cleanup(curl);
        }
    }
    return returnv;
}
