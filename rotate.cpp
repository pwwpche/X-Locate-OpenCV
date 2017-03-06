#include "cv.h"
#include "highgui.h"

int main_r()
{
	IplImage *imgSrc = cvLoadImage("C:/20.jpg");
	int w_src = imgSrc->width;
	int h_src = imgSrc->height;

	///////////实现旋转////////////////
	double degree1 =0; 
	double angle1 = degree1  * CV_PI / 180.; 
	double a1 = sin(angle1), b1 = cos(angle1); 
	//得到旋转后图像大小
	int w_dst = int(h_src * fabs(a1) + w_src * fabs(b1));
	int h_dst = int(w_src * fabs(a1) + h_src * fabs(b1));
	//得到旋转矩阵
	double map[6];
	CvMat map_matrix = cvMat(2, 3, CV_64FC1, map);
	CvPoint2D32f pt = cvPoint2D32f(w_src / 2, h_src / 2);
	cv2DRotationMatrix(pt, degree1, 1.0, &map_matrix);
	//一定要将中心位置移到目标图像中心
	map[2] += (w_dst - w_src) / 2;
	map[5] += (h_dst - h_src) / 2;

	IplImage *imgDst = cvCreateImage(cvSize(w_dst, h_dst), 8, 3);
	cvWarpAffine(imgSrc, imgDst,&map_matrix,CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS,cvScalarAll(0));

	///////////旋转的基础上倾斜////////////////
	double degree2 =30; 
	double angle2 = degree2  * CV_PI / 180.; 
	double a2 = sin(angle2), b2 = cos(angle2); 

	//由对应点关系计算倾斜矩阵
	CvPoint2D32f src_point[4];
	CvPoint2D32f dst_point[4];
	src_point[0].x=-w_dst/2;
	src_point[0].y=-h_dst/2;
	dst_point[0].x=-w_dst/2-h_dst/2*fabs(a2)*fabs(b2);
	dst_point[0].y=-h_dst/2*pow(fabs(b2),2.0);
	src_point[1].x=w_dst/2;
	src_point[1].y=-h_dst/2;
	dst_point[1].x=w_dst/2-h_dst/2*fabs(a2)*fabs(b2);
	dst_point[1].y=-h_dst/2*pow(fabs(b2),2.0);
	src_point[2].x=w_dst/2;
	src_point[2].y=h_dst/2;
	dst_point[2].x=w_dst/2+h_dst/2*fabs(a2)*fabs(b2);
	dst_point[2].y=h_dst/2*pow(fabs(b2),2.0);
	src_point[3].x=-w_dst/2;
	src_point[3].y=h_dst/2;
	dst_point[3].x=-w_dst/2+h_dst/2*fabs(a2)*fabs(b2);
	dst_point[3].y=h_dst/2*pow(fabs(b2),2.0);
	double m[6];
	CvMat M = cvMat(2, 3, CV_64FC1, m);
	cvGetAffineTransform(src_point,dst_point,&M);
	int w_dst2 =h_dst*fabs(a2)*fabs(b2)+w_dst;
	int h_dst2 =h_dst*pow(fabs(b2),2.0);

	IplImage *imgDst2 = cvCreateImage(cvSize(w_dst2, h_dst2), 8, 3);
	cvWarpAffine(imgDst, imgDst2,&M,CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS,cvScalarAll(0));
	//得到感兴趣区域
	int min_x=0;
	int min_y=0;
	int max_x=0;
	int max_y=0;
	bool min_flag=1;
	uchar* old_ptr;
	for( int y=0 ; y<h_dst2; y++)  
	{   
		for (int x=0 ; x<w_dst2; x++)  
		{  
			old_ptr = &((uchar*)(imgDst2->imageData + imgDst2->widthStep*y))[(x)*3];  
			if(old_ptr[0]!=0&&old_ptr[1]!=0&&old_ptr[2]!=0)
			{
				if(y>min_y&&min_flag)	{min_y=y;min_flag=0;}
				if (y>max_y)	max_y=y;
			}
		}  
	}

	min_flag=1;
	for(int x=0 ; x<w_dst2; x++)  
	{   
		for ( int y=0 ; y<h_dst2; y++)  
		{  
			old_ptr = &((uchar*)(imgDst2->imageData + imgDst2->widthStep*y))[(x)*3];  
			if(old_ptr[0]!=0&&old_ptr[1]!=0&&old_ptr[2]!=0)
			{
				if(x>min_x&&min_flag)	{min_x=x;min_flag=0;}
				if (x>max_x)	max_x=x;
			}
		}  
	}

	CvRect rect;
	rect.x=min_x;
	rect.y=min_y;
	rect.width=max_x-min_x;
	rect.height=max_y-min_y;
	cvSetImageROI(imgDst2,rect);
	IplImage *imgDst3 = cvCreateImage(cvSize(rect.width,rect.height), 8, 3);
	cvCopyImage(imgDst2,imgDst3);
	cvResetImageROI(imgDst2);
	
	cvNamedWindow ("src", 1);
	cvShowImage ("src", imgSrc);
	cvNamedWindow( "dst", 1 );
	cvShowImage( "dst", imgDst);
	cvNamedWindow( "dst2", 1 );
	cvShowImage( "dst2", imgDst2);
	cvNamedWindow( "dst3", 1 );
	cvShowImage( "dst3", imgDst3);//最终效果图
	cvWaitKey(0);

	cvReleaseImage(&imgSrc);
	cvReleaseImage(&imgDst);
	cvReleaseImage(&imgDst2);
	cvReleaseImage(&imgDst3);
	return 0;
}