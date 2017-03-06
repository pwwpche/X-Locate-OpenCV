//  �ֲ�ͼ��������ȡ��ƥ��  
//  Author:  www.icvpr.com  
//  Blog  �� http://blog.csdn.net/icvpr    
  #include "LocalFeature.h"  
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>  
  
using namespace cv;  
using namespace std;  
  
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


  
int main7(int argc, char** argv)  
{  
	//if (argc != 6)  
	//{  
	//	cout << "wrong usage!" << endl;  
	//	cout << "usage: .exe FAST SIFT BruteForce queryImage trainImage" << endl;  
	//	return -1;  
	//}  
  
	string detectorType = "FAST";  
	string extractorType = "SIFT";  
	//string matchType = "BruteForce";  
	string matchType = "FlannBased";  
	//BruteForce-L1
	//BruteForce-Hamming

	string queryImagePath = "C:/1.jpg";  
	string trainImagePath = "C:/2.jpg";  
	  
	Mat queryImage = imread(queryImagePath, CV_LOAD_IMAGE_GRAYSCALE);  
	if (queryImage.empty())  
	{  
		cout<<"read failed"<< endl;  
		return -1;  
	}  
	  
	Mat trainImage = imread(trainImagePath, CV_LOAD_IMAGE_GRAYSCALE);  
	if (trainImage.empty())  
	{  
		cout<<"read failed"<< endl;  
		return -1;  
	}  
	//-- Step 1: Detect the keypoints using SURF Detector
	Feature feature(detectorType, extractorType, matchType);  
	  
	vector<KeyPoint> queryKeypoints, trainKeypoints;  
	feature.detectKeypoints(queryImage, queryKeypoints);  
	feature.detectKeypoints(trainImage, trainKeypoints);  


	//-- Step 2: Calculate descriptors (feature vectors)  
	Mat queryDescriptor, trainDescriptor;  
	
	feature.extractDescriptors(queryImage, queryKeypoints, queryDescriptor);  
	feature.extractDescriptors(trainImage, trainKeypoints, trainDescriptor);  


	//-- Step 3: Matching descriptor vectors using matcher
	vector<DMatch> matches;  
	feature.bestMatch(queryDescriptor, trainDescriptor, matches);  
	  
	vector<vector<DMatch> > knnmatches;  
	feature.knnMatch(queryDescriptor, trainDescriptor, knnmatches, 2);  
	  
	Mat outImage;  
	feature.saveMatches(queryImage, queryKeypoints, trainImage, trainKeypoints, matches, "../");  
	  
	return 0;  
}  




//
//
///// ȫ�ֱ���
//Mat img; Mat templ; Mat result;
//char* image_window = "Source Image";
//char* result_window = "Result window";
//
//int match_method;
//int max_Trackbar = 5;
//
///// ��������
//void MatchingMethod( int, void* );
//
//
//
//int main4( int argc, char** argv )
//{
//	//  ��ͼƬ
//	string queryImagePath = "C:/1.jpg";  
//	string trainImagePath = "C:/2.jpg";  
//	img = imread( queryImagePath , IMREAD_GRAYSCALE );
//	templ = imread( trainImagePath , IMREAD_GRAYSCALE );
//	
//	//  ����ͼ����ʾ����
//	namedWindow( image_window, CV_WINDOW_AUTOSIZE );
//	namedWindow( result_window, CV_WINDOW_AUTOSIZE );
//
//	//  �����춯��
//	char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
//	createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
//
//	MatchingMethod( 0, 0 );
//
//	waitKey(0);
//	return 0;
//}
//
//
//
//// ģ��ƥ��
//void MatchingMethod( int, void* )
//{
//	// ������ʾ���
//	Mat img_display;
//	img.copyTo( img_display );
//
//	// ���ڴ洢ƥ�����ľ���
//	int result_cols =  img.cols - templ.cols + 1;
//	int result_rows = img.rows - templ.rows + 1;
//	result.create( result_cols, result_rows, CV_32FC1 );
//
//	// ����ģ��ƥ��
//	matchTemplate( img, templ, result, match_method );
//	// ��һ�������������ʾ�����
//	normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
//
//	// �ҵ����ƥ��λ��
//	double minVal; 
//	double maxVal; 
//	Point minLoc; 
//	Point maxLoc;
//	Point matchLoc;
//
//	minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );   // Ѱ��result�е�������Сֵ���Լ���������������λ��
//
//	// ʹ��SQDIFF��SQDIFF_NORMED����ʱ��ֵԽС����Խ����
//	// ʹ����������ʱ��ֵԽ�����Խ����
//	if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
//	{
//		matchLoc = minLoc; 
//	}
//	else
//	{ 
//		matchLoc = maxLoc; 
//	}
//
//	// ��ʾƥ����
//	rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
//	rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
//
//	imshow( image_window, img_display );
//	imshow( result_window, result );
//}