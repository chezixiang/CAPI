#include"CAPIheader.h"
// 用于存储响应数据的结构  
struct string_writer {
    std::string* s;

    string_writer(std::string* p) : s(p) {}

    // libcurl 写入数据的回调函数  
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

        // 初始化 libcurl  
        curl_global_init(CURL_GLOBAL_ALL);

        // 创建一个新的 curl 会话  
        curl = curl_easy_init();
        if (curl) {
            // 设置 URL  
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // 如果不下载，则设置写入函数以捕获响应  
                string_writer writer(&response);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &string_writer::writefunction);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writer);

            // 执行 GET 请求  
            res = curl_easy_perform(curl);

            // 检查是否有错误发生  
            if (res != CURLE_OK) {
                //std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                returnv = "4003";
            }

            // 清理  
            curl_easy_cleanup(curl);
        }
    }
    return returnv;
}
