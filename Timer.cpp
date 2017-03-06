#include"Timer.h"

void Timer::start()
{
	 QueryPerformanceFrequency(&large_interger);  
    dff = large_interger.QuadPart;  
    QueryPerformanceCounter(&large_interger);  
    c1 = large_interger.QuadPart;  
}

void Timer::stop()
{
	QueryPerformanceCounter(&large_interger);  
    c2 = large_interger.QuadPart;  
}

double Timer::result()
{
	return (c2 - c1) * 1000 / dff;
}