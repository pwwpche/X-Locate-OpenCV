#include "Match_MatchingHelper.h"


void MatchingHelper::Mat2KeyPoint(const Mat& mat, vector<KeyPoint>& v_kp)
{
	v_kp.clear();
	assert(mat.type()==CV_32FC(7) && mat.cols==1);
	for(int i=0; i<mat.rows; i++)
	{
		Vec<float, 7> v = mat.at< Vec<float, 7> >(i, 0);
		KeyPoint kp(v[0], v[1], v[2], v[3], v[4], (int)v[5], (int)v[6]);
		v_kp.push_back(kp);
	}
	return;
}

void MatchingHelper::KeyPoint2Mat(vector<KeyPoint>& v_kp, Mat& mat)
{
	int count = (int)v_kp.size();
	mat.create(count, 1, CV_32FC(7));
	for(int i=0; i<count; i++)
	{
		KeyPoint kp = v_kp[i];
		mat.at< Vec<float, 7> >(i, 0) = Vec<float, 7>(kp.pt.x, kp.pt.y, kp.size, kp.angle, kp.response, (float)kp.octave, (float)kp.class_id);
	}
}

void MatchingHelper::getKeyPoints(string keysFileName, vector<KeyPoint> &keypoints, Mat &img){
#ifdef USE_SIFT
	SiftFeatureDetector detector(400, 3, 0.04, 10.0, 1.6);
#else
	SurfFeatureDetector detector(MY_MIN_HESSIAN);
	////cout<<"SURF4000"<<endl;
#endif
	//cv::SIFT detector = cv::SIFT(0,3,0.4,10, 0.2);
	ifstream ifs(keysFileName, ios::app);
	int filetest = -1;
	ifs >> filetest;
	if(ifs.eof() ){
		detector.detect( img, keypoints );
		FileStorage fs(keysFileName.c_str(), FileStorage::WRITE);
		cv::write( fs , "keys", keypoints );
		fs.release();
	}else{
		FileStorage fs2(keysFileName.c_str(), FileStorage::READ);
		FileNode kptFileNode = fs2["keys"];
		read( kptFileNode, keypoints );
		fs2.release();
	}
	ifs.close();
}

void MatchingHelper::getExtractor(string matFileName, vector<KeyPoint> &keypoints, Mat &img, Mat &descriptors){
#ifdef USE_SIFT
	SiftDescriptorExtractor extractor;
#else
	SurfDescriptorExtractor extractor;
#endif
	//cv::SIFT extractor = cv::SIFT(0,3,0.4,10, 0.2);
	ifstream ifs(matFileName, ios::app);
	int filetest = -1;
	ifs >> filetest;
	if(ifs.eof() ){
		extractor.compute( img, keypoints, descriptors );
		FileStorage fs(matFileName,FileStorage::WRITE);
		cv::write( fs , "descriptorsMat", descriptors );
		fs.release();
	}else{

		FileStorage fs2(matFileName,FileStorage::READ);
		FileNode kptFileNode = fs2["descriptorsMat"];
		cv::read( kptFileNode, descriptors );
		fs2.release();

	}
	ifs.close();
}

MatchingHelper::MatchingHelper(){
	none = false;
}

double MatchingHelper::checkColor(IplImage* img){

	//Get Information of the image
	int height = img->height;
	int width = img->width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	int blackCount = 0;
	int whiteCount = 0;

	//Inverse color pixel by pixel
	for(unsigned i = 0 ; i < height ; i++){
		for(int j=0 ; j < width ; j++) {
			if(data[i*step+j*channels+1] == 255){
				whiteCount++;
			}
			else{
				blackCount++;
			}
		}
	}
	double result = (double)whiteCount / (double)blackCount;
	return result;
}

