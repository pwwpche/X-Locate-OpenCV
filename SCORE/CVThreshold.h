//图像的二值化



#include <iostream>
#include <string>
#include "RectChecker.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>


using namespace std;
using namespace cv;
#define CHECK_BLACK true
#define CHECK_WHITE false
#define CHECK_MIN_RATIO 0.015
class checkValid{

	//SURF match
	double checkColor(IplImage* img);
	void inverseColor(IplImage* img);
	void inverseColor(Mat &img);
	IplImage* binarize(IplImage* pSrcImage, const char* saveFileName, int defaultBinary );
	IplImage* cannyEdge(IplImage* pSrcImage, const char* saveFileName, int threshold);

	//Cut Image
	vector<pair<int, int> > horizonAreas(IplImage* img);
	void cutImage(IplImage* pSrcImage, const char* saveFileName );

	//Get Outline Points
	int findVerticleLines(CvSeq *lines,int height,  int width);
	int findHorizonalLines(CvSeq *lines, int height, int width);
	vector<cv::Point2f> getOutlinePoints(IplImage *grayImg);

	//Perspective Wrap
	void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);
	cv::Mat perspectiveWrap(cv::Mat img, vector<cv::Point2f> corners);
	IplImage* getWarppedImg(IplImage* srcRGB);
	

	//Detect Logo
	cv::Rect findLogoRect(IplImage *g_pSrcImage);		
	double nextWhiteHorizonal(IplImage* img, cv::Rect rect);
	double nextWhite(IplImage* img, cv::Rect rect);
	double whiteRatio(IplImage* img, cv::Rect rect);
	double brightness(IplImage* img, int threshold);
	double brightness(Mat img, int threshold);
	

	//Detect Logo2
	Mat getLogoMask(IplImage* scene);
	cv::Rect getLogoRect(IplImage *scene);
	Mat getMaskedScene(IplImage* img);
	Mat getMaskCannyScene(IplImage* img);
	Mat getMaskCannyLogo(IplImage* img);
	IplImage* cutAndWarp(IplImage *g_pSrcImage);
	bool none;

public:

	checkValid();

	/*	二值化函数（未做错误处理）
	*	说明：
	*	用于对一张图片进行二值化，可以去掉大部分无关北京，突出商店logo部分	
	*	默认二值化参数为210， 适用于带有灯箱或者亮度较高的logo
	*	
	*	参数：
	*	path：string类型，被二值化的图片路径，请确保此图片路径有效
	*	saveFileName：string类型，生成的二值化图片的保存路径
	*	threshold：int类型，二值化参数，范围为(0, +inf)。参数值越大，去掉的背景越多，但也越有可能去掉logo本身，一般在（0 , 220）之间
	*	
	*	返回值：
	*	无
	*
	*	用法：
	*	binarizePic("/storage/sdcard0/original.jpg", "/storage/sdcard0/binarized.jpg", 150);
	*
	*/
	void binarizePic(string path, string saveFileName, int threshold = 210);

		/*	Canny边缘检测函数（未做错误处理）
	*	说明：
	*	边缘检测函数，采用Canny边缘检测方法，可以将图片中大部分边缘大致描绘出来
	*	
	*	参数：
	*	path：string类型，被检测的图片路径，请确保此图片路径有效
	*	saveFileName：string类型，生成的边缘检测图片的保存路径
	*	threshold：int类型，二值化参数，范围为(0, +inf)。参数值越大，去掉的背景越多，但也越有可能去掉logo本身，一般在（1,12）之间
	*	
	*	返回值：
	*	无
	*
	*	用法：
	*	cannyPic("/storage/sdcard0/original.jpg", "/storage/sdcard0/binarized.jpg", );
	*
	*/
	void cannyPic(string path, string saveFileName, int threshold);

	/*	图像匹配函数（未做错误处理）
	*	说明：
	*	采用SIFT算法生成logo图和场景图各自的描述符，并采用BruteForce暴力匹配两张图中描述符相同的点。
	*	获得匹配的特征点之后，计算出相似矩阵，并将第一张图（logo图）在第二张图（场景图）中出现的位置标示出来
	*	生成的匹配结果图中包含第1、2张图中匹配的特征点位置和对应匹配关系，并会标示出第一张图在第二张图中出现的位置和形状
	*
	*	参数：
	*	logo：string类型，商店logo图片的路径，请确保此图片路径有效
	*	scene：string类型，商场实景图，请确保此图片路径有效
	*	saveName：string类型，生成的结果图片的保存路径
	*	method：int类型，暂时无效
	*
	*	返回值：
	*	1：匹配成功
	*	0：匹配失败
	*	其他：错误代号，暂时未实现
	*
	*	用法：
	*	match("/storage/sdcard0/fila_logo.jpg", "/storage/sdcard0/fila_scene.jpg", "/storage/sdcard0/result.jpg");
	*
	*/
	int match(string logo, string scene, string saveName, int method=0);

		/*	相似度评分（未做错误处理）
	*	说明：
	*	匹配方法与图像匹配函数相同，
	*	判断两个图片的相似度并给出得分，得分越高，相似度越高
	*
	*	参数：
	*	logo：string类型，商店logo图片的路径，请确保此图片路径有效
	*	scene：string类型，商场实景图，请确保此图片路径有效
	*	method：int类型，暂时无效
	*
	*	返回值：
	*	int类型，范围为[0, 100]，得分越高相似度越高
	*	
	*	用法：
	*	int matchScore = score("/storage/sdcard0/fila_logo.jpg", "/storage/sdcard0/fila_scene.jpg");
	*
	*/
	int score(string logo, string scene, int method=0);

	/*	图片裁剪（未做错误处理）
	*	说明：
	*	将图片中的选中区域裁剪并生成新的图片
	*
	*	参数：
	*	img：IplImage类型，待裁剪图片，请确保此指针有效
	*	rect：Rect类型，为待裁剪的矩形区域
	*	saveFileName：string类型，为保存图片的路径
	*
	*	返回值：
	*	无
	*	
	*
	*	用法：
	*	cv::Rect cutArea(10,20,100,200);
	*	IplImage *img = cvLoadImage(imgPath.c_str(), 0);
	*	cutImage(img, cutArea, "/storage/sdcard0/new.jpg");
	*/
	void cutImage(IplImage* img, cv::Rect rect, string saveFileName );
};

