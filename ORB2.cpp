#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std; 

char* image_filename1 = "C:/1.jpg"; 
char* image_filename2 = "C:/2.jpg"; 

int main5()
{
	Mat img1 = imread( image_filename1, CV_LOAD_IMAGE_GRAYSCALE );
	Mat img2 = imread( image_filename2, CV_LOAD_IMAGE_GRAYSCALE );

	int64 st,et;
	ORB orb1;
	ORB orb2(100);

	vector<KeyPoint>keys1,keys2;
	Mat descriptor1,descriptor2;
	orb1(img1,Mat(),keys1,descriptor1,false);
	st=getTickCount();
	orb2(img2,Mat(),keys2,descriptor2,false);
	et=getTickCount()-st;
	et=et*1000/(double)getTickFrequency();
	cout<<"extract time:"<<et<<"ms"<<endl;

	vector<DMatch> matches;
         //<em>class </em><tt class="descclassname">gpu::</tt><tt class="descname"><span class="highlighted">BruteForce</span>Matcher_GPU</tt>	
	BruteForceMatcher<HammingLUT>matcher;//BruteForceMatcher支持<Hamming> <L1<float>> <L2<float>>
	//FlannBasedMatcher matcher;不支持 
	st=getTickCount();
	matcher.match(descriptor1,descriptor2,matches);
	et=getTickCount()-st;
	et=et*1000/getTickFrequency();
	cout<<"match time:"<<et<<"ms"<<endl;
	

	Mat img_matches;
	drawMatches( img1, keys1, img2, keys2,
				matches, img_matches, Scalar::all(-1), Scalar::all(-1),
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	imshow("match",img_matches);


	cout<<"match size:"<<matches.size()<<endl;
	/*
	Mat showImg;
	drawMatches(img1,keys1,img2,keys2,matchs,showImg);
	imshow( "win", showImg ); 
	*/
	waitKey(0);
	

	st=getTickCount();
	vector<Point2f>pt1;
	vector<Point2f>pt2;
	float x=0,y=0;
	for(size_t i=0;i<matches.size();i++)
	{
		pt1.push_back(keys1[matches[i].queryIdx].pt);
		pt2.push_back(keys2[matches[i].trainIdx].pt);
		x+=keys2[matches[i].trainIdx].pt.x;
		y+=keys2[matches[i].trainIdx].pt.y;
	}
	x=x/matches.size();
	y=y/matches.size();
	
	Mat homo;
	homo=findHomography(pt1,pt2,CV_RANSAC);


	
 	vector<Point2f>src_cornor(4);
	vector<Point2f>dst_cornor(4);
	src_cornor[0]=cvPoint(0,0);
	src_cornor[1]=cvPoint(img1.cols,0);
	src_cornor[2]=cvPoint(img1.cols,img1.rows);
	src_cornor[3]=cvPoint(0,img1.rows);
	perspectiveTransform(src_cornor,dst_cornor,homo);
	
	Mat img=imread(image_filename2,1);
	
	line(img,dst_cornor[0],dst_cornor[1],Scalar(255,0,0),2);
	line(img,dst_cornor[1],dst_cornor[2],Scalar(255,0,0),2);
	line(img,dst_cornor[2],dst_cornor[3],Scalar(255,0,0),2);
	line(img,dst_cornor[3],dst_cornor[0],Scalar(255,0,0),2);
	/*
	line(img,cvPoint((int)dst_cornor[0].x,(int)dst_cornor[0].y),cvPoint((int)dst_cornor[1].x,(int)dst_cornor[1].y),Scalar(255,0,0),2);
	line(img,cvPoint((int)dst_cornor[1].x,(int)dst_cornor[1].y),cvPoint((int)dst_cornor[2].x,(int)dst_cornor[2].y),Scalar(255,0,0),2);
	line(img,cvPoint((int)dst_cornor[2].x,(int)dst_cornor[2].y),cvPoint((int)dst_cornor[3].x,(int)dst_cornor[3].y),Scalar(255,0,0),2);
	line(img,cvPoint((int)dst_cornor[3].x,(int)dst_cornor[3].y),cvPoint((int)dst_cornor[0].x,(int)dst_cornor[0].y),Scalar(255,0,0),2);
	*/

	circle(img,Point(x,y),10,Scalar(0,0,255),3,CV_FILLED);
	line(img,Point(x-img1.cols/2,y-img1.rows/2),Point(x+img1.cols/2,y-img1.rows/2),Scalar(0,0,255),2);
	line(img,Point(x+img1.cols/2,y-img1.rows/2),Point(x+img1.cols/2,y+img1.rows/2),Scalar(0,0,255),2);
	line(img,Point(x+img1.cols/2,y+img1.rows/2),Point(x-img1.cols/2,y+img1.rows/2),Scalar(0,0,255),2);
	line(img,Point(x-img1.cols/2,y+img1.rows/2),Point(x-img1.cols/2,y-img1.rows/2),Scalar(0,0,255),2);

	imshow("location",img);
	
	et=getTickCount()-st;
	et=et*1000/getTickFrequency();
	cout<<"location time:"<<et<<"ms"<<endl;
	
	waitKey(0);
	return 0;
}