#include"CAPIheader.h" 
#include"ServerStdOut.hpp"
#include "urlbase.h"

extern bool debugmode;

// �������Ѿ�ʵ������Щ����  
static std::wstring utf8ToWide(const std::string& utf8);
static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* stream);
static CURLcode download_file(const std::string& url, const std::string& filepath);
static void play_wav(const std::string& filepath); 
static bool isnum(const std::string& str) {
    bool hasDecimal = false;
    for (char c : str) {
        if (std::isdigit(c)) {
            // �ַ�������  
            continue;
        }
        if (c == '.' && !hasDecimal) {
            // С������֮ǰδ���ֹ�  
            hasDecimal = true;
            continue;
        }
        // �����������ҷ��״γ��ֵ�С����  
        return false;
    }
    // ����ַ����ǿ���ֻ�������֣���ѡ�ذ���һ��С���㣩  
    return !str.empty() && !(str == "." && !hasDecimal); // �ų�������"."  
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
            // ����д����󣨿�ѡ��  
            std::cerr << "Failed to write to file." << std::endl;
            // ע�⣺���ﲻ��ֱ����ֹ libcurl �Ĳ����������Լ�¼������ȡ������ʩ  
        }
        return size * nmemb; // ����ʵ��д����ֽ���  
    }
    // �����δ�򿪣���д�����ݲ�����0�����ʵ��Ĵ����룬��libcurl���ܲ���������  
    return 0;
}
static size_t write_callback2(void* contents, size_t size, size_t nmemb, std::string* userp) {
    if (userp) {
        userp->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb; // ����ʵ�ʽ��յ��ֽ���  
    }
    // ��� userp Ϊ nullptr����д�����ݲ�����0  
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