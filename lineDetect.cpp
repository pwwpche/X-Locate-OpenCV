#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <limits>
#include <fstream>
using namespace std;
using namespace cv;
#define MAX_K 10
#define MIN_K 1.2
#define DEBUG_ONE1

int findVerticleLines(CvSeq *lines,int height,  int width);
int findHorizonalLines(CvSeq *lines, int height, int width);

vector<cv::Point2f> getOutlinePoints(IplImage *grayImg,string path0);
cv::Mat perspectiveWrap(cv::Mat img, vector<cv::Point2f> corners);
void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);

int main_ld(int argc, char** argv)
{
#ifdef DEBUG_ONE 
	string path0 = "C:/Metro_refined_640/StarBucks4.jpg";
	string saveName = "C:/Hough.jpg";
	string wrapName = "C:/Hough_wrap.jpg";
	ofstream create("C:/Hough.txt" ,ios_base::out|ios::app);
	create.close();
	ofstream out("C:/Hough.txt"  ,ios_base::out);
#else
	if( argc != 2 )
	{
		return -1; 
	}
	//Load Image
	string path0 = argv[1];
	unsigned found = path0.find_last_of("/\\");
	string filename = path0.substr(found+1);
	string saveName ="E:/Hough/" +  filename.insert(filename.find_last_of(".") , "_hough");
	string wrapName ="E:/Hough/" +  filename.insert(filename.find_last_of(".") , "_wrap");
	string origWrapName ="E:/Hough/_orig_" +  filename.insert(filename.find_last_of(".") , "_wrap");
	//Create Output log file
	string txtName = saveName.substr(0, saveName.find_last_of(".")) + ".txt";
	ofstream create(txtName ,ios_base::out|ios::app);
	create.close();
	ofstream out(txtName ,ios_base::out);

#endif


	//Detect the edge of the image using Canny
	IplImage *srcRGB = cvLoadImage(path0.c_str());
	cvSmooth(srcRGB,srcRGB,CV_MEDIAN,3,srcRGB->nChannels);		//中值滤波
	IplImage *src = cvCreateImage(cvSize(srcRGB->width,srcRGB->height),IPL_DEPTH_8U,1);
	
	cvCvtColor(srcRGB,src,CV_RGB2GRAY);		//Set image color to GRAY
	vector<cv::Point2f> corners = getOutlinePoints(srcRGB, path0);

	cv::Mat img(src, 0);
	cv::Mat dstImg = img;
	
	cv::Mat warpMat = perspectiveWrap(img, corners);

	cv::warpPerspective(img, dstImg, warpMat, img.size());
	//cv::imshow("Viewdst", dstImg);
	cv::imwrite(wrapName.c_str(), dstImg);

	out.close();
	return 0;
}

