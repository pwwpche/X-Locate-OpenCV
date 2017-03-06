////图像的二值化
//#include <opencv2/opencv.hpp>
//using namespace std;
//using namespace cv;
////#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
//
//class BinarizeWindow{
//public:
//	BinarizeWindow(){
//		IplImage *g_pGrayImage = NULL;
//		const char *pstrWindowsBinaryTitle = "Bin";
//		Mat imgBin_bw;
//	}
//
//	void mouse_callback_bw(int event, int x, int y, int flags, void* param){
//		switch(event)
//		{
//		case CV_EVENT_LBUTTONDOWN:
//			{
//				cvtColor(imgBin_bw, imgBin_bw, CV_GRAY2BGR);
//				imshow("before floodfill", imgBin_bw);
//				//floodFill(imgBin_bw, Point(x, y), cv::Scalar(255,255,0, 10));
//				//imshow("floodfill", imgBin_bw);
//				imwrite("C:/img.jpg", imgBin_bw);
//			}
//		}
//		return ;
//	}
//
//	void on_trackbar_b1(int pos)
//	{
//		// 转为二值图
//		IplImage* img2 = cvCloneImage(g_pGrayImage);
//		//IplConvKernel* kernal = cvCreateStructuringElementEx(2,2, 1, 0, CV_SHAPE_RECT);
//		//cvDilate(img2, img2, kernal, 1);//X方向膨胀连通数字
//		//cvErode(img2, img2, kernal, 1);//X方向腐蚀去除碎片
//		//cvDilate(img2, img2, kernal, 1);//X方向膨胀回复形态
//		//IplConvKernel* kernal_y = cvCreateStructuringElementEx(3,1, 1, 0, CV_SHAPE_RECT);
//
//		Mat img(img2, 0);
//		imshow("before", img);
//
//		//cvThreshold(g_pGrayImage, g_pBinaryImage, minThreshold_b, maxThreshold_b, CV_THRESH_BINARY);
//		threshold(img, img, pos, 255, CV_THRESH_BINARY);
//
//		// 显示二值图
//		//cvShowImage(pstrWindowsBinaryTitle, g_pBinaryImage);
//		//for (int i=0; i< img.rows; i++)
//		//{ 
//		//	for (int j=0; j< img.cols; j++)
//		//	{
//		//		int editValue=img.at<uchar>(i,j);
//
//		//		if((editValue>pos)&&(editValue<255)) //check whether value is within range.
//		//		{
//		//			img.at<uchar>(i,j)=255;
//		//		}
//		//		else
//		//		{
//		//			img.at<uchar>(i,j)=0;
//		//		}
//		//	}
//		//}
//		//blur(img, img, Size(1,1));
//		cv::imshow("min", img);
//		img.copyTo(imgBin_bw);
//		cvWaitKey(0);
//	}
//
//	void on_trackbar_b2(int pos)
//	{
//		// 转为二值图
//		IplImage* img2 = cvCloneImage(g_pGrayImage);
//		Mat img(img2, 0);
//		//cvThreshold(g_pGrayImage, g_pBinaryImage, minThreshold_b, maxThreshold_b, CV_THRESH_BINARY);
//
//
//		// 显示二值图
//		//cvShowImage(pstrWindowsBinaryTitle, g_pBinaryImage);
//		for (int i=0; i< img.rows; i++)
//		{ 
//			for (int j=0; j< img.cols; j++)
//			{
//				int editValue=img.at<uchar>(i,j);
//
//				if((editValue>pos)&&(editValue<255)) //check whether value is within range.
//				{
//					img.at<uchar>(i,j)=255;
//				}
//				else
//				{
//					img.at<uchar>(i,j)=0;
//				}
//			}
//		}
//		cv::imshow("min", img);
//		cvWaitKey(0);
//	}
//
//	int execute(string imgPath)
//	{	
//		const char *pstrWindowsSrcTitle = "ORIG";
//		const char *pstrWindowsToolBarName = "BIN PARAMETER";
//		const char *pstrWindowsToolBarName2 = "BIN PARAMETER2";
//		// 从文件中加载原图
//		IplImage *pSrcImage = cvLoadImage(imgPath.c_str(), 0);
//
//		// 转为灰度图
//		//g_pGrayImage =  cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
//		g_pGrayImage = pSrcImage;
//		//cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);
//
//
//		int minThreshold_b = 1;
//		int maxThreshold_b = 255;
//
//		// 显示原图
//		namedWindow(pstrWindowsBinaryTitle, CV_WINDOW_AUTOSIZE);
//		cvShowImage(pstrWindowsBinaryTitle, pSrcImage);
//		cv::setMouseCallback(pstrWindowsBinaryTitle, mouse_callback_bw);
//		// 滑动条	
//		cvCreateTrackbar(pstrWindowsToolBarName, pstrWindowsBinaryTitle, &minThreshold_b, 254, on_trackbar_b1);
//		cvCreateTrackbar(pstrWindowsToolBarName2, pstrWindowsBinaryTitle, &maxThreshold_b, 254, on_trackbar_b2);
//
//		on_trackbar_b1(110);
//		cvWaitKey(0);
//
//		cvDestroyWindow(pstrWindowsSrcTitle);
//		cvDestroyWindow(pstrWindowsBinaryTitle);
//		cvReleaseImage(&pSrcImage);
//		cvReleaseImage(&g_pGrayImage);
//		return 0;
//	}
//private:
//	IplImage *g_pGrayImage;
//	const char *pstrWindowsBinaryTitle;
//	Mat imgBin_bw;
//};
//
