#pragma once
#ifndef SERVERSTDOUT_H
#define SERVERSTDOUT_H

#define WARN warn
#define INFO info
#define DEBUG debug
#define ERR err

#include"CAPIheader.h"
#include <time.h>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <direct.h>
#include <string>
#include <wtypes.h>
#include<iostream>
#include<cstdlib>
#include<windows.h>

#define eout exit(3000)

static void co(int = 15, int = 0);
static void t();
static void info(std::string, std::string, short = 1);
static void warn(std::string, std::string, short = 1);
static void err(std::string, std::string, short = 1);
static void fatal(std::string, std::string, int = -1,short = 1);
static void debug(std::string, short = 1);
static bool readLineXFromIni(int, std::string&, std::string&);
static std::string t2();

static char str[2049] = {};
static std::string currentDirPath;
static std::string logDir;
static std::string logFilePath;
static std::ofstream iout;
static std::ofstream fout;


static void co(int ForgC, int BackC)
{
	using namespace std;
	WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
}
static void t()
{
	time_t timep;
	struct tm* p;
	time(&timep);
	p = localtime(&timep);
	printf("%02d:%02d:%02d ", p->tm_hour, p->tm_min, p->tm_sec);
	return;
}
static std::string t2()
{
	using namespace std;
	std::string re;
	time_t timep;
	struct tm* p;
	time(&timep); 
	p = localtime(&timep);
	re = to_string(1900 + p->tm_year) + "_" + to_string(1 + p->tm_mon) + "_" + to_string(p->tm_mday) + "_" + to_string(p->tm_hour) + "_" + to_string(p->tm_min) + "_" + to_string(p->tm_sec);
	return re;
}
static void info(std::string who, std::string text, short l)
{
	using namespace std;
	t();
	cout << "<";
	co(9);
	cout << "INFO";
	co(8);
	cout << ":" << who;
	co();
	cout << "> " << text;
	if (l)cout << endl;
	fout << "<INFO:" << who << ">" << text;
	if (l)fout << endl;
}
static void debug(std::string text, short l)
{
	using namespace std;
	t();
	cout << "<";
	co(10);
	cout << "DEBUG";
	co();
	cout << "> " << text;
	if (l)cout << endl;
	fout << "<DEBUG>" << text;
	if (l)fout << endl;
}
static void warn(std::string who, std::string text, short l)
{
	using namespace std;
	t();
	cout << "<";
	co(14);
	cout << "WARN";
	co(8);
	cout << ":" << who;
	co();
	cout << "> " << text;
	if (l)cout << endl;
	fout << "<WARN:" << who << ">" << text;
	if (l)fout << endl;
}
static void err(std::string who, std::string text, short l)
{
	using namespace std;
	t();
	cerr << "<";
	co(12);
	cerr << "ERROR";
	co(8);
	cerr << ":" << who;
	co();
	cerr << "> " << text;
	if (l)cout << endl;
	fout << "<ERROR:" << who << ">" << text;
	if (l)fout << endl;
}
static bool readLineXFromIni(int x, std::string& key, std::string& value) {
	std::string line;
	int currentLine = 1; 

	if (currentLine == x) {
		std::size_t pos = line.find('=');
		if (pos != std::string::npos) {
			key = line.substr(0, pos);
			value = line.substr(pos + 1);
			return true;
		}
	}
 
	return false;
}
static void fatal(std::string who, std::string text, int exitcode,short l)
{
	using namespace std;
	t();
	cerr << "<";
	co(12);
	cerr << "FATAL";
	co(8);
	cerr << ":" << who;
	co();
	cerr << "> " << text;
	if (l)cout << endl;
	fout << "<FATAL:" << who << ">" << text;
	if (l)fout << endl;
	if (exitcode) std::exit(exitcode);
}
static void startup() {
	using namespace std;
	co(); // 假设这个函数是安全的，并且已经定义  

	// 获取当前工作目录  
	char str[1024];
	if (_getcwd(str, sizeof(str)) == NULL) {
		perror("getcwd() error");
		exit(-1000);// 使用perror打印错误信息，包括errno的值  
	}
	currentDirPath = str;

	// 设置日志目录和文件路径  
	logDir = currentDirPath + "\\data";
	logFilePath = logDir + "\\log\\" + t2() + ".log";

	// 调试信息  
	
	info("server", "starting server...");

	// 尝试打开配置文件  
	iout.open("config.ini", ios_base::out | ios_base::app);
	if (!iout.is_open()) {
		warn("stringio", "Failed to open configuration file");
		ifstream iin("config.ini");
		if (!iin.is_open()) eout; // 假设 eout 是一个输出错误信息的函数  
		iout.open("config.ini", ios_base::out | ios_base::app);
		if (!iout.is_open()) {
			err("stringio", "Failed to open configuration file");
		}
	}

	// 尝试打开日志文件  
	fout.open(logFilePath.c_str(), ios_base::out | ios_base::app);
	if (!fout.is_open()) {
		// 尝试创建目录  
		if (!CreateDirectory(utf8ToWide(logDir).c_str(), NULL)) {
			err("fileio", "Failed to open data folder");
			eout;
		}

		// 尝试创建日志目录  
		if (!CreateDirectory(utf8ToWide(logDir + "\\log").c_str(), NULL)) {
			err("fileio", "Failed to open log folder");
			eout;
		}

		// 再次尝试打开日志文件  
		fout.open(logFilePath.c_str(), ios_base::out | ios_base::app);
		if (!fout.is_open()) {
			err("stringio", "Failed to open log file");
		}
	}
	info("EMS", "EMS(Error monitoring system,错误监测系统)已开启。");
}
#endif