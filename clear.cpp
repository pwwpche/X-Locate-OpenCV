#include"cv.h"  
#include"highgui.h" 
#include<iostream> 
using namespace std;
double focus(IplImage* image);
int main_cl() {   
	IplImage* previous1;
	previous1=cvLoadImage("E://Mask//zhishidao_mask.jpg",1);


	if(previous1!=0)  {
		cvNamedWindow("previous1",1);
		cvShowImage("previous1",previous1);

		cout<<"The definition of the previous1 is: "<<focus(previous1)<<endl;
		cvDestroyWindow("previous1");
		cvReleaseImage(&previous1); 
		return 0;
	}   
	return -1; 
}   

double focus(IplImage* image) {
	IplImage* picone=cvCreateImage(cvGetSize(image),8,3);
	cvCvtColor(image,picone,CV_BGR2YCrCb);
	CvScalar gety;
	double z=0,zy1=0,zy2=0,total=0;
	double gety1=0,gety2=0;
	double final=0;
	for(int ix=0;ix<(picone->height);ix++)
	{
		gety1=0;
		gety2=0;
		zy1=0;
		zy2=0;
		for(int jy=0;jy<(picone->width);jy++) 
		{
			gety=cvGet2D(picone,ix,jy);
			z=0.5*gety.val[0]-gety1+0.5*gety2+zy1-0.5*zy2;
			total=total+z;
			gety2=gety1;
			gety1=gety.val[0];
			zy2=zy1;
			zy1=z;
		}  
	}   
	cvReleaseImage(&picone);
	final=abs(total/((image->height)*(image->width)));
	return final; 
}