//  局部图像特征提取与匹配  
//  Author:  www.icvpr.com
//  Blog  :  http://blog.csdn.net/icvpr

#include "LocalFeature.h"

Feature::Feature()
{
	m_detectType = "SIFT";
	m_extractType = "SIFT";
	m_matchType = "BruteForce";
	initModule_nonfree(); 
}

Feature::~Feature()
{

}


Feature::Feature(const string& detectType, const string& extractType, const string& matchType)
{
	assert(!detectType.empty());
	assert(!extractType.empty());
	assert(!matchType.empty());

	m_detectType = detectType;
	m_extractType = extractType;
	m_matchType = matchType;
	initModule_nonfree(); 
}


void Feature::detectKeypoints(const Mat& image, std::vector<KeyPoint>& keypoints) 
{
	assert(image.type() == CV_8UC1);
	assert(!m_detectType.empty());

	keypoints.clear();
	m_detector = FeatureDetector::create(m_detectType);
	m_detector->detect(image, keypoints);

}



void Feature::extractDescriptors(const Mat& image, std::vector<KeyPoint>& keypoints, Mat& descriptor)
{
	assert(image.type() == CV_8UC1);
	assert(!m_extractType.empty());

	m_extractor = DescriptorExtractor::create(m_extractType);
	m_extractor->compute(image, keypoints, descriptor);

}


void Feature::bestMatch(const Mat& queryDescriptor, Mat& trainDescriptor, std::vector<DMatch>& matches) 
{
	assert(!queryDescriptor.empty());
	assert(!trainDescriptor.empty());
	assert(!m_matchType.empty());

	matches.clear();

	m_matcher = DescriptorMatcher::create(m_matchType);
	m_matcher->add(std::vector<Mat>(1, trainDescriptor));
	m_matcher->train();
	m_matcher->match(queryDescriptor, matches);

}


void Feature::knnMatch(const Mat& queryDescriptor, Mat& trainDescriptor, std::vector<std::vector<DMatch>>& matches, int k)
{
	assert(k > 0);
	assert(!queryDescriptor.empty());
	assert(!trainDescriptor.empty());
	assert(!m_matchType.empty());

	matches.clear();

	m_matcher = DescriptorMatcher::create(m_matchType);
	m_matcher->add(std::vector<Mat>(1, trainDescriptor));
	m_matcher->train();
	m_matcher->knnMatch(queryDescriptor, matches, k);

}



void Feature::saveKeypoints(const Mat& image, const vector<KeyPoint>& keypoints, const string& saveFileName)
{
	assert(!saveFileName.empty());

	Mat outImage;
	cv::drawKeypoints(image, keypoints, outImage, Scalar(255,255,0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

	//
	string saveKeypointsImgName = saveFileName + "_" + m_detectType + ".jpg";
	imwrite(saveKeypointsImgName, outImage);

}



void Feature::saveMatches(const Mat& queryImage,
							const vector<KeyPoint>& queryKeypoints,
							const Mat& trainImage,
							const vector<KeyPoint>& trainKeypoints,
							const vector<DMatch>& matches,
							const string& saveFileName)
{
	assert(!saveFileName.empty());

	Mat outImage;
	cv::drawMatches(queryImage, queryKeypoints, trainImage, trainKeypoints, matches, outImage, 
				Scalar(255, 0, 0), Scalar(0, 255, 255), vector<char>(),  DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//
	string saveMatchImgName = saveFileName + "_" + m_detectType + "_" + m_extractType + "_" + m_matchType + ".jpg";
	imwrite(saveMatchImgName, outImage);
}