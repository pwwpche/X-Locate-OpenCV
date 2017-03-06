#include "Match_MatchingHelper.h"

cv::Rect MatchingHelper::getLogoRect(IplImage *g_pSrcImage){
	//从文件中载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图
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
			cv::Rect rect = cvBoundingRect(contours);
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

			rects.push_back(rect);
			double distX = rect.x + rect.width / 2 - g_pSrcImage->width / 2;
			double distY = rect.y + rect.height / 2 - g_pSrcImage->height / 2;
			double distance = sqrt(distX * distX + distY * distY);
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

	if(rects.size() == 0){
		index = 0;
		rects.push_back(cv::Rect(0,0,g_pSrcImage->width,g_pSrcImage->height));
	}
	return rects[index];
}

Mat MatchingHelper::getLogoMask(IplImage* img)
{
	IplImage* img_clone = cvCloneImage(img);
	Mat src(img_clone);

	int edgeThresh = 1;
	int const max_lowThreshold = 300;
	int ratio = 3;
	int kernel_size = 3;
	int const maxLen = 300;
	int const maxGaps = 20;
	int lowThreshold = 25;
	int minLen = 75;
	int minGaps = 4;
	Mat logoMask = Mat::zeros(src.size(), src.type() );

	//获取logo所在位置
	IplImage* img_logo =  cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,1);
	cvCvtColor(img, img_logo, CV_RGB2GRAY);
	cvCanny(img_logo, img_logo, 100, 100*3, kernel_size);
	Rect logoPosition = getLogoRect(img_logo);
	cvReleaseImage(&img_logo);
	double centerX = logoPosition.x + logoPosition.width / 2;		//logo中心位置
	double centerY = logoPosition.y + logoPosition.height / 2;

	Mat src_gray;
	cvtColor( src, src_gray, CV_BGR2GRAY );
	///Reduce noise with a 3x3 kernel
	blur( src_gray, src_gray, Size(3,3) );

	/// Canny detector
	Canny( src_gray, src_gray, lowThreshold, lowThreshold*ratio, kernel_size );

	IplImage* img_canny = new IplImage(src_gray);
	cvThreshold(img_canny, img_canny, 10, 255, 0);

	CvMemStorage *storage = cvCreateMemStorage();
	CvSeq *lines = 0;

	//获取图片中可能的轮廓线
	lines = cvHoughLines2(img_canny,storage,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,10,minLen,minGaps);		//Find verticle lines in the image
	vector<double> linesHor_k;
	vector<double> linesHor_b;
	vector<double> logoY;

	int posCount = 0;
	int negCount = 0;
	double posAvg = 0;
	double negAvg = 0;
	for(int i = 0;i<lines->total; i++){
		//计算当前直线的直线方程
		CvPoint *line = (CvPoint*)cvGetSeqElem(lines,i);
		double k = (double)(line[1].y - line[0].y) / (double)(line[1].x - line[0].x);
		double b = -k * line[0].x + line[0].y;

		//过滤竖线
		if(k > 1.5 || k < -1.5)
		{
			continue;
		}

		//剔除重合或直线方程相似的直线
		int linesCount  = linesHor_k.size();
		bool valid = true;
		for(int i = 0 ; i < linesCount ; i++){
			if(linesHor_k[i] - k < 0.07 && linesHor_k[i] - k > -0.07 ){
				if(linesHor_b[i] - b < 30 && linesHor_b[i] - b > -30){		//Find a similar line
					valid = false;
					break;
				}
			}
		}

		if(valid){
			linesHor_k.push_back(k);
			linesHor_b.push_back(b);
			if(k >= 0){
				posCount++;	//统计两个方向的轮廓线数量
				posAvg += k;
			}else{
				negCount++;
				negAvg -= k;
			}

		}else{
			continue;
		}
	}

	//计算每个方向轮廓线平均值的绝对值，绝对值越小，直线越平
	posAvg /= posCount;
	negAvg /= negCount;
	vector<double> res_k(linesHor_k);
	vector<double> res_b(linesHor_b);
	linesHor_k.clear();
	linesHor_b.clear();

	//判断应当留下哪一个方向的轮廓线
	bool isPos = (posCount > negCount && negCount <= 2 )||		
		(posCount > negCount && negCount >= 2 && posCount - negCount >= 2) ||
		(negCount >= 2 && posCount - negCount < 2 && posCount - negCount > -2 && posAvg < negAvg);

	//筛选出该方向的轮廓线
	for(int i = 0 ; i < res_k.size() ; i++){
		double k = res_k[i];
		double b = res_b[i];
		if( isPos )
		{
			if(k >= 0){
				linesHor_k.push_back(k);
				linesHor_b.push_back(b);
				logoY.push_back(k * centerX + b);
			}
		}else{
			if(k <= 0){
				linesHor_k.push_back(k);
				linesHor_b.push_back(b);
				logoY.push_back(k * centerX + b);	
			}
		}
	}



	//找到logo上方且最接近logo的一条直线
	vector<double> logoY_copy(logoY);
	sort(logoY_copy.begin(), logoY_copy.end());
	int lineAbove = -1;
	int lineBelow = -1;
	for(int i = 0 ; i < logoY_copy.size() ; i++){
		if(logoY_copy[i] > centerY - 15){
			if(i == 0){		//所有轮廓线都在logo下方
				break;
			}
			lineAbove = i - 1;
			break;
		}
	}
	if(lineAbove == -1 && logoY_copy.size() > 0){
		if(logoY_copy[logoY_copy.size() - 1] < centerY - 15){	//所有直线都在logo上方
			lineAbove = logoY_copy.size() - 1;
		}
	}
	if(lineAbove >= 0){
		for(int i = 0 ; i < logoY.size() ; i++){
			if(logoY[i] == logoY_copy[lineAbove]){		//在linesHor轮廓线数组中标记这条找到的轮廓线
				lineAbove = i;
				break;
			}
		}
	}
	//找到logo下方且最接近logo的一条直线
	for(int i =  logoY_copy.size() - 1 ; i >= 0 ; i--){
		if(logoY_copy[i] < centerY + 15){
			if(i == logoY_copy.size() - 1){
				break;
			}
			lineBelow = i + 1;
			break;
		}
	}
	if(lineBelow == -1 && logoY_copy.size() > 0){
		if(logoY_copy[0] > centerY - 15){
			lineBelow = 0;
		}
	}
	if(lineBelow >= 0){
		for(int i = 0 ; i < logoY.size() ; i++){
			if(logoY[i] == logoY_copy[lineBelow]){
				lineBelow = i;
				break;
			}
		}
	}

	//判断上下两条轮廓线是否均有效
	bool lineValid = true;
	if(lineAbove < 0 || lineBelow < 0){		//有至少一条无效
		lineValid = false;
	}else{		//两条都可能有效
		double kDist = linesHor_k[lineAbove] -  linesHor_k[lineBelow] ;
		double k0 = linesHor_k[lineAbove];
		double k1 = linesHor_k[lineBelow];
		double b0 = linesHor_b[lineAbove];
		double b1 = linesHor_b[lineBelow];
		double imgWidth = img->width;
		if( kDist > 0.3 || kDist < -0.3){		//两条线斜率差距过大
			lineValid = false;
		}
		if(( b0 - b1) * (imgWidth * k0 + b0 - imgWidth * k1 - b1) < 0 ) //两条线在图像中相交
		{
			lineValid = false;
		}
	}

	Rect resultRect;
	if( lineValid == false ){		//轮廓线无效，则选用找到的logo
		if(logoPosition.width >= img->width / 2 || logoPosition.height > img->height / 2 || logoPosition.area() > 15000){
			resultRect = logoPosition;
		}else{
			resultRect.x = 0;
			resultRect.y = 0;
			resultRect.width = img->width;
			resultRect.height = img->height;
		}
	}
	//设置logo的mask
	int step = logoMask.step;
	int channels = logoMask.channels();
	uchar* data = (uchar *)logoMask.data;
	if(lineValid){
		//从左到右逐列设置mask
		for(int i = 0 ; i < logoMask.cols ; i++){
			int above = linesHor_k[lineAbove] * i + linesHor_b[lineAbove];
			above = above < 0 ? 0 : above;
			above = above > img->height - 1 ?  img->height - 1 : above;
			int below = linesHor_k[lineBelow] * i + linesHor_b[lineBelow];
			below = below < 0 ? 0 : below;
			below = below > img->height - 1 ?  img->height - 1 : below;
			for(int j = above ; j < below ; j++){
				for(int k = 0 ; k < channels ; k++){
					data[j*step+i* channels + k] = 255;
				}
			}
		}
	}else{
		int left = resultRect.x;
		int right = resultRect.x + resultRect.width;
		int top = resultRect.y;
		int bottom = resultRect.y + resultRect.height;
		//从上到下逐行设置mask
		for(int i = top ; i < bottom ; i++){
			for(int j = left ; j < right ; j++){
				for(int k = 0 ; k < channels ; k++){
					data[i*step+j* channels + k] = 255;
				}
			}
		}
	}
	return logoMask;

}

