#include <highgui.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <map>
#include <cvaux.h>
using namespace std;
typedef pair<int,int> PointPair;
const int INF=240000;

IplImage *bfsImg=cvLoadImage("C:/floors/B1F.jpg",1); //黑白轮廓图
IplImage *mapImg = cvLoadImage("C:/B1F.png",1);
CvScalar s;

bool visited[605][405];
vector<PointPair> queue;
vector<int> len;
vector<int> path;
vector<PointPair> resPath; //结果存在resPath中

struct MyRect{
	//x_min,y_min,x_max,y_max
	int pos[4];
	MyRect(int a,int b,int c,int d){
		pos[0]=a-1; //偏移
		pos[1]=b-1;
		pos[2]=c+1;
		pos[3]=d+1;
	}
};

struct MyLine{
	PointPair start;
	PointPair end;
	int length;
	int direction;
};
int pairs[16][2][2]={ 
	//第i对点为(a.pos[pairs[i][0][0]],a.pos[pairs[i][0][1]])
	//       和(b.pos[pairs[i][1][0]],b.pos[pairs[i][1][1]])
	0,1,0,1,0,1,2,1,0,1,0,3,0,1,2,3,
	2,1,0,1,2,1,2,1,2,1,0,3,2,1,2,3,
	0,3,0,1,0,3,2,1,0,3,0,3,0,3,2,3,
	2,3,0,1,2,3,2,1,2,3,0,3,2,3,2,3
};
int dir[4][2]={0,1,1,0,0,-1,-1,0};

int color(int x,int y) //black or white
{
	s=cvGet2D(bfsImg,x,y);
	if(s.val[0]>=248 && s.val[0]<=255) return 255;
	if(s.val[0]>=0 && s.val[0]<=7) return 0;
	return 0;
}

void ssetcolor(int x,int y,int tmp)
{
	s.val[0]=tmp;
    s.val[1]=tmp;
    s.val[2]=tmp;
    cvSet2D(bfsImg,x,y,s);//sset the (i,j) pixel value
}

void deal_color()
{
	printf("%d %d\n",bfsImg->width,bfsImg->height);
    for(int i=0;i<bfsImg->width;i++){
        for(int j=0;j<bfsImg->height;j++){
			// get the (i,j) pixel value
			//printf("B=%f, G=%f, R=%f\n ",s.val[0],s.val[1],s.val[2]);
			int tmp=color(j,i); //先传高度维，再传宽度维
			ssetcolor(j,i,tmp);
        }
    }
	cvSaveImage("d://1F_s.jpg",bfsImg); 
}

int bfs(PointPair startPnt,PointPair endPnt)
{
	int width=bfsImg->width;
	int height=bfsImg->height;
	memset(visited,0,sizeof(visited));
	queue.clear();
	len.clear();
	path.clear();
	queue.push_back(startPnt);
	len.push_back(0);
	path.push_back(-1);
	visited[startPnt.first][startPnt.second]=true;
	int head=0;

	while(1){
		if(head==queue.size()) return INF;
		PointPair now=queue[head++];
		int x=now.first,y=now.second;
		int now_len=len[head-1];
		for(int i=0;i<4;i++){
			int nx=x+dir[i][0],ny=y+dir[i][1];
			if(!(nx>=0 && ny>=0 && nx<width && ny<height)) continue;
			if(color(ny,nx)!=255 || visited[nx][ny]!=false) continue;
			queue.push_back(PointPair(nx,ny));
			len.push_back(now_len+1);
			path.push_back(head-1);
			visited[nx][ny]=true;
			if(PointPair(nx,ny)==endPnt) return len[len.size()-1];
		}
	}
}

void getPath()
{
	resPath.clear();
	int now=path.size()-1;
	while(1){
		resPath.push_back(queue[now]);
		now=path[now];
		if(now==-1) return;
	}
}

