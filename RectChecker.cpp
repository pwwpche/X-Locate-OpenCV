#include "RectChecker.h"
double RectChecker::my_max(double len1, double len2){
	return len1 > len2 ? len1 : len2;
}

double RectChecker::my_min(double len1, double len2){
	return len1 < len2 ? len1 : len2;
}

double RectChecker::get_length(const Point2f &s1,const Point2f &s2){
	double lenX = (s1.x - s2.x) * (s1.x - s2.x) ;
	double lenY = (s1.y - s2.y) * (s1.y - s2.y) ;
	double res = sqrt(lenX + lenY);
	return res;
}

bool RectChecker::check_length(const vector<Point2f> &corners){
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

double RectChecker::get_angle(const Point2f &s1,const Point2f &s2,const Point2f &s3)
{
	double cos_val = ((s1.x-s2.x)*(s3.x-s2.x)+(s1.y-s2.y)*(s3.y-s2.y))/(get_length(s1,s2)*get_length(s2,s3));
	//cout<<acos(cos_val)*180/3.1415926535<<"\t";
	return acos(cos_val)*180/3.1415926535;
}

bool RectChecker::check_order(const vector<Point2f> &corners, int height, int width){
	if(corners[0].y + 5 >= corners[3].y || corners[1].y + 5 >= corners[2].y){		//Point 0 should be above Point 3, so do Point1 and Point2
		return false;
	}
	if(corners[0].x + 5 >= corners[1].x || corners[3].x + 5 >= corners[2].x){		//Point 0 should be on the left of Point 1, so do Point3 and Point2
		return false;
	}
	int cnt = 0;
	for(int i = 0 ; i < corners.size() ; i++){			
		if(corners[i].x < -30 || corners[i].x > width + 30){
				cnt++;
				continue;
		}
		if(corners[i].y < -20 || corners[i].y > height + 20){				//This point is outside the image
				cnt++;
		}
	}
	if(cnt == 4){		//All points are outside the image
		return false;
	}
	return true;
}

bool RectChecker::check_angle(const vector<Point2f> &corners){
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

bool RectChecker::check(vector<Point2f> corners, int imgHeight, int imgWidth){
	if(check_order(corners, imgHeight, imgWidth) && check_length(corners) && check_angle(corners)){
		return true;
	}else{
		return false;
	}
}
