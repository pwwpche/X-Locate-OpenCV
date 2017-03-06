
//图像的Canny边缘检测
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


IplImage *g_pSrcImage, *g_pCannyImg;
const char *pstrWindowsCannyTitle = "Edge";
int binThreshold = 100;
double whiteRatio(IplImage* img, cv::Rect rect){
	//Get Information of the image
	int height = rect.height;
	int width = rect.width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	double whiteCount = 0;

	//Count the number of white pixels. 
	for(unsigned i = rect.y ; i < rect.y + height ; i++){
		for(int j=rect.x ; j < rect.x + width ; j++) {
			if(data[i*step+j*channels+1] > 200){
				whiteCount++;
			}
		}
	}
	double area = height * width;
	double result = whiteCount / area;
	return result;

}

double nextWhite(IplImage* img, cv::Rect rect){
			rect.x = rect.x <= 0 ? 0 : rect.x;
		rect.x = rect.x >= img->width - 1 ? img->width - 1 : rect.x;
		rect.y = rect.y <= 0 ? 0 : rect.y;
		rect.y = rect.y >= img->height - 1 ? img->height - 1 : rect.y;

	int height = rect.height;
	int width = rect.width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	double whiteCount = 0;

	//Check the right edge
	for(unsigned i = rect.y ; i < rect.y + height ; i++){
		if(data[i*step+(rect.x + width)*channels+1] > 200){
			whiteCount++;
		}
	}
	double result1 = whiteCount / height;

	whiteCount = 0;
	//Check the left edge
	for(unsigned i = rect.y ; i < rect.y + height ; i++){
		if(data[i*step+(rect.x)*channels+1] > 200){
			whiteCount++;
		}
	}
	double result2 = -whiteCount / height;

	return result1 > -result2 ? result1 : result2;
}

double nextWhiteHorizonal(IplImage* img, cv::Rect rect){
	int height = rect.height;
	int width = rect.width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	double whiteCount = 0;

	//Check the upper edge
	for(unsigned i = 0 ; i < width ; i++){
		if((rect.x + i) >= img->width - 1)
			break;
		if(data[(rect.y)*step+(rect.x + i)*channels+1] > 200){
			whiteCount++;
		}
	}
	double result2 = -whiteCount / width;

	whiteCount = 0;
	//Check the lower edge
	for(unsigned i = 0 ; i < width; i++){
		if((rect.y + height) >= img->height - 1)
			break;
		if(data[(rect.y + height)*step+(rect.x + i)*channels+1] > 200){
			whiteCount++;
		}
	}
	double result1 = whiteCount / width;

	return result1 > -result2 ? result1 : result2;
}


void inverseColor(IplImage* img){
	//Get Information of the image
	int height = img->height;
	int width = img->width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	//Inverse color pixel by pixel
	for(int i = 0 ; i < height ; i++){
		for(int j=0 ; j < width ; j++) {
			for(int k = 0 ; k < channels ; k++){
				data[i*step+j*channels+k]=255-data[i*step+j*channels+k];    
			}
		}
	}

	//Save image
	//cvSaveImage("C:\\inverse.jpg", img);
}


