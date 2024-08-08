#include"CAPIheader.h" 
#include"ServerStdOut.hpp"
#include "urlbase.h"

extern bool debugmode;

// 假设你已经实现了这些函数  
static std::wstring utf8ToWide(const std::string& utf8);
static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* stream);
static CURLcode download_file(const std::string& url, const std::string& filepath);
static void play_wav(const std::string& filepath); 
static bool isnum(const std::string& str) {
    bool hasDecimal = false;
    for (char c : str) {
        if (std::isdigit(c)) {
            // 字符是数字  
            continue;
        }
        if (c == '.' && !hasDecimal) {
            // 小数点且之前未出现过  
            hasDecimal = true;
            continue;
        }
        // 遇到非数字且非首次出现的小数点  
        return false;
    }
    // 如果字符串非空且只包含数字（可选地包含一个小数点）  
    return !str.empty() && !(str == "." && !hasDecimal); // 排除单独的"."  
}
static std::string toJsonString(const std::unordered_map<std::string, std::string>& params) {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& p : params) {
        if (!first) {
            ss << ",";
        }
        first = false;
        if (isnum(p.second))
        {
            ss << "\"" << p.first << "\":" << p.second;
        }
        else
        {
            ss << "\"" << p.first << "\":\"" << p.second << "\"";
        }
    }
    ss << "}";
    if (debugmode) debug("JSON:" + ss.str());
    return ss.str();
}


static size_t write_callback(void* contents, size_t size, size_t nmemb, std::ofstream* stream) {
    if (stream && stream->is_open()) {
        stream->write(static_cast<char*>(contents), size * nmemb);
        if (!stream->good()) {
            // 处理写入错误（可选）  
            std::cerr << "Failed to write to file." << std::endl;
            // 注意：这里不能直接中止 libcurl 的操作，但可以记录错误或采取其他措施  
        }
        return size * nmemb; // 返回实际写入的字节数  
    }
    // 如果流未打开，则不写入数据并返回0（或适当的错误码，但libcurl可能不处理它）  
    return 0;
}
static size_t write_callback2(void* contents, size_t size, size_t nmemb, std::string* userp) {
    if (userp) {
        userp->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb; // 返回实际接收的字节数  
    }
    // 如果 userp 为 nullptr，则不写入数据并返回0  
    return 0;
}
std::string post(const std::string& baseUrl, const std::unordered_map<std::string, std::string>& params, bool saveToFile, const std::string& filePath) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::ofstream fileStream;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl == nullptr) {
        err("EMS", "Failed to initialize CURL.");
        curl_global_cleanup();
        return "2";  
    }
    std::string json = GbkToUtf8(toJsonString(params).c_str());
    curl_easy_setopt(curl, CURLOPT_URL, baseUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    

    if (saveToFile) {
        fileStream.open(filePath, std::ios::out | std::ios::binary);
        if (!fileStream.is_open()) {
            err("EMS", "Failed to open file for writing: " + filePath);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return "2";
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fileStream);
    }
    else {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback2);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    }
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        err("EMS", "curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
        if (saveToFile && fileStream.is_open()) {
            fileStream.close();
            std::remove(filePath.c_str());
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return std::to_string(res);
    }
    if (saveToFile && fileStream.is_open()) {
        fileStream.close();
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (!saveToFile) {
        return readBuffer;
    }
    return "0";  
}