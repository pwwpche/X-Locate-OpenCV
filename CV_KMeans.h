
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <vector>
#include<string>
using namespace std;

class KMeans{
	int execute(string imgPath){
		IplImage * im_src = cvLoadImage(imgPath.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		IplImage * im_show = cvCreateImage(cvGetSize(im_src),8,3);
		cvZero(im_show);
		cvThreshold(im_src,im_src,210,255,CV_THRESH_BINARY);
    
		int nwidth = im_src->width;
		int nheight = im_src->height;
		int nwidthstep = im_src->widthStep;
    
		unsigned char *ppiexl = NULL;
		int ntotal = 0;
		for (int i=0;i<nheight;++i)
		{
			ppiexl =(unsigned char *) im_src->imageData + i * nwidthstep;
			for (int j=0;j<nwidth;++j)
			{
				if (ppiexl[j] ==255)
				{
					ntotal++;
				}
            
			}
		}
		CvMat * sample = cvCreateMat(ntotal,1,CV_32FC2); 
		CvMat * cluster = cvCreateMat(ntotal,1,CV_32SC1);
    
		int ntag = 0;
		for (int i=0;i<nheight;++i)
		{
			ppiexl =(unsigned char *) im_src->imageData + i * nwidthstep;
			for (int j=0;j<nwidth;++j)
			{
				if (ppiexl[j] ==255)
				{
					sample->data.fl[ntag*2] = i; //ÐÐ
					sample->data.fl[ntag*2+1]=j; //ÁÐ
					ntag++;
				}
			}
		}
		cvKMeans2(sample,4,cluster,cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 ),5);
		CvScalar color[4] = {cvScalar(0,255,0,0),
								cvScalar(0,0,255,0),
								cvScalar(255,0,0,0),
								cvScalar(0,255,255,0)};
		for (int i = 0; i < ntotal ;++i)
		{
			int nrow = sample->data.fl[i*2];
			int ncol = sample->data.fl[i*2+1];
        
			cvSet2D(im_show,nrow,ncol,color[cluster->data.i[i]]);
		}
		cvShowImage("s",im_show);
		cvShowImage("ss",im_src);
		cvWaitKey(-1);
		return 0;
	}
};
