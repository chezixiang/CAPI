
#pragma once
#ifndef CAPIHEADER_H
#define CAPIHEADER_H

#define CURL_STATICLIB
#define HTTP_ONLY

#include <iostream>
#include <windows.h> 
#include <fstream>  
#include <curl/curl.h>  
#include <mmsystem.h>  
#include <filesystem> // C++17 或更高版本  
#include <unordered_map> 
#include <errno.h>

#pragma comment(lib, "winmm.lib")  
#pragma comment(lib, "libcurl_a.lib") // 确保链接了libcurl库  

std::string get(std::string url, bool download = false, std::string filepath = "C:\\music\\music.wav", std::string path = "C:\\music");
std::string post(const std::string& baseUrl, const std::unordered_map<std::string, std::string>& params, bool saveToFile = false, const std::string& filePath = "");


// 函数用于将 UTF-8 编码的 std::string 转换为 std::wstring（UTF-16）  
static std::wstring utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return std::wstring();

    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], len);
    return wstr;
}

//作为 libcurl 的回调函数来使用的，用于处理从网络上接收到的数据，并将这些数据写入到一个文件中
static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* stream) {
    if (stream) {
        stream->write((char*)ptr, size * nmemb);
        return size * nmemb;
    }
    return 0;
}

//使用 libcurl 来下载文件并将其保存到指定的路径
static CURLcode download_file(const std::string& url, const std::string& filepath) {
    CURL* curl; 
    CURLcode res= CURLE_FAILED_INIT;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        std::ofstream ofs(filepath, std::ios::binary);
        if (!ofs.is_open()) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return CURLE_WRITE_ERROR;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            //std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return res;
        }

        ofs.close();
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return res;
}

//使用 MCI (Media Control Interface) 命令来播放 WAV 文件
static void play_wav(const std::string& filepath) {
    MCIERROR mciError;
    std::wstring wideCommand = utf8ToWide("open \"" + filepath + "\" type waveaudio alias Sound");
    mciError = mciSendString(wideCommand.c_str(), NULL, 0, NULL);
    if (mciError) {
        std::cerr << "Error opening WAV file: " << mciError << std::endl;
        return;
    }

    mciError = mciSendString(L"play Sound", NULL, 0, NULL);
    if (mciError) {
        std::cerr << "Error playing WAV file: " << mciError << std::endl;
        mciSendString(L"close Sound", NULL, 0, NULL); // 尝试关闭文件，但可能已经在播放中  
        return;
    }

    // 注意：这里不立即关闭文件，因为播放是异步的  
    // 如果需要等待播放完成，请考虑使用 MCI 通知  

    // 假设在某个点（如用户请求或程序结束时）关闭文件  
    // mciSendString(L"close Sound", NULL, 0, NULL);  
}
#endif