//Inverse the color of whole image
void MatchingHelper::inverseColor(IplImage* img){

	//Get Information of the image
	int height = img->height;
	int width = img->width;
	int step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	//Inverse color pixel by pixel
	for(int i = 0 ; i < height ; i++){
		for(int j=0 ; j < width ; j++) {
			for(int k = 0 ; k < channels ; k++){
				data[i*step+j*channels+k]=255-data[i*step+j*channels+k];    
			}
		}
	}

	//Save image
	//cvSaveImage("C:\\inverse.jpg", img);
}

void MatchingHelper::inverseColor(Mat &img){
	IplImage img2(img);
	inverseColor(&img2);
	return ;
}

IplImage* MatchingHelper::binarize(IplImage* pSrcImage, const char* saveFileName, int defaultBinary = 210 ){

	//Create gray image
	IplImage *g_pGrayImage = NULL;
	IplImage *g_pBinaryImage = NULL;

	//g_pGrayImage =  cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	g_pGrayImage = pSrcImage;
	//cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	//Create Binary Image
	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	//Change gray mage to binary image
	cvThreshold(g_pGrayImage, g_pBinaryImage, defaultBinary, 255, CV_THRESH_BINARY);

	//Change main color to black
	double colorRatio = checkColor(g_pBinaryImage);
	if( colorRatio > 1 && 0){
		inverseColor(g_pBinaryImage);
		colorRatio = 1.0 / colorRatio;
	}

	if(colorRatio < CHECK_MIN_RATIO && defaultBinary > 140 && 0){
		binarize(pSrcImage, saveFileName, defaultBinary - 20);
	}else{
		//Save image
		string str = saveFileName;
		if(str.length() > 2){
			cvSaveImage(saveFileName, g_pBinaryImage);
		}
		//cutImage(g_pBinaryImage, saveFileName);
		//cvReleaseImage(&g_pGrayImage);//=====================================
		//cvReleaseImage(&g_pBinaryImage);
	}
	return g_pBinaryImage;
}

IplImage* MatchingHelper::cannyEdge(IplImage* pSrcImage, const char* saveFileName, int threshold = 95 )
{

	//Create gray image
	IplImage *g_pCannyImg = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);

	//Change gray mage to binary image
	cvCanny(pSrcImage, g_pCannyImg, threshold, threshold * 10, 3);

	//Save image
	cvSaveImage(saveFileName, g_pCannyImg);
	return g_pCannyImg;
}

