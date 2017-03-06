//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <opencv2/opencv.hpp>
//#include <stdlib.h>
//#include <stdio.h>
//using namespace std;
//using namespace cv;
//
///// Global variables
//
//Mat src, src_gray;
//Mat dst, detected_edges;
//
//int edgeThresh = 1;
//int lowThreshold;
//int const max_lowThreshold = 100;
//int ratio = 3;
//int kernel_size = 3;
//char* window_name = "Edge Map";
//
//
//
/////**
//// * @function CannyThreshold
//// * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
//// */
//void CannyThreshold(int, void*)
//{
//	/// Reduce noise with a kernel 3x3
//	blur( src_gray, detected_edges, Size(3,3) );
//
//	/// Canny detector
//	Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
//
//	/// Using Canny's output as a mask, we display our result
//	dst = Scalar::all(0);
//
//	src.copyTo( dst, detected_edges);
//	IplImage* img;
//	img = new IplImage(detected_edges);
//
//	cvThreshold(img, img, 10, 255, 0);
//#ifdef DEBUG_TRACK
//	cvShowImage("img_0", img);
//#endif
//	CvMemStorage *storage = cvCreateMemStorage();
//	CvSeq *lines = 0;
//
//	//获取图片中可能的轮廓线
//	lines = cvHoughLines2(img,storage,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,10,minLen,minGaps);		//Find verticle lines in the image
//
//	vector<double> linesHor_k;
//	vector<double> linesHor_b;
//	vector<double> logoY;
//
//	int posCount = 0;
//	int negCount = 0;
//	double posAvg = 0;
//	double negAvg = 0;
//	for(int i = 0;i<lines->total; i++){
//		//计算当前直线的直线方程
//		CvPoint *line = (CvPoint*)cvGetSeqElem(lines,i);
//		double k = (double)(line[1].y - line[0].y) / (double)(line[1].x - line[0].x);
//		double b = -k * line[0].x + line[0].y;
//
//		////过滤竖线
//		//if(k > 1.5 || k < -1.5)
//		//{
//		//	continue;
//		//}
//
//		//剔除重合或直线方程相似的直线
//		int linesCount  = linesHor_k.size();
//		bool valid = true;
//		for(int i = 0 ; i < linesCount ; i++){
//			if(linesHor_k[i] - k < 0.07 && linesHor_k[i] - k > -0.07 ){
//				if(linesHor_b[i] - b < 30 && linesHor_b[i] - b > -30){		//Find a similar line
//					valid = false;
//					break;
//				}
//			}
//		}
//	}
//	for(int i = 0 ; i < linesHor_k.size() ; i++){
//		double k = linesHor_k[i];
//		double b = linesHor_b[i];
//		CvPoint pt1, pt2;
//		pt1.x = 0;
//		pt1.y = b;
//		pt2.x = 3000;
//		pt2.y = 3000 * k + b;
//		cvLine(img,pt1,pt2,CV_RGB(0,255,0),2,8);
//	}
//	imshow( window_name, dst );
//}
//
//
/////* @function main */
//int main_cob( int argc, char** argv )
//{
//	/// Load an image
//	src = imread( "E:\\Metro_Photos_Compressed\\Metro0\\burger king2.jpg" );
//
//	if( !src.data )
//	{ return -1; }
//
//	/// Create a matrix of the same type and size as src (for dst)
//	dst.create( src.size(), src.type() );
//
//	/// Convert the image to grayscale
//	cvtColor( src, src_gray, CV_BGR2GRAY );
//
//	/// Create a window
//	namedWindow( window_name, CV_WINDOW_AUTOSIZE );
//
//	/// Create a Trackbar for user to enter threshold
//	createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
//
//	/// Show the image
//	CannyThreshold(0, 0);
//
//	/// Wait until user exit program by pressing a key
//	waitKey(0);
//
//	return 0;
//}