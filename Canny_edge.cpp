//图像的Canny边缘检测
//By MoreWindows (http://blog.csdn.net/MoreWindows)
#include <opencv2/opencv.hpp>
#include<iostream>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
void drawCornerOnImage(Mat& image,const Mat&binary);



////=================================================
//
//IplImage *g_pSrcImage, *g_pCannyImg;
//const char *pstrWindowsCannyTitle = "edge";
////cvCreateTrackbar的回调函数
//void on_trackbar(int threshold)
//{
//	//canny边缘检测
//	cvCanny(g_pSrcImage, g_pCannyImg, threshold, threshold * 3, 3);
//	cvShowImage(pstrWindowsCannyTitle, g_pCannyImg);
//}
//int main5()
//{
//	const char *pstrImageName = "C:\1.jpg";
//	const char *pstrWindowsSrcTitle = "ORIGIN SRC";
//	const char *pstrWindowsToolBar = "Threshold";
//
//	//从文件中载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图
//	g_pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_GRAYSCALE);
//	g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);
//
//	//创建窗口
//	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
//	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);
//
////创建滑动条
//	int nThresholdEdge = 1;
//	cvCreateTrackbar(pstrWindowsToolBar, pstrWindowsCannyTitle, &nThresholdEdge, 100, on_trackbar);
//
//	//在指定窗口中显示图像
//	cvShowImage(pstrWindowsSrcTitle, g_pSrcImage);
//	on_trackbar(1);
//
//	//等待按键事件
//	cvWaitKey();
//
//	cvDestroyWindow(pstrWindowsSrcTitle);
//	cvDestroyWindow(pstrWindowsCannyTitle);
//	cvReleaseImage(&g_pSrcImage);
//	cvReleaseImage(&g_pCannyImg);
//	return 0;
//}
////=================================================


//=================================================
int main6()
{
    Mat image=imread("C:\2.jpg");
    Mat gray;
    cvtColor(image,gray,CV_BGR2GRAY);

    Mat cornerStrength;
    cornerHarris(gray,cornerStrength,3,3,0.01);

    double maxStrength;
    double minStrength;
    // 找到图像中的最大、最小值
    minMaxLoc(cornerStrength,&minStrength,&maxStrength);

    Mat dilated;
    Mat locaMax;
    // 膨胀图像，最找出图像中全部的局部最大值点
    dilate(cornerStrength,dilated,Mat());
    // compare是一个逻辑比较函数，返回两幅图像中对应点相同的二值图像
    compare(cornerStrength,dilated,locaMax,CMP_EQ);

    Mat cornerMap;
    double qualityLevel=0.01;
    double th=qualityLevel*maxStrength; // 阈值计算
    threshold(cornerStrength,cornerMap,th,255,THRESH_BINARY);
    cornerMap.convertTo(cornerMap,CV_8U);
    // 逐点的位运算
    bitwise_and(cornerMap,locaMax,cornerMap);

    drawCornerOnImage(image,cornerMap);
    namedWindow("result");
    imshow("result",image);
    waitKey();
    
    return 0;
}
void drawCornerOnImage(Mat& image,const Mat&binary)
{
    Mat_<uchar>::const_iterator it=binary.begin<uchar>();
    Mat_<uchar>::const_iterator itd=binary.end<uchar>();
    for(int i=0;it!=itd;it++,i++)
    {
        if(*it)
            circle(image,Point(i%image.cols,i/image.cols),3,Scalar(0,255,0),1);    
    }
}