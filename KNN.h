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
	void GetAllFormatFiles(string path, vector<string>& files, string format);//��ȡ�ļ��б�
	vector<int> insertSort(vector<int> nums);//����ԭʼ���������������㷨
	vector<int> imageToVector(string fileName);//��ȡ�ı��ļ����ݣ�����һά����
	int classify(vector<int> inX, vector<vector<int>> dataSet, vector<char> labels, int k);//KNN�㷨��ʵ��


	vector<int> imageToVector();
	vector<vector<int>> traingMat;
	vector<char> labels;
};
#endif