int MatchingHelper::match(string path1, string path2, string saveName, int method){

	////Perspectve Warp(bin)
	//IplImage *pSrcImage1 = cvLoadImage(path1.c_str(), 0);
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 0);
	////IplImage *pSrcImage2_gray = getWarppedImg(pSrcImage2);
	//pSrcImage1 = binarize(pSrcImage1, "C:/tmp_logo.jpg");
	//pSrcImage2 = binarize(pSrcImage2, "C:/tmp_scene.jpg", 100);
	//Mat img_object = Mat(pSrcImage1, 0);
	//Mat img_scene = Mat(pSrcImage2, 0);

	////Cut and warp
	//IplImage *pSrcImage1 = cvLoadImage(path1.c_str(), 0);
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 0);
	//IplImage *refinedLogo = cutAndWarp(pSrcImage2);
	//pSrcImage1 = binarize(pSrcImage1, "C:/tmp_logo.jpg");
	//refinedLogo = binarize(refinedLogo, "C:/tmp_scene.jpg", 180);
	//Mat img_object = Mat(pSrcImage1, 0);
	//Mat img_scene = Mat(refinedLogo, 0);

	//Blur and contrast
	ofstream timeOfs("E:\\time_det.txt", ios::app);
	Timer timer;
	timer.start();
	Mat img_object = imread(path1.c_str(), CV_LOAD_IMAGE_UNCHANGED );
	Mat img_scene = imread(path2.c_str(), CV_LOAD_IMAGE_UNCHANGED );
	timer.stop();
	timeOfs << timer.result() << "\t" ;
	//cout<<"Read File: "<<timer.result()<<endl;
	//timer.start();
	//int blurWidth= img_object.cols / 150;
	//int blurHeight = img_object.rows / 50;
	//if(blurWidth > 0 && blurHeight > 0){
	//	blur(img_object, img_object, Size(blurWidth, blurHeight));
	//}
	//timer.stop();
	//cout<<"Blur: "<<timer.result()<<endl;
	timer.start();
	//Mat img_scene = Mat::zeros( img_scene2.size(), img_scene2.type() );
	//img_scene2.convertTo(img_scene, -1, 1.2, -50);	//Improve contrast
	timer.stop();
	//cout<<"Contrast: "<<timer.result()<<endl;

	////Mask Scene
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 1);
	//Mat img_scene = getMaskedScene(pSrcImage2);
	//Mat img_object = imread(path1.c_str(), 1);
	//blur(img_object, img_object, Size(4, 4));
	////blur(img_object, img_object, Size(4, 4));



	////Canny & mask
	//IplImage *pSrcImage1 = cvLoadImage(path1.c_str(), 1);
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 1);
	//Mat img_scene = getMaskCannyLogo(pSrcImage2);
	//Mat img_object = getMaskCannyLogo(pSrcImage1);

	////No rotation and perspective warp
	//IplImage *pSrcImage1 = cvLoadImage(path1.c_str(), 0);
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 0);
	//pSrcImage1 = binarize(pSrcImage1, "C:/tmp_logo.jpg");
	//pSrcImage2 = binarize(pSrcImage2, "C:/tmp_scene.jpg", 210);
	//Mat img_object = Mat(pSrcImage1, 0);
	//Mat img_scene = Mat(pSrcImage2, 0);

	////Perspectve Warp without bin
	//IplImage *pSrcImage1 = cvLoadImage(path1.c_str(), 0);
	//inverseColor(pSrcImage1);
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 0);
	////IplImage *pSrcImage2_gray = getWarppedImg(pSrcImage2);
	//Mat img_object = Mat(pSrcImage1, 0);
	//Mat img_scene = Mat(pSrcImage2, 0);

	if( !img_object.data || !img_scene.data )
	{ 
		//cout<< " --(!) Error reading images " << std::endl; 
		return -1; 
	}

	//-- Step 1: Detect the keypoints using SIFT Detector
	std::vector<KeyPoint> keypoints_object, keypoints_scene;

	
	
	//计算特征点后存储
	string logo_keypointsName =path1.substr(0, path1.find_last_of(".")) + "_keyPoints.yml";
	string scene_keypointsName = path2.substr(0, path2.find_last_of(".")) + "_keyPoints.yml";
	string logo_matName = path1.substr(0, path1.find_last_of(".")) + "_mat.yml";
	string scene_matName = path2.substr(0, path2.find_last_of(".")) + "_mat.yml";
	timer.start();
	getKeyPoints(logo_keypointsName, keypoints_object, img_object);
	getKeyPoints(scene_keypointsName, keypoints_scene, img_scene);
	timer.stop();
	timeOfs << timer.result() << "\t" ;
	//cout<<"Calculate keypoint: "<<timer.result()<<endl;
	//detector.detect( img_scene, keypoints_scene );
	//detector.detect( img_object, keypoints_object );

	//-- Step 2: Calculate descriptors (feature vectors)
	//SiftDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;
	timer.start();
	getExtractor(logo_matName, keypoints_object, img_object, descriptors_object);
	getExtractor(scene_matName, keypoints_scene, img_scene, descriptors_scene);
	timer.stop();
	timeOfs << timer.result() << "\t" ;
	//cout<<"Calculate extractor: "<<timer.result()<<endl;
	//extractor.compute( img_object, keypoints_object, descriptors_object );
	//extractor.compute( img_scene, keypoints_scene, descriptors_scene );

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	//FlannBasedMatcher matcher;
	Mat object_out, scene_out;
	drawKeypoints(img_object,keypoints_object,object_out,Scalar::all(-1),0);  
	drawKeypoints(img_scene,keypoints_scene,scene_out,Scalar::all(-1),0);  
	imwrite("C:/1.jpg", object_out);
	imwrite("C:/2.jpg", scene_out);
	
	timer.start();
	BFMatcher matcher(NORM_L2, true);
	std::vector< DMatch > matches;
	vector<double> matchesDistance;
	if(descriptors_object.rows > 0 && descriptors_scene.rows > 0){
		matcher.match( descriptors_object, descriptors_scene, matches);
	}
	timer.stop();
	timeOfs << timer.result() << "\t" ;
	//cout<<"Matching: "<<timer.result()<<endl;
	//-- Quick calculation of max and min distances between keypoints

	string homoName = saveName.substr(0, saveName.find_last_of(".")) + ".txt";
	string matchName = saveName.substr(0, saveName.find_last_of(".")) + "_match.txt";
	ofstream ofs(homoName ,ios_base::app);
	ofstream matchFs(matchName ,ios_base::app);
	ofs << img_scene.cols << "\t" << img_scene.rows << endl;
	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	timer.start();
	std::vector< DMatch > good_matches;
	size_t matches_size = matches.size();
	for( unsigned i = 0; i < matches_size; i++ )
	{
		if( matches[i].distance < MY_GOOD_DISTANCE)
		{
			good_matches.push_back( matches[i]); 
		}
	}
	//Draw good matches
	Mat img_matches;
	drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );


	//-- Localize the object from img_1 in img_2 
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	int good_size = good_matches.size() > 30 ? 30 : good_matches.size();
	for( size_t i = 0; i < good_matches.size() ; i++ )
	{
		//-- Get the keypoints from the good matches
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}

	//-- Get the corners from the image_1 ( the object to be "detected" )
	bool bonus = false;
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0,0); 
	obj_corners[1] = Point( img_object.cols, 0 );
	obj_corners[2] = Point( img_object.cols, img_object.rows ); 
	obj_corners[3] = Point( 0, img_object.rows );
	std::vector<Point2f> scene_corners(4);
	if(good_matches.size() < 4){
		ofs << "0" << endl;
		ofs << "0" << endl;
	}else{
		Mat H = findHomography( obj, scene, RANSAC );
		perspectiveTransform( obj_corners, scene_corners, H);
		int scene_size = scene_corners.size() ;
		ofs << scene_size << endl;
		for (unsigned i = 0; i < scene_size; i++){
			ofs << scene_corners[i].x << "\t" << scene_corners[i].y << endl;
		}
		int goodmatch_size =  good_matches.size() ;
		ofs << goodmatch_size << endl;
		for(unsigned i = 0 ; i < goodmatch_size ; i++){
			int trainIdx0 = good_matches[i].trainIdx;
			ofs << keypoints_scene[trainIdx0].pt.x << "\t" << keypoints_scene[trainIdx0].pt.y << endl;
		}
		//-- Draw lines between the corners (the mapped object in the scene - image_2 )
		Point2f offset( (float)img_object.cols, 0);
		line( img_matches, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 1 );
		line( img_matches, scene_corners[1] + offset, scene_corners[2] + offset, Scalar( 0, 0, 255), 2 );
		line( img_matches, scene_corners[2] + offset, scene_corners[3] + offset, Scalar( 255, 0, 0), 3 );
		line( img_matches, scene_corners[3] + offset, scene_corners[0] + offset, Scalar( 255, 255, 255), 4 );

		ofstream ofs3(saveName.substr(0, saveName.find_last_of("/\\")) + "\\res.txt", std::ios::app);
		RectChecker checker;
		bool rectCheckResult = checker.check(scene_corners, img_scene.rows, img_scene.cols);
		if(rectCheckResult){
			ofs3 << saveName<<endl;
			bonus = true;
		}
		ofs3.close();
	}
	timer.stop();
	timeOfs << timer.result() << "\t" ;
	//cout<<"Get corners: "<<timer.result()<<endl;
	//===========SCORE ============
	timer.start();
	vector<double> matchDistance;
	double avg = 0;
	int avgCount = 0;
	int goodCount = 0 ;
	for( unsigned i = 0; i < matches.size(); i++ )
	{
		double dist = matches[i].distance;
		matchFs<<dist<<"\n";
		if( dist < MY_AVG_DISTANCE  && dist > MY_LEAST_DISTANCE )
		{
			avg += dist;
			avgCount++;
		}
		if(dist < MY_GOOD_DISTANCE && dist > MY_LEAST_DISTANCE ){
			goodCount++;
		}
		matchDistance.push_back(dist);
	}
	if(avgCount > 6){
		avg /= avgCount;
		if(goodCount < 12){
			avg = avg + (12-goodCount) * 4;
		}
	}else{
		avg = MY_MAX_DISTANCE;
	}

	avg = avg > MY_AVG_DISTANCE ? MY_AVG_DISTANCE : avg;
	avg = avg < MY_MIN_DISTANCE ? MY_MIN_DISTANCE : avg;


	double score_avg = (MY_AVG_DISTANCE - avg) / ( MY_AVG_DISTANCE - MY_MIN_DISTANCE ) * 100;
	if(bonus){
		score_avg += 40;
		score_avg = score_avg > 100 ? 100 : score_avg;
	}else{
		score_avg -= 5;
		score_avg = score_avg < 0 ? 0 : score_avg;
	}
	timer.stop();
	timeOfs << timer.result() << "\t" ;
	//cout<<"Get score: "<<timer.result()<<endl;

	//sort(matchDistance.begin(), matchDistance.end());

	//int half = 400;
	//if(matchDistance.size() > 1){
	//	//max_dist = matchDistance[matchDistance.size() - 2];
	//	//min_dist = matchDistance[1];
	//	half = matchDistance[matchDistance.size() / 2];
	//	half = half > 400 ? 400 : half;
	//	half = half < 250 ? 250 : half; 
	//}

	//int score_mid = (400 - half) / 1.5 ;
	//if(goodCount < 12){
	//	score_mid -= ( 13 - goodCount) * 1.5;
	//}

	//printf("Good Matches Count: %d/%d [%f]\n", good_matches.size(), descriptors_object.rows, res);
	////cout<<"Score_avg: " << score_avg<<endl;
	////cout<<"Score_mid: " << score_mid<<endl;
	string logPath = saveName.substr(0, saveName.find_last_of("/\\")) + "\\log.txt";
	string fileName = saveName.substr(saveName.find_last_of("/\\") + 1 , saveName.find_last_of(".") - saveName.find_last_of("/\\") - 1);
	ofstream ofs2(logPath, ios::app);
	ofs2 << fileName << "\t"<<score_avg<<endl;
	ofs2.close();
	ofs2.open(saveName.substr(0, saveName.find_last_of("/\\")) + "\\correct.txt", std::ios::app);
	if(score_avg > 65){
		ofs2 << fileName <<endl;
	}
	matchFs.close();
	ofs2.close();
	ofs.close();

	//-- Show detected matches
	//	imshow( "Good Matches & Object detection", img_matches );
	imwrite(saveName.c_str(), img_matches);
	timeOfs <<endl;
	return score_avg;
}

