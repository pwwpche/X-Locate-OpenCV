#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
int main_sg(int argc, char** argv){
#if 1
	if(argc != 2){
		return 0;
	}
	string fileName = argv[1];
#else
	string fileName = "C:\\score.txt";
#endif
	ifstream ifs(fileName, std::ios::in);
	string fileName2 = fileName;
	string sortFileName = fileName2.insert(fileName.find_last_of("/\\") + 1, "sort_");



	//ofstream sortOut(sortFileName, std::ios::out);
	vector<double> score;
	double result;
	//Read score from file
	while(!ifs.eof()){
		double temp = -1;
		ifs>>temp;
		if(temp == -1){
			break;
		}
		score.push_back(temp);
		
	}

	vector<double> scoreSort(score);
	sort(scoreSort.begin(), scoreSort.end());
	int size = scoreSort.size();
	double avg = 0 ;
	int maxIdx = -1;	//Index of the first number > 350
	int midIdx = -1;
	for(int i = 0 ; i < size ; i++){
		//sortOut<<scoreSort[i]<<endl;
		if(scoreSort[i] < 380){
			avg += scoreSort[i];
		}else{
			maxIdx = i ; 
			break;
		}
	}
	if(maxIdx != -1){
		avg = avg / (maxIdx + 1);
	}

	if(maxIdx > 40){
		midIdx = maxIdx / 2;
	}else	if(maxIdx <= 40 && maxIdx > 20){
		maxIdx = 20;
	}else if(maxIdx <= 20 && maxIdx > 12){
		midIdx = maxIdx;
	}else{
		midIdx = -1;
	}

	if(midIdx == -1){
		result = 0;
	}else{
		double midDistance = scoreSort[midIdx];
		midDistance = midDistance < 230 ? 230 : midDistance;
		midDistance = midDistance > 380 ? 380 : midDistance;
		result = (380 - midDistance) / 1.5;
	}


	//sortOut.close();


	//Write result to log
	string logName = fileName.substr(0, fileName.find_last_of("/\\") + 1) + "!log_score.txt";
	ofstream ofs(logName, std::ios::app);
	string fileNum = fileName.substr(fileName.find_last_of("/\\") + 1 , fileName.find_last_of(".") - fileName.find_last_of("/\\") - 1);
	ofs << fileNum << "\t" << result << "\t" << endl;
	return 0;
}