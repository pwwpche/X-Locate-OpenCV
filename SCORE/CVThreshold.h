//ͼ��Ķ�ֵ��



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

	/*	��ֵ��������δ��������
	*	˵����
	*	���ڶ�һ��ͼƬ���ж�ֵ��������ȥ���󲿷��޹ر�����ͻ���̵�logo����	
	*	Ĭ�϶�ֵ������Ϊ210�� �����ڴ��е���������Ƚϸߵ�logo
	*	
	*	������
	*	path��string���ͣ�����ֵ����ͼƬ·������ȷ����ͼƬ·����Ч
	*	saveFileName��string���ͣ����ɵĶ�ֵ��ͼƬ�ı���·��
	*	threshold��int���ͣ���ֵ����������ΧΪ(0, +inf)������ֵԽ��ȥ���ı���Խ�࣬��ҲԽ�п���ȥ��logo����һ���ڣ�0 , 220��֮��
	*	
	*	����ֵ��
	*	��
	*
	*	�÷���
	*	binarizePic("/storage/sdcard0/original.jpg", "/storage/sdcard0/binarized.jpg", 150);
	*
	*/
	void binarizePic(string path, string saveFileName, int threshold = 210);

		/*	Canny��Ե��⺯����δ��������
	*	˵����
	*	��Ե��⺯��������Canny��Ե��ⷽ�������Խ�ͼƬ�д󲿷ֱ�Ե����������
	*	
	*	������
	*	path��string���ͣ�������ͼƬ·������ȷ����ͼƬ·����Ч
	*	saveFileName��string���ͣ����ɵı�Ե���ͼƬ�ı���·��
	*	threshold��int���ͣ���ֵ����������ΧΪ(0, +inf)������ֵԽ��ȥ���ı���Խ�࣬��ҲԽ�п���ȥ��logo����һ���ڣ�1,12��֮��
	*	
	*	����ֵ��
	*	��
	*
	*	�÷���
	*	cannyPic("/storage/sdcard0/original.jpg", "/storage/sdcard0/binarized.jpg", );
	*
	*/
	void cannyPic(string path, string saveFileName, int threshold);

	/*	ͼ��ƥ�亯����δ��������
	*	˵����
	*	����SIFT�㷨����logoͼ�ͳ���ͼ���Ե���������������BruteForce����ƥ������ͼ����������ͬ�ĵ㡣
	*	���ƥ���������֮�󣬼�������ƾ��󣬲�����һ��ͼ��logoͼ���ڵڶ���ͼ������ͼ���г��ֵ�λ�ñ�ʾ����
	*	���ɵ�ƥ����ͼ�а�����1��2��ͼ��ƥ���������λ�úͶ�Ӧƥ���ϵ�������ʾ����һ��ͼ�ڵڶ���ͼ�г��ֵ�λ�ú���״
	*
	*	������
	*	logo��string���ͣ��̵�logoͼƬ��·������ȷ����ͼƬ·����Ч
	*	scene��string���ͣ��̳�ʵ��ͼ����ȷ����ͼƬ·����Ч
	*	saveName��string���ͣ����ɵĽ��ͼƬ�ı���·��
	*	method��int���ͣ���ʱ��Ч
	*
	*	����ֵ��
	*	1��ƥ��ɹ�
	*	0��ƥ��ʧ��
	*	������������ţ���ʱδʵ��
	*
	*	�÷���
	*	match("/storage/sdcard0/fila_logo.jpg", "/storage/sdcard0/fila_scene.jpg", "/storage/sdcard0/result.jpg");
	*
	*/
	int match(string logo, string scene, string saveName, int method=0);

		/*	���ƶ����֣�δ��������
	*	˵����
	*	ƥ�䷽����ͼ��ƥ�亯����ͬ��
	*	�ж�����ͼƬ�����ƶȲ������÷֣��÷�Խ�ߣ����ƶ�Խ��
	*
	*	������
	*	logo��string���ͣ��̵�logoͼƬ��·������ȷ����ͼƬ·����Ч
	*	scene��string���ͣ��̳�ʵ��ͼ����ȷ����ͼƬ·����Ч
	*	method��int���ͣ���ʱ��Ч
	*
	*	����ֵ��
	*	int���ͣ���ΧΪ[0, 100]���÷�Խ�����ƶ�Խ��
	*	
	*	�÷���
	*	int matchScore = score("/storage/sdcard0/fila_logo.jpg", "/storage/sdcard0/fila_scene.jpg");
	*
	*/
	int score(string logo, string scene, int method=0);

	/*	ͼƬ�ü���δ��������
	*	˵����
	*	��ͼƬ�е�ѡ������ü��������µ�ͼƬ
	*
	*	������
	*	img��IplImage���ͣ����ü�ͼƬ����ȷ����ָ����Ч
	*	rect��Rect���ͣ�Ϊ���ü��ľ�������
	*	saveFileName��string���ͣ�Ϊ����ͼƬ��·��
	*
	*	����ֵ��
	*	��
	*	
	*
	*	�÷���
	*	cv::Rect cutArea(10,20,100,200);
	*	IplImage *img = cvLoadImage(imgPath.c_str(), 0);
	*	cutImage(img, cutArea, "/storage/sdcard0/new.jpg");
	*/
	void cutImage(IplImage* img, cv::Rect rect, string saveFileName );
};

