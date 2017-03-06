#include <iostream>
#include <opencv2\opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

class RectChecker{
private:
	bool check_length(const vector<Point2f> &corners);//判断长度不小于20，并且长短比不超过7
	bool check_angle(const vector<Point2f> &corners);//判断角度不小于30度
	bool check_order(const vector<Point2f> &corners, int height, int width);
	double my_max(double len1, double len2);
	double my_min(double len1, double len2);
	double get_length(const Point2f &s1,const Point2f &s2);
	double get_angle(const Point2f &s1,const Point2f &s2,const Point2f &s3);
public:
	bool check(vector<Point2f> corners, int imgHeight, int imgWidth);
};