void on_trackbar_c(int threshold)
{
	//canny边缘检测
	//cvCanny(g_pSrcImage, g_pCannyImg, 250, 255, 3);
	//cvSobel( g_pSrcImage, g_pCannyImg, 2, 0); 
	
	cvSobel( g_pSrcImage, g_pCannyImg, 2, 0); 
	Mat mat(g_pCannyImg);
	cv::blur(mat, mat, Size(2,2));
	
	//cvCanny(g_pCannyImg, g_pCannyImg, 300, 300, 3);
	cvThreshold(g_pCannyImg, g_pCannyImg, binThreshold, 255, 0);
	cvShowImage("bin",g_pCannyImg );
	


	IplConvKernel* kernal = cvCreateStructuringElementEx(3,1, 1, 0, CV_SHAPE_RECT);
	cvDilate(g_pCannyImg, g_pCannyImg, kernal, threshold * 2 );//X方向膨胀连通数字
	cvErode(g_pCannyImg, g_pCannyImg, kernal, threshold * 4);//X方向膨胀连通数字
	cvDilate(g_pCannyImg, g_pCannyImg, kernal, threshold * 2 );//X方向膨胀连通数字


	IplConvKernel* kernalY = cvCreateStructuringElementEx(1,3, 0, 1, CV_SHAPE_RECT);
	cvDilate(g_pCannyImg, g_pCannyImg, kernalY, 5);//X方向膨胀连通数字
	cvErode(g_pCannyImg, g_pCannyImg, kernalY, 5 * 2);//X方向膨胀连通数字
	cvDilate(g_pCannyImg, g_pCannyImg, kernalY, 5);//X方向膨胀连通数字

	IplImage* copy = cvCloneImage(g_pCannyImg);

	IplImage* original = cvCloneImage(g_pSrcImage);
	cvThreshold(original, original, binThreshold, 255, 0);
	IplImage* ori_bin = cvCloneImage(g_pSrcImage);
	cvThreshold(ori_bin, ori_bin, binThreshold, 255, 0);

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* contours;
	cvFindContours(copy, storage, &contours);
	vector<cv::Rect> rects;
	vector<double> centerDistances;
	//while(contours != nullptr)
	//{
	//	rects.push_back(cvBoundingRect(contours));//list<CvRect> rects
	//	contours= contours->h_next;
	//}

	//for(int i = 0 ; i < rects.size() ; i++)
	//{
	//	cv::Rect rect = rects[i];
	//	cvRectangle(original, cvPoint(rect.x,  rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), cvScalar(255, 255, 255, 0), 1, 8, 0);
	//}

	while(contours != nullptr)
	{
		cv::Rect rect = cvBoundingRect(contours);
		//if( rect.height < 30){
		//	contours= contours->h_next;	
		//	continue;
		//}
		if(rect.y + rect.height / 2> g_pSrcImage->height * 2 / 3 ){
			contours= contours->h_next;	
			continue;
		}
		if((rect.x < g_pSrcImage->width / 4 || rect.x + rect.width > g_pSrcImage->width * 3 / 4) && 
			rect.width < g_pSrcImage->width / 3 )
		{
			contours= contours->h_next;	
			continue;
		}
		double next = 0;
		int iterCount = 0;
		int nextValid = 0;
		int lastMove = 0;
		IplImage* ori2;
		cv::Rect rect0 = rect;		

		//Correction
		rect.x = rect.x <= 0 ? 0 : rect.x;
		rect.x = rect.x >= original->width - 1 ? original->width - 1 : rect.x;
		rect.width = rect.width + rect.x > original->width  ? original->width  - rect.x  : rect.width;
		rect.y = rect.y <= 0 ? 0 : rect.y;
		rect.y = rect.y >= original->height - 1 ? original->height - 1 : rect.y;

		//while( nextValid < 4  && iterCount < 15){	
		//	rect.x = rect.x <= 0 ? 0 : rect.x;
		//	rect.x = rect.x >= original->width - 1 ? original->width - 1 : rect.x;
		//	rect.width = rect.width + rect.x > original->width  ? original->width  - rect.x  : rect.width;
		//	next = nextWhite(original, rect);
		//	iterCount++;
		//	if(next > 0.03 && rect.x >= 0 && rect.x + rect.width < original->width){
		//		nextValid = 0;
		//		rect.width = rect.width + 15;
		//		rect0.width = rect.width;
		//		lastMove = 1;
		//	}else if(next < -0.03 && rect.x >= 0 && rect.x + rect.width < original->width){
		//		nextValid = 0;
		//		rect.x = rect.x - 15;
		//		rect.width = rect.width + 15;
		//		rect0.x = rect.x;
		//		rect0.width = rect.width;
		//		lastMove = -1;
		//	}else{

		//		if(lastMove == 1){
		//			rect.width = rect.width + 10;
		//			lastMove = -1;
		//		}else if (lastMove == -1){
		//			rect.x = rect.x - 10;
		//			rect.width = rect.width + 10;
		//			lastMove = 1;
		//		}
		//		nextValid++;
		//	}
		//}
		////if(next >= 0.01 || next <= -0.01){
		////	contours= contours->h_next;	
		////	continue;
		////}

		//	if(next >= 0.01 || next <= -0.01){
		//		contours= contours->h_next;	
		//		continue;
		//	}
		////Chech whether rectangle needs to expand horizonally
		//rect = rect0;
		//iterCount = 0;
		//nextValid = 0;
		//lastMove = 0;

		//while( nextValid < 4  && iterCount < 15){	
		//	if(rect.x < 0 || rect.x >= original->width || rect.y <= 0 || rect.y >= original->height)
		//	{
		//		break;
		//	}
		//	next = nextWhiteHorizonal(original, rect);
		//	iterCount++;
		//	if(next > 0.0){
		//		nextValid = 0;
		//		rect.height = rect.height + 5;
		//		rect0.height = rect.height;
		//		lastMove = 1;

		//	}else if(next < -0.0){
		//		nextValid = 0;
		//		rect.y = rect.y - 5;
		//		rect.height = rect.height + 5;
		//		rect0.y = rect.y;
		//		rect0.height = rect.height;
		//		lastMove = -1;
		//	}else{
		//		if(lastMove == 1){
		//			rect.height = rect.height + 10;
		//			lastMove = -1;
		//		}else if (lastMove == -1){
		//			rect.y = rect.y - 10;
		//			rect.height = rect.height + 10;
		//			lastMove = 1;
		//		}
		//		nextValid++;
		//	}
		//}
		//rect = rect0;
		////if(next >= 0.4 || next <= -0.4){
		////	contours= contours->h_next;	
		////	continue;
		////}

	if(
				(rect.x + rect.width > g_pSrcImage->width * 0.85)	||	//Rect is not at the center of the image
				(rect.x < g_pSrcImage->width * 0.04) ||
				(rect.y + rect.height > g_pSrcImage->height * 0.85) ||
				(rect.y < g_pSrcImage->height * 0.04))
			{
				if(rect.width < 150){
					contours= contours->h_next;	
					continue;
				}
				if(rect.y < g_pSrcImage->height * 0.05){
					contours= contours->h_next;	
					continue;
				}
			}
			if(rect.width * rect.height < 2000){			//Area is too small
				contours= contours->h_next;	
				continue;
			}

		//double white = whiteRatio(original, rect);
		//double white_threshold =  400.0 / (double)(rect.width * rect.height);
		//white_threshold = white_threshold > 0.6 ? 0.6 : white_threshold;
		//if(white <white_threshold ){		//Too much black area
		//	contours= contours->h_next;	
		//	continue;
		//}
		rects.push_back(rect);
		double distX = rect.x + rect.width / 2 - g_pSrcImage->width / 2;
		double distY = rect.y + rect.height / 2 - g_pSrcImage->height / 2;
		distX = distX > 0 ? distX : -distX;
		distY = distY > 0 ? distY : -distY;
		//double distance = sqrt(distX * distX + distY * distY);
		//double distance = rect.y + rect.height / 2 - g_pSrcImage->height / 2 >= 0 ? rect.y+ rect.height / 2 - g_pSrcImage->height / 2 : g_pSrcImage->height / 2 - rect.y - rect.height / 2;
		double distance = distX + distY * g_pSrcImage->width * 2 / g_pSrcImage->height;
		centerDistances.push_back(distance);
		contours= contours->h_next;	
	}
	////Combine two colliding rectangles
	//if(rects.size() == 2){

	//	//if(rects[0].x + rects[0].width > rects[1].x && 
	//	//	rects[0].y + rects[0].height > rects[1].y){
	//	int rect_x = rects[0].x < rects[1].x ? rects[0].x : rects[1].x;
	//	int rect_y = rects[0].y > rects[1].y ? rects[0].y : rects[1].y;
	//			cv::Rect rect = cv::Rect(rects[0].x, rects[0].y, 
	//				rects[1].x - rects[0].x + rects[1].width, rects[1].y - rects[0].y + rects[1].height);
	//							rects.clear();
	//			rects.push_back(rect);
	//	//}
	//}
////Find the rectangle closest to center
//	double minDist = 100000;
//	int index = 0;
//	for(unsigned i = 0 ; i < rects.size() ; i++){
//		if(centerDistances[i] <  minDist){
//			minDist = centerDistances[i];
//			index = i;
//		}
//	}
		//Find the rectangle closest to center
	double minDist = 100000;
	double min2Dist = 100000;
	int index = 0;
	int index_min2 = 0;
	vector<double> dist_sort(centerDistances);
	sort(dist_sort.begin(), dist_sort.end());
	minDist = dist_sort.size() > 0 ? dist_sort[0] : minDist;
	min2Dist = dist_sort.size() > 1 ? dist_sort[1] : min2Dist;
	for(unsigned i = 0 ; i < rects.size() ; i++){
		if(centerDistances[i] ==  minDist){
			minDist = centerDistances[i];
			index = i;
		}
		if(centerDistances[i] ==  min2Dist){
			min2Dist = centerDistances[i];
			index_min2 = i;
		}
	}
	
	if(rects.size() > 1){
		min2Dist = rects[index_min2].y + rects[index_min2].height / 2;
		//minDist =  rects[index].y + rects[index].height / 2;
		minDist = g_pSrcImage->height / 2;
		if(min2Dist  - minDist < 120 && min2Dist - minDist > -120 && rects[index_min2].area() > 30000 && rects[index].area() < 10000){
			index = index_min2;	
		}
	}

	for(unsigned i = 0 ; i < rects.size() ; i++)
	{
		cv::Rect rect = rects[i];
		//cvRectangle(original, cvPoint(rect.x * 0.7 ,  rect.y * 0.7), cvPoint(rect.x * 1.2 + rect.width, rect.y * 1.2 + rect.height), cvScalar(255, 255, 255, 0), 1, 8, 0);
		if(i != index){
			cvRectangle(original, cvPoint(rect.x ,  rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), cvScalar(255, 255, 255, 0), 1, 8, 0);
		}else{
			cvRectangle(original, cvPoint(rect.x ,  rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), cvScalar(255, 255, 255, 0), 3, 7, 0);
		}
	}

	cvShowImage("Edge", original);
	cvShowImage("Canny", g_pCannyImg);
}

