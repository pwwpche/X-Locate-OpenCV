#include "BFS.h"
using namespace std;
int main_bfs()
{
	IplImage* bfsImg = cvLoadImage("C:\\1F_s.jpg",1);
	Road road(bfsImg);
	int i[8] = {354, 21, 400, 67, 322, 220, 342, 227};
	cv::Mat mat = road.get_mat(i);
	return 0;
}
