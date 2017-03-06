#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "cv.h"
#include <opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace cv;
using namespace std;
#define DEBUG_IMG

class Histogram{
public:
	void getHistogram(string imgPath){
		IplImage * src= cvLoadImage(imgPath.c_str());
		cvSmooth(src,src,CV_MEDIAN,3,src->nChannels);
		IplImage* gray_plane = cvCreateImage(cvGetSize(src),8,1);
	
		cvCvtColor(src,gray_plane,CV_BGR2GRAY);

		int hist_size = 256;    //ֱ��ͼ�ߴ�
		int hist_height = 256;
		float range[] = {0,255};  //�Ҷȼ��ķ�Χ
		float* ranges[]={range};
		//����һάֱ��ͼ��ͳ��ͼ����[0 255]���صľ��ȷֲ�
		CvHistogram* gray_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);
		//����Ҷ�ͼ���һάֱ��ͼ
		cvCalcHist(&gray_plane,gray_hist,0,0);
		//��һ��ֱ��ͼ
		cvNormalizeHist(gray_hist,1.0);

		int scale = 2;
		//����һ��һάֱ��ͼ�ġ�ͼ����������Ϊ�Ҷȼ���������Ϊ���ظ�����*scale��
		IplImage* hist_image = cvCreateImage(cvSize(hist_size*scale,hist_height),8,3);
		cvZero(hist_image);
		//ͳ��ֱ��ͼ�е����ֱ����
		float max_value = 0;
		cvGetMinMaxHistValue(gray_hist, 0,&max_value,0,0);
		max_value = max_value > 0.02 ? 0.02 : max_value;
		//�ֱ�ÿ��ֱ�����ֵ���Ƶ�ͼ��
		for(int i=0;i<hist_size;i++)
		{
		
			float bin_val = cvQueryHistValue_1D(gray_hist,i); //����i�ĸ���
			int intensity = cvRound(bin_val*hist_height/max_value);  //Ҫ���Ƶĸ߶�
			cvRectangle(hist_image,
				cvPoint(i*scale,hist_height-1),
				cvPoint((i+1)*scale - 1, hist_height - intensity),
				CV_RGB(255,255,255));  
		}
	#ifdef DEBUG_IMG
		cvNamedWindow( "GraySource", 1 );
		cvShowImage("GraySource",gray_plane);
		cvNamedWindow( "H-S Histogram", 1 );
		cvShowImage( "H-S Histogram", hist_image );
		cvWaitKey(0);
	#else
		unsigned found = imgPath.find_last_of("/\\");
		string filename = imgPath.substr(found+1);
		string saveName ="E:/Histo/" +  filename.insert(filename.find_last_of(".") , "_hist");
		cvSaveImage(saveName.c_str(), hist_image);
	#endif
	}

	void execute(string imgPath){
		 Mat src, dst;
		  char* source_window = "Source image";
		  char* equalized_window = "Equalized Image";
		  /// Load image
		  src = imread( imgPath.c_str(), 1 );

		  /// Convert to grayscale
		  cvtColor( src, src, CV_BGR2GRAY );

		  /// Apply Histogram Equalization
		  //equalizeHist( src, dst );
		  getHistogram(imgPath);

		   //Display results
		  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
		  namedWindow( equalized_window, CV_WINDOW_AUTOSIZE );

		  imshow( source_window, src );
		  imshow( equalized_window, dst );

		   //Wait until user exits the program
		  waitKey(0);

		  return ;
	}
};