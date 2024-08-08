#pragma once
#ifndef URL_BASE_H
#define URL_BASE_H
#include<iostream>
#include<string>
#include<cstring>
#include<stdio.h>
#include<assert.h>
#include <windows.h>
#include<fstream>


static unsigned char ToHex(unsigned char x)
{
	using namespace std;
	return  x > 9 ? x + 55 : x + 48;
}

static unsigned char FromHex(unsigned char x)
{
	using namespace std;
	unsigned char y=0;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

static std::string UrlEncode(const std::string& str)
{
	using namespace std;
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

static std::string UrlDecode(const std::string& str)
{
	using namespace std;
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>



#ifdef _WIN32
#include <windows.h>
static std::string GbkToUtf8(const char* src_str)
{
	using namespace std;

	// 第一步：将GBK字符串转换为宽字符字符串  
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	if (len == 0) {
		// 处理转换失败的情况  
		throw std::runtime_error("GBK to wide char conversion failed");
	}

	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, (static_cast<unsigned long long>(len) + 1) * sizeof(wchar_t)); // 确保宽字符字符串以null结尾  
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);

	// 第二步：将宽字符字符串转换为UTF-8字符串  
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (len == 0) {
		delete[] wstr; // 清理已分配的内存  
		throw std::runtime_error("Wide char to UTF-8 conversion failed");
	}

	char* utf8str = new char[len + 1];
	memset(utf8str, 0, static_cast<size_t>(len) + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8str, len, NULL, NULL);

	// 使用std::string管理转换后的UTF-8字符串  
	string result(utf8str);

	// 清理资源  
	delete[] wstr;
	delete[] utf8str;

	return result;
}

static std::string Utf8ToGbk(const char* src_str)
{
	using namespace std;
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, static_cast<size_t>(len) * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, static_cast<size_t>(len) + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
#endif
#endif




