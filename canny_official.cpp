/* This is a standalone program. Pass an image name as the first parameter
of the program.  Switch between standard and probabilistic Hough transform
by changing "#if 1" to "#if 0" and back */
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <string>
using namespace cv;
using namespace std;
int main_co(int argc, char** argv)
{
    Mat src, dst, color_dst, src2;
	string path0 = "E:/Metro_Photos_Compressed/Metro3/baishida.jpg";
	IplImage* img = cvLoadImage(path0.c_str(), 0);
	IplImage* img2 = cvLoadImage(path0.c_str(), 0);
	src=imread(path0.c_str(), 0);
	src2=imread(path0.c_str(), 0);
	
	blur( src, src, Size(3,3) );
    Canny( src, dst, 150, 400, 3 );
    cvtColor( dst, color_dst, CV_GRAY2BGR );
	threshold(src2, src2, 150, 255, 0);
	cvSobel(img, img,0,2,3);
	cvThreshold(img, img, 220, 255, 0);
	IplConvKernel* kernalY = cvCreateStructuringElementEx(1,3, 0, 1, CV_SHAPE_RECT);
	cvDilate(img, img, kernalY, 6 );
	cvErode(img, img, kernalY, 9);
	cvDilate(img, img, kernalY, 3);

	IplConvKernel* kernal = cvCreateStructuringElementEx(3,1, 1, 0, CV_SHAPE_RECT);
	cvDilate(img, img, kernal, 9 * 2 );
	cvErode(img, img, kernal, 9 * 4);
	cvDilate(img, img, kernal, 9 * 2);


	cvSobel(img2, img2,2,0,3);
	cvThreshold(img2, img2, 220, 255, 0);
	cvDilate(img2, img2, kernal, 9 * 2 );//X方向膨胀连通数字
	cvErode(img2, img2, kernal, 9 * 4);//X方向膨胀连通数字
	cvDilate(img2, img2, kernal, 9 * 2);//X方向膨胀连通数字
	cvDilate(img2, img2, kernalY, 5);//X方向膨胀连通数字
	cvErode(img2, img2, kernalY, 5 * 2);//X方向膨胀连通数字
	cvDilate(img2, img2, kernalY, 5);//X方向膨胀连通数字

#if 0
    vector<Vec2f> lines;
    HoughLines( dst, lines, 1, CV_PI/180, 100 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        Point pt1(cvRound(x0 + 1000*(-b)),
                  cvRound(y0 + 1000*(a)));
        Point pt2(cvRound(x0 - 1000*(-b)),
                  cvRound(y0 - 1000*(a)));
        line( color_dst, pt1, pt2, Scalar(0,0,255), 3, 8 );
    }
#else
    //vector<Vec4i> lines;
    //HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
    //for( size_t i = 0; i < lines.size(); i++ )
    //{
    //    line( color_dst, Point(lines[i][0], lines[i][1]),
    //        Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    //}
#endif
    //namedWindow( "Source", 1 );
	cvShowImage("Sobel_X", img);
	cvShowImage("Sobel_Y", img2);
    imshow( "Canny", src );
	imshow( "Threshold", src2 );
    namedWindow( "Detected Lines", 1 );
    imshow( "Detected Lines", color_dst );

    waitKey(0);
    return 0;
}