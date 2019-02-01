#include "HandWriting.h"

HandWriting::HandWriting() 
	//:m_client("localhost", imi::g_Inputs_constants.DEFAULT_USERTRACKING_SERVICE_PORT)
{
	depthImage.create(DepthHeight, DepthWidth, CV_16UC1);
	depthImageColor.create(DepthHeight, DepthWidth, CV_8UC3);
	InterfaceImage.create(DepthHeight, DepthWidth, CV_8UC3);
	userIdImage.create(DepthHeight, DepthWidth, CV_8UC1);
	handzone.create(DepthHeight, DepthWidth, CV_8UC3);
	FingeTipTrack.create(DepthHeight, DepthWidth, CV_8UC3);

	bodyDepthDectected = 0;
	depthImageColor.setTo(255);
	InterfaceImage.setTo(255);

	//trackcontrol
	GOODBYE = false;
	usertrackstate = false;
	SAYHELLO = false ;
	SAYHELLOcount = 0;
	HandPositionBool = false ;
	FingerTipDetectedBool = false;
	FingerTipDetectedBoolOld = false;
	StartdetectCount = 0;
	FingerTipDownBool = false;
	LeftHandDownBool = false;
	StartWriting = false;
	Stopwriting = false;
	LHCount = 0;
	MattoTxtCount = 0;
	Handdetected = false;
	fingernumberhistory[0] = 0;
	fingernumberhistory[1] = 0;
	fingernumberhistory[2] = 0;
	fingernumberhistory[3] = 0;
	fingernumberhistory[4] = 0;
	RandHandFingerTipThershold = 200;
	DepthBoundaryMax = 2000;
	DepthBoundaryMin = 100;
	tracksizerate = 0.6;
}

HandWriting::~HandWriting()
{

}

void HandWriting::getFingetTip()
{
	if(SAYHELLO && !usertrackstate ) //&& !usertrackstate
	{
		SAYHELLOcount++;
		//SendToServer(10);
		usertrackstate = true;
		cout<<"say hello"<<endl;
		// send the start message to vh control				
			/*	try 
				{
					m_client.getClient()->gestureStart(std::string("HandTrack"),
						imi::getTimeStamp(),0,imi::Gesture::HAND_GESTURE_START);
				}   
				catch( apache::thrift::TException &tx ) 
				{
					std::cerr << "EXCEPTION opening the network conn: " << tx.what() << "\n";
					m_client.receiveNetworkException();
				}*/
	}
	if(GOODBYE && usertrackstate && SAYHELLOcount>=1)
	{
		usertrackstate = false;
		//SendToServer(11);
		// send the stop message to vh control				
				/*try 
				{
					m_client.getClient()->gestureStart(std::string("HandTrack"),
						imi::getTimeStamp(),0,imi::Gesture::HAND_GESTURE_STOP);
				}   
				catch( apache::thrift::TException &tx ) 
				{
					std::cerr << "EXCEPTION opening the network conn: " << tx.what() << "\n";
					m_client.receiveNetworkException();
				}*/
	}
	InterfaceImage.setTo(255);//交互界面
	depthdixelCount = 0;
	FingerTipDetectedBool = false;
	//FingerTipPointOld = FingerTipPoint;
	//FingerTipDetectedBool = false;
	//HanDetectedBool = 0;
	//cout << "bodyDepthDectected  " << bodyDepthDectected << endl;
	//cout << "skeletonDectected  " << skeletonDectected << endl;

	//在depthImage寻找出人的范围
	//在depthImage寻找最小距离
	mindistance = 65535;
	depthImageColor.setTo(255);
	//KINECT2
	/*  
	for (int i = 0; i < DepthHeight-80; i++) //row  行数 y值  //去除脚部干扰
		for (int j = 0; j < DepthWidth; j++)// col  列数 X值
		{
			if (depthImage.at<UINT16>(i, j) < DepthBoundaryMax && depthImage.at<UINT16>(i, j) > DepthBoundaryMin && userIdImage.at<uchar>(i, j) > 0)
			{
				depthdixelCount++;
				depthImageColor.at<Vec3b>(i, j)[0] = 0;//蓝色通道
				depthImageColor.at<Vec3b>(i, j)[1] = 255;//绿色通道
				depthImageColor.at<Vec3b>(i, j)[2] = 0;//红色通道
				if (depthImage.at<UINT16>(i, j) < mindistance)
				{
					FingerTipDetectedBool = true;
					mindistance = depthImage.at<UINT16>(i, j);
					FingerTipPoint = Point(j, i);
				}
			}
		}
	*/
	// KINECT1
	for (int i = 0; i < DepthHeight; i++) //row  行数 y值  //去除脚部干扰
		for (int j = 0; j < DepthWidth; j++)// col  列数 X值
		{
			if (depthImage.at<UINT16>(i, j) < DepthBoundaryMax && depthImage.at<UINT16>(i, j) > DepthBoundaryMin)
			{
				depthdixelCount++;
				depthImageColor.at<Vec3b>(i, j)[0] = 0;//蓝色通道
				depthImageColor.at<Vec3b>(i, j)[1] = 255;//绿色通道
				depthImageColor.at<Vec3b>(i, j)[2] = 0;//红色通道
				if (depthImage.at<UINT16>(i, j) < mindistance)
				{
					FingerTipDetectedBool = true;
					mindistance = depthImage.at<UINT16>(i, j);
					FingerTipPoint = Point(j, i);
				}
			}
		}
	/*if (sqrt((Point_HAND_RIGHT.x - FingerTipPoint.x)*(Point_HAND_RIGHT.x - FingerTipPoint.x)
		+ (Point_HAND_RIGHT.y - FingerTipPoint.y)*(Point_HAND_RIGHT.y - FingerTipPoint.y))
		< RandHandFingerTipThershold)//在.h修改
	{
		FingerTipDetectedBool = true; FingerTipColor = Scalar(0, 0, 255);// 黑色
	}
	else
	{
		FingerTipPoint = FingerTipPointOld;
		FingerTipDetectedBool = false; FingerTipColor = Scalar(0, 0, 0);//红色
	}*/
}