void MatchingHelper::binarizePic(string path, string saveFileName, int threshold){
	//IplImage *pSrcImage1 = cvLoadImage(path.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	IplImage *pSrcImage1 = cvLoadImage(path.c_str(), 0);
	pSrcImage1 = binarize(pSrcImage1, saveFileName.c_str(), threshold);
	cvReleaseImage(&pSrcImage1);
}

void MatchingHelper::cannyPic(string path, string saveFileName, int threshold){
	//IplImage *pSrcImage1 = cvLoadImage(path.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	IplImage *pSrcImage1 = cvLoadImage(path.c_str(), 0);
	pSrcImage1 = binarize(pSrcImage1, saveFileName.c_str(), threshold);
	cvReleaseImage(&pSrcImage1);
}

//Path1 is logo path, path2 is scene path
int MatchingHelper::score(string path1, string path2, int method){
	////Perspectve Warp(bin)
	//IplImage *pSrcImage1 = cvLoadImage(path1.c_str(), 0);
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 1);
	//IplImage *pSrcImage2_gray = getWarppedImg(pSrcImage2);

	//pSrcImage1 = binarize(pSrcImage1, "C:/tmp_logo.jpg");
	//pSrcImage2 = binarize(pSrcImage2_gray, "C:/tmp_scene.jpg", 210);
	//Mat img_object = Mat(pSrcImage1, 0);
	//Mat img_scene = Mat(pSrcImage2, 0);

	////Cut and warp
	//IplImage *pSrcImage1 = cvLoadImage(path1.c_str(), 0);
	//IplImage *pSrcImage2 = cvLoadImage(path2.c_str(), 0);
	//IplImage *refinedLogo = cutAndWarp(pSrcImage2);
	//pSrcImage1 = binarize(pSrcImage1, "");
	//refinedLogo = binarize(refinedLogo, "", 180);
	//Mat img_object = Mat(pSrcImage1, 0);
	//Mat img_scene = Mat(refinedLogo, 0);

	//Blur and contrast
	Mat img_object = imread(path1.c_str(), CV_LOAD_IMAGE_UNCHANGED );
	//blur(img_object, img_object, Size(3, 3));
	Mat img_scene = imread(path2.c_str(), CV_LOAD_IMAGE_UNCHANGED );
	//Mat img_scene = Mat::zeros( img_scene2.size(), img_scene2.type() );
	//img_scene2.convertTo(img_scene, -1, 1.2, -50);	//Improve contrast

	////No Change
	//Mat img_object = imread(path1.c_str(), 0);
	//Mat img_scene = imread(path2.c_str(), 0);
	//if( !img_object.data || !img_scene.data )
	//{ std:://cout<< " --(!) Error reading images " << std::endl; return -1; }
	//Prepare cache file name
	string logo_keypointsName =path1.substr(0, path1.find_last_of(".")) + "_keyPoints.yml";
	string scene_keypointsName = path2.substr(0, path2.find_last_of(".")) + "_keyPoints.yml";
	string logo_matName = path1.substr(0, path1.find_last_of(".")) + "_mat.yml";
	string scene_matName = path2.substr(0, path2.find_last_of(".")) + "_mat.yml";

	//-- Step 1: Detect the keypoints using SIFT Detector  --using cache
	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	getKeyPoints(logo_keypointsName, keypoints_object, img_object);
	getKeyPoints(scene_keypointsName, keypoints_scene, img_scene);
	//-- Step 2: Calculate descriptors (feature vectors)   --using cache
	Mat descriptors_object, descriptors_scene;
	getExtractor(logo_matName, keypoints_object, img_object, descriptors_object);
	getExtractor(scene_matName, keypoints_scene, img_scene, descriptors_scene);
	Size logoSize = Size(img_object.cols, img_object.rows);
	Size sceneSize = Size(img_scene.cols, img_scene.rows);
	return score(descriptors_object, descriptors_scene, keypoints_object, keypoints_scene, logoSize, sceneSize);
}

