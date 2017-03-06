// PerspectiveWarp.cpp : 定义控制台应用程序的入口点。
//

#include<iostream>
#include <opencv.hpp>
#include <highgui.h>
using namespace std;
using namespace cv;
Mat img_pw;
void mouse_callback(int event, int x, int y, int flags, void* param);
cv::Mat perspectiveWrap2(Mat img_pw, vector<cv::Point2f> corners);
void sortCorners3(std::vector<cv::Point2f>& corners, cv::Point2f center);


int main_pw(int argc, char* argv[])
{
	//cv::Mat img_pw = cv::imread("C:/Metro_refined_640/Plory3.jpg");
	//cv::Mat img_pw = cv::imread("E:/Hough3/GXG2_hough.jpg");
	img_pw = cv::imread("E:\\Metro_Photos_Compressed\\Metro0\\royal shen.jpg");
	cv::Size imgSize = img_pw.size();
	
	cv::vector<cv::Point2f> corners;
	cv::namedWindow("View", CV_WINDOW_AUTOSIZE);
	//cv::resizeWindow("View", 600, 1000);

	//手动点坐标，从左上角开始顺时针
	cv::setMouseCallback("View", mouse_callback, (void*)&corners);

	cv::imshow("View", img_pw);
	cvWaitKey(0);

	cv::Mat warpMat = perspectiveWrap2(img_pw, corners);
	cv::Mat dstImg = img_pw;
	cv::warpPerspective(img_pw, dstImg, warpMat, img_pw.size());


	//cv::namedWindow("Viewdst", CV_WINDOW_NORMAL);
	//cv::resizeWindow("Viewdst", 800, 600);

	cv::imshow("Viewdst", dstImg);

	cvWaitKey(0);
	cv::destroyAllWindows();
	return 0;
}


void sortCorners3(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;
	std::vector<cv::Point2f> left, right;
	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].x < center.x)
			left.push_back(corners[i]);
		else
			right.push_back(corners[i]);
	}

	cv::Point2f tl, tr, bl, br;
	if(top.size() == 2 && bot.size() == 2){
		tl = top[0].x > top[1].x ? top[1] : top[0];
		tr = top[0].x > top[1].x ? top[0] : top[1];
		bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
		br = bot[0].x > bot[1].x ? bot[0] : bot[1];
	}else if(left.size() ==2 && right.size() == 2){
		tl = left[0].y < left[1].y ? left[0] : left[1];
		bl = left[0].y < left[1].y ? left[1] : left[0];
		tr = right[0].y < right[1].y ? right[0] : right[1];
		br = right[0].y < right[1].y ? right[1] : right[0];
	}else{
		tl = left[0].y < left[1].y ? left[0] : left[1];
		bl = left[0].y < left[1].y ? left[1] : left[0];
		tr = right[0].y < right[1].y ? right[0] : right[1];
		br = right[0].y < right[1].y ? right[1] : right[0];
	}
	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}


cv::Mat perspectiveWrap2(Mat img_pw, vector<cv::Point2f> corners){
	std::vector<cv::Point2f> src(4);
	std::vector<cv::Point2f> dst(4);

	for(int i=0; i<4; i++)
	{
		src[i] = corners[i];
	}

	cv::Point2f center;
	for (int i = 0; i < corners.size(); i++)
		center += corners[i];
	center *= (1. / corners.size());
	sortCorners3(corners, center);
	float area = cv::contourArea(corners);

	float edge1 = sqrt((corners[1].x - corners[0].x) * (corners[1].x - corners[0].x) + 
		(corners[1].y - corners[0].y) * (corners[1].y - corners[0].y));
	float edge2 = sqrt((corners[2].x - corners[1].x) * (corners[2].x - corners[1].x) + 
		(corners[2].y - corners[1].y) * (corners[2].y - corners[1].y));
	float edge3 = sqrt((corners[3].x - corners[2].x) * (corners[3].x - corners[2].x) + 
		(corners[3].y - corners[2].y) * (corners[3].y - corners[2].y));
	float edge4 = sqrt((corners[0].x - corners[3].x) * (corners[0].x - corners[3].x) + 
		(corners[0].y - corners[3].y) * (corners[0].y - corners[3].y));


	float dst_height, dst_width;
	//if(edge1 < edge2){
	//	//dst_height = edge1 > edge3 ? edge1 : edge3;
	//	//dst_width = edge2 > edge4 ? edge2 : edge4;
	//	dst_height = (edge1 + edge3) / 2 ;
	//	dst_width = (edge2 + edge4) / 2 ;
	//}else{
	//	//dst_width = edge1 > edge3 ? edge1 : edge3;
	//	//dst_height = edge2 > edge4 ? edge2 : edge4;
	//	dst_width = (edge1 + edge3) / 2;
	//	dst_height = (edge2 + edge4) / 2;
	//}
	dst_width = (edge1 + edge3) / 1.7;
	dst_height = (edge2 + edge4) / 2;

	//dst赋值，从零点开始顺时针
	dst[0].x = corners[0].x;
	dst[0].y = corners[0].y;
	dst[1].x = corners[0].x + dst_width-1;
	dst[1].y = corners[0].y;
	dst[2].x = corners[0].x + dst_width-1;
	dst[2].y = corners[0].y + dst_height-1;
	dst[3].x = corners[0].x;
	dst[3].y = corners[0].y + dst_height-1;

	//通过4个点得到变换矩阵,然后进行变换
	cv::Mat warpMat = cv::getPerspectiveTransform(corners, dst);
	return warpMat;
}


void mouse_callback(int event, int x, int y, int flags, void* param)
{
	cv::vector<cv::Point2f> *corners = (cv::vector<cv::Point2f>*) param;

	//int ev = event;

	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		{
			if(corners->size() == 4)
				break;
			corners->push_back(cv::Point(x, y));
			circle(img_pw, cv::Point(x, y), 2, Scalar(255,0,0), 2);
			imshow("View", img_pw);
			break;
		}
	}
	return ;
}