vector<cv::Point2f> getOutlinePoints(IplImage *srcRGB, string path0){
#ifdef DEBUG_ONE 
	string saveName = "C:/Hough.jpg";
	string wrapName = "C:/Hough_wrap.jpg";
	ofstream create("C:/Hough.txt" ,ios_base::out|ios::app);
	create.close();
	ofstream out("C:/Hough.txt"  ,ios_base::out);
#else
	unsigned found = path0.find_last_of("/\\");
	string filename = path0.substr(found+1);
	string saveName ="E:/Hough/" +  filename.insert(filename.find_last_of(".") , "_hough");
	string wrapName ="E:/Hough/" +  filename.insert(filename.find_last_of(".") , "_wrap");
	
	//Create Output log file
	string txtName = saveName.substr(0, saveName.find_last_of(".")) + ".txt";
	ofstream create(txtName ,ios_base::out|ios::app);
	create.close();
	ofstream out(txtName ,ios_base::out);
#endif

	IplImage *src = cvCreateImage(cvSize(srcRGB->width,srcRGB->height),IPL_DEPTH_8U,1);
	cvCvtColor(srcRGB,src,CV_RGB2GRAY);		//Set image color to GRAY
	IplImage *dst = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,src->nChannels);
	IplImage *color_dst = cvCloneImage(srcRGB);
	cvSetZero(color_dst);
	CvMemStorage *storage = cvCreateMemStorage();
	CvSeq *lines = 0;
	cvCanny(src,dst,8,80);		//Use Canny to detect the edges in the image

	cvCvtColor(dst,color_dst,CV_GRAY2RGB);
	vector<double> linesVer_k;
	vector<double> linesVer_b;
	vector<double> linesHor_k;
	vector<double> linesHor_b;

	int minLen = 60;
	int lineGaps = 10;
	int iterCount = 0;
	bool stablized = false;
	int validLines = 0;


	//===============Verticle Lines=====================
	linesVer_k.clear();
	linesVer_b.clear();

	do{
		lines = cvHoughLines2(dst,storage,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,60,minLen,lineGaps);		//Find verticle lines in the image
		validLines = findVerticleLines(lines, src->height, src->width);
		//Too many lines
		if(validLines > 8){		
			if(stablized || lineGaps <= 1){
				minLen += 5;
			}else{
				if(lineGaps > 11){
					stablized = true;		
				}
				lineGaps -- ;
			}
		}
		//Too few lines
		if(validLines < 3){
			if(stablized || lineGaps > 6){
				minLen -= 10;		//Shorten minimum-length-of-line 
			}else{
				if(lineGaps < 10){
					stablized = true;
				}
				lineGaps +=2 ;
			}
		}
		iterCount++;
	}
	while( (validLines  >8 || validLines < 3) && iterCount < 3);


	for(int i = 0;i<lines->total; ++i){
		CvPoint *line = (CvPoint*)cvGetSeqElem(lines,i);
		//Wipe out points( Lines with a start point the same as end point is not a line)
		if(line[1].x == line[0].x && line[1].y == line[0].y)
			continue;
		//Calculate the line formula
		double k = (double)(line[1].y - line[0].y) / (double)(line[1].x - line[0].x);
		double b = -k * line[0].x + line[0].y;

		//Wipe out the verticle lines
		if( k > -MAX_K && k < MAX_K){
			continue;
		}

		//Find lines that stick close together
		int linesCount  = linesVer_k.size();
		bool valid = true;
		if(k > 0)
			k = (k > 100) ? 100 : k;
		else
			k = (k < -100) ? -100 : k;
		double abs_k = k > 0 ? k : -k;


		if(!(b == b)){
			b = 10000;		//If b is NaN, this line will be executed
		}else if(b > abs_k * src->width){
			b = abs_k * src->width * 0.9;
		}else if(b < -abs_k * src->width){
			b = - abs_k * src->width * 0.9;
		}

		//Change the line formula
		double k0 = 1 / k;
		double b0 = -b * k0;
		for(int i = 0 ; i < linesCount ; i++){
			double k1 = 1 / linesVer_k[i];
			if(k1 - k0 < 0.07 && k1 - k0 > -0.07 ){
				double b1 = -linesVer_b[i] / linesVer_k[i];
				if(b1 - b0 < 70 && b1 - b0 > -70){		//A similar line found
					valid = false;
					break;
				}
			}
		}

		if(valid){
			linesVer_k.push_back(k);
			linesVer_b.push_back(b);
		}else{
			continue;
		}
	}



	//===============Horizonal Lines=====================
	minLen = 100;
	lineGaps = 5;
	iterCount = 0;
	stablized = false;
	linesHor_k.clear();
	linesHor_b.clear();
	validLines = 0;
	do{
		lines = cvHoughLines2(dst,storage,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,80,minLen,lineGaps);
		validLines = findHorizonalLines(lines, src->height, src->width);
		if(validLines > 8){
			if(stablized || lineGaps <= 1){
				minLen += 20;
			}else{
				if(lineGaps > 5){
					stablized = true;
				}
				lineGaps -=2 ;
			}
		}
		if(validLines < 4){
			if(stablized || lineGaps > 9){
				minLen -= 10;
			}else{
				if(lineGaps < 4){
					stablized = true;
				}
				lineGaps ++ ;
			}
		}
		iterCount++;
	}
	while( (validLines  >8 || validLines < 4) && iterCount < 3);




	bool isPos = true;
	int posCount = 0, negCount = 0;

	for(int i = 0;i<lines->total; ++i)
	{
		CvPoint *line = (CvPoint*)cvGetSeqElem(lines,i);
		if(line[1].x == line[0].x && line[1].y == line[0].y)
			continue;

		//Calculate the line formula
		double k = (double)(line[1].y - line[0].y) / (double)(line[1].x - line[0].x);
		double b = -k * line[0].x + line[0].y;

		//Wipe out the verticle lines
		if(k < -MIN_K ||  k > MIN_K){
			continue;
		}

		//Find lines that stick close together
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
			if(k >= 0 ){		//Will wipe out lines of different directions
				posCount++;
			}else{
				negCount++;
			}
		}else{
			continue;
		}
	}

	vector<double> k2(linesHor_k);
	sort(k2.begin(), k2.end());
	isPos = posCount > negCount ? true : false;
	double mid_k = 0;

	//Unify direction of lines( Is K positive or negative? )
	for(unsigned i = 0 ; i < linesHor_k.size() ; i++){
		double k = linesHor_k[i];
		double b = linesHor_b[i];
		if(k < 0 && isPos || k >= 0 && !isPos){
			//Delete invalid lines
			linesHor_k[i] = linesHor_k[linesHor_k.size() - 1];
			linesHor_k.pop_back();
			linesHor_b[i] = linesHor_b[linesHor_b.size() - 1];
			linesHor_b.pop_back();
			i--;
			continue;
		}
		mid_k += k;
	}

	mid_k = mid_k / (double)linesHor_k.size();
	mid_k = mid_k > 0 ? mid_k : -mid_k;		//Calculate the mean K of lines

	//Delete lines with K too big or small
	for(unsigned i = 0 ; i < linesHor_k.size() ; i++){
		double k = linesHor_k[i] > 0 ? linesHor_k[i] : -linesHor_k[i];
		double b = linesHor_b[i];
		double dist = k - mid_k;
		if( dist > mid_k * 0.7 || dist < mid_k * -0.7){
			linesHor_k[i] = linesHor_k[linesHor_k.size() - 1];
			linesHor_k.pop_back();
			linesHor_b[i] = linesHor_b[linesHor_b.size() - 1];
			linesHor_b.pop_back();
			i--;
			continue;
		}
		//Draw the lines
		CvPoint pt1, pt2;
		pt1.x = 0;
		pt1.y = b;
		pt2.x = 3000;
		pt2.y = 3000 * linesHor_k[i] + b;
		cvLine(color_dst,pt1,pt2,CV_RGB(255,0,0),1,8);
	}


	//==========Print the candidate lines==================
	out<<"Verticle Lines"<<endl;
	for(unsigned i = 0 ; i < linesVer_k.size() ; i++){
		out<<"y = "<<linesVer_k[i]<<"x + "<<linesVer_b[i]<<endl;
	}
	out<<"Horizonal Lines"<<endl;
	for(unsigned i = 0 ; i < linesHor_k.size() ; i++){
		out<<"y = "<<linesHor_k[i]<<"x + "<<linesHor_b[i]<<endl;
	}


	//===========Find the suitable lines=============
	vector<double> resLines_k;
	vector<double> resLines_b;

	//--Find the verticle lines
	if(linesVer_k.size() == 0){
		resLines_k.push_back(40);
		resLines_k.push_back(40);
		resLines_b.push_back(-40.0 * (src->width * 0.1));
		resLines_b.push_back(-40.0 * (src->width * 0.9));
	}else if(linesVer_k.size() == 1){
		resLines_k.push_back(linesVer_k[0]);
		resLines_k.push_back(linesVer_k[0]);
		resLines_b.push_back(-linesVer_k[0] * (src->width * 0.1));
		resLines_b.push_back(-linesVer_k[0]* (src->width * 0.9));
	}else{
		vector<double> k2 (linesVer_k);
		sort(k2.begin(), k2.end());
		int linesCount = k2.size();
		for(int i =0  ; i < linesCount - 1 ; i++){
			double ratio = k2[i] / k2[i+1];

			if(ratio > 0.99 && ratio < 1.01){	//Find two parellel lines
				resLines_k.push_back(k2[i]);
				resLines_k.push_back(k2[i+1]);
				break;
			}
		}

		if(resLines_k.size() == 0){	//Lines not found, use the smallest and the largest
			vector<double> b2(linesVer_b);
			for(unsigned i = 0 ; i < linesVer_b.size() ; i++){
				b2[i] = -linesVer_b[i] / linesVer_k[i];
			}
			sort(b2.begin(), b2.end());
			for(unsigned i = 0 ; i < linesVer_b.size() ; i++){
				double temp_b = -linesVer_b[i] / linesVer_k[i];
				if( temp_b == b2[0] || temp_b == b2[b2.size() - 1]){
					resLines_k.push_back(linesVer_k[i]);
				}
			}
		}

		//Add corresponding b to line formula vector
		for(int i = 0 ; i < linesCount ; i++){
			if(linesVer_k[i] == resLines_k[0]){
				resLines_b.push_back(linesVer_b[i]);
				break;
			}
		}
		for(int i = 0 ; i < linesCount ; i++){
			if(linesVer_k[i] == resLines_k[1]){
				if(linesVer_b[i] == resLines_b[0])
					continue;
				resLines_b.push_back(linesVer_b[i]);
				break;
			}
		}
	}

	//--Find the horizonal lines
	k2.clear();
	if(linesHor_k.size() == 0){
		resLines_k.push_back(0);
		resLines_k.push_back(0);
		resLines_b.push_back((src->height * 0.1));
		resLines_b.push_back((src->height * 0.9));
	}else if(linesHor_k.size() == 1){
		resLines_k.push_back(linesHor_k[0]);
		resLines_k.push_back(linesHor_k[0]);
		if(resLines_k[0] > -0.5){
			resLines_b.push_back(src->height * 0.1);
			resLines_b.push_back(src->height * 0.9);
		}else{
			resLines_b.push_back(src->height * 0.4);
			resLines_b.push_back(src->height * 0.6);
		}
		
	}else{
		vector<double> mid_b;

		//Check lines by calculating y = k * mid_pos + b
		double mid_pos = isPos ? src->width * 0.35 : src->width * 0.65;
		for(unsigned i = 0; i < linesHor_b.size() ; i++){
			mid_b.push_back(linesHor_k[i] * mid_pos + linesHor_b[i]);
		}

		//Find the largest gap
		vector<double> mid_b_sort(mid_b);
		sort(mid_b_sort.begin(), mid_b_sort.end());
		double maxGap = 0;
		int pos = 0;
		for(int i = 0 ; i < mid_b_sort.size() - 1 ; i++){
			double thisGap = mid_b_sort[i+1] - mid_b_sort[i] ;
			if(thisGap > maxGap){
				maxGap = thisGap;
				pos = i;
			}
		}

		//Choose the lines that has largest gap between them 
		for(int i = 0 ; i < mid_b.size() ; i++){
			if(mid_b[i] == mid_b_sort[pos] || mid_b[i] == mid_b_sort[pos + 1]){
				resLines_k.push_back(linesHor_k[i]);
				resLines_b.push_back(linesHor_b[i]);
			}
		}

	}



	vector<cv::Point2f> crossPoints;
	bool valid = false;
	while(!valid){
		crossPoints.clear();
		for(int i = 0 ; i < 2 ; i++){
			double k1 = resLines_k[i];
			double b1 = resLines_b[i];
			for(int j = 2 ; j < 4 ; j++){
				double k2 = resLines_k[j];
				double b2 = resLines_b[j];
				double crossX = (b2 - b1) / (k1 - k2);
				double crossY = k1 * crossX + b1;
				crossPoints.push_back(cv::Point2f(crossX, crossY));
			}
		}

		//Check whether all points are inside the image
		for(unsigned i = 0 ; i < crossPoints.size() ;i++){
			if(crossPoints[i].y < 0 || crossPoints[i].y > src->height){	//Crossing point is not inside the image
				int verLineNum = i / 2;
				double p0 = 0;
				if( crossPoints[i].y > src->height){
					p0= resLines_k[2] < 0 ? src->width : 0;
				}else{
					p0 = resLines_k[2] > 0 ? src->width : 0;
				}
				resLines_b[verLineNum] = -(-resLines_b[verLineNum] / resLines_k[verLineNum]  + p0 )  / 2 * resLines_k[verLineNum] ;
				//resLines_b[verLineNum] = -(-resLines_b[0] / resLines_k[0]  + -resLines_b[1] / resLines_k[1] )  / 2 * resLines_k[verLineNum] ;
				valid = false;
				break;
			}
			if(i == crossPoints.size() - 1){
				valid = true;
			}
		}
	}


	//Print the result lines
	for(int i = 0 ; i < resLines_k.size() ; i++){
		if(resLines_k[i] < 5 && resLines_k[i] > -5){			//Print the horizonal lines
			CvPoint pt1, pt2;
			pt1.x = 0;
			pt1.y = resLines_b[i];
			pt2.x = 3000;
			pt2.y = 3000 * resLines_k[i] + resLines_b[i];
			cvLine(color_dst,pt1,pt2,CV_RGB(0,255,0),1,8);
		}else{			//Print the verticle lines
			CvPoint pt1, pt2;
			pt1.x = -resLines_b[i] / resLines_k[i] ;
			pt1.y = 0;
			pt2.x = (src->height - 1 - resLines_b[i]) / resLines_k[i];
			pt2.y = src->height - 1;
			cvLine(color_dst,pt1,pt2,CV_RGB(0,0,255),1,8);
		}
	}

	out<<"Crossing Points"<<endl;
	for(unsigned i = 0 ; i < crossPoints.size() ;i++){
		out<<"Point "<<i<<": (" << crossPoints[i].x << ", " << crossPoints[i].y <<")"<<endl;
		cvCircle(color_dst, cv::Point2f(crossPoints[i].x,  crossPoints[i].y), 5, CV_RGB(255,0,0) );
	}
	cvSaveImage(saveName.c_str(), color_dst);
	return crossPoints;
}


