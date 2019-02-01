#include "kinect1Reader.h"
#include "HandWriting.h"
#include "Global.h"

int main(int argc, char *argv[])
{
	kinect1Reader mykinect;
	HandWriting myhandwriting;
	mykinect.kinect1Reader_Init();
	while (1)
	{
		mykinect.kinect1Reader_Update();

		myhandwriting.getDatafromKinect1(
			mykinect.kinect1Reader_getdepthImage(),
			mykinect.kinect1Reader_getcolorImage(),
			mykinect.kinect1Reader_getbodyDepthDectected(),
			mykinect.kinect1Reader_getskeletonDectected(),
			mykinect.kinect1Reader_getSAYHELLO(),
			mykinect.kinect1Reader_getGOODBYE()
			);

		if (mykinect.kinect1Reader_getskeletonDectected())
		{
			myhandwriting.getBodyPoint(
				mykinect.kinect1Reader_getPoint_SHOULDER_CENTER(),
				mykinect.kinect1Reader_getPoint_SPINE(),
				mykinect.kinect1Reader_getPoint_HIP_CENTER(),
				mykinect.kinect1Reader_getPoint_HAND_LEFT(),
				mykinect.kinect1Reader_getPoint_HAND_RIGHT()
				);
		}
		myhandwriting.getFingetTip();
		myhandwriting.HandExtraciotn();
		myhandwriting.Interface();
		if (cvWaitKey(1) == 27)
			break;
	}

	mykinect.kinect1Reader_shutdown();
	return 0;
}
