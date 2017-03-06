#include "CVThreshold.h"
using namespace cv;
void checkValid::sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;
	std::vector<cv::Point2f> left, right;

	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].x < center.x)
			left.push_back(corners[i]);
		else
			right.push_back(corners[i]);
	}

	cv::Point2f tl, tr, bl, br;
	if(left.size() == 2){
		tl = left[0].y < left[1].y ? left[0] : left[1];
		bl = left[0].y < left[1].y ? left[1] : left[0];
		tr = right[0].y < right[1].y ? right[0] : right[1];
		br = right[0].y < right[1].y ? right[1] : right[0];
		corners.clear();
		corners.push_back(tl);
		corners.push_back(tr);
		corners.push_back(br);
		corners.push_back(bl);

	}else if(top.size() == 2) {
		tl = top[0].x > top[1].x ? top[1] : top[0];
		tr = top[0].x > top[1].x ? top[0] : top[1];
		bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
		br = bot[0].x > bot[1].x ? bot[0] : bot[1];
		corners.clear();
		corners.push_back(tl);
		corners.push_back(tr);
		corners.push_back(br);
		corners.push_back(bl);
	}
	return ;
}

cv::Mat checkValid::perspectiveWrap(Mat img, vector<cv::Point2f> corners){
	std::vector<cv::Point2f> src(4);
	std::vector<cv::Point2f> dst(4);

	for(int i=0; i<4; i++)
	{
		src[i] = corners[i];
	}

	cv::Point2f center;
	for (unsigned i = 0; i < corners.size(); i++)
		center += corners[i];
	center *= (1. / corners.size());
	sortCorners(corners, center);
	double area = cv::contourArea(corners);

	float edge1 = sqrt((corners[1].x - corners[0].x) * (corners[1].x - corners[0].x) + 
		(corners[1].y - corners[0].y) * (corners[1].y - corners[0].y));
	float edge2 = sqrt((corners[2].x - corners[1].x) * (corners[2].x - corners[1].x) + 
		(corners[2].y - corners[1].y) * (corners[2].y - corners[1].y));
	float edge3 = sqrt((corners[3].x - corners[2].x) * (corners[3].x - corners[2].x) + 
		(corners[3].y - corners[2].y) * (corners[3].y - corners[2].y));
	float edge4 = sqrt((corners[0].x - corners[3].x) * (corners[0].x - corners[3].x) + 
		(corners[0].y - corners[3].y) * (corners[0].y - corners[3].y));


	float dst_height, dst_width;
	dst_width = (edge1 + edge3) / 2;
	dst_height = (edge2 + edge4) / 2;


	//dst赋值，从零点开始顺时针
	dst[0].x = corners[0].x;
	dst[0].y = corners[0].y;
	dst[1].x = corners[0].x + dst_width-1;
	dst[1].y = corners[0].y;
	dst[2].x = corners[0].x + dst_width-1;
	dst[2].y = corners[0].y + dst_height-1;
	dst[3].x = corners[0].x;
	dst[3].y = corners[0].y + dst_height-1;

	//通过4个点得到变换矩阵,然后进行变换
	cv::Mat warpMat = cv::getPerspectiveTransform(corners, dst);
	return warpMat;
}

IplImage* checkValid::getWarppedImg(IplImage* srcRGB){
	IplImage *src = cvCreateImage(cvSize(srcRGB->width,srcRGB->height),IPL_DEPTH_8U,1);
	cvCvtColor(srcRGB,src,CV_RGB2GRAY);		//Set image color to GRAY
	vector<cv::Point2f> corners = getOutlinePoints(srcRGB);
	cv::Mat img(src, 0);
	cv::Mat dstImg = img;
	cv::Mat warpMat = perspectiveWrap(img, corners);
	cv::warpPerspective(img, dstImg, warpMat, img.size());

	cv::Mat img_RGB(srcRGB, 0);
	cv::Mat img_RGB_w = img_RGB;
	cv::warpPerspective(img_RGB, img_RGB_w, warpMat, img_RGB.size());
	cv::imwrite("C:/img_RGB.jpg", img_RGB_w);
	IplImage* ret =new IplImage(dstImg);
	return ret;
}

IplImage* checkValid::cutAndWarp(IplImage *g_pSrcImage){
	cv::Rect roi = findLogoRect(g_pSrcImage);

	if(roi.area() == 0){
		roi.height = g_pSrcImage->height;
		roi.width = g_pSrcImage->width;
	}
	cvSetImageROI(g_pSrcImage, roi);
	IplImage* warpLogo = cvCreateImage( cvSize(roi.width, roi.height), g_pSrcImage->depth, g_pSrcImage->nChannels );
	cv:cvCopy(g_pSrcImage, warpLogo);
	cvResetImageROI(g_pSrcImage);
	////IplImage *warppedLogo = cvCreateImage(cvSize(roi.width,roi.height),IPL_DEPTH_8U,1);
	////cvCvtColor(warpLogo,warppedLogo,CV_RGB2GRAY);		//Set image color to GRAY
	//vector<cv::Point2f> corners = getOutlinePoints(g_pSrcImage);
	//cv::Mat img(warpLogo, 0);
	//cv::Mat dstImg = warpLogo;
	//cv::Mat warpMat = perspectiveWrap(g_pSrcImage, corners);
	//cv::warpPerspective(img, dstImg, warpMat, img.size());
	//IplImage* ret =new IplImage(dstImg);
	//return ret;
	return warpLogo;

}