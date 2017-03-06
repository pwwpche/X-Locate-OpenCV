#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// Global Variables
Mat img_tm; 
Mat templ; 
Mat result_tm;
char* image_window = "Source Image";
char* result_window = "result_tm window";

int match_method;
int max_Trackbar = 5;

/// Function Headers
void MatchingMethod( int, void* );

/** @function main */
int main_tm( int argc, char** argv )
{
  /// Load image and template

  templ = imread( "C:/1.jpg", 1 );
    img_tm = imread( "C:/2.jpg", 1 );
  /// Create windows
  namedWindow( image_window, CV_WINDOW_AUTOSIZE );
  namedWindow( result_window, CV_WINDOW_AUTOSIZE );

  /// Create Trackbar
  char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
  createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );

  MatchingMethod( 0, 0 );

  waitKey(0);
  return 0;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod( int, void* )
{
  /// Source image to display
  Mat img_display;
  img_tm.copyTo( img_display );

  /// Create the result_tm matrix
  int result_cols =  img_tm.cols - templ.cols + 1;
  int result_rows = img_tm.rows - templ.rows + 1;

  result_tm.create( result_cols, result_rows, CV_32FC1);

  /// Do the Matching and Normalize
  matchTemplate( img_tm, templ, result_tm, match_method );
  
  //normalize( result_tm, result_tm, 0, 1, NORM_MINMAX, -1, Mat() );
  cout << result_tm << endl;

  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( result_tm, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

  /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
  if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
  else
    { matchLoc = maxLoc; }

  /// Show me what you got
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result_tm, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

  imshow( image_window, img_display );
  imshow( result_window, result_tm );

  return;
}