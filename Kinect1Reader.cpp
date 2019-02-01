
#include "kinect1Reader.h"

kinect1Reader::kinect1Reader()
{
	colorImage.create(ColorHeight, ColorWidth, CV_8UC3);
	depthImage.create(DepthHeight, DepthWidth, CV_16UC1);
	skeletonImage.create(DepthHeight, DepthWidth, CV_8UC3);
	mask.create(DepthHeight, DepthWidth, CV_8UC3);

	trackedSkeletonID = NULL;
	depthUseerID = NULL;
	depthUseerIDdececeted = false;
	trackedSkeletonIDdececeted = false;
	trackedSkeletonIDNeedToRefresh = true;

	SAYHELLO = false;
	GOODBYE = false;

	tracked[NUI_SKELETON_COUNT] = ( FALSE );
	skeletonPoint[NUI_SKELETON_COUNT][NUI_SKELETON_POSITION_COUNT] = (cvPoint(0, 0) );
	colorPoint[NUI_SKELETON_COUNT][NUI_SKELETON_POSITION_COUNT] = ( cvPoint(0, 0) );

	skeletonDectected = bodyDepthDectected= 0;

	colorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	depthEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	skeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	colorStreamHandle = NULL;
	depthStreamHandle = NULL;
}
kinect1Reader::~kinect1Reader()
{
	
	NuiShutdown();

}
HRESULT kinect1Reader::kinect1Reader_Init()
{
	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if (hr != S_OK)
	{
		cout << "NuiInitialize failed" << endl;
		return hr;
	}

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, NULL, 4, colorEvent, &colorStreamHandle);
	if (hr != S_OK)
	{
		cout << "Open the color Stream failed" << endl;
		NuiShutdown();
		return hr;
	}
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, NULL, 2, depthEvent, &depthStreamHandle);
	if (hr != S_OK)
	{
		cout << "Open the depth Stream failed" << endl;
		NuiShutdown();
		return hr;
	}
	hr = NuiSkeletonTrackingEnable(skeletonEvent, 0);//�򿪹��������¼�   
	if (hr != S_OK)
	{
		cout << "NuiSkeletonTrackingEnable failed" << endl;
		NuiShutdown();
		return hr;
	}

}
void kinect1Reader::kinect1Reader_Update()
{
	if (WaitForSingleObject(colorEvent, 0) == 0)
		getColorImage(colorEvent, colorStreamHandle, colorImage);
	if (WaitForSingleObject(depthEvent, 0) == 0)
		getDepthImage(depthEvent, depthStreamHandle, depthImage);
	//����ʹ��INFINITE��Ϊ�˱���û�м���skeletonEvent�������˴������colorimageƵ�������� 
	if (WaitForSingleObject(skeletonEvent, INFINITE) == 0)
		getSkeletonImage(skeletonEvent, skeletonImage, colorImage, depthImage);

	imshow("colorImage", colorImage);
	//imshow("skeletonImage", skeletonImage);

	//imshow("depthImage", depthImage);
}

void kinect1Reader::kinect1Reader_shutdown()
{
	NuiShutdown();
}

void  kinect1Reader::getColorImage(HANDLE &colorEvent, HANDLE &colorStreamHandle, Mat &colorImage)
{
	const NUI_IMAGE_FRAME *colorFrame = NULL;

	NuiImageStreamGetNextFrame(colorStreamHandle, 0, &colorFrame);
	INuiFrameTexture *pTexture = colorFrame->pFrameTexture;

	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if (LockedRect.Pitch != 0)
	{
		for (int i = 0; i<colorImage.rows; i++)
		{
			uchar *ptr = colorImage.ptr<uchar>(i);  //��i�е�ָ��					
			//ÿ���ֽڴ���һ����ɫ��Ϣ��ֱ��ʹ��uchar
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			for (int j = 0; j<colorImage.cols; j++)
			{
				ptr[3 * j] = pBuffer[4 * j];  //�ڲ�������4���ֽڣ�0-1-2��BGR����4������δʹ�� 
				ptr[3 * j + 1] = pBuffer[4 * j + 1];
				ptr[3 * j + 2] = pBuffer[4 * j + 2];
			}
		}
	}
	else
	{
		cout << "��׽ɫ��ͼ����ִ���" << endl;
	}

	pTexture->UnlockRect(0);
	NuiImageStreamReleaseFrame(colorStreamHandle, colorFrame);
}