vector<PointPair>& deal_path(MyRect& startPnt,MyRect& endPnt)
{
	int res=INF,tmp;
	for(int i=0;i<16;i++){
		if(color(startPnt.pos[pairs[i][0][1]],startPnt.pos[pairs[i][0][0]])!=255 ||
		   color(endPnt.pos[pairs[i][1][1]],endPnt.pos[pairs[i][1][0]])!=255) continue;
		int now=bfs(PointPair(startPnt.pos[pairs[i][0][0]],startPnt.pos[pairs[i][0][1]]),
					PointPair(endPnt.pos[pairs[i][1][0]],endPnt.pos[pairs[i][1][1]]));
		if(now<res){
			res=now;
			tmp=i;
			getPath();
		}
	}
	//结果存在resPath中
	return resPath;
}

int getLinePoints(const vector<PointPair>& myPath, vector<PointPair> &startPoints, vector<PointPair>& endPoints, vector<int> lineDirection){
	size_t pathSize = myPath.size();
	if(pathSize < 2){
		startPoints.push_back(myPath[0]);
		endPoints.push_back(myPath[1]);
		return 0;
	}
	//获取路径起点
	PointPair startPoint = myPath[0];	//记录直线的起点和终点
	PointPair endPoint = myPath[0];
	int sameDirCount = 0;
	int lastDirection = myPath[0].first == myPath[1].first ? 1 : 2;		//路径初始时方向
	int thisDirection = 0;
	for(size_t i = 1 ; i < pathSize ; i++){
		
		if(myPath[i].first == myPath[i - 1].first ){		//前后两点横坐标相同
			thisDirection = 1;
		}else	if(myPath[i].second == myPath[i - 1].second ){		//前后两点纵坐标相同
			thisDirection = 2;
		}
		if(thisDirection == lastDirection){		//连续的直线
			sameDirCount++;
		}else{
			if(sameDirCount > 7){		//直线长度大于7，为有效直线
				startPoints.push_back(startPoint);
				endPoint = myPath[i-1];
				endPoints.push_back(endPoint);	//记录上一条直线终点
				if(lastDirection == 1){	//记录直线方向（横左，横右，纵上，纵下）
					if(startPoint.second < myPath[i-1].second){
						lastDirection = -lastDirection;
					}
				}else{
					if(startPoint.first < myPath[i-1].first){
						lastDirection = -lastDirection;
					}
				}
				lineDirection.push_back(lastDirection);
			}
			startPoint = myPath[i];		//重置起点和直线长度
			sameDirCount = 0;
		}
		lastDirection = thisDirection;	
	}
	if(sameDirCount > 7){		//记录最后一点和终点是否构成直线
		startPoints.push_back(startPoint);
		endPoint = myPath[pathSize - 1];
		endPoints.push_back(endPoint);	
	}

}


bool checkLine(PointPair startPoint, PointPair endPoint){
	//Get Information of the image
	
	int height = bfsImg->height;
	int width = bfsImg->width;
	int step = bfsImg->widthStep;
	int channels  = bfsImg->nChannels;
	uchar* data = (uchar *)bfsImg->imageData;
	if(startPoint.first < 0 || startPoint.first >= width || startPoint.second < 0 || startPoint.second > height){
		return false;
	}
	if(endPoint.first < 0 || endPoint.first >= width || endPoint.second < 0 || endPoint.second > height){
		return false;
	}
	//Inverse color pixel by pixel
	for(unsigned i = startPoint.second ; i < endPoint.second ; i++){
		for(int j=startPoint.first ; j < endPoint.first ; j++) {
			if(data[i*step+j*channels+1] < 30){
				return false;
			}
		}
	}
	return true;
}
//Deprecated
vector<PointPair> getPath(PointPair startPoint, PointPair endPoint){
	if(startPoint.first == endPoint.first || startPoint.second == endPoint.second){
		vector<PointPair> result;
		for(int i = startPoint.first ; i <= endPoint.first ; i++){
			for(int j = startPoint.second ; j < endPoint.second ; j++){
				PointPair point;
				point.first = i;
				point.second = j;
				result.push_back(point);
			}
		}
		return result;
	}else{
		bfs(startPoint, endPoint);
		getPath();
		vector<PointPair> result(resPath);
		return result;
	}
}