int findVerticleLines(CvSeq *lines, int height, int width)
{
	vector<double> lines_k;
	vector<double> lines_b;
	for(int i = 0;i<lines->total; ++i)
	{
		CvPoint *line = (CvPoint*)cvGetSeqElem(lines,i);
		//cvLine(color_dst,line[0],line[1],CV_RGB(255,0,0),1,8);
		if(line[1].x == line[0].x && line[1].y == line[0].y)
			continue;

		//Calculate the line formula
		double k = (double)(line[1].y - line[0].y) / (double)(line[1].x - line[0].x);
		double b = -k * line[0].x + line[0].y;

		if(k > 0)
			k = (k > 100) ? 100 : k;
		else
			k = (k < -100) ? -100 : k;

		double abs_k = k > 0 ? k : -k;

		if(!(b == b)){
			b = 10000;		//If b is NaN, change b into a rather large number. 
		}else if(b > abs_k * 300){
			b = abs_k * 300 * 0.9;
		}else if(b < -abs_k * 300){
			b = - abs_k * 300 * 0.9;
		}

		//Wipe out the invalid lines
		if( k > -MIN_K && k < MIN_K){
			continue;
		}

		//Find lines that stick close together
		int linesCount  = lines_k.size();
		bool valid = true;
		double k0 = 1 / k;
		double b0 = -b * k0;
		for(int i = 0 ; i < linesCount ; i++){
			double k1 = 1 / lines_k[i];
			if(k1 - k0 < 0.07 && k1 - k0 > -0.07 ){
				double b1 = -lines_b[i] / lines_k[i];
				if(b1 - b0 < 70 && b1 - b0 > -70){		//Find a similar line
					valid = false;
					break;
				}
			}
		}

		if(valid){
			lines_k.push_back(k);
			lines_b.push_back(b);
		}else{
			continue;
		}
	}
	return lines_k.size();
}


