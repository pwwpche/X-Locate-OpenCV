
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

	//�Զ���1*3�ĺ˽���X��������͸�ʴ
	IplConvKernel* kernal = cvCreateStructuringElementEx(3,1, 1, 0, CV_SHAPE_RECT);
	cvDilate(image1, image1, kernal, 2);//X����������ͨ����
	cvErode(image1, image2, kernal, 4);//X����ʴȥ����Ƭ
	cvDilate(image2, image1, kernal, 2);//X�������ͻظ���̬

	//�Զ���3*1�ĺ˽���Y��������͸�ʴ
	kernal = cvCreateStructuringElementEx(1, 3, 0, 1, CV_SHAPE_RECT);
	cvErode(image1, image1, kernal, 1);// Y����ʴȥ����Ƭ
	cvDilate(image1, image1, kernal, 2);//�ظ���̬
	//cvShowImage("Dilate",image1);
	cvSaveImage("C:/corr.jpg", image1);
	//cvWaitKey(0);

	return 0;
}