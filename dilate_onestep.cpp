
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
int main_do(int argc,char** argv)
{

	IplImage *image,*image1,*image2;
	image = cvLoadImage("C:/img_cor.jpg");
	image1 = cvCreateImage(cvSize(image->width,image->height),8,1);
	image2 = cvCreateImage(cvSize(image->width,image->height),8,1);

	cvCvtColor(image,image1,CV_RGB2GRAY);
	cvCvtColor(image,image2,CV_RGB2GRAY);

	//自定义1*3的核进行X方向的膨胀腐蚀
	IplConvKernel* kernal = cvCreateStructuringElementEx(3,1, 1, 0, CV_SHAPE_RECT);
	cvDilate(image1, image1, kernal, 2);//X方向膨胀连通数字
	cvErode(image1, image2, kernal, 4);//X方向腐蚀去除碎片
	cvDilate(image2, image1, kernal, 2);//X方向膨胀回复形态

	//自定义3*1的核进行Y方向的膨胀腐蚀
	kernal = cvCreateStructuringElementEx(1, 3, 0, 1, CV_SHAPE_RECT);
	cvErode(image1, image1, kernal, 1);// Y方向腐蚀去除碎片
	cvDilate(image1, image1, kernal, 2);//回复形态
	//cvShowImage("Dilate",image1);
	cvSaveImage("C:/corr.jpg", image1);
	//cvWaitKey(0);

	return 0;
}