int findHorizonalLines(CvSeq *lines, int height, int width)
{
	vector<double> lines_k;
	vector<double> lines_b;
	for(int i = 0;i<lines->total; ++i)
	{
		CvPoint *line = (CvPoint*)cvGetSeqElem(lines,i);
		//cvLine(color_dst,line[0],line[1],CV_RGB(255,0,0),1,8);
		if(line[1].x == line[0].x && line[1].y == line[0].y)
			continue;

		//Calculate the line formula
		double k = (double)(line[1].y - line[0].y) / (double)(line[1].x - line[0].x);
		double b = -k * line[0].x + line[0].y;

		//Wipe out the verticle lines
		if(k < -MIN_K ||  k > MIN_K){
			continue;
		}

		//Find lines that stick close together
		int linesCount  = lines_k.size();
		bool valid = true;
		for(int i = 0 ; i < linesCount ; i++){
			if(lines_k[i] - k < 0.07 && lines_k[i] - k > -0.07 ){
				if(lines_b[i] - b < 30 && lines_b[i] - b > -30){		//Find a similar line
					valid = false;
					break;
				}
			}
		}

		if(valid){
			lines_k.push_back(k);
			lines_b.push_back(b);
		}else{
			continue;
		}
	}
	return lines_k.size();
}

//Reorder 4 corners as top-left, top-right, bottom-left, bottom-right
void sortCorners2(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;
	std::vector<cv::Point2f> left, right;
	for (unsigned i = 0; i < corners.size(); i++)
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
	//if(top.size() == 2 && bot.size() == 2){
	//	tl = top[0].x > top[1].x ? top[1] : top[0];
	//	tr = top[0].x > top[1].x ? top[0] : top[1];
	//	bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	//	br = bot[0].x > bot[1].x ? bot[0] : bot[1];
	//}else if(left.size() ==2 && right.size() == 2){
	//	tl = left[0].y < left[1].y ? left[0] : left[1];
	//	bl = left[0].y < left[1].y ? left[1] : left[0];
	//	tr = right[0].y < right[1].y ? right[0] : right[1];
	//	br = right[0].y < right[1].y ? right[1] : right[0];
	//}else{}
		tl = left[0].y < left[1].y ? left[0] : left[1];
		bl = left[0].y < left[1].y ? left[1] : left[0];
		tr = right[0].y < right[1].y ? right[0] : right[1];
		br = right[0].y < right[1].y ? right[1] : right[0];
	
	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}

//Get the transformation matrix that transforms img
cv::Mat perspectiveWrap(Mat img, vector<cv::Point2f> corners){
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
	sortCorners2(corners, center);
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

