#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <ctime>
#include "MvCameraControl.h"
//#include "../utils/string_operate.h"
#include "../utils/easylogging++.h"

#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H

struct cameraConfig {
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

struct camState {

	void* handler{nullptr};

	bool grabbing = false;

	bool offline = false;
	
	MV_CC_DEVICE_INFO deviceInfo = { 0 };
	MVCC_INTVALUE stIntvalue = { 0 };
	MV_FRAME_OUT_INFO_EX stInfo;
	unsigned char* pFrameBuf{nullptr};

	cameraConfig config;
};

camState* Cam_Get_State(int);

MV_CC_DEVICE_INFO_LIST Cam_Get_Online_Cameras();

int Cam_Open(int);

int Cam_Close(int);

int Cam_Start_Grabbing(int);

int Cam_Trigger_Software(int);

int Cam_Stop_Grabbing(int);

int Cam_Max_Number(void);

void InitWorkingCameras(cameraConfig*, int, int* result);
#endif
