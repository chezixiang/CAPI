#include "Progress_Bar.h" // 假设头文件名为ProgressBar.h  
#include <iostream>  
#include <iomanip> // 用于std::setw  

ProgressBer::ProgressBer() : percentage(0.0), MaxPercentage(100.0), step(1.0) {
    // 默认构造函数  
}

ProgressBer::ProgressBer(int _MaxPercentage, double _step)
    : percentage(0.0), MaxPercentage(static_cast<double>(_MaxPercentage)), step(_step) {
    // 初始化构造函数  
}

bool ProgressBer::update() {
    if (percentage < MaxPercentage) {
        percentage += step;
        if (percentage > MaxPercentage) {
            percentage = MaxPercentage;
        }
        return true;
    }
    return false; // 如果没有更新（即已经达到或超过最大值），则返回false  
}

bool ProgressBer::clear() {
    // 假设“清除”是指重置进度条到初始状态  
    percentage = 0.0;
    return true;
}

bool ProgressBer::rewind(double num) {
    if (num < 0) {
        // 如果回退的数量是负数，则忽略  
        return false;
    }
    percentage = std::max(0.0, percentage - num);
    return true;
}

double ProgressBer::nownum() {
    return percentage;
}

ProgressBer::~ProgressBer() {
    // 析构函数，通常用于清理资源，但这里没有特别的资源需要清理  
}

// 这是一个简单的示例函数，用于打印进度条到控制台  
void ProgressBer::printProgressBar(const ProgressBer& pb) {
    int barWidth = MAX_PROGRESS_BAR_LENGTH;
    int pos = static_cast<int>(pb.percentage * barWidth / pb.MaxPercentage);
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "█";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(2) << pb.percentage << "%\r";
    std::cout.flush(); // 确保立即打印  
}

// 注意：printProgressBar函数不是ProgressBer类的一部分，但它可以很方便地与ProgressBer实例一起使用来显示进度。