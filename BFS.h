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
#include <math.h>
#include <cvaux.h>
using namespace std;

typedef pair<int,int> PointPair;
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

class  Road{
public:
	Road(IplImage* pic){
		bfsImg = cvCloneImage(pic);
		mapImg = cvCloneImage(pic);
	}

	cv::Mat get_mat(int points[8]){
		MyRect startPnt(points[0],points[1],points[2],points[3]);
		MyRect endPnt(points[4],points[5],points[6],points[7]);	
		//----------------------------------------------
		int i;
		vector<PointPair> myPath=deal_path(startPnt,endPnt);
		vector<PointPair> startPoints;
		int height = bfsImg->height;
		int width = bfsImg->width;
		IplImage* pic = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,4);//result
		road_move(myPath, startPoints);
		for(int k=0;k<2;k++){
			for(i=1;i<startPoints.size()-1;i++){
				startPoints[i].first = (startPoints[i-1].first+startPoints[i+1].first)/2;
				startPoints[i].second = (startPoints[i-1].second+startPoints[i+1].second)/2;
			}
		}
		draw_road(startPoints,pic);
		change_alpha(pic);
		//over
		cvSaveImage("C:\\pic.png",pic); 
		cv::Mat img(pic,0);
		return img;
	}
private:
	IplImage *bfsImg;
	IplImage *mapImg;
	vector<int> len;
	vector<int> path;
	vector<PointPair> resPath; 

	int color(int x,int y) //black or white
	{
		CvScalar s;
		s=cvGet2D(bfsImg,x,y);
		if(s.val[0]>=248 && s.val[0]<=255) return 255;
		if(s.val[0]>=0 && s.val[0]<=7) return 0;
		return 0;
	}

	void ssetcolor(int x,int y,int tmp)
	{
		CvScalar s;
		s.val[0]=tmp;
		s.val[1]=tmp;
		s.val[2]=tmp;
		cvSet2D(bfsImg,x,y,s);//sset the (i,j) pixel value
	}

	int bfs(PointPair startPnt,PointPair endPnt,vector<PointPair> &queue)
	{
		int width=bfsImg->width;
		int height=bfsImg->height;
		bool visited[605][405];
		memset(visited,0,sizeof(visited));
		queue.clear();
		len.clear();
		path.clear();
		queue.push_back(startPnt);
		len.push_back(0);
		path.push_back(-1);
		visited[startPnt.first][startPnt.second]=true;
		int head=0;
		int dir[4][2]={0,1,1,0,0,-1,-1,0};
		const int INF=240000;
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

	void getPath(vector<PointPair> queue)
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
		const int INF=240000;
		int res=INF,tmp;
		vector<PointPair> queue;
		int pairs[16][2][2] ={ 
			0,1,0,1,0,1,2,1,0,1,0,3,0,1,2,3,
			2,1,0,1,2,1,2,1,2,1,0,3,2,1,2,3,
			0,3,0,1,0,3,2,1,0,3,0,3,0,3,2,3,
			2,3,0,1,2,3,2,1,2,3,0,3,2,3,2,3
		};
		for(int i=0;i<16;i++){
			if(color(startPnt.pos[pairs[i][0][1]],startPnt.pos[pairs[i][0][0]])!=255 ||
				color(endPnt.pos[pairs[i][1][1]],endPnt.pos[pairs[i][1][0]])!=255) continue;
			int now=bfs(PointPair(startPnt.pos[pairs[i][0][0]],startPnt.pos[pairs[i][0][1]]),
				PointPair(endPnt.pos[pairs[i][1][0]],endPnt.pos[pairs[i][1][1]]),queue);
			if(now<res){
				res=now;
				tmp=i;
				getPath(queue);
			}
		}
		//结果存在resPath中
		return resPath;
	}

	//Deprecated
	vector<PointPair> getPath(PointPair startPoint, PointPair endPoint,vector<PointPair> &queue){
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
			bfs(startPoint, endPoint,queue);
			getPath(queue);
			vector<PointPair> result(resPath);
			return result;
		}
	}




	//--------------------------------------------------------------------------
	bool get_dir(PointPair p1,PointPair p2){ // true:up down,o.w right.left 
		int x = abs(p1.first-p2.first);
		int y = abs(p1.second-p2.second);
		if(x-y <= 0)
			return true;
		else
			return false;
	}

	int get_movedis(PointPair p,bool flag){
		int x = p.first;
		int y = p.second;
		int i1 = 0;
		int i2 = 0;
		if(flag){
			while(cvGet2D(bfsImg,y,x+i1).val[0] > 30)
				i1--;
			while(cvGet2D(bfsImg,y,x+i2).val[0] > 30)
				i2++;
		}
		else{
			while(cvGet2D(bfsImg,y+i1,x).val[0] > 30)
				i1--;
			while(cvGet2D(bfsImg,y+i2,x).val[0] > 30)
				i2++;	
		}
		int res = (i1+i2)/2;
		if(i1 <= -10){
			if(i2 < 10)
				res = (-10+i2)/2;
			else
				res = 0;
		}
		else{
			if(i2 >= 10)
				res = (i1+10)/2;
		}

		return res;
	}

	void road_move(const vector<PointPair>& myPath, vector<PointPair> &res){
		size_t pathSize = myPath.size();
		int range = 2;
		if(pathSize <= range){
			return ;
		}
		int i;
		bool flag;
		PointPair new_p;
		res.push_back(myPath[0]);
		for(i=range;i<pathSize-range;i+=range){
			if(flag=get_dir(myPath[i],myPath[i+range])){
				new_p.first = myPath[i].first+get_movedis(myPath[i],flag);
				new_p.second = myPath[i].second;
				res.push_back(new_p);
			}
			else{
				new_p.first = myPath[i].first;
				new_p.second = myPath[i].second+get_movedis(myPath[i],flag);
				res.push_back(new_p);		
			}
		}

		res.push_back(myPath[pathSize-1]);
	} 

	void draw_road(const vector<PointPair>startPoints,IplImage* mapImg){
		int i;
		for(i=0;i<startPoints.size()-1;i++){
			cvLine(mapImg,cvPoint(startPoints[i].first,startPoints[i].second),
				cvPoint(startPoints[i+1].first,startPoints[i+1].second),cvScalar(250,250,0,255),2);		
		}
		cvCircle(mapImg,cvPoint(startPoints[0].first,startPoints[0].second), 2,cvScalar(0,0,180,255),-1);
		cvCircle(mapImg,cvPoint(startPoints[i].first,startPoints[i].second), 2,cvScalar(0,0,180,255),-1);
	}

	void change_alpha(IplImage* pic){
		int height = pic->height;
		int width = pic->width;
		for(int i=0;i<width;i++){
			for(int j=0;j<height;j++){
				if(cvGet2D(pic,j,i).val[3] != 255)
					cvSet2D(pic,j,i,cvScalar(255,255,255,0));
			}	
		}
	}
};