void HandWriting::HandExtraciotn()
{
	handzone.create(DepthHeight, DepthWidth, CV_8UC3);
	Mat handzone2;
	handzone2.create(500, 500, CV_8UC3);
	handzone.setTo(0);
	depthdixelhandzoneCount = 0;
	if (FingerTipDetectedBool)
	{
		// 阈值+120 抠出手型 计算depthdixelhandzoneCount
		for (int i = 0; i < DepthHeight; i++)//row  行数 y值 
			for (int j = 0; j < DepthWidth; j++)// col  列数 X值
			{
				if (depthImage.at<UINT16>(i, j) < mindistance + 120)
				{
					depthdixelhandzoneCount++;
				}

			}
		//cout << depthdixelhandzoneCount << endl;
		if (depthdixelhandzoneCount > 100 && depthdixelhandzoneCount < 1500)
		{
			// 阈值+120 抠出手型
			for (int i = 0; i < DepthHeight; i++)//row  行数 y值 
				for (int j = 0; j < DepthWidth; j++)// col  列数 X值
				{
					//if (depthImageColor.at<Vec3b>(i, j) == Vec3b(0, 255, 0) && depthImage.at<UINT16>(i, j) < mindistance + 120)//绿色通道
					if (depthImage.at<UINT16>(i, j) < mindistance + 100)
					{
						handzone.at<Vec3b>(i, j)[0] = 255;
						handzone.at<Vec3b>(i, j)[1] = 255;
						handzone.at<Vec3b>(i, j)[2] = 255;
						InterfaceImage.at<Vec3b>(i, j)[0] = 0;
						InterfaceImage.at<Vec3b>(i, j)[1] = 0;
						InterfaceImage.at<Vec3b>(i, j)[2] = 0;
					}

				}
			//medianBlur(handzone, handzone, 7);
			erode(handzone, handzone, Mat(), Point(-1, -1), 1);		// 腐蚀
			dilate(handzone, handzone, Mat(), Point(-1, -1), 1.5);   //膨胀
			
			//resize(handzone, handzone2, Size(500, 500));
			
		}
	}
	imshow("handzone", handzone);
}
void HandWriting::FingerNumberDetect()
{
	Handdetected = false;
	if (depthdixelhandzoneCount > 100 && depthdixelhandzoneCount < 1500)
	{
		Handdetected = true;
		//circle(handzone, FingerTipPoint, 10, Scalar(0, 0, 255), -1);
		cvtColor(handzone, handzone, CV_RGB2GRAY);
		Canny(handzone, handzone, 3, 9, 3);

		static vector< vector<cv::Point> > contours;   // 轮廓  
		static vector< Vec4i > hierarchy;    // 轮廓的结构信息   
		contours.clear();
		hierarchy.clear();
		//寻找轮廓
		findContours(handzone, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		if (!contours.empty())
		{
			// 找到最大的轮廓  
			int index = 0;
			double area, maxArea(0);
			for (int i = 0; i < contours.size(); i++)
			{
				area = contourArea(Mat(contours[i]));
				if (area > maxArea)
				{
					maxArea = area;
					index = i;
				}
			}
			//画轮廓
			drawContours(handzone, contours, -1, Scalar(0, 0, 255), 2);

			// 寻找指尖  
			vector<Point> couPoint = contours[index];
			vector<Point> fingerTips;
			vector<double> fingerTipsdist;
			int maxFingerValue(0), count(0), notice(0);

			//求最小圆形
			Point2f center;
			float radius;
			minEnclosingCircle(Mat(couPoint), center, radius);

			/*
			//曲率  手心用Point_HAND_RIGHT
			Point p, q, r, p1, p2;

			for (int i = 3; (i < (couPoint.size() - 3)) && (i < couPoint.size()); i++)
			{
			q = couPoint[i - 3];
			p = couPoint[i];
			r = couPoint[i + 3];
			double dist = sqrt((p.x - Point_HAND_RIGHT.x) * (p.x - Point_HAND_RIGHT.x) + (p.y - Point_HAND_RIGHT.y) * (p.y - Point_HAND_RIGHT.y));
			//int distP1 = (p1.x - Point_HAND_RIGHT.x) * (p1.x - Point_HAND_RIGHT.x) + (p1.y - Point_HAND_RIGHT.y) * (p1.y - Point_HAND_RIGHT.y);
			//int distP2 = (p2.x - Point_HAND_RIGHT.x) * (p2.x - Point_HAND_RIGHT.x) + (p2.y - Point_HAND_RIGHT.y) * (p2.y - Point_HAND_RIGHT.y);
			int dot = (q.x - p.x) * (r.x - p.x) + (q.y - p.y) * (r.y - p.y);
			int cross = (q.x - p.x) * (r.y - p.y) - (r.x - p.x) * (q.y - p.y);
			if (dist > maxFingerValue)
			{
			maxFingerValue = dist;
			notice = i;
			}
			if (dist != maxFingerValue)
			{
			count++;
			}
			if ((dot < 15 && dot > -15 && cross > 0) || (count > 40))
			{
			if (count > 40)
			{
			p = couPoint[notice];
			count = 0;
			maxFingerValue = 0;

			}
			bool flag = false;
			// 低于手心的点不算
			if (Point_HAND_RIGHT.y < p.y) continue;
			//距离手心太近
			if (dist < double(radius / 5 * 3))continue;
			//if (dist < 10)continue;
			// 找出来的两个手指点离得太近的不算
			if (fingerTips.size()>0)
			for (int j = 0; j < fingerTips.size(); j++)
			{
			if (pow((p.x - fingerTips[j].x), 2) + pow((p.y - fingerTips[j].y), 2) < radius / 2)
			{
			flag = true;
			break;
			}
			}

			if (flag) continue;
			if (p.x > 0)
			if (p.y > 0)
			if (p.x < DepthWidth)
			if (p.y < DepthHeight)
			{
			fingerTips.push_back(p);
			fingerTipsdist.push_back(dist);
			}
			}
			}
			if (fingerTips.size()>5)
			{
			double tempdist;
			Point temppoint;
			for (int pass = 1; pass < fingerTips.size(); pass++)
			{
			for (int k = 0; k<fingerTips.size() - pass; k++)
			if (fingerTipsdist[k]>fingerTipsdist[k + 1])
			{
			tempdist = fingerTipsdist[k];
			fingerTipsdist[k] = fingerTipsdist[k + 1];
			fingerTipsdist[k + 1] = tempdist;
			temppoint = fingerTips[k];
			fingerTips[k] = fingerTips[k + 1];
			fingerTips[k + 1] = temppoint;
			}
			}
			for (int k = 0; k < 5; k++)
			{
			circle(InterfaceImage, fingerTips[k], 5, Scalar(255, 0, 0), CV_FILLED);
			line(InterfaceImage, Point_HAND_RIGHT, fingerTips[k], Scalar(255, 0, 0), 2);
			}
			Fingernumber = 5;
			}
			else
			{
			for (int k = 0; k < fingerTips.size(); k++)
			{
			circle(InterfaceImage, fingerTips[k], 5, Scalar(255, 0, 0), CV_FILLED);
			line(InterfaceImage, Point_HAND_RIGHT, fingerTips[k], Scalar(255, 0, 0), 2);
			}
			Fingernumber = fingerTips.size();
			}*/
			//峰值算法找手指
			Point tmp;
			center.y = center.y + radius/4;
			center.x = center.x + radius / 5;
			for (int i = 0; i < couPoint.size(); i++)
			{
				tmp = couPoint[i];
				double dist = sqrt((tmp.x - center.x) * (tmp.x - center.x) + (tmp.y - center.y) * (tmp.y - center.y));
				if (dist > maxFingerValue)
				{
					maxFingerValue = dist;
					notice = i;
				}

				// 计算最大值保持的点数，如果大于40（这个值需要设置，本来想根据max值来设置，  
				// 但是不成功，不知道为何），那么就认为这个是指尖  
				if (dist != maxFingerValue)
				{
					count++;
					if (count > 20)
					{
						count = 0;
						maxFingerValue = 0;
						bool flag = false;

						// 低于手心的点不算  
						if (center.y < couPoint[notice].y)continue;
						//距离手心太近不算 
						if (dist < double(radius / 5 * 3))continue;
						//离得太近不算
						if (fingerTips.size()>0)
							for (int j = 0; j < fingerTips.size(); j++)
							{
								if ((tmp.x - fingerTips[j].x)*(tmp.x - fingerTips[j].x) +(tmp.y - fingerTips[j].y)*(tmp.y - fingerTips[j].y)  < radius / 3)
								{
									flag = true;
									break;
								}
							}

						if (flag) continue;
						fingerTips.push_back(couPoint[notice]);
						circle(InterfaceImage, couPoint[notice], 6, Scalar(0, 255, 0), CV_FILLED);
						line(InterfaceImage, center, couPoint[notice], Scalar(255, 0, 0), 2);
					}


				}
			}
			Fingernumber = fingerTips.size();
		}
	}
	else Fingernumber = 0;
	//cout << Fingernumber << endl;

}
void HandWriting::Interface()
{
	showdepthImageColor();
	FingerNumberDetect();
	fingernumberhistory[4] = fingernumberhistory[3];
	fingernumberhistory[3] = fingernumberhistory[2];
	fingernumberhistory[2] = fingernumberhistory[1];
	fingernumberhistory[1] = fingernumberhistory[0];

	// handstarting position Rectangle
	Rect handstartingRectM((DepthWidth / 2) - 40, (DepthHeight / 2 - 60), 80, 70);

	if (!HandPositionBool)//手放在矩形框内
	{
		rectangle(InterfaceImage, handstartingRectM, Scalar(0, 255, 0), 5);//矩形框显示
		/*if (FingerTipDetectedBool
			&& FingerTipPoint.x > handstartingRectM.x  &&   FingerTipPoint.x < (handstartingRectM.x + handstartingRectM.width)
			&& FingerTipPoint.y > handstartingRectM.y  &&   FingerTipPoint.y < (handstartingRectM.y + handstartingRectM.height)
			&& ((Fingernumber >3) || RightHandState == HandState_Open))*/ //KINECT2
		if (FingerTipDetectedBool
			&& FingerTipPoint.x > handstartingRectM.x  &&   FingerTipPoint.x < (handstartingRectM.x + handstartingRectM.width)
			&& FingerTipPoint.y > handstartingRectM.y  &&   FingerTipPoint.y < (handstartingRectM.y + handstartingRectM.height)
			&& ((Fingernumber >2)))
			StartdetectCount++;
		if (StartdetectCount > 15)
		{
			HandPositionBool = true;
			cout << "Start Tracking" << endl;

			fingernumberhistory[0] = 0;
			fingernumberhistory[1] = 0;
			fingernumberhistory[2] = 0;
			fingernumberhistory[3] = 0;
			fingernumberhistory[4] = 0;

		}
	}

	//HandPositionBool = StartdetectCount = 0;
	if (HandPositionBool && !StartWriting)
	{
		//InterfaceImage.setTo(255);
		//if ((Fingernumber <= 2 && Fingernumber > 0) || RightHandState == HandState_Lasso)
		//SDK右手状态 //KINECT2
		/*if(RightHandState == HandState_Lasso)
		{
			StartWriting = true;
			cout << "StartWriting SDK" << endl;
			fingernumberhistory[0] = 0;
			fingernumberhistory[1] = 0;
			fingernumberhistory[2] = 0;
			fingernumberhistory[3] = 0;
		}*/

		//右手自己的手指数    
		if (Fingernumber < 2)
		{
			fingernumberhistory[0] = 1;
		}
		else
		{
			fingernumberhistory[0] = 0;
		}

		int sum = fingernumberhistory[0] + fingernumberhistory[1] + fingernumberhistory[2] + fingernumberhistory[3] + fingernumberhistory[4] ;
		if (sum > 3)
		{
			StartWriting = true;
			cout << "StartWriting1" << endl;
			fingernumberhistory[0] = 0;
			fingernumberhistory[1] = 0;
			fingernumberhistory[2] = 0;
			fingernumberhistory[3] = 0;
			fingernumberhistory[4] = 0;
		}
		//起手式
		/*if (FingerTipPoint.y > Point_HAND_RIGHT.y)FingerTipDownBool = true;
		if (FingerTipDownBool && FingerTipPoint.y < Point_HAND_RIGHT.y - 5)
		{
			StartWriting = true;
			cout << "StartWriting2" << endl;
			fingernumberhistory[0] = 0;
			fingernumberhistory[1] = 0;
			fingernumberhistory[2] = 0;
			fingernumberhistory[3] = 0;
		}*/
		//左手手掌控制  Kinect2
		//if (FingerTipDownBool && LeftHandState == HandState_Closed)
		/*if (Point_SHOULDER_CENTER.y > Point_HAND_LEFT.y)
			if (LeftHandState == HandState_Closed)
			{
				StartWriting = true;
				cout << "StartWriting3" << endl;
				fingernumberhistory[0] = 0;
				fingernumberhistory[1] = 0;
				fingernumberhistory[2] = 0;
				fingernumberhistory[3] = 0;
			}*/
	}
	//开始写
	if (StartWriting && (!Stopwriting))
	{
		//FingerNumberDetect();
		LHCount++;
		//if (LHCount == 1)	FingeTipTrack.setTo(0); 
		if (LHCount == 1)
		{
			fingernumberhistory[0] = 0;
			fingernumberhistory[1] = 0;
			fingernumberhistory[2] = 0;
			fingernumberhistory[3] = 0;
			fingernumberhistory[4] = 0;
			//fingernumberhistory[5] = 0;
			FingeTipTrack.setTo(0);//清空手写轨迹
		}
		if (LHCount == 2) FingerTipPointOld = FingerTipPoint;
		if (LHCount > 2)
		{

			//指尖点处理
			double distfgt = sqrt(double((FingerTipPointOld.x - FingerTipPoint.x) * (FingerTipPointOld.x - FingerTipPoint.x) + (FingerTipPointOld.y - FingerTipPoint.y) * (FingerTipPointOld.y - FingerTipPoint.y)));
			if (FingerTipPoint.x > 5 && FingerTipPoint.x < DepthWidth - 5 && FingerTipPoint.y>5 && FingerTipPoint.y < DepthHeight - 5 )
			{
				if (distfgt < 100)
				{
					HandTrack.push_back(FingerTipPoint);
					circle(FingeTipTrack, FingerTipPoint, 6, Scalar(255, 255, 255), -1);//画手指 
					if (HandTrack.size() > 2)
					{
						int z = HandTrack.size() - 1;
						//if (FingerTipDetectedBoolOld && FingerTipDetectedBool)
							line(FingeTipTrack, HandTrack[z], HandTrack[z - 1], Scalar(255, 255, 255), 4);
						//FingerTipDetectedBoolOld = FingerTipDetectedBool;
					}
					FingerTipPointOld = FingerTipPoint;
				}
			}
			Mat FingeTipTrack1;
			resize(FingeTipTrack,FingeTipTrack1,Size(DepthWidth*1.3,DepthHeight*1.3));
			imshow("track", FingeTipTrack1);

			//判断结束
			//手指数目识别结束

			if (Fingernumber > 2 || !FingerTipDetectedBool || depthdixelCount<50 ||! Handdetected)
			{
				fingernumberhistory[0] = 1;
			}
			else
			{
				fingernumberhistory[0] = 0;
			}
			int sum = fingernumberhistory[0] + fingernumberhistory[1] + fingernumberhistory[2] + fingernumberhistory[3] + fingernumberhistory[4];
			if (sum >= 2 ) 
			{
				Stopwriting = true;
				cout << "Stopwriting1  fingernumber" << endl;
			}
			// KINECT2
			/*if(RightHandState == HandState_Open)
			{
				Stopwriting = true;
				cout << "Stopwriting RightHandState" << endl;
			}
			if (LeftHandState == HandState_Open && Point_SHOULDER_CENTER.y > Point_HAND_LEFT.y)
			{
				Stopwriting = true;
				cout << "Stopwriting LeftHand" << endl;
			}*/
		}
	}

	if (StartWriting && Stopwriting)
	{
		if (LHCount > 10 && HandTrack.size() > 10)
		{
			bool OneRecognziedBool = false;
			//轨迹重新画  根据字的大小算出笔画粗细
			Mat FingeTipTrackcopy;
			Rect HandTrackrect = minAreaRect(HandTrack).boundingRect();
			if (double(HandTrackrect.width) / double(HandTrackrect.height) > 0.45)//主要是识别是否是1  
			{
				int tracksize = HandTrackrect.width* HandTrackrect.height / 32 / 32 * tracksizerate;
				FingeTipTrack.setTo(0);
				for (int i = 1; i < HandTrack.size()-5; i++)
				{
					line(FingeTipTrack, HandTrack[i], HandTrack[i - 1], Scalar(255, 255, 255), tracksize);
					//circle(FingeTipTrack, Point_SHOULDER_CENTER, 10, Scalar(255, 255, 255), -1);//画SHOULDER_CENTER 蓝色
				}
			}
			else//对1的特殊处理
			{
				OneRecognziedBool = true;
				int tracksize = HandTrackrect.height* HandTrackrect.height / 32 / 32 * tracksizerate;
				FingeTipTrack.setTo(0);
				for (int i = 1; i < HandTrack.size(); i++)
				{
					line(FingeTipTrack, HandTrack[i], HandTrack[i - 1], Scalar(255, 255, 255), tracksize);
					//circle(FingeTipTrack, Point_SHOULDER_CENTER, 10, Scalar(255, 255, 255), -1);//画SHOULDER_CENTER 蓝色
				}
			}
			medianBlur(FingeTipTrack, FingeTipTrack, 7);  //滤波
			erode(FingeTipTrack, FingeTipTrack, Mat(), Point(-1, -1), 1);		// 腐蚀
			dilate(FingeTipTrack, FingeTipTrack, Mat(), Point(-1, -1), 1.5);   //膨胀
			FingeTipTrack.copyTo(FingeTipTrackcopy);
			cvtColor(FingeTipTrackcopy, FingeTipTrackcopy, CV_RGB2GRAY);
			static vector< vector<cv::Point> > contours;   // 轮廓  
			static vector< Vec4i > hierarchy;    // 轮廓的结构信息   
			contours.clear();
			hierarchy.clear();
			int area1;
			//寻找轮廓
			findContours(FingeTipTrackcopy, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
			//找最大轮廓
			int index = 0;
			double area, maxArea(0);
			if (!contours.empty())
			{
				for (int i = 0; i < contours.size(); i++)
				{
					area = contourArea(Mat(contours[i]));
					if (area > maxArea)
					{
						maxArea = area;
						index = i;
					}
				}
				Rect rect = minAreaRect(contours[index]).boundingRect();
				//调整框大小
				double wh = double(rect.width) / double(rect.height);
				cout << " wh1:" << wh << endl;
				/*int deta1 = (rect.height / 3 * 2 - rect.width) / 2;
				int deta2 = (rect.width / 3 * 2 - rect.height) / 2;
				if (wh < 0.5){ rect.width = rect.height / 3 * 2; rect.x = rect.x - deta1; }
				if (wh > 2){ rect.height = rect.width / 3 * 2; rect.y = rect.y - deta2; }
				*/
				if (OneRecognziedBool)
				{
					if ((rect.x - (rect.height - rect.width) / 2) > 0)
					{
						rect.x = rect.x - (rect.height - rect.width) / 2;
						rect.width = rect.height;
					}
					else
					{
						rect.x = 0;
						rect.width = rect.height;
					}
					double wh = double(rect.width) / double(rect.height);
					cout << " wh2:" << wh << endl;

				}
				if (rect.x <= 0) rect.x = 1;
				if (rect.y <= 0) rect.y = 1;
				if (rect.x + rect.width >= DepthWidth) rect.width = DepthWidth - rect.x;
				if (rect.y + rect.height >= DepthHeight) rect.height = DepthHeight - rect.y;
				imshow("轨迹", FingeTipTrack(rect));
				MattoJPG1(FingeTipTrack(rect));
				MattoTxt(extractandtransform(FingeTipTrackcopy(rect)));//转换成 32*32 保存图片
				//TXT01();
				shownumresult(myKNN.KNN_Classify(TXT01()));
				
			}
			else cout << "没找到轮廓" << endl;

		}

		else cout << "写的太少了" << endl;

		HandTrack.clear();
		LHCount = 0;
		StartdetectCount = 0;
		StartWriting = Stopwriting = HandPositionBool = FingerTipDownBool = LeftHandDownBool = FingerTipDetectedBoolOld = false;
		fingernumberhistory[0] = 0;
		fingernumberhistory[1] = 0;
		fingernumberhistory[2] = 0;
		fingernumberhistory[3] = 0;
		fingernumberhistory[4] = 0;

	}
	//if (FingerTipDetectedBool)
		//circle(InterfaceImage, FingerTipPoint, 10, Scalar(0, 0, 255), CV_FILLED);

	Mat InterfaceImage1;
	resize(InterfaceImage, InterfaceImage1, Size(DepthWidth * 2, DepthHeight * 2));
	imshow("InterfaceImage", InterfaceImage1);

}
void HandWriting::shownumresult(int result)
{
	cout << result << endl;
	char resultpath[100];
	sprintf_s(resultpath, "..\\ResultNumberPic\\%d.jpg", result);//写名字
	cout << resultpath << endl;
	Mat Number = imread(resultpath);
	//SendToServer(result);
	imshow("Result", Number);
}
/*
void HandWriting::SendToServer(int nGestureID)
{
	if( nGestureID != g_nGestureID_old)
	{
		m_reco = imi::getTimeStamp();
		m_sendReco = false;
	}


	if( m_client.ensureConnection() )//microsecond
		//if( (imi::getTimeStamp()-m_reco) > 1000000  && m_client.ensureConnection() )//microsecond
	{
		try 
		{
			imi::Gesture::type recoGest;
			switch (nGestureID)
			{
			case 0:
				recoGest = imi::Gesture::HAND_GESTURE_0;
				break;
			case 1:
				recoGest = imi::Gesture::HAND_GESTURE_1;
				break;
			case 2:
				recoGest = imi::Gesture::HAND_GESTURE_2;
				break;
			case 3:
				recoGest = imi::Gesture::HAND_GESTURE_3;
				break;
			case 4:
				recoGest = imi::Gesture::HAND_GESTURE_4;
				break;
			case 5:
				recoGest = imi::Gesture::HAND_GESTURE_5;
				break;
			case 6:
				recoGest = imi::Gesture::HAND_GESTURE_6;
				break;
			case 7:
				recoGest = imi::Gesture::HAND_GESTURE_7;
				break;
			case 8:
				recoGest = imi::Gesture::HAND_GESTURE_8;
				break;
			case 9:
				recoGest = imi::Gesture::HAND_GESTURE_9;
				break;
			case 10:
				recoGest = imi::Gesture::HAND_GESTURE_START;
				break;
			case 11:
				recoGest = imi::Gesture::HAND_GESTURE_STOP;
				break;
			default:
				recoGest = imi::Gesture::HAND_GESTURE_0;
				break;
			}
			m_client.getClient()->gestureStart(std::string("HandTrack"),imi::getTimeStamp(),0,recoGest);
			
		}   
		catch( apache::thrift::TException &tx ) 
		{
			std::cerr << "EXCEPTION opening the network conn: " << tx.what() << "\n";
			m_client.receiveNetworkException();
		}
	}
	g_nGestureID_old = nGestureID;
}*/
/*
void HandWriting::SendToServer(int nGestureID)
{
	if( nGestureID != g_nGestureID_old)
	{
		m_reco = imi::getTimeStamp();
		m_sendReco = false;
	}

	if( (imi::getTimeStamp()-m_reco) > 2000000 && ! m_sendReco && m_client.ensureConnection() )//microsecond
	{
		try 
		{
			imi::Gesture::type recoGest;
			switch (nGestureID)
			{
			case 0:
				//recoGest = imi::Gesture::ROCK;
				recoGest = imi::Gesture::HAND_GESTURE_0;
				break;
			case 1:
				recoGest = imi::Gesture::HAND_GESTURE_1;
				//recoGest = imi::Gesture::ROCK;
				break;
			case 2:
				//recoGest = imi::Gesture::SCISSOR;
				recoGest = imi::Gesture::HAND_GESTURE_2;
				break;
			case 3:
				recoGest = imi::Gesture::HAND_GESTURE_3;
				break;
			case 4:
				recoGest = imi::Gesture::HAND_GESTURE_4;
				break;
			case 5:
				recoGest = imi::Gesture::HAND_GESTURE_5;
				break;
			case 6:
				recoGest = imi::Gesture::HAND_GESTURE_6;
				break;
			case 7:
				recoGest = imi::Gesture::HAND_GESTURE_7;
				break;
			case 8:
				recoGest = imi::Gesture::HAND_GESTURE_8;
				break;
			case 9:
				recoGest = imi::Gesture::HAND_GESTURE_9;
				break;
			case 10:
				recoGest = imi::Gesture::HAND_GESTURE_START;
				break;
			}
			m_client.getClient()->gestureStart(std::string("HandTrack"),imi::getTimeStamp(),0,recoGest);
			m_sendReco = true;
		}   
		catch( apache::thrift::TException &tx ) 
		{
			std::cerr << "EXCEPTION opening the network conn: " << tx.what() << "\n";
			m_client.receiveNetworkException();
		}
	}
	g_nGestureID_old = nGestureID;
}*/
// 保存图片
void HandWriting::MattoJPG1(Mat Track)
{
	//MattoTxtCount++;
	sprintf_s(pathMatTemp, "..\\TrackImage\\TrackImage1.%d.jpg", MattoTxtCount);//写名字
	//normalize(track,track,0,255,)
	//cvtColor(Track, Track, CV_RGB2GRAY);
	imwrite(pathMatTemp, Track);
	cout << "Save TrackImage1 Done!" << endl;

}
void HandWriting::MattoJPG2(Mat Track)
{
	//MattoTxtCount++;
	sprintf_s(pathMatTemp, "..\\TrackImage\\TrackImage2.%d.jpg", MattoTxtCount);//写名字
	//normalize(track,track,0,255,)
	//cvtColor(Track, Track, CV_RGB2GRAY);
	imwrite(pathMatTemp, Track);
	cout << "Save TrackImage2 Done!" << endl;

}
//转换成 32*32  保存图片  化成01图
Mat HandWriting::extractandtransform(Mat trackimage)
{
	resize(trackimage, trackimage, Size(32, 32));
	//MattoJPG2(trackimage);
	threshold(trackimage, trackimage, 0, 1, CV_THRESH_BINARY);
	return trackimage;
}
//保存到 txt中
void HandWriting::MattoTxt(Mat Track)
{
	ofstream outfile;
	MattoTxtCount++;
	sprintf_s(pathMatTemp, "..\\track\\temp%d.txt", MattoTxtCount);//写名字
	outfile.open(pathMatTemp); //存放数据的文件名  
	if (outfile.is_open())
	{
		outfile << Track << endl;    //message是程序中处理的数据  
		outfile.close();
		printf("temp%d.txt创建存储成功", MattoTxtCount);
		cout << endl;
	}
	else
	{
		printf("temp%d.txt创建存储失败", MattoTxtCount);
		cout << endl;
	}
	outfile.close();
}

//提取txt的mat 删除标点符号
string HandWriting::TXT01()
{
	string s1[300], s2[300];
	ifstream infile;
	//sprintf_s(pathMatTemp, "..\\track\\temp%d.txt", i);//写名字
	infile.open(pathMatTemp); //存放数据的文件名  
	if (!infile){
		cout << "Unable to open myfile";
		exit(1); // terminate with error  

	}
	ofstream outfile;
	sprintf_s(pathtrack, "..\\track\\%d.txt", MattoTxtCount);//写名字
	outfile.open(pathtrack);
	int w = 0;
	while (w<32)
	{
		getline(infile, s1[w]);
		Getnum(s1[w], s2[w]);
		outfile << s2[w];
		if (w<31)outfile << endl;
		w++;
	}
	outfile.close();
	printf("%d.txt创建存储成功", MattoTxtCount);
	cout << endl;
	cout << pathtrack << endl;
	return pathtrack;
}
void HandWriting::showdepthImageColor()
{
	//depthImageColor画图  KINECT2
/*	CvScalar color;
	switch (LeftHandState){
	case HandState_Open:
		color = cvScalar(255, 0, 0);
		break;
	case HandState_Closed:
		color = cvScalar(0, 255, 0);
		break;
	case HandState_Lasso:
		color = cvScalar(0, 0, 255);
		break;
	default://如果没有确定的手势，就不要画
		return;
	}*/
	//circle(depthImageColor, Point_HAND_LEFT, 20, color, -1);//画左手状态
	//circle(depthImageColor, FingerTipPoint, 10, FingerTipColor, -1);//画手指 
	//circle(depthImageColor, Point_SHOULDER_CENTER, 10, Scalar(255, 0, 0), -1);//画SHOULDER_CENTER 蓝色
	//circle(depthImageColor, Point_SPINE, 10, Scalar(255, 0, 0), -1);//画Point_SPINE 蓝色
	//circle(depthImageColor, Point_HIP_CENTER, 10, Scalar(255, 0, 0), -1);//画Point_HIP_CENTER 蓝色
	//circle(depthImageColor, Point_HAND_LEFT, 10, Scalar(0, 0, 255), -1);//画左手 红色
	//circle(depthImageColor, Point_HAND_RIGHT, 10, Scalar(0, 0, 255), -1);//画右手 红色
	imshow("depthImageColor", depthImageColor);
}
void HandWriting::Getnum(string &s1, string &s2)
{
	string::iterator   it;
	for (it = s1.begin(); it != s1.end(); it++)
	{
		if (*it != ' ' && *it != ',' && *it != '[' && *it != ';'&&*it != ']')
		{
			s2.push_back(*it);
		}
	}
}




void HandWriting::getDatafromKinect2(Mat useridimage, Mat depthimage, bool bodydepthdectected, bool skeletondectected)
{
	userIdImage = useridimage;
	depthImage = depthimage;
	//colorImage = colorimage;
	bodyDepthDectected = bodydepthdectected;
	skeletonDectected = skeletondectected;
}
void HandWriting::getDatafromKinect1(Mat depthimage, Mat colorimage, bool bodydepthdectected, bool skeletondectected, bool sayhello, bool goodbye)
{
	depthImage = depthimage;
	colorImage = colorimage;
	bodyDepthDectected = bodydepthdectected;
	skeletonDectected = skeletondectected;
	SAYHELLO = sayhello;
	GOODBYE = goodbye;
}

//void HandWriting::getBodyPoint(Point a, Point b, Point c, Point d, Point e, HandState LHState, HandState RHState)//KINECT2
void HandWriting::getBodyPoint(Point a, Point b, Point c, Point d, Point e)
{
	Point_SHOULDER_CENTER = a;
	Point_SPINE = b;
	Point_HIP_CENTER = c;
	Point_HAND_LEFT = d;
	Point_HAND_RIGHT = e;
	//LeftHandState = LHState; //KINECT2
	//RightHandState = RHState;//KINECT2
}

