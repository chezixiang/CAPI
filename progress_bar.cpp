#include "Progress_Bar.h" // ����ͷ�ļ���ΪProgressBar.h  
#include <iostream>  
#include <iomanip> // ����std::setw  

ProgressBer::ProgressBer() : percentage(0.0), MaxPercentage(100.0), step(1.0) {
    // Ĭ�Ϲ��캯��  
}

ProgressBer::ProgressBer(int _MaxPercentage, double _step)
    : percentage(0.0), MaxPercentage(static_cast<double>(_MaxPercentage)), step(_step) {
    // ��ʼ�����캯��  
}

bool ProgressBer::update() {
    if (percentage < MaxPercentage) {
        percentage += step;
        if (percentage > MaxPercentage) {
            percentage = MaxPercentage;
        }
        return true;
    }
    return false; // ���û�и��£����Ѿ��ﵽ�򳬹����ֵ�����򷵻�false  
}

bool ProgressBer::clear() {
    // ���衰�������ָ���ý���������ʼ״̬  
    percentage = 0.0;
    return true;
}

bool ProgressBer::rewind(double num) {
    if (num < 0) {
        // ������˵������Ǹ����������  
        return false;
    }
    percentage = std::max(0.0, percentage - num);
    return true;
}

double ProgressBer::nownum() {
    return percentage;
}

ProgressBer::~ProgressBer() {
    // ����������ͨ������������Դ��������û���ر����Դ��Ҫ����  
}

// ����һ���򵥵�ʾ�����������ڴ�ӡ������������̨  
void ProgressBer::printProgressBar(const ProgressBer& pb) {
    int barWidth = MAX_PROGRESS_BAR_LENGTH;
    int pos = static_cast<int>(pb.percentage * barWidth / pb.MaxPercentage);
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "��";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(2) << pb.percentage << "%\r";
    std::cout.flush(); // ȷ��������ӡ  
}

// ע�⣺printProgressBar��������ProgressBer���һ���֣��������Ժܷ������ProgressBerʵ��һ��ʹ������ʾ���ȡ