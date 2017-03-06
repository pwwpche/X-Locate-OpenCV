/**
* @file SURF_Homography
* @brief SURF detector + descriptor + FLANN Matcher + FindHomography
* @author A. Huaman
*/

//#include <stdio.h>
//#include <iostream>
//#include "opencv2/core/core.hpp"
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/nonfree/features2d.hpp"
//#include <fstream>
#define NOMINMAX
#include "Match_MatchingHelper.h"
#include "Timer.h"
#include <iostream>
#include<string>


#include "CV_Histogram.h"
#include "CV_Quadrize.h"

using namespace cv;
/**
* @function main
* @brief Main function
*/

int main(){
	Quadrize quad;
	quad.execute("C:/logo.jpg");
	return 0;
}


int main_homo(int argc, char** argv)
{

#if 1
	Timer timer;
	string path1 = "C:/logo.jpg";
	//string path2 = "E:\\Metro_Photos_Compressed\\Metro1\\marion.jpg";
	string path2 = "C:/back.jpg";
	//string path2 ="E:\\Tencent Files\\80489546\\FileRecv\\RESULT\\134\\1.jpg";
	string save = "C:/save.jpg";
	MatchingHelper helper;
	timer.start();
	cout<< helper.match(path1, path2, save)<<endl;
	timer.stop();
	cout<<timer.result() << endl;
#else
	if( argc != 4 )
	{
		return -1; 
	}
	string path1 = argv[1];
	//string path2 = "C:/fila_scene.jpg";
	string path2 = argv[2];
	string save = argv[3];

	//string path1 = "C:/Skymall_logo/ajidou.jpg";
	//string path2 ="C:/Metro_refined_640/Ajidou.JPG";
	//string save = "C:/save.jpg";
	MatchingHelper validChecker;
	validChecker.match(path1, path2, save);

	//if(argc != 2){
	//	return -1;
	//}
	//string path0 = argv[1];
	////string path0 = "C:\\29_4.png";
	//unsigned found = path0.find_last_of("\\");
	//string filename = path0.substr(fo5und+1);
	//MatchingHelper validChecker;
	//string saveName = filename.insert(filename.find_last_of(".") , "_bin");
	//validChecker.binarizePic(path0, "E:/bin/" + saveName, 130);
#endif
	waitKey(0);
	
	return 0;
}
