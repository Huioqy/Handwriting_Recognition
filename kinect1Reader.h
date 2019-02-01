#pragma once

#define _WINSOCKAPI_ //before windows.h conflicts with winsock2

#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>

#include "Global.h"

using namespace std;
using namespace cv;

class kinect1Reader
{
public:
	kinect1Reader();
	~kinect1Reader();
	HRESULT kinect1Reader_Init();
	void kinect1Reader_Update();
	void kinect1Reader_shutdown();

	Mat  kinect1Reader_getcolorImage(){ return colorImage; };
	Mat  kinect1Reader_getdepthImage(){ return depthImage; };

	bool kinect1Reader_getbodyDepthDectected(){ return bodyDepthDectected; };
	bool kinect1Reader_getskeletonDectected(){ return skeletonDectected; };
	bool kinect1Reader_getSAYHELLO(){ return SAYHELLO; };
	bool kinect1Reader_getGOODBYE(){ return GOODBYE; };

	Point  kinect1Reader_getPoint_SHOULDER_CENTER(){ return Point_SHOULDER_CENTER; };
	Point  kinect1Reader_getPoint_SPINE(){ return Point_SPINE; };
	Point  kinect1Reader_getPoint_HIP_CENTER(){ return Point_HIP_CENTER; };
	Point  kinect1Reader_getPoint_HAND_LEFT(){ return Point_HAND_LEFT; };
	Point  kinect1Reader_getPoint_HAND_RIGHT(){ return Point_HAND_RIGHT; };

private:

	void getColorImage(HANDLE &colorEvent, HANDLE &colorStreamHandle, Mat &colorImage);
	void getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage);
	void getSkeletonImage(HANDLE &skeletonEvent, Mat &skeletonImage, Mat &colorImage, Mat &depthImage);
	void drawSkeleton(Mat &image, CvPoint pointSet[], int witchone);
	void getTheContour(Mat &image, int whichone, Mat &mask);

	bool bodyDepthDectected;
	bool skeletonDectected;


	int trackedSkeletonID ;
	int depthUseerID ;
	bool depthUseerIDdececeted ;
	bool trackedSkeletonIDdececeted ;
	bool trackedSkeletonIDNeedToRefresh;

	Point Point_SHOULDER_CENTER, Point_SPINE,Point_HIP_CENTER,Point_HAND_LEFT, Point_HAND_RIGHT;

	Mat colorImage;
	Mat depthImage;
	Mat skeletonImage;
	Mat mask;

	bool tracked[NUI_SKELETON_COUNT];
	CvPoint skeletonPoint[NUI_SKELETON_COUNT][NUI_SKELETON_POSITION_COUNT];
	CvPoint colorPoint[NUI_SKELETON_COUNT][NUI_SKELETON_POSITION_COUNT];

	HANDLE colorEvent ;
	HANDLE depthEvent ;
	HANDLE skeletonEvent;
	HANDLE colorStreamHandle ;
	HANDLE depthStreamHandle ;

	bool SAYHELLO;
	bool GOODBYE;

};