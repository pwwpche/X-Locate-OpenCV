
//图像的Canny边缘检测
#include <opencv2/opencv.hpp>
using namespace std;
#include "Match_MatchingHelper.h"

double whiteRatio2(IplImage* img, cv::Rect rect){
	//Get Information of the image
	int height = rect.height;
	int width = rect.width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	double whiteCount = 0;

	//Inverse color pixel by pixel
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

double nextWhite2(IplImage* img, cv::Rect rect){
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

double nextWhiteHorizonal2(IplImage* img, cv::Rect rect){
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

cv::Rect findLogoRect(IplImage *g_pSrcImage, string saveName){
	//从文件中载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图
	//
	IplImage* g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);
	cvSobel( g_pSrcImage, g_pCannyImg, 1, 0); 
	Mat mat(g_pCannyImg);
	cv::blur(mat, mat, Size(2,2));
	cvThreshold(g_pCannyImg, g_pCannyImg, 100, 255, 0);
	IplImage* original = cvCloneImage(g_pSrcImage);
	cvThreshold(original, original, 100, 255, 0);
	IplImage* ori = cvCloneImage(g_pSrcImage);

	vector<double> centerDistances;
	vector<cv::Rect> rects;
	int threshold = 7;
	int maxthreshold = 28;
	int direction = 10;


begin:
	while(rects.size() != 1){
		rects.clear();
		centerDistances.clear();
		IplImage* copy = cvCloneImage(g_pCannyImg);

		IplConvKernel* kernal = cvCreateStructuringElementEx(3,1, 1, 0, CV_SHAPE_RECT);
		cvDilate(copy, copy, kernal, threshold * 2 );//X方向膨胀连通数字
		cvErode(copy, copy, kernal, threshold * 4);//X方向膨胀连通数字
		cvDilate(copy, copy, kernal, threshold * 2);//X方向膨胀连通数字


		IplConvKernel* kernalY = cvCreateStructuringElementEx(1,3, 0, 1, CV_SHAPE_RECT);
		cvDilate(copy, copy, kernalY, 5);//X方向膨胀连通数字
		cvErode(copy, copy, kernalY, 5 * 2);//X方向膨胀连通数字
		cvDilate(copy, copy, kernalY, 5);//X方向膨胀连通数字


		CvMemStorage* storage = cvCreateMemStorage();
		CvSeq* contours;
		cvFindContours(copy, storage, &contours);

		while(contours != nullptr)
		{
			//if(
			//	(rect.x + rect.width > g_pSrcImage->width * 0.99)	||	//Rect is not at the center of the image
			//	(rect.x < g_pSrcImage->width * 0.01) ||
			//	(rect.y + rect.height > g_pSrcImage->height * 0.99) ||
			//	(rect.y < g_pSrcImage->height * 0.01))
			//{
			//	if(rect.width * rect.height < 30000){
			//		contours= contours->h_next;	
			//		continue;
			//	}
			//}

			//double next = 0;
			//int iterCount = 0;
			//int nextValid = 0;
			//int lastMove = 0;
			//cv::Rect rect0 = rect;		
			////Correction
			//rect.x = rect.x <= 0 ? 0 : rect.x;
			//rect.x = rect.x >= original->width - 1 ? original->width - 1 : rect.x;
			//rect.width = rect.width + rect.x > original->width  ? original->width  - rect.x  : rect.width;
			//rect.y = rect.y <= 0 ? 0 : rect.y;
			//rect.y = rect.y >= original->height - 1 ? original->height - 1 : rect.y;

			//while( nextValid < 4  && iterCount < 15){	

			//	rect.x = rect.x <= 0 ? 0 : rect.x;
			//	rect.x = rect.x >= original->width - 1 ? original->width - 1 : rect.x;
			//	rect.width = rect.width + rect.x > original->width  ? original->width  - rect.x  : rect.width;

			//	next = nextWhite2(original, rect);
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
			//if(next >= 0.01 || next <= -0.01){
			//	contours= contours->h_next;	
			//	continue;
			//}
			////Chech whether rectangle needs to expand horizonally
			//rect = rect0;
			//iterCount = 0;
			//nextValid = 0;
			//lastMove = 0;

			//while( nextValid < 4  && iterCount < 15){	
			//	next = nextWhiteHorizonal2(original, rect);
			//	iterCount++;
			//	if(next > 0.0){
			//		nextValid = 0;
			//		rect.height = rect.height + 5;
			//		rect0.height = rect.height;
			//		lastMove = 1;

			//	}else if(next < -0){
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



			//if(next >= 0.4 || next <= -0.4){
			//	contours= contours->h_next;	
			//	continue;
			//}
			cv::Rect rect = cvBoundingRect(contours);
			//if( rect.height < 20){
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
				if(	(rect.y + rect.height > g_pSrcImage->height * 0.85)){
					contours= contours->h_next;	
					continue;
				}

			}
			if(rect.width * rect.height < 5000){			//Area is too small
				contours= contours->h_next;	
				continue;
			}

			//double white = whiteRatio2(original, rect);
			//double white_threshold =  400.0 / (double)(rect.width * rect.height);
			//white_threshold = white_threshold > 0.6 ? 0.6 : white_threshold;
			//if(white <white_threshold ){		//Too much black area
			//	contours= contours->h_next;	
			//	continue;
			//}
			rects.push_back(rect);
			double distX = rect.x + rect.width / 2 - g_pSrcImage->width / 2;
			double distY = rect.y + rect.height / 2 - g_pSrcImage->height / 2;
			double distance = sqrt(distX * distX + distY * distY);
			//double distance = rect.y + rect.height / 2 - g_pSrcImage->height / 2 >= 0 ? rect.y+ rect.height / 2 - g_pSrcImage->height / 2 : g_pSrcImage->height / 2 - rect.y - rect.height / 2;
			centerDistances.push_back(distance);
			contours= contours->h_next;	
		}
		threshold += direction;
		if(threshold >= maxthreshold){
			break;
		}
	}
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
		if(min2Dist  - minDist < 80 && 
			min2Dist - minDist > -80 &&
			rects[index_min2].area() > rects[index].area() &&
			rects[index_min2].area() > 10000 &&
			rects[index].area() < 8000	)
		{
			index = index_min2;	
		}
	}


	for(unsigned i = 0 ; i < rects.size() ; i++)
	{
		cv::Rect rect = rects[i];
		//cvRectangle(original, cvPoint(rect.x * 0.7 ,  rect.y * 0.7), cvPoint(rect.x * 1.2 + rect.width, rect.y * 1.2 + rect.height), cvScalar(255, 255, 255, 0), 1, 8, 0);
		if(i != index){
			cvRectangle(ori, cvPoint(rect.x ,  rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), cvScalar(255, 255, 255, 0), 1, 8, 0);
		}else{
			cvRectangle(ori, cvPoint(rect.x ,  rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), cvScalar(255, 255, 255, 0), 3, 7, 0);
		}
	}

	//if(rects.size() != 1 && threshold == 17){
	//	threshold = 7;
	//	maxthreshold = 1;
	//	direction = -1;
	//	goto begin;
	//}
	cvSaveImage(saveName.c_str(), ori);
	return cv::Rect(0,0,0,0);
}



int main_ce(int argc, char** argv)
{
#if 0
	if( argc != 2 )
	{
		return -1; 
	}
	string path0 = argv[1];
#else
	string path0 = "C:/a.png";
#endif
	//Load Image
	IplImage *g_pSrcImage;

	unsigned found = path0.find_last_of("/\\");
	string filename = path0.substr(found+1);
	string saveName ="E:/Contour/" +  filename.insert(filename.find_last_of(".") , "");
	g_pSrcImage = cvLoadImage(path0.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	cv::Rect rect = findLogoRect(g_pSrcImage, saveName);
	//MatchingHelper valid;
	//if(rect.area() > 100){
	//	valid.cutImage(g_pSrcImage, rect, filename);
	//}else{
	//	valid.cutImage(g_pSrcImage, cvRect(0,0,g_pSrcImage->width, g_pSrcImage->height), filename);
	//}

	//cvShowImage("Contour", original);
	//cvShowImage("Canny", g_pCannyImg);
	//
	cvReleaseImage(&g_pSrcImage);
	return 0;
}