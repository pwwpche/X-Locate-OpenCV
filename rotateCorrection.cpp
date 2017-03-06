// ration2.cpp : 定义控制台应用程序的入口点。
//
#include <stdlib.h>
#include "highgui.h"
#include "cv.h"
#include "cxcore.h"
class RotateCorrection{
public:
	int execute(std::string imgPath){
		IplImage *im_src = cvLoadImage(imgPath.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		IplImage *im_show = cvCreateImage(cvGetSize(im_src), 8, 3);
		cvConvertImage(im_src,im_show);
		IplImage *im_threshold =cvCreateImage(cvGetSize(im_src),8,1);
		cvNot(im_src,im_src);

		cvThreshold(im_src,im_threshold,0,255,CV_THRESH_OTSU);
		cvShowImage("im_thresold",im_threshold);
		CvMemStorage *storage = cvCreateMemStorage(0);
		CvSeq * contour = NULL;
		cvFindContours(im_threshold,storage,&contour);
		float angle ;
		for (;contour!=0;contour=contour->h_next)
		{
			if (contour->total > 10)
			{
				CvBox2D ellipse = cvFitEllipse2(contour);
				cvEllipseBox(im_show,ellipse,CV_RGB(255,0,0));
				angle = ellipse.angle - 180 ; 
				printf("%f \n",ellipse.angle);
			}
		}

		IplImage * ration = cvCreateImage(cvGetSize(im_src),8,1);
		float m[6];
		CvMat M = cvMat( 2, 3, CV_32F, m );
		CvPoint2D32f pt = cvPoint2D32f(im_src->width/2.0, im_src->height/2.0);
		cv2DRotationMatrix(pt, angle, 1.0, &M);
		cvWarpAffine(im_src,ration,&M,CV_INTER_AREA |CV_WARP_FILL_OUTLIERS,cvScalarAll(0)); 
		cvNot(ration,ration);
		cvSaveImage("s.bmp",ration);
		cvShowImage("ration",ration);
		cvShowImage("show",im_show);
		cvWaitKey(-1);
		getchar();

		cvReleaseImage(&ration);
		cvReleaseImage(&im_threshold);
		cvReleaseImage(&im_src);
		return 0;
	}
};