#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


class CVFindBlob{
public:
	void execute(){
		Mat img = imread("C:/a.png", 0);
		if(!img.data) {
			cout << "File not found" << endl;
			return ;
		}

		namedWindow("binary");
		namedWindow("labelled");
		Mat output = Mat::zeros(img.size(), CV_8UC3);
		Mat binary;
		vector < vector<Point2i > > blobs;

		threshold(img, binary, 0, 1, THRESH_BINARY_INV);
		FindBlobs(binary, blobs);

		// Randomy color the blobs
		for(size_t i=0; i < blobs.size(); i++) {
			unsigned char r = 255 * (rand()/(1.0 + RAND_MAX));
			unsigned char g = 255 * (rand()/(1.0 + RAND_MAX));
			unsigned char b = 255 * (rand()/(1.0 + RAND_MAX));

			for(size_t j=0; j < blobs[i].size(); j++) {
				int x = blobs[i][j].x;
				int y = blobs[i][j].y;
				output.at<Vec3b>(y,x)[0] = b;//Vec3b RGB color order
				output.at<Vec3b>(y,x)[1] = g;
				output.at<Vec3b>(y,x)[2] = r;
			}
		}
		imshow("binary", img);
		imshow("labelled", output);
		waitKey(0);
		return ;
	}

	void FindBlobs(const Mat &binary, vector < vector<Point2i> > &blobs)
	{
		blobs.clear();
		Mat label_image;
		binary.convertTo(label_image, CV_32SC1);
		int label_count = 2; // starts at 2 because 0,1 are used already

		for(int y=0; y < label_image.rows; y++) {
			int *row = (int*)label_image.ptr(y);
			for(int x = 0 ; x < label_image.cols; x++ ) {
				if(row[x] != 1) {
					continue;
				}
				Rect rect;
				floodFill(label_image, Point(x,y), label_count, &rect, 0, 0, 4);
				vector <Point2i> blob;
				for(int i=rect.y; i < (rect.y+rect.height); i++) {
					int *row2 = (int*)label_image.ptr(i);
					for(int j=rect.x; j < (rect.x+rect.width); j++) {
						if(row2[j] != label_count) {
							continue;
						}
						blob.push_back(Point2i(j,i));
					}
				}
				if(blob.size() > 50){
					blobs.push_back(blob);
				}
				label_count++;
			}
		}

	}
};
