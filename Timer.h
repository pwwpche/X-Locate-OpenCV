#pragma once
#include<Windows.h>
#include<iostream>
using namespace std;
class Timer
{
private:
	 LARGE_INTEGER  large_interger;  
    double dff;  
    __int64  c1, c2;  
public:
	Timer(){}
	~Timer(){}
	void start();
	void stop();
	double result();
};