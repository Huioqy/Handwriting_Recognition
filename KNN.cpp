#include"KNN.h"
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>  
#include <Windows.h>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include<io.h>
#include<string>
#include<vector>
using namespace cv;
using namespace std;


/**********************************获取文件列表********************************/
void KNN::GetAllFormatFiles(string path, vector<string>& files, string format)
{
	//文件句柄    
	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表 
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}


/*****************************返回原始队列索引的排序算法-********************************/
vector<int> KNN::insertSort(vector<int> nums)
{
	vector<int> sortedIndx;
	for (int i = 0; i<nums.size(); i++){
		sortedIndx.push_back(i);
	}
	for (int j = 1; j < nums.size(); j++){
		int key = nums[j];
		int indx = sortedIndx[j];
		int i = j - 1;
		while (i >= 0 && nums[i]>key)
		{
			nums[i + 1] = nums[i];
			sortedIndx[i + 1] = sortedIndx[i];
			i--;
		}
		nums[i + 1] = key;
		sortedIndx[i + 1] = indx;
	}
	return sortedIndx;
}


/****************************读取文本文件内容，存入一维向量*****************************/
vector<int> KNN::imageToVector(string fileName)
{
	vector<int> returnVector;
	fstream infile;
	infile.open(fileName, ios::in);
	while (!infile.eof())
	{
		char buffer[256];
		infile.getline(buffer, 256);
		for (int i = 0; i < 32; i++){
			returnVector.push_back(buffer[i] - 48);		// 字符转int [3/4/2015 pan]
		}
	}
	return returnVector;
}


/*************************************KNN算法的实现*************************************/
// inX 待分类向量，dataSet 训练数据集，labels 训练数据的类别(0,1,2,3,4,5,6,7,8,9)，k  
int KNN::classify(vector<int> inX, vector<vector<int>> dataSet, vector<char> labels, int k)
{
	int dataSetSize = dataSet.size();
	int labelsum = 0;
	vector<int> distances;
	for (int i = 0; i < dataSetSize; i++){
		int sum = 0;
		for (int j = 0; j < inX.size(); j++){
			int tmp = inX[j] - dataSet[i][j];
			tmp *= tmp;
			sum += tmp;
		}
		sum = sqrt(double(sum));
		distances.push_back(sum);
	}
	vector<int> sortedDistIndix;
	sortedDistIndix = insertSort(distances);
	/*for (int i = 0; i < k; i++){

		labelsum += labels[sortedDistIndix[i]];
	}
	return labelsum / k + 0.5;
	*/
	//*********************改*******************************//
	double km = double(k) / 2;
	int resultlabel[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < k; i++){
		int r = labels[sortedDistIndix[i]];
		resultlabel[r] = resultlabel[r] + 1*(double(k-i-km)/k+1);
	}
	int max = 0;
	int notice = 0;
	for (int i = 0; i < 10; i++)
	{
		if (resultlabel[i] > max)
		{
			max = resultlabel[i];
			notice = i;
		}
	}
	if (max == 1) return labels[sortedDistIndix[0]];
	else return notice;
	//*********************改*******************************//
}


//***************************最终测试********************************************************/
int KNN::KNN_Classify(string filename)
{
	string path = "..\\MNISTDigits";
	string format = "txt";
	vector<string> files;
	GetAllFormatFiles(path, files, format);
	fstream infile;
	vector<vector<int>> traingMat;
	vector<char> labels;
	for (int i = 0; i < files.size(); i++){
		//cout << path.size();
		string str;
		str.assign(files[i], path.size() + 1, 1);
		const char *c = str.c_str();
		labels.push_back(*c - 48);
		traingMat.push_back(imageToVector(files[i]));
	}
	vector<int> inX = imageToVector(filename);

	int result = classify(inX, traingMat, labels, 3);
	//cout << result << endl;
	return result ;
}