//Path1 is logo path, path2 is scene path
int MatchingHelper::score(const Mat &vec_logo, const Mat &vec_scene,
	const Mat& descriptors_object, const Mat &descriptors_scene,
	Size logoSize, Size sceneSize, int method){
		//Read Keypoint from Mat
		std::vector<KeyPoint> keypoints_object, keypoints_scene;
		//Terminate if one of the Mats is not ready
		if(vec_logo.data == NULL || 
			vec_scene.data == NULL ||
			descriptors_object.data == NULL || 
			descriptors_scene.data == NULL	){
				return -1;
		}
		Mat2KeyPoint(vec_logo, keypoints_object);
		Mat2KeyPoint(vec_scene, keypoints_scene);
		return score(descriptors_object, descriptors_scene, keypoints_object, keypoints_scene, logoSize, sceneSize);
}


int MatchingHelper::score(const Mat &descriptors_object,const Mat &descriptors_scene, 
	const std::vector<KeyPoint> &keypoints_object, const std::vector<KeyPoint> &keypoints_scene, 
	Size logoSize, Size sceneSize, int method){

		//-- Step 3: Matching descriptor vectors using BruteForce matcher and RANSAC filter
		//FlannBasedMatcher matcher;
		BFMatcher matcher(4, true);
		std::vector< DMatch > matches;
		if(descriptors_object.rows > 0 && descriptors_scene.rows > 0){
			matcher.match( descriptors_object, descriptors_scene, matches);
		}

		//-- Get only "good" matches (i.e. whose distance is less than 300 )
		std::vector< DMatch > good_matches;
		for( unsigned i = 0; i < matches.size(); i++ )
		{
			if( matches[i].distance < MY_GOOD_DISTANCE)
			{
				good_matches.push_back( matches[i]); 
			}
		}

		//-- Localize the object from img_1 in img_2 
		std::vector<Point2f> obj;
		std::vector<Point2f> scene;

		int good_size = good_matches.size() > 30 ? 30 : good_matches.size();
		for( size_t i = 0; i < good_matches.size() ; i++ )
		{
			//-- Get the keypoints from the good matches
			obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
			scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
		}

		if(good_matches.size() < 4){
			return 0;
		}

		Mat H = findHomography( obj, scene, RANSAC );

		//-- Get the corners from the image_1 ( the object to be "detected" )
		std::vector<Point2f> obj_corners(4);
		obj_corners[0] = Point(0,0); 
		obj_corners[1] = Point( logoSize.width, 0 );
		obj_corners[2] = Point( logoSize.width, logoSize.height ); 
		obj_corners[3] = Point( 0, logoSize.height );

		//-- Get the mapped object in the scene - image_2 )
		std::vector<Point2f> scene_corners(4);
		perspectiveTransform( obj_corners, scene_corners, H);

		//-- Check whether the mapping is valid
		RectChecker checker;
		bool bonus = false;
		bool rectCheckResult = checker.check(scene_corners, sceneSize.height, sceneSize.width);
		if(rectCheckResult){
			bonus = true;
		}

		//-- Calculate score of this matching
		double avg = 0;			
		int avgCount = 0;			//Number of matches with a distance less than 350
		int goodCount = 0 ;		//Number of matches with a distance less than 300

		for( unsigned i = 0; i < matches.size(); i++ )
		{
			double dist = matches[i].distance;
			if( dist < MY_AVG_DISTANCE && dist > 100)	
			{
				avg += dist;
				avgCount++;
			}
			if(dist < MY_GOOD_DISTANCE && dist > 100){
				goodCount++;
			}
		}
		//-- Too few good match, punish this result
		if(avgCount > 6){
			avg /= avgCount;
			if(goodCount < 12){			
				avg = avg + (12-goodCount) * 4;
			}
		}else{
			avg = MY_MAX_DISTANCE;
		}

		avg = avg > MY_AVG_DISTANCE ? MY_AVG_DISTANCE : avg;
		avg = avg < MY_MIN_DISTANCE ? MY_MIN_DISTANCE : avg;


		double score_avg = (MY_AVG_DISTANCE - avg) / ( MY_AVG_DISTANCE - MY_MIN_DISTANCE ) * 100;

		//-- If found a valid mapping in the scene, then reward a bonus
		if(bonus){
			score_avg += 50;
			score_avg = score_avg > 100 ? 100 : score_avg;
		}else{
			score_avg -= 5;
			score_avg = score_avg < 0 ? 0 : score_avg;
		}
		return score_avg;
}

int MatchingHelper::score(string logo, string scene, string saveName, int method){
	return score(logo, scene);
}