Mat MatchingHelper::getMaskedScene(IplImage* img){
	Mat final, logoMask;
	Mat src(img);
	logoMask = getLogoMask(img);
	final = Mat::zeros(src.size(), src.type());
	src.copyTo(final, logoMask);
	return final;
}

Mat MatchingHelper::getMaskCannyScene(IplImage* img){
	Mat mask = getLogoMask(img);
	
	Mat src(img);
	Mat src_gray, edge;
	Mat final = Mat::zeros(src_gray.size(), src_gray.type());
	cvtColor( src, src_gray, CV_BGR2GRAY );
	blur( src_gray, edge, Size(3,3) );
	Canny( edge, edge, 100, 100*3, 3 );
	Mat dst;
	dst.create(src.size(), src.type());
	dst = Scalar::all(0);

	src.copyTo( dst, edge);
	dst.copyTo(final, mask);
	threshold(final, final, 10, 255, 0);
	return final;
}

Mat MatchingHelper::getMaskCannyLogo(IplImage* img){
	
	Mat src(img);
	Mat src_gray;
	cvtColor( src, src_gray, CV_BGR2GRAY );
	blur( src_gray, src_gray, Size(3,3) );
	/// Canny detector
	Canny( src_gray, src_gray, 100, 100*3, 3 );
	return src_gray;
}