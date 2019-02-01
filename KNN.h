#ifndef KNN_H
#define KNN_H
#include <vector>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include<io.h>
using namespace std;
class KNN
{
public:
	int KNN_Classify(string filename);
private:
	void GetAllFormatFiles(string path, vector<string>& files, string format);//获取文件列表
	vector<int> insertSort(vector<int> nums);//返回原始队列索引的排序算法
	vector<int> imageToVector(string fileName);//读取文本文件内容，存入一维向量
	int classify(vector<int> inX, vector<vector<int>> dataSet, vector<char> labels, int k);//KNN算法的实现


	vector<int> imageToVector();
	vector<vector<int>> traingMat;
	vector<char> labels;
};
#endif