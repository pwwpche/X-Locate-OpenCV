#include <highgui.h>
#include <cv.h>

int main_p(int argc, char** argv)
{
	CvPoint2D32f srcTri[4], dstTri[4]; //��ά�����µĵ㣬����Ϊ����
	//CvMat* rot_mat = cvCreateMat( 2, 3, CV_32FC1 );  //��ͨ������
	CvMat* warp_mat = cvCreateMat( 3, 3, CV_32FC1 );
	IplImage *src, *dst;

	if( ( ( src = cvLoadImage( "C:/6.jpg", 1 ) ) != 0 ) )
	{
		dst = cvCloneImage( src );  //����ͼ�����������
		dst ->origin = src ->origin;  
		/*
		int origin; /* 0 - ������ṹ,
		1 - �ס���ṹ (Windows bitmaps ���) 
		*/
		cvZero( dst );  //�������

		//����������任
		srcTri[0].x = 0;
		srcTri[0].y = 0;
		srcTri[1].x = src -> width - 1;  //��Сһ������
		srcTri[1].y = 0;
		srcTri[2].x = 0;
		srcTri[2].y = src -> height - 1;
		srcTri[3].x = src -> width - 1;  //bot right
		srcTri[3].y = src -> height - 1;

		//�ı�Ŀ��ͼ���С
		dstTri[0].x = 0;
		dstTri[0].y = src->height * 0.25;
		dstTri[1].x = src -> width - 1;
		dstTri[1].y = 0;
		dstTri[2].x = 0;
		dstTri[2].y = src -> height * 0.75;
		dstTri[3].x = src -> width - 1;
		dstTri[3].y = src -> height - 1;

		cvGetPerspectiveTransform( srcTri, dstTri, warp_mat );  //�����Ե�������任 
		cvWarpPerspective( src, dst, warp_mat );  //��ͼ��������任

		//���
		cvNamedWindow( "Perspective Warp", 1 );
		cvShowImage( "Perspective Warp", dst );  //���������dst 
		cvSaveImage( "C:/persp.jpg" , dst);
		cvWaitKey();
	}
	cvReleaseImage( &dst );
	cvReleaseMat( &warp_mat );

	return 0;
}