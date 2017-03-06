//ͼ���Canny��Ե���
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
////cvCreateTrackbar�Ļص�����
//void on_trackbar(int threshold)
//{
//	//canny��Ե���
//	cvCanny(g_pSrcImage, g_pCannyImg, threshold, threshold * 3, 3);
//	cvShowImage(pstrWindowsCannyTitle, g_pCannyImg);
//}
//int main5()
//{
//	const char *pstrImageName = "C:\1.jpg";
//	const char *pstrWindowsSrcTitle = "ORIGIN SRC";
//	const char *pstrWindowsToolBar = "Threshold";
//
//	//���ļ�������ͼ��ĻҶ�ͼCV_LOAD_IMAGE_GRAYSCALE - �Ҷ�ͼ
//	g_pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_GRAYSCALE);
//	g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);
//
//	//��������
//	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
//	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);
//
////����������
//	int nThresholdEdge = 1;
//	cvCreateTrackbar(pstrWindowsToolBar, pstrWindowsCannyTitle, &nThresholdEdge, 100, on_trackbar);
//
//	//��ָ����������ʾͼ��
//	cvShowImage(pstrWindowsSrcTitle, g_pSrcImage);
//	on_trackbar(1);
//
//	//�ȴ������¼�
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
    // �ҵ�ͼ���е������Сֵ
    minMaxLoc(cornerStrength,&minStrength,&maxStrength);

    Mat dilated;
    Mat locaMax;
    // ����ͼ�����ҳ�ͼ����ȫ���ľֲ����ֵ��
    dilate(cornerStrength,dilated,Mat());
    // compare��һ���߼��ȽϺ�������������ͼ���ж�Ӧ����ͬ�Ķ�ֵͼ��
    compare(cornerStrength,dilated,locaMax,CMP_EQ);

    Mat cornerMap;
    double qualityLevel=0.01;
    double th=qualityLevel*maxStrength; // ��ֵ����
    threshold(cornerStrength,cornerMap,th,255,THRESH_BINARY);
    cornerMap.convertTo(cornerMap,CV_8U);
    // ����λ����
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