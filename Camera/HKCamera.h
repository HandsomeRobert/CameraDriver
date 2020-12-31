#pragma once
/*
 * Copyright 2021 <chenqi33> <email>
 * 
 */
#include <string>
#include <string.h>
#include <math.h>
#include <iostream>
#include "CameraBase.h"

namespace DPCGrabbers 
{
	class HKCamera : public CameraBase
	{
	private:
		CameraParameters* Para = NULL;
		string cameraSerial = "";

	public:
		HKCamera();
		HKCamera(std::string cameraSerials);
		~HKCamera();
		int Initial();
		int Open();
		int Close();
		int LoadParametersFromFile(const char* pFileName);
		int SetParameters(void *parameters);
		int SaveParametersToFile(const char* pFileName);
		int GetCameraInfo();
		//SetAOI(int x, int y);
		int SetNumBuffers(int numberBuffers);
		int SetTransferTime(int transferTime);
		int SetExposureTime(int exposureTime);
		int SetGain(int gain);
		int SetStrobelInvert(bool strobelInvert);
		int SetROI(int offsetX, int offsetY, int width, int height, bool reverseX, bool reverseY);

		int RetrivePicture(GrabbedImage* pImage);
		int IsCameraGrabbing();
		int IsGrabSucceed();
		int SaveGrabbedImage();

	};

}
