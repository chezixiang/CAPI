
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
#include <filesystem> // C++17 ����߰汾  
#include <unordered_map> 
#include <errno.h>

#pragma comment(lib, "winmm.lib")  
#pragma comment(lib, "libcurl_a.lib") // ȷ��������libcurl��  

std::string get(std::string url, bool download = false, std::string filepath = "C:\\music\\music.wav", std::string path = "C:\\music");
std::string post(const std::string& baseUrl, const std::unordered_map<std::string, std::string>& params, bool saveToFile = false, const std::string& filePath = "");


// �������ڽ� UTF-8 ����� std::string ת��Ϊ std::wstring��UTF-16��  
static std::wstring utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return std::wstring();

    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], len);
    return wstr;
}

//��Ϊ libcurl �Ļص�������ʹ�õģ����ڴ���������Ͻ��յ������ݣ�������Щ����д�뵽һ���ļ���
static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* stream) {
    if (stream) {
        stream->write((char*)ptr, size * nmemb);
        return size * nmemb;
    }
    return 0;
}

//ʹ�� libcurl �������ļ������䱣�浽ָ����·��
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

//ʹ�� MCI (Media Control Interface) ���������� WAV �ļ�
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
        mciSendString(L"close Sound", NULL, 0, NULL); // ���Թر��ļ����������Ѿ��ڲ�����  
        return;
    }

    // ע�⣺���ﲻ�����ر��ļ�����Ϊ�������첽��  
    // �����Ҫ�ȴ�������ɣ��뿼��ʹ�� MCI ֪ͨ  

    // ������ĳ���㣨���û������������ʱ���ر��ļ�  
    // mciSendString(L"close Sound", NULL, 0, NULL);  
}
#endif