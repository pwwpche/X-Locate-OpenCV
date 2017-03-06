#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;


IplImage *g_pGrayImage0 = NULL;

const char *pstrWindowsBinaryTitle0 = "Bin";


float minThresholdBar = 100;
float maxThresholdBar = 120;
size_t minRepeatability = 0;
float minDistBetweenBlobs = 0;
float minArea = 100, maxArea = 10000;
void processImg();

void ontrackbar1(int pos)
{
	minThresholdBar = pos;

	processImg();
}
void ontrackbar2(int pos)
{
	maxThresholdBar = pos;
	processImg();
}
void on_trackbar3(int pos)
{
	minRepeatability = pos;
	processImg();
}
void on_trackbar4(int pos)
{
	minDistBetweenBlobs = pos;
	processImg();
}
void on_trackbar5(int pos)
{
	minArea = pos;
	processImg();
}
void on_trackbar6(int pos)
{
	maxArea = pos;
	processImg();
}

void processImg(){
	SimpleBlobDetector::Params params;
	params.minThreshold = minThresholdBar;
	params.maxThreshold = maxThresholdBar;
	params.minRepeatability = minRepeatability;
	params.minDistBetweenBlobs = minDistBetweenBlobs;

	params.minConvexity = 0.3;
	params.minInertiaRatio = 0.01;

	params.minArea = minArea;
	params.maxArea = maxArea;
	params.filterByArea = true;
	SimpleBlobDetector detector(params);
	Mat img(g_pGrayImage0, 0);
	Mat out;
	vector<cv::KeyPoint> keyPoints;
	vector< vector <Point>> contours;
	vector< vector <Point>> approxContours;
	detector.create("SimpleBlob");

	for(;;){

		detector.detect( img, keyPoints );

		//detector.detectEx( img, keyPoints, contours );
		drawKeypoints( img, keyPoints, out, CV_RGB(255,0,0), DrawMatchesFlags::DEFAULT);
		approxContours.resize( contours.size() );
		for( int i = 0; i < contours.size(); ++i )
		{
			approxPolyDP( Mat(contours[i]), approxContours[i], 4, 1 );
			drawContours( out, contours, i, CV_RGB(rand()&255, rand()&255, rand()&255) );
			drawContours( out, approxContours, i, CV_RGB(rand()&255, rand()&255, rand()&255) );
		}
		cout << "Keypoints " << keyPoints.size() << endl;

		imshow( "detecting", out );
		waitKey(0);
	}


}


int main_sb( int argc, char** argv )
{	
	const char *pstrWindowsSrcTitle = "ORIG";
	const char *pstrWindowsToolBarName1 = "minThresholdBar";
	const char *pstrWindowsToolBarName2 = "maxThresholdBar";
	const char *pstrWindowsToolBarName3 = "minRepeatability";
	const char *pstrWindowsToolBarName4 = "minDistBetweenBlobs";
	const char *pstrWindowsToolBarName5 = "minArea";
	const char *pstrWindowsToolBarName6 = "maxArea";

	// 从文件中加载原图
	IplImage *pSrcImage = cvLoadImage("C:/apples.jpg", 0);

	// 转为灰度图
	//g_pGrayImage =  cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	g_pGrayImage0 = pSrcImage;
	//cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	// 创建二值图
	//g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	// 显示原图
	//cvNamedWindow(pstrWindowsBinaryTitle0, CV_WINDOW_AUTOSIZE);
	//cvShowImage(pstrWindowsBinaryTitle0, pSrcImage);
	// 创建二值图窗口
	cvNamedWindow(pstrWindowsBinaryTitle0, CV_WINDOW_AUTOSIZE);

	// 滑动条	

	int track1 = 0;
	int track2 = 255;
	int track3 = 3;
	int track4 = 4;
	int track5 = 5;
	int track6 = 6;

	cvCreateTrackbar(pstrWindowsToolBarName1, pstrWindowsBinaryTitle0, &track1, 254, ontrackbar1);
	cvCreateTrackbar(pstrWindowsToolBarName2, pstrWindowsBinaryTitle0, &track2, 254, ontrackbar2);
	//cvCreateTrackbar(pstrWindowsToolBarName3, pstrWindowsBinaryTitle0, &track3, 50, on_trackbar3);
	//cvCreateTrackbar(pstrWindowsToolBarName4, pstrWindowsBinaryTitle0, &track4, 500, on_trackbar4);
	//cvCreateTrackbar(pstrWindowsToolBarName5, pstrWindowsBinaryTitle0, &track5, 10000, on_trackbar5);
	//cvCreateTrackbar(pstrWindowsToolBarName6, pstrWindowsBinaryTitle0, &track6, 100000, on_trackbar6);

	//on_trackbar1(100);
	//on_trackbar2(120);
	//on_trackbar3(1);
	//on_trackbar4(10);
	//on_trackbar5(100);
	//on_trackbar6(90000);
	cvWaitKey(0);

	cvDestroyWindow(pstrWindowsBinaryTitle0);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&g_pGrayImage0);
	return 0;
}

int main2()
{
	const char      * wndName = "Source image",
		* wndNameGray = "Gray img", 
		* wndNameOut = "Out",
		* filename = "C:/apples.jpg";

	Mat src, gray, thresh, binary;
	Mat out;
	vector<KeyPoint> keyPoints;
	vector< vector <Point>> contours;
	vector< vector <Point>> approxContours;

	SimpleBlobDetector::Params params;
	params.minThreshold = 0;
	params.maxThreshold = 255;
	params.thresholdStep = 1;

	params.minArea = 100; 
	params.minConvexity = 0.3;
	params.minInertiaRatio = 0.01;

	params.maxArea = 8000;
	params.maxConvexity = 10;

	params.filterByColor = false;
	params.filterByCircularity = false;

	namedWindow( wndNameOut, CV_GUI_NORMAL );

	src = imread( filename, CV_LOAD_IMAGE_GRAYSCALE );
	line( src, Point(0, src.rows-1), Point( src.cols-1, src.rows-1 ), Scalar::all(255) );

	SimpleBlobDetector blobDetector( params );
	blobDetector.create("SimpleBlob");

	for(;;)
	{
		blobDetector.detect( src, keyPoints );
		//blobDetector.detectEx( src, keyPoints, contours );
		drawKeypoints( src, keyPoints, out, CV_RGB(0,255,0), DrawMatchesFlags::DEFAULT);
		approxContours.resize( contours.size() );

		for( int i = 0; i < contours.size(); ++i )
		{
			approxPolyDP( Mat(contours[i]), approxContours[i], 4, 1 );
			drawContours( out, contours, i, CV_RGB(rand()&255, rand()&255, rand()&255) );
			drawContours( out, approxContours, i, CV_RGB(rand()&255, rand()&255, rand()&255) );
		}
		cout << "Keypoints " << keyPoints.size() << endl;
		imshow( wndNameOut, out );
		cvWaitKey(0);
	}
}
