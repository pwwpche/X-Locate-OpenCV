#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

class CVBlur{
public:
	void execute(){
		string path0 = "C:/marion_logo.jpg";
		Mat img = imread(path0.c_str(), 0);
		Mat img2;
		threshold(img, img2, 100, 255, 0);
		imshow("img2", img2);

		blur(img, img, Size(4, 4));
		blur(img, img, Size(4, 4));
		imshow("img", img);
		//imshow("img2", img2);
		cvWaitKey(0);
		return ;
	}
};
