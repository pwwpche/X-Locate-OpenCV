#include "Match_MatchingHelper.h"


vector<pair<int, int> > MatchingHelper::horizonAreas(IplImage* img){
	//Get Information of the image
	unsigned height = img->height;
	unsigned width = img->width;
	unsigned step = img->widthStep;
	int channels  = img->nChannels;
	uchar* data = (uchar *)img->imageData;

	int blackCount = 0;
	int whiteCount = 0;

	//Inverse color pixel by pixel
	unsigned i = 0 , j = 0;
	int startY = 0, endY = 0;
	vector<pair<int, int> > result;
	for(i = 0 ; i < height ; i++){
		for(j = 0 ; j < width ; j++) {
			if(data[i*step+j*channels+1] == 255){
				endY = i;		//Record this line's Y axis
				break;
			}
		}
		if(j == width)	//This is a black line.
		{
			if(startY != endY)		//end of a white area
			{
				if(endY - startY > 3){
					result.push_back( std::make_pair(startY, endY - 1) );
				}
			}
			startY = i;
			endY = i;
		}
	}
	if(startY != endY)		//end of a white area
	{
		result.push_back( std::make_pair(startY, endY - 1) );
	}
	return result;
}

void MatchingHelper::cutImage(IplImage* g_pBinaryImage, const char* saveFileName ){
	vector<pair<int, int> > areas = horizonAreas(g_pBinaryImage);

	//Save image part by part
	unsigned size = areas.size();
	for(unsigned i = 0 ; i < size ; i++)
	{
		int width = g_pBinaryImage->width;
		int height = areas[i].second - areas[i].first;
		cvSetImageROI(g_pBinaryImage, cvRect(0, areas[i].first, width, height));
		//dst = cvCreateImage(cvSize(width, height), 
		//	IPL_DEPTH_8U, g_pBinaryImage->nChannels);  
		//cvCopy(g_pBinaryImage,dst,0);  


		//Extract file name
		string path0 = saveFileName;
		unsigned found = path0.find_last_of("/\\");
		string filename = path0.substr(found+1);
		stringstream ss;
		ss << i;
		string saveName ="E:/logo_cut/" +  filename.insert(filename.find_last_of(".") , "_" + ss.str() );

		//Save file
		cvSaveImage( saveName.c_str() , g_pBinaryImage);
		cvResetImageROI(g_pBinaryImage);  
	}
	//string saveName = filename.insert(filename.find_last_of(".") , "i");
	//validChecker.binarizePic(path0, "E:/logo2/" + saveName);
}

void MatchingHelper::cutImage(IplImage* g_pBinaryImage, cv::Rect rect, string saveFileName ){
		int width = rect.width;
		int height = rect.height;
		cvSetImageROI(g_pBinaryImage, rect);

		//Extract file name
		string path0 = saveFileName;
		string saveName ="E:/cut/" +  saveFileName ;

		//Save file
		cvSaveImage( saveName.c_str() , g_pBinaryImage);
		cvResetImageROI(g_pBinaryImage);  
	

}

