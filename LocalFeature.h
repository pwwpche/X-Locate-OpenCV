//	�ֲ�ͼ��������ȡ��ƥ��
//      Author:  www.icvpr.com
//	Blog  :  http://blog.csdn.net/icvpr
	
#ifndef _FEATURE_H_ 
#define _FEATURE_H_

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Feature
{
public:
	Feature();
	~Feature();

	Feature(const string& detectType, const string& extractType, const string& matchType);

public:
	
	void detectKeypoints(const Mat& image, vector<KeyPoint>& keypoints);   // ���������
	void extractDescriptors(const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptor);   // ��ȡ��������
	void bestMatch(const Mat& queryDescriptor, Mat& trainDescriptor, vector<DMatch>& matches);  // �����ƥ��
	void knnMatch(const Mat& queryDescriptor, Mat& trainDescriptor, vector<vector<DMatch>>& matches, int k);   // K����ƥ��

	void saveKeypoints(const Mat& image, const vector<KeyPoint>& keypoints, const string& saveFileName = "");  // ����������
	void saveMatches(const Mat& queryImage,
			 const vector<KeyPoint>& queryKeypoints,
			 const Mat& trainImage,
			 const vector<KeyPoint>& trainKeypoints,
			 const vector<DMatch>& matches,
			 const string& saveFileName = "");   // ����ƥ������ͼƬ��

private:
	Ptr<FeatureDetector> m_detector;
	Ptr<DescriptorExtractor> m_extractor;
	Ptr<DescriptorMatcher> m_matcher;

	string m_detectType;
	string m_extractType;
	string m_matchType;

};


#endif