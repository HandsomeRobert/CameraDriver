/*
 * Copyright 2019 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#ifndef CAMERABASE_H
#define CAMERABASE_H
#include <string>
#include <string.h>
#include <math.h>
#include <iostream>

using namespace std;

namespace DPCGrabbers
{   
     enum CameraType
     {
 		HKVisionCamera	 = 0,
		BaslerCamera = 1,
 		BRCamera,
 		Others
     };

	 struct GrabbedImage
	 {
		 uint64_t Width;
		 uint64_t Height;
		 uint8_t  channels;
		 uint8_t* ImageBuffer;
	 };

	 struct CameraParameters {
		 // 相机参数配置
		 unsigned char* id;		// 相机序列号
		 int trigger_mode;		// 触发模式
		 int trigger_source;		// 触发源
		 float trigger_delay;	// 触发延时
		 int width;				// 相机画面宽度
		 int height;				// 相机画面高度
		 int BalanceWhiteAuto;	// 相机白平衡模式
		 int* BalanceWhiteRGB;	// 相机关闭白平衡时的值
		 int ExposureAuto;		// 相机曝光模式
		 int ExposureTime;		// 相机曝光时间
		 int Gain_Mode;			// 相机增益模式
		 double Gain_Value;		// 相机增益值
	 };

    class CameraBase
    {
    private:
		//pthread_t imageGrabThread;
		bool stopThreadImageGrab = false;
		static void ThreadImageGrab(void* pObject);
	
    public:
		CameraBase() {};
	    virtual ~CameraBase() {}//c++ supprot decline and define virtual function in sametime

	    virtual int Initial() = 0;
	    virtual int Open() = 0;
	    virtual int Close() = 0;
	    virtual int LoadParametersFromFile(const char* pFileName) = 0;
		virtual int SetParameters(void* parameters) = 0;
	    virtual int SaveParametersToFile(const char* pFileName)   = 0;
	    virtual int GetCameraInfo() = 0;
	    virtual int SetNumBuffers(int numberBuffers)  = 0;
	    virtual int SetTransferTime(int transferTime) = 0;
	    virtual int SetExposureTime(int exposureTime) = 0;
	    virtual int SetGain(int gain) = 0;
	    virtual int SetStrobelInvert(bool strobelInvert) = 0;
	    virtual int SetROI(int offsetX, int offsetY, int width, int height, bool reverseX, bool reverseY) = 0;
	    
	    virtual int RetrivePicture(GrabbedImage *pImage) = 0;
	    virtual int IsCameraGrabbing() = 0;
	    virtual int IsGrabSucceed()    = 0;
	    virtual int SaveGrabbedImage() = 0;
	    
	    void ThreadGrabInitial();
	    static int  StringToInt(char* pValue)
	    {
		int value = 0;
		char* temp = pValue;
		while(*temp != '\0')
		{
		    value = value*10 + (*temp - '0');
		    temp++;
		}
		
		return value;
	    }
	    static bool StringToBool(char* pValue)
	    {
		if(strcmp(pValue, "true") == 0) return true;
		else  return false;
	    }

// // 	    virtual bool SetGrabPictureNum(int Num) = 0;
// // 	    virtual bool SetAllowTransferTime(int timeTransfer) = 0;
    };
 
}

#endif // CAMERABASE_H