void  kinect1Reader::getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage)
{
	const NUI_IMAGE_FRAME *depthFrame = NULL;

	NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame);
	INuiFrameTexture *pTexture = depthFrame->pFrameTexture;

	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	RGBQUAD q;

	bodyDepthDectected = 0;

	if (LockedRect.Pitch != 0)
	{
		for (int i = 0; i<depthImage.rows; i++)
		{
			UINT16* depthData = depthImage.ptr<UINT16>(i);
			//uchar *ptr = depthImage.ptr<uchar>(i);
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*)pBuffer;

			for (int j = 0; j < depthImage.cols; j++)
			{

				UINT16 data = (pBufferRun[j] & 0xfff8) >> 3;
				int player = pBufferRun[j] & 7;

						if (depthUseerID == NULL && player > 0 )
						{
							depthUseerID = player;

							//cout << "player   " << player << endl;
						}
						/*if (depthUseerID != player)
						{
							depthUseerID = player;
							cout << "player   " << player << endl;
						}*/

				if (player == depthUseerID &&  depthUseerID > 0 )
				{
					depthUseerIDdececeted = true;
					bodyDepthDectected = true;
					depthData[j] = data;
				}
				else
				{
					
					depthData[j] = 65535;
				}
			}
		}
		if (depthUseerIDdececeted){ depthUseerIDdececeted = false;}
		else {depthUseerID = NULL;depthUseerIDdececeted = false;}
	}
	else
	{
		cout << "��׽���ͼ����ִ���" << endl;
	}

	pTexture->UnlockRect(0);
	NuiImageStreamReleaseFrame(depthStreamHandle, depthFrame);
}

void  kinect1Reader::getSkeletonImage(HANDLE &skeletonEvent, Mat &skeletonImage, Mat &colorImage, Mat &depthImage)
{
	NUI_SKELETON_FRAME skeletonFrame = { 0 };

	bool bFoundSkeleton = false;
	SAYHELLO = false;
	//skeletonDectected = false;//��ȡ��һ������

	if (NuiSkeletonGetNextFrame(0, &skeletonFrame) == S_OK)
	{
		for (int i = 0; i < NUI_SKELETON_COUNT; i++)
		{
			if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
			{
				//if (trackedSkeletonID == 0 )
				if (trackedSkeletonIDNeedToRefresh )
				{
					GOODBYE = false;
					trackedSkeletonID = i;
					SAYHELLO = true;
					cout << "trackedSkeletonID   " << trackedSkeletonID << endl;

				}
				if (i == trackedSkeletonID)
				{
					skeletonDectected = true;
					trackedSkeletonIDdececeted = true;
				}
				bFoundSkeleton = true;
				break;
			}
		}
	}
	else
	{
		GOODBYE = true;
		trackedSkeletonIDNeedToRefresh = true;
		skeletonDectected = false;
		trackedSkeletonID = NULL;
		trackedSkeletonIDdececeted = false;
		cout << "û���ҵ����ʵĹ���֡" << endl;
		return;
	}
	
	if (!bFoundSkeleton)
	{
		GOODBYE = true;
		trackedSkeletonIDNeedToRefresh = true;
		skeletonDectected = false;
		trackedSkeletonID = NULL;
		trackedSkeletonIDdececeted = false;
		return;
	}

	NuiTransformSmooth(&skeletonFrame, NULL);//ƽ������֡,��������   
	skeletonImage.setTo(0);

	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			
			float fx, fy;

			for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)//���е�����ת��Ϊ���ͼ������   
			{
				NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[j], &fx, &fy);
				skeletonPoint[i][j].x = (int)fx;
				skeletonPoint[i][j].y = (int)fy;
			}

			for (int j = 0; j<NUI_SKELETON_POSITION_COUNT; j++)
			{
				if (skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[j] != NUI_SKELETON_POSITION_NOT_TRACKED)//���ٵ�һ��������״̬��1û�б����ٵ���2���ٵ���3���ݸ��ٵ��Ĺ��Ƶ�   
				{
					LONG colorx, colory;
					NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, 0,
						skeletonPoint[i][j].x, skeletonPoint[i][j].y, 0, &colorx, &colory);
					colorPoint[i][j].x = int(colorx);
					colorPoint[i][j].y = int(colory); //�洢����� 
					circle(colorImage, colorPoint[i][j], 4, cvScalar(0, 255, 255), 1, 8, 0);
					circle(skeletonImage, skeletonPoint[i][j], 3, cvScalar(0, 255, 255), 1, 8, 0);

					tracked[i] = TRUE;
				}
			}

			drawSkeleton(colorImage, colorPoint[i], i);
			drawSkeleton(skeletonImage, skeletonPoint[i], i);
		}
		if (skeletonDectected)
		{
			Point_SHOULDER_CENTER = skeletonPoint[trackedSkeletonID][NUI_SKELETON_POSITION_SHOULDER_CENTER];
			Point_SPINE = skeletonPoint[trackedSkeletonID][NUI_SKELETON_POSITION_SPINE];
			Point_HIP_CENTER = skeletonPoint[trackedSkeletonID][NUI_SKELETON_POSITION_HIP_CENTER];
			Point_HAND_LEFT = skeletonPoint[trackedSkeletonID][NUI_SKELETON_POSITION_HAND_LEFT];
			Point_HAND_RIGHT = skeletonPoint[trackedSkeletonID][NUI_SKELETON_POSITION_HAND_RIGHT];
		}
	}
	//��ȡ��
	

	if (trackedSkeletonIDdececeted)
	{ 
		//cout << "trackedSkeletonIDdececeted" << "= True" << endl;
		trackedSkeletonIDdececeted = false;
		trackedSkeletonIDNeedToRefresh= false;
	}
	else
	{
		skeletonDectected = false;
		trackedSkeletonID = NULL;
		trackedSkeletonIDdececeted = false;
		trackedSkeletonIDNeedToRefresh = true;
	}

}

