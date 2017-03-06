#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include<string>

using namespace std;

class Dilate{
public:
	int execute(string imgPath){
		int a=1;
		int eIter=1, dIter=1, exitFlag=1;
		IplImage *image,*image1,*image2;
		image = cvLoadImage(imgPath.c_str());
		image1 = cvCreateImage(cvSize(image->width,image->height),8,1);
		image2 = cvCreateImage(cvSize(image->width,image->height),8,1);
		IplConvKernel* kernal = cvCreateStructuringElementEx(3,1, 1, 0, CV_SHAPE_RECT);

		cvCvtColor(image,image1,CV_RGB2GRAY);
		cvCvtColor(image,image2,CV_RGB2GRAY);
		cvNamedWindow("image",1);
		cvShowImage("image",image);

		cvNamedWindow("Select",1);
		cvCreateTrackbar("eIterations","Select",&eIter,20,0);
		cvCreateTrackbar("dIterations","Select",&dIter,20,0);
		cvNamedWindow("Erode",1);
		cvNamedWindow("Dilate",1);
		cvShowImage("Erode",image1);
		cvShowImage("Dilate",image2);
		cvWaitKey(0);

		while(1)
		{
			a=cvWaitKey(0);
			if((char) a == 'q')
				break;
			//腐蚀，消除细小物质
			kernal = cvCreateStructuringElementEx(1, 1, 0, 1, CV_SHAPE_RECT);
			cvErode(image1,image1,kernal,1);

			cvShowImage("Erode",image1);
			std::cout<<"Erode"<<std::endl;

		}

		cvCopy(image1,image2);
		a=1;
		while(1)
		{
			kernal = cvCreateStructuringElementEx(1, 1, 0, 1, CV_SHAPE_RECT);
			//腐蚀，填充物体内细小物质
			cvDilate(image2,image2,kernal,1);

			cvShowImage("Dilate",image2);
			std::cout<<"Dilate"<<std::endl;

			a=cvWaitKey(0);

			if((char) a == 'q')
				break;
		}
		return 0;
	}
};
