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
#include <Windows.h>
#include <conio.h>

#include "MvCameraControl.h"
#include "../utils/common/DataType.h"
#include "../utils/easylogging++.h"
#include "../utils/rapidxml/rapidxml.hpp"
#include "../utils/rapidxml/rapidxml_utils.hpp"
#include "../utils/rapidxml/rapidxml_print.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp" 
#include "opencv2/imgproc.hpp" 

namespace DPCGrabbers 
{
	using namespace MCDataType;
	using namespace rapidxml;

	class HKCamera : public CameraBase
	{
	private:
		CameraParameters* Para = NULL;
		
		/*CameraInfo*/
		std::string 	ConfigurationID;
		std::string 	GrabberName;
		std::string 	CameraID;
		std::string 	CameraName;
		std::string 	CameraSerial;
		std::string 	CameraModelName;
		std::string 	CameraDeviceID;
		std::string    	CameraIndex;
		std::string 	ConfigFile;
		std::string 	CameraIP;
		int   		NumBuffers;
		int 		TransferTime;
		/*Camera Configuration*/
		int 		ExposureTime;
		int 		Gain;	//感光增益
		std::string TriggerSource;
		std::string TriggerEdge;
		bool Strobe1Invert;	//闪光灯信号翻转
		int Strobe1Delay;
		int Strobe1Length;
		std::string ScanMode;
		int WhiteBalanceR;
		int WhiteBalanceG;
		int WhiteBalanceB;
		int BlackLevel;
		int Gamma;
		int OffsetX;
		int OffsetY;
		int Width;
		int Height;
		bool ReverseX;
		bool ReverseY;
		std::string TriggerSourceLine;
		std::string TriggerEdgeLine;

		int TriggerDelayLine;
		std::string ShaftEncoderLineSourcePhaseA;
		std::string ShaftEncoderLineSourcePhaseB;
		int ShaftEncoderMultiplier;
		int ShaftEncoderDivider;


	public:
		void* handle = NULL;
		MV_CC_DEVICE_INFO* pDeviceInfo = NULL;

		HKCamera();
		HKCamera(std::string cameraSerials);
		~HKCamera();
		int Initial();
		int Open();
		int Close();
		int StartGrabbing();
		int StopGrabbing();
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