void  kinect1Reader::drawSkeleton(Mat &image, CvPoint pointSet[], int whichone)
{
	CvScalar color;
	switch (whichone) //���ٲ�ͬ������ʾ��ͬ����ɫ 
	{
	case 0:
		color = cvScalar(255);
		break;
	case 1:
		color = cvScalar(0, 255);
		break;
	case 2:
		color = cvScalar(0, 0, 255);
		break;
	case 3:
		color = cvScalar(255, 255, 0);
		break;
	case 4:
		color = cvScalar(255, 0, 255);
		break;
	case 5:
		color = cvScalar(0, 255, 255);
		break;
	}

	if ((pointSet[NUI_SKELETON_POSITION_HEAD].x != 0 || pointSet[NUI_SKELETON_POSITION_HEAD].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_HEAD], pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_SPINE].x != 0 || pointSet[NUI_SKELETON_POSITION_SPINE].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SPINE], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_SPINE].x != 0 || pointSet[NUI_SKELETON_POSITION_SPINE].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x != 0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_SPINE], pointSet[NUI_SKELETON_POSITION_HIP_CENTER], color, 2);

	//����֫ 
	if ((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_LEFT], pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_ELBOW_LEFT], pointSet[NUI_SKELETON_POSITION_WRIST_LEFT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_WRIST_LEFT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_HAND_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_HAND_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_WRIST_LEFT], pointSet[NUI_SKELETON_POSITION_HAND_LEFT], color, 2);

	//����֫ 
	if ((pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_CENTER], pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_SHOULDER_RIGHT], pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_ELBOW_RIGHT], pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_HAND_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_HAND_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_WRIST_RIGHT], pointSet[NUI_SKELETON_POSITION_HAND_RIGHT], color, 2);

	//����֫ 
	if ((pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x != 0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_HIP_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_HIP_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_CENTER], pointSet[NUI_SKELETON_POSITION_HIP_LEFT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_HIP_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_HIP_LEFT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_LEFT], pointSet[NUI_SKELETON_POSITION_KNEE_LEFT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_KNEE_LEFT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_KNEE_LEFT], pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_FOOT_LEFT].x != 0 || pointSet[NUI_SKELETON_POSITION_FOOT_LEFT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_ANKLE_LEFT], pointSet[NUI_SKELETON_POSITION_FOOT_LEFT], color, 2);

	//����֫ 
	if ((pointSet[NUI_SKELETON_POSITION_HIP_CENTER].x != 0 || pointSet[NUI_SKELETON_POSITION_HIP_CENTER].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_CENTER], pointSet[NUI_SKELETON_POSITION_HIP_RIGHT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_HIP_RIGHT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_HIP_RIGHT], pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_KNEE_RIGHT], pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT], color, 2);
	if ((pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT].y != 0) &&
		(pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT].x != 0 || pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT].y != 0))
		line(image, pointSet[NUI_SKELETON_POSITION_ANKLE_RIGHT], pointSet[NUI_SKELETON_POSITION_FOOT_RIGHT], color, 2);
}