vector<PointPair> roadCorrection(const vector<PointPair>& myPath, vector<PointPair> &startPoints, vector<PointPair>& endPoints, vector<int> lineDirection ){
	vector<PointPair> originalRoad(myPath);
	vector<PointPair> result;
	int currentLine = 0;
	if(startPoints.size() < 1){
		return myPath;
	}

	//将直线端点平移到道路中央
	int lineCount = startPoints.size();
	for(int i = 0 ; i < lineCount ; i++){
		PointPair startPoint = startPoints[i];
		PointPair endPoint = endPoints[i];
		PointPair newStart = startPoint;
		PointPair newEnd = endPoint;
		int direction = startPoint.first == endPoint.first ? 1 : 2;		//获取直线方向
		int modifyDirection = 0;

		if(direction == 1){		// 判断修正方向
			newStart.first--;
			newEnd.first--;
			if(checkLine(newStart, newEnd) == false) {	//
				modifyDirection = 1;
			}else{
				modifyDirection = -1;
			}
		}else{
			newStart.second--;
			newEnd.second--;
			if(checkLine(newStart, newEnd) == false) {
				modifyDirection = 1;
			}else{
				modifyDirection = -1;
			}
		}
		int spaceCount = 0;
		for(int k = 0 ; k < 70 ; k++){		//将直线平移
			newStart = startPoint;
			newEnd = endPoint;
			if(direction == 1){		//计算新的起点终点坐标
				newStart.first += k * modifyDirection;
				newEnd.first += k * modifyDirection;
			}else{
				newStart.second += k * modifyDirection;
				newEnd.second += k * modifyDirection;
			}
			if(checkLine(newStart, newEnd)){		//检查是否与黑色部分重合
				spaceCount++;
			}else{
				break;
			}
		}
		if(direction == 1){		//将直线移到走廊范围二分之一处
			startPoints[i].first += spaceCount / 2 * modifyDirection;
			endPoints[i].first += spaceCount / 2 * modifyDirection; 
		}else{
			startPoints[i].second += spaceCount / 2 * modifyDirection;
			endPoints[i].second += spaceCount / 2 * modifyDirection; 
		}
	}

	//获取修正后的路线
	vector<PointPair> getFullPath;
	vector<MyLine>  resultLines;
	PointPair start =  myPath[0];
	if(start.first != startPoints[0].first || start.second != startPoints[0].second){		//总路径起点和第一条直线起点相同
		MyLine line;
		line.start = myPath[0];
		line.end = startPoints[0];
		line.direction = 3;
		resultLines.push_back(line);
	}

	for(size_t i = 0 ; i < lineCount ; i++){
		MyLine line;	
		line.start = startPoints[i];
		line.end = endPoints[i];
		line.direction = lineDirection[i];
		resultLines.push_back(line);


	}
	return myPath;
}
//第一维i是宽度,第二位j是高度
int main_bfs()
{
	//deal_color();

	//起点和终点
	MyRect startPnt(354,21,400,67);
	//MyRect endPnt(307,182,330,202);	
	MyRect endPnt(322,220,342,227);	
	vector<PointPair> &myPath=deal_path(startPnt,endPnt);
	vector<PointPair> startPoints, endPoints;
	vector<int> lineDirection;
	//getLinePoints(myPath, startPoints, endPoints, lineDirection);
//	myPath = roadCorrection(myPath, startPoints, endPoints);
	if(myPath.size()==0) printf("can not connect!\n");
	//myPath

	s.val[0]=234;
	s.val[1]=176;
	s.val[2]=0;
	for(int i=0;i<myPath.size();i++){
		int x=myPath[i].first;
		int y=myPath[i].second;
		cvSet2D(mapImg,y,x,s);
	}
	
	cvSaveImage("D://1F_path.jpg",mapImg); 

	system("pause");
	return 0;
}
