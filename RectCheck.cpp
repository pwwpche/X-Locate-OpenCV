#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

#include "RectChecker.h"
using namespace std;
using namespace cv;
//struct Point2f{
//
//	Point2f(double xx,double yy){x=xx;y=yy;};
//	double x;
//	double y;
//};

bool check_length(const vector<Point2f> &corners);//判断长度不小于20，并且长短比不超过7
bool check_angle(const vector<Point2f> &corners);//判断角度不小于30度
bool check_intersect(const vector<Point2f> &corners);//判断两边是否相交
double my_max(double len1, double len2){
	return len1 > len2 ? len1 : len2;
}

double my_min(double len1, double len2){
	return len1 < len2 ? len1 : len2;
}

double get_length(const Point2f &s1,const Point2f &s2){
	double lenX = (s1.x - s2.x) * (s1.x - s2.x) ;
	double lenY = (s1.y - s2.y) * (s1.y - s2.y) ;
	double res = sqrt(lenX + lenY);
	return res;
}

bool check_length(const vector<Point2f> &corners){
	double length1 = get_length(corners[0],corners[1]);
	double length2 = get_length(corners[1],corners[2]);
	double length3 = get_length(corners[2],corners[3]);
	double length4 = get_length(corners[3],corners[0]);
	if(length1<20 ||length2<20  || length3<20 ||length4<20 )
		return false;
	double max = my_max( my_max( my_max( length1,length2),length3),length4);
	double min = my_min(my_min(my_min(length1,length2),length3),length4);
	if(max/min >7)
		return false;
	return true;
}

double get_angle(const Point2f &s1,const Point2f &s2,const Point2f &s3)
{
	double cos_val = ((s1.x-s2.x)*(s3.x-s2.x)+(s1.y-s2.y)*(s3.y-s2.y))/(get_length(s1,s2)*get_length(s2,s3));
	//cout<<acos(cos_val)*180/3.1415926535<<"\t";
	return acos(cos_val)*180/3.1415926535;
}

bool check_order(const vector<Point2f> &corners, int height, int width){
	if(corners[0].y + 5 >= corners[3].y || corners[1].y + 5 >= corners[2].y){		//Point 0 should be above Point 3, so do Point1 and Point2
		return false;
	}
	if(corners[0].x + 5 >= corners[1].x || corners[3].x + 5 >= corners[2].x){		//Point 0 should be on the left of Point 1, so do Point3 and Point2
		return false;
	}
	int cnt = 0;
	for(int i = 0 ; i < corners.size() ; i++){			
		if(corners[i].x < 0 || corners[i].x > width){
				cnt++;
				continue;
		}
		if(corners[i].y < 0 || corners[i].y > height){				//This point is outside the image
				cnt++;
		}
	}
	if(cnt == 4){		//All points are outside the image
		return false;
	}
	return true;
}

bool check_angle(const vector<Point2f> &corners){
	if(get_angle(corners[0],corners[1],corners[2])<30)
		return false;
	if(get_angle(corners[1],corners[2],corners[3])<30)
		return false;
	if(get_angle(corners[2],corners[3],corners[0])<30)
		return false;
	if(get_angle(corners[3],corners[0],corners[1])<30)
		return false;
	return true;
}


double direction(Point2f p1,Point2f p2,Point2f p3){
    return (p3.x-p1.x)*(p2.y-p1.y)-(p3.y-p1.y)*(p2.x-p1.x);
}

bool intersect(Point2f p1,Point2f p2,Point2f p3){
    double x_min,x_max,y_min,y_max;
    if(p1.x<p2.x){
        x_min=p1.x;
        x_max=p2.x;
    }else{
        x_min=p2.x;
        x_max=p1.x;
    }
    if(p1.y<p2.y){
        y_min=p1.y;
        y_max=p2.y;
    }else{
        y_min=p2.y;
        y_max=p1.y;
    }
    if(p3.x<x_min || p3.x>x_max || p3.y<y_min || p3.y>y_max)
        return false;
    else
        return true;
}

bool check_intersect(const vector<Point2f> &corners){
    return false;
	Point2f p1 = corners[0];
	Point2f p4 = corners[1];
	Point2f p3 = corners[2];
	Point2f p2 = corners[3];
	double d1=direction(p3,p4,p1);
    double d2=direction(p3,p4,p2);
    double d3=direction(p1,p2,p3);
    double d4=direction(p1,p2,p4);

    if(d1*d2<0 && d3*d4<0)
        return true;
    else if(d1==0 && intersect(p3,p4,p1))
        return true;
    else if(d2==0 && intersect(p3,p4,p2))
        return true;
    else if(d3==0 && intersect(p1,p2,p3))
        return true;
    else if(d4==0 && intersect(p1,p2,p4))
        return true;
    else
        return false;
}

//用于测试的主函数。可以无视
//int main1(){
//	CFileFind cff;
//	CString fstr="C:\\Users\\Administrator\\Desktop\\intel\\Test\\t\\*.txt";
//	BOOL IsMore = cff.FindFile(fstr);
//	map<string, string>my_Map; 
//	fstream fs;
//
//	fs.open("C:\\Users\\Administrator\\Desktop\\intel\\Test\\right.txt");
//	string s;
//	while(!fs.eof()){
//		fs>>s;
//		s= s+".txt";
//		my_Map.insert(pair<string,string>(s,s)); 
//	}
//	fs.close();
//	while(IsMore)
//	{   
//		IsMore=cff.FindNextFile();
//		CString name = cff.GetFileName();
//
//		fs.open("C:\\Users\\Administrator\\Desktop\\intel\\Test\\t\\"+name,ios::in);
//		double s1,s2,s3,s4,s5,s6,s7,s8;
//		fs>>s1;
//		fs>>s1;
//		fs>>s1;
//		fs>>s1;
//		fs>>s2;
//		fs>>s3;
//		fs>>s4;		
//		fs>>s5;
//		fs>>s6;		
//		fs>>s7;
//		fs>>s8;
//		fs.close();
//		vector<Point2f>corners;
//		Point2f c[4] = {Point2f(s1,s2),Point2f(s3,s4),Point2f(s5,s6),Point2f(s7,s8)};
//		for(int i=0;i<4;i++)
//			corners.push_back(c[i]);
//
//		if(check_length(corners) && check_angle(corners) && !check_intersect(corners))
//		{
//			string s = name;
//			if(!my_Map.count(s))
//				cout<<name<<endl;
//		}
//
//	}
//	system("pause");
//}

int main_rc(int argc, char** argv){
#if 1
	if(argc != 2){
		return -1;
	}
	string path = argv[1];
#else
	string path = "C:\\save.txt";
#endif
	ifstream ifs( path, std::ios::in);
	ofstream ofs("C:\\res.txt", std::ios::app);
	int imgWidth, imgHeight;
	int cornerCnt;
	int goodMatchCnt;
	bool result = false;
	vector<Point2f> corners;
	float tempX = 0;
	float tempY = 0;
	while(!ifs.eof()){
		ifs >> imgWidth;
		ifs >> imgHeight;
		ifs >> cornerCnt;
		if(cornerCnt == 0){
			break;
		}
		for(int i = 0 ; i < 4 ; i++){
			ifs >> tempX;
			ifs >> tempY;
			corners.push_back(Point2f(tempX, tempY));
		}
		RectChecker checker;
		bool result = checker.check(corners, imgHeight, imgWidth);
		if(result){
			ofs << path << endl;
		}
		break;
	}
	ifs.close();
	ofs.close();
	return 0;
}