//���ݸ�����������ݵĹ�ϵ����getDepthImage()�еģ�ȷ����ͬ�ĸ���Ŀ�� 
void  kinect1Reader::getTheContour(Mat &image, int whichone, Mat &mask)
{
	for (int i = 0; i<image.rows; i++)
	{
		uchar *ptr = image.ptr<uchar>(i);
		uchar *ptrmask = mask.ptr<uchar>(i);
		for (int j = 0; j<image.cols; j++)
		{
			if (ptr[3 * j] == 0 && ptr[3 * j + 1] == 0 && ptr[3 * j + 2] == 0) //��Ϊ0��ʱ�����Ժ��� 
			{
				ptrmask[3 * j] = ptrmask[3 * j + 1] = ptrmask[3 * j + 2] = 0;
			}
			else if (ptr[3 * j] == 0 && ptr[3 * j + 1] == 0 && ptr[3 * j + 2] != 0)//IDΪ1��ʱ����ʾ��ɫ 
			{
				ptrmask[3 * j] = 0;
				ptrmask[3 * j + 1] = 255;
				ptrmask[3 * j + 2] = 0;
			}
			else if (ptr[3 * j] == 0 && ptr[3 * j + 1] != 0 && ptr[3 * j + 2] == 0)//IDΪ2��ʱ����ʾ��ɫ 
			{
				ptrmask[3 * j] = 0;
				ptrmask[3 * j + 1] = 0;
				ptrmask[3 * j + 2] = 255;
			}
			else if (ptr[3 * j] == ptr[3 * j + 1] && ptr[3 * j] == 4 * ptr[3 * j + 2])//IDΪ3��ʱ�� 
			{
				ptrmask[3 * j] = 255;
				ptrmask[3 * j + 1] = 255;
				ptrmask[3 * j + 2] = 0;
			}
			else if (4 * ptr[3 * j] == ptr[3 * j + 1] && ptr[3 * j + 1] == ptr[3 * j + 2])//IDΪ4��ʱ�� 
			{
				ptrmask[3 * j] = 255;
				ptrmask[3 * j + 1] = 0;
				ptrmask[3 * j + 2] = 255;
			}
			else if (ptr[3 * j] == 4 * ptr[3 * j + 1] && ptr[3 * j] == ptr[3 * j + 2])//IDΪ5��ʱ�� 
			{
				ptrmask[3 * j] = 0;
				ptrmask[3 * j + 1] = 255;
				ptrmask[3 * j + 2] = 255;
			}
			else if (ptr[3 * j] == 2 * ptr[3 * j + 1] && ptr[3 * j + 1] == ptr[3 * j + 2])//IDΪ6��ʱ�� 
			{
				ptrmask[3 * j] = 255;
				ptrmask[3 * j + 1] = 255;
				ptrmask[3 * j + 2] = 255;
			}
			else if (ptr[3 * j] == ptr[3 * j + 1] && ptr[3 * j] == ptr[3 * j + 2])//IDΪ7��ʱ�����IDΪ0��ʱ����ʾ��ɫ 
			{
				ptrmask[3 * j] = 255;
				ptrmask[3 * j + 1] = 0;
				ptrmask[3 * j + 2] = 0;
			}
			else
			{
				cout << "��������δ��룬˵������©����������ѯgetTheContour����" << endl;
			}
		}
	}
}

