#include "CVThreshold.h"

double checkValid::whiteRatio(IplImage* img, cv::Rect rect){
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

double checkValid::nextWhite(IplImage* img, cv::Rect rect){
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

double checkValid::nextWhiteHorizonal(IplImage* img, cv::Rect rect){
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


cv::Rect checkValid::findLogoRect(IplImage *g_pSrcImage){
	//从文件中载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图
	//
	IplImage* g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);
	cvSobel( g_pSrcImage, g_pCannyImg, 1, 0); 
	cvThreshold(g_pCannyImg, g_pCannyImg, 200, 255, 0);
	IplImage* original = cvCloneImage(g_pSrcImage);
	cvThreshold(original, original, 200, 255, 0);
	IplImage* ori = cvCloneImage(g_pSrcImage);

	vector<cv::Rect> rects;
	int threshold = 9;
	int maxthreshold = 17;
	int direction = 1;


begin:
	while(rects.size() != 1){
		rects.clear();
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
			cv::Rect rect = cvBoundingRect(contours);
			if(
				(rect.x + rect.width > g_pSrcImage->width * 0.99)	||	//Rect is not at the center of the image
				(rect.x < g_pSrcImage->width * 0.01) ||
				(rect.y + rect.height > g_pSrcImage->height * 0.99) ||
				(rect.y < g_pSrcImage->height * 0.01))
			{
				if(rect.width * rect.height < 30000){
					contours= contours->h_next;	
					continue;
				}
			}

			double next = 0;
			int iterCount = 0;
			int nextValid = 0;
			int lastMove = 0;
			cv::Rect rect0 = rect;		
			//Correction
			rect.x = rect.x <= 0 ? 0 : rect.x;
			rect.x = rect.x >= original->width - 1 ? original->width - 1 : rect.x;
			rect.width = rect.width + rect.x > original->width  ? original->width  - rect.x  : rect.width;
			rect.y = rect.y <= 0 ? 0 : rect.y;
			rect.y = rect.y >= original->height - 1 ? original->height - 1 : rect.y;

			while( nextValid < 4  && iterCount < 15){	

				rect.x = rect.x <= 0 ? 0 : rect.x;
				rect.x = rect.x >= original->width - 1 ? original->width - 1 : rect.x;
				rect.width = rect.width + rect.x > original->width  ? original->width  - rect.x  : rect.width;

				next = nextWhite(original, rect);
				iterCount++;
				if(next > 0.03 && rect.x >= 0 && rect.x + rect.width < original->width){
					nextValid = 0;
					rect.width = rect.width + 15;
					rect0.width = rect.width;
					lastMove = 1;
				}else if(next < -0.03 && rect.x >= 0 && rect.x + rect.width < original->width){
					nextValid = 0;
					rect.x = rect.x - 15;
					rect.width = rect.width + 15;
					rect0.x = rect.x;
					rect0.width = rect.width;
					lastMove = -1;
				}else{

					if(lastMove == 1){
						rect.width = rect.width + 10;
						lastMove = -1;
					}else if (lastMove == -1){
						rect.x = rect.x - 10;
						rect.width = rect.width + 10;
						lastMove = 1;
					}
					nextValid++;
				}
			}
			if(next >= 0.01 || next <= -0.01){
				contours= contours->h_next;	
				continue;
			}
			//Chech whether rectangle needs to expand horizonally
			rect = rect0;
			iterCount = 0;
			nextValid = 0;
			lastMove = 0;

			while( nextValid < 4  && iterCount < 15){	
				next = nextWhiteHorizonal(original, rect);
				iterCount++;
				if(next > 0.0){
					nextValid = 0;
					rect.height = rect.height + 5;
					rect0.height = rect.height;
					lastMove = 1;

				}else if(next < -0){
					nextValid = 0;
					rect.y = rect.y - 5;
					rect.height = rect.height + 5;
					rect0.y = rect.y;
					rect0.height = rect.height;
					lastMove = -1;
				}else{
					if(lastMove == 1){
						rect.height = rect.height + 10;
						lastMove = -1;
					}else if (lastMove == -1){
						rect.y = rect.y - 10;
						rect.height = rect.height + 10;
						lastMove = 1;
					}
					nextValid++;
				}
			}
			rect = rect0;



			//if(next >= 0.4 || next <= -0.4){
			//	contours= contours->h_next;	
			//	continue;
			//}

			if(
				(rect.x + rect.width > g_pSrcImage->width * 0.96)	||	//Rect is not at the center of the image
				(rect.x < g_pSrcImage->width * 0.04) ||
				(rect.y + rect.height > g_pSrcImage->height * 0.96) ||
				(rect.y < g_pSrcImage->height * 0.04))
			{
				if(rect.width < 200){
					contours= contours->h_next;	
					continue;
				}
				if(	(rect.y + rect.height > g_pSrcImage->height * 0.95)){
					contours= contours->h_next;	
					continue;
				}

			}
			if(rect.width * rect.height < 1200 || rect.height / rect.width > 1.5){			//Area is too small
				contours= contours->h_next;	
				continue;
			}

			double white = whiteRatio(original, rect);
			double white_threshold =  400.0 / (double)(rect.width * rect.height);
			white_threshold = white_threshold > 0.6 ? 0.6 : white_threshold;
			if(white <white_threshold ){		//Too much black area
				contours= contours->h_next;	
				continue;
			}
			rects.push_back(rect);
			contours= contours->h_next;	
		}
		threshold += direction;
		if(threshold == maxthreshold){
			break;
		}
	}

	if(rects.size() == 1){
		return rects[0];
	}

	if(rects.size() != 1 && threshold == 17){
		threshold = 7;
		maxthreshold = 3;
		direction = -1;
		goto begin;
	}
	return cv::Rect(0,0,0,0);
}


double checkValid::brightness(IplImage* img, int threshold){
		//Get Information of the image
	int height = img->height;
	int width = img->width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	double whiteCount = 0;

	//Inverse color pixel by pixel
	for(unsigned i = 0 ; i < height ; i++){
		for(int j = 0 ; j < width ; j++) {
			if(channels == 3){
				if( data[i*step+j*channels] > threshold && 
					data[i*step+j*channels + 1] > threshold && 
					data[i*step+j*channels + 2] > threshold)
				{
					whiteCount++;
				}
			}else{
				if( data[i*step+j*channels] > threshold){
					whiteCount++;
				}
			}
		}
	}
	double area = height * width;
	double result = whiteCount / area;
	return result;
}

double checkValid::brightness(Mat img, int threshold){
	if(!img.data){
		return 0;
	}

	IplImage img0(img);
	double result = brightness(&img0, threshold);
	return result;
}

