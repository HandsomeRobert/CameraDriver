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
	// �����������
	unsigned char* id;		// ������к�
    int trigger_mode;		// ����ģʽ
    int trigger_source;		// ����Դ
    float trigger_delay;	// ������ʱ
	int width;				// ���������
	int height;				// �������߶�
	int BalanceWhiteAuto;	// �����ƽ��ģʽ
	int* BalanceWhiteRGB;	// ����رհ�ƽ��ʱ��ֵ
	int ExposureAuto;		// ����ع�ģʽ
	int ExposureTime;		// ����ع�ʱ��
	int Gain_Mode;			// �������ģʽ
	double Gain_Value;		// �������ֵ
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
