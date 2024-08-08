#pragma once
#ifndef __PROGRESS_BAR_H
#define __PROGRESS_BAR_H

#define MAX_PROGRESS_BAR_LENGTH 100
#define MIN_PROGRESS_BAR_LENGTH 1
#define PB ProgressBer

class ProgressBer
{
	private:
		double percentage;
		double MaxPercentage;
		double step;
		//static int member;
	public:
		ProgressBer();
		ProgressBer(int _MaxPercentage,double _step=1.0);
		bool update();
		bool clear();
		bool rewind(double num);
		double nownum();
		void printProgressBar(const ProgressBer& pb);
		~ProgressBer();
};
#endif
