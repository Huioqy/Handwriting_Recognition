#pragma once
#define _WINSOCKAPI_ 
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream> 
#include <math.h> 
#include<fstream>
#include<string>
#include<stdio.h>
#include <NuiApi.h>
#include "Global.h"
#include "KNN.h"
/*
#include "UserTrackingService.h"
#include "ProtectedClient.h"
#include "Inputs_constants.h"
#include "ThriftTools.hpp"
*/
using namespace std;
using namespace cv;

class HandWriting
{
public:
	HandWriting();
	~HandWriting();
	void getFingetTip();
	void HandExtraciotn();
	void FingerNumberDetect();
	void Interface();
	void getDatafromKinect2(Mat useridimage, Mat depthimage, bool bodydepthdectected, bool skeletondectected);
	void getDatafromKinect1(Mat depthimage, Mat colorimage, bool bodydepthdectected, bool skeletondectected, bool sayhello, bool saygoodbye);
	//void getBodyPoint(Point a, Point b, Point c, Point d, Point e, HandState LHState, HandState RHState); //KINECT2
	void getBodyPoint(Point a, Point b, Point c, Point d, Point e);

	void	SendToServer(int nGestureID);
	
private:

	void showdepthImageColor();
	void MattoTxt(Mat Track);
	void MattoJPG1(Mat Track);
	void MattoJPG2(Mat Track);
	Mat extractandtransform(Mat track);
	void Getnum(string &s1, string &s2);
	void shownumresult(int result);
	string TXT01();

	Mat userIdImage;
	Mat colorImage;
	Mat depthImage;
	Mat depthImageColor;//�����˵Ĳ���
	Mat InterfaceImage;
	Mat handzone;//��

	Point Point_SHOULDER_CENTER, Point_SPINE, Point_HAND_LEFT, Point_HAND_RIGHT, Point_HIP_CENTER;
	Point Point_HandTipRight;

	UINT16 mindistance;

	bool bodyDepthDectected;
	bool skeletonDectected;

	Point FingerTipPoint;
	int Fingernumber;

	Scalar FingerTipColor;

	//���ٿ���
	//HandState LeftHandState, RightHandState; //KINECT2
	int depthdixelhandzoneCount;

	int fingernumberhistory[5];

	bool HandPositionBool, FingerTipDetectedBool;
	bool Handdetected;
	bool FingerTipDetectedBoolOld;
	bool FingerTipDownBool, LeftHandDownBool;
	Point FingerTipPointOld;
	int StartdetectCount;
	bool StartWriting, Stopwriting;
	int LHCount;
	Mat FingeTipTrack;
	vector<Point> HandTrack;


	//ʶ��ת��
	char pathMatTemp[100];
	char pathtrack[100];
	int MattoTxtCount;

	KNN myKNN;

	int depthdixelCount;
	int RandHandFingerTipThershold;
	int DepthBoundaryMax;
	int DepthBoundaryMin;
	float tracksizerate;

	//I2P_SERVER
	/*ProtectedClient<imi::UserTrackingServiceClient> m_client;
	int64_t m_reco;
	bool m_sendReco;
	int g_nGestureID_old;
	*/
	bool SAYHELLO;
	bool GOODBYE;
	bool usertrackstate;
	int SAYHELLOcount;
};