int main_c(int argc, char** argv)
{
	//const char *pstrImageName = "E:/Metro_Photos_Compressed/metro/baodao2.jpg";
	const char *pstrImageName = "E:/Metro_Photos_Compressed/Metro0/burger king2.jpg";
	const char *pstrWindowsSrcTitle = "Orig";
	const char *pstrWindowsToolBar = "ErodeThreshold";

	//从文件中载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图
	g_pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat imgMat(g_pSrcImage, 0);
	//cvSmooth(g_pSrcImage,g_pSrcImage,CV_MEDIAN,3,g_pSrcImage->nChannels);
	//IplConvKernel* kernal = cvCreateStructuringElementEx(10,1, 1, 0, CV_SHAPE_RECT);
	//cvDilate(g_pSrcImage, g_pSrcImage, kernal, 2);//X方向膨胀连通数字
	//cvErode(g_pSrcImage, g_pSrcImage, kernal, 4);//X方向腐蚀去除碎片
	//cvDilate(g_pSrcImage, g_pSrcImage, kernal, 2);//X方向膨胀回复形态
	// cv::Mat kernel(3,3,CV_32F,cv::Scalar(0));
	//    kernel.at<float>(1,1) = 5.0;
	//    kernel.at<float>(0,1) = -1.0;
	//    kernel.at<float>(1,0) = -1.0;
	//    kernel.at<float>(1,2) = -1.0;
	//    kernel.at<float>(2,1) = -1.0;
	//cv::Mat result;
	//    result.create(imgMat.size(),imgMat.type());
	//    
	//    //对图像进行滤波
	//    cv::filter2D(imgMat,imgMat,imgMat.depth(),kernel);

	g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);

	//创建窗口
	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);
	//创建滑动条
	int nThresholdEdge = 7;
	cvCreateTrackbar("ErodeThreshold", pstrWindowsCannyTitle, &nThresholdEdge, 100, on_trackbar_c);
	cvCreateTrackbar("binaryThreshold", pstrWindowsCannyTitle, &binThreshold, 300, on_trackbar_c);

	//在指定窗口中显示图像
	//cvShowImage(pstrWindowsSrcTitle, g_pSrcImage);
	on_trackbar_c(10);

	//等待按键事件
	cvWaitKey();

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsCannyTitle);
	cvReleaseImage(&g_pSrcImage);
	cvReleaseImage(&g_pCannyImg);
	return 0;
}