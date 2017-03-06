#include <highgui.h>
#include <cv.h>

int main_p(int argc, char** argv)
{
	CvPoint2D32f srcTri[4], dstTri[4]; //二维坐标下的点，类型为浮点
	//CvMat* rot_mat = cvCreateMat( 2, 3, CV_32FC1 );  //多通道矩阵
	CvMat* warp_mat = cvCreateMat( 3, 3, CV_32FC1 );
	IplImage *src, *dst;

	if( ( ( src = cvLoadImage( "C:/6.jpg", 1 ) ) != 0 ) )
	{
		dst = cvCloneImage( src );  //制作图像的完整拷贝
		dst ->origin = src ->origin;  
		/*
		int origin; /* 0 - 顶—左结构,
		1 - 底—左结构 (Windows bitmaps 风格) 
		*/
		cvZero( dst );  //清空数组

		//计算矩阵仿射变换
		srcTri[0].x = 0;
		srcTri[0].y = 0;
		srcTri[1].x = src -> width - 1;  //缩小一个像素
		srcTri[1].y = 0;
		srcTri[2].x = 0;
		srcTri[2].y = src -> height - 1;
		srcTri[3].x = src -> width - 1;  //bot right
		srcTri[3].y = src -> height - 1;

		//改变目标图像大小
		dstTri[0].x = 0;
		dstTri[0].y = src->height * 0.25;
		dstTri[1].x = src -> width - 1;
		dstTri[1].y = 0;
		dstTri[2].x = 0;
		dstTri[2].y = src -> height * 0.75;
		dstTri[3].x = src -> width - 1;
		dstTri[3].y = src -> height - 1;

		cvGetPerspectiveTransform( srcTri, dstTri, warp_mat );  //由三对点计算仿射变换 
		cvWarpPerspective( src, dst, warp_mat );  //对图像做仿射变换

		//输出
		cvNamedWindow( "Perspective Warp", 1 );
		cvShowImage( "Perspective Warp", dst );  //最终是输出dst 
		cvSaveImage( "C:/persp.jpg" , dst);
		cvWaitKey();
	}
	cvReleaseImage( &dst );
	cvReleaseMat( &warp_mat );

	return 0;
}