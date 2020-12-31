#include "camera.h"

using namespace std;

vector<camState> workingCameras{};

camState *Cam_Get_State(int deviceIndex) {
    return &workingCameras[deviceIndex];
}

//异常回调函数
void __stdcall ExceptionCB(unsigned int nMsgType, void *status) {
    LOG(ERROR) << "Camera-HKVISION:Camera offline...";
    camState *myStatus = (camState *) status;
    int nRet = MV_CC_CloseDevice(myStatus->handler);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Camera close failed...";
        return;
    }
    myStatus->offline = true;
}

MV_CC_DEVICE_INFO_LIST Cam_Get_Online_Cameras() {
    int nRet = -1;
    MV_CC_DEVICE_INFO_LIST m_stDevList = {0};
    unsigned int nTLayerType = MV_GIGE_DEVICE;
    nRet = MV_CC_EnumDevices(nTLayerType, &m_stDevList);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: EnumDevices fail:" << nRet;
    }
    if (m_stDevList.nDeviceNum == 0) {
        LOG(INFO) << "Camera-HKVISION: Found 0 online cameras";
    }
    return m_stDevList;
}

void InitWorkingCameras(cameraConfig *camserasConfig, int length, int *result) {

    bool devListLoaded = false;

    workingCameras.clear();

    MV_CC_DEVICE_INFO_LIST m_stDevList = {0};

    for (size_t i = 0; i < length; i++) {
        camState newCameraState;

        cameraConfig cameraConfig = camserasConfig[i];

        result[i] = 0;

        newCameraState.config = cameraConfig;

        bool realCameraFounded = false;

        if (!devListLoaded) {
            int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE, &m_stDevList);
            if (MV_OK != nRet) {
                result[i] = nRet;
                continue;
            }
            devListLoaded = true;
        }

        for (size_t j = 0; j < m_stDevList.nDeviceNum; j++) {
            MV_CC_DEVICE_INFO *deviceInfo = m_stDevList.pDeviceInfo[j];
            bool isEqual = true;
            //isEqual = unsignedCharPtrCompare(cameraConfig.id, deviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);

            if (isEqual) {

                realCameraFounded = true;

                memcpy(&newCameraState.deviceInfo, m_stDevList.pDeviceInfo[j], sizeof(MV_CC_DEVICE_INFO));

                break;
            }

        }

        if (!realCameraFounded) {
            result[i] = -1;
        }
        workingCameras.push_back(newCameraState);
    }
}

int Cam_Max_Number(void) {
    return workingCameras.size();
}

int Cam_Open(int nCameraIndex) {
    int nRet = -1;

    camState *statusPtr = Cam_Get_State(nCameraIndex);

    nRet = MV_CC_CreateHandle(&statusPtr->handler, &statusPtr->deviceInfo);

    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Create handler fail:" << nRet;
        return 2;
    }

    unsigned int nAccessMode = MV_ACCESS_Exclusive;
    unsigned short nSwitchoverKey = 0;
    nRet = MV_CC_OpenDevice(statusPtr->handler, nAccessMode, nSwitchoverKey);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: OpenDevice fail:" << nRet;
        return nRet;
    }

    // 更新相机配置

    // 脱机重连
    nRet = MV_CC_RegisterExceptionCallBack(statusPtr->handler, ExceptionCB, statusPtr);

    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: RegisterExceptionCallBack fail:" << nRet;
        return nRet;
    }

    statusPtr->offline = false;

    // 设置图像宽度
    unsigned int ValueWidth = statusPtr->config.width;
    nRet = MV_CC_SetIntValue(statusPtr->handler, "Width", ValueWidth);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Set image width fail:" << nRet;
    }


    // 设置图像高度
    unsigned int ValueHeight = statusPtr->config.height;
    nRet = MV_CC_SetIntValue(statusPtr->handler, "Height", ValueHeight);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Set image height fail:" << nRet;
    }

    // 设置触发方式
    nRet = MV_CC_SetEnumValue(statusPtr->handler, "TriggerMode", statusPtr->config.trigger_mode);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Set trigger mode fail:" << nRet;
    }

    if(statusPtr->config.trigger_mode == 1) {
        // 设置触发源
        nRet = MV_CC_SetEnumValue(statusPtr->handler, "TriggerSource", statusPtr->config.trigger_source);
        if (MV_OK != nRet) {
            LOG(ERROR) << "Camera-HKVISION: Set trigger source fail:" << nRet;
        }

        //设置触发延迟
        float ValueDelay = statusPtr->config.trigger_delay;
        nRet = MV_CC_SetFloatValue(statusPtr->handler, "TriggerDelay", ValueDelay);

        if (MV_OK != nRet) {
            LOG(ERROR) << "Camera-HKVISION: Set trigger delay fail:" << nRet;
        }
    }

    //设置白平衡模式，0~关闭，1~连续，2~一次
    nRet = MV_CC_SetBalanceWhiteAuto(statusPtr->handler, statusPtr->config.BalanceWhiteAuto);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Set SetBalanceWhiteAuto fail:" << nRet;
    }
    if (statusPtr->config.BalanceWhiteAuto == 0) {
        //设置红绿蓝分量
        nRet = MV_CC_SetBalanceRatioRed(statusPtr->handler, statusPtr->config.BalanceWhiteRGB[0]);
        if (MV_OK != nRet) {
            LOG(ERROR) << "Camera-HKVISION: SetBalanceRatioRed error:" << nRet;
        }
        nRet = MV_CC_SetBalanceRatioGreen(statusPtr->handler, statusPtr->config.BalanceWhiteRGB[1]);
        if (MV_OK != nRet) {
            LOG(ERROR) << "Camera-HKVISION: Set SetBalanceRatioGreen fail:" << nRet;
        }
        nRet = MV_CC_SetBalanceRatioBlue(statusPtr->handler, statusPtr->config.BalanceWhiteRGB[2]);
        if (MV_OK != nRet) {
            LOG(ERROR) << "Camera-HKVISION: Set SetBalanceRatioBlue fail:" << nRet;
        }
    }

    //设置自动曝光模式,0~关闭，1~连续，2~一次
    nRet = MV_CC_SetExposureAutoMode(statusPtr->handler, statusPtr->config.ExposureAuto);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Set SetExposureAutoMode fail:" << nRet;
    }

    //设置曝光时间，设置曝光时间时将会关闭相机的自动曝光模式.
    if (statusPtr->config.ExposureAuto == 0) {
        nRet = MV_CC_SetExposureTime(statusPtr->handler, statusPtr->config.ExposureTime);
        if (MV_OK != nRet) {
            LOG(ERROR) << "Camera-HKVISION: Set SetExposureTime fail:" << nRet;
        }
    }

    //设置增益方式，0~关闭，1~一次，2~连续
    nRet = MV_CC_SetGainMode(statusPtr->handler, statusPtr->config.Gain_Mode);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Set SetGainMode fail:" << nRet;
    }

    if (statusPtr->config.Gain_Mode == 0) {
        //设置设备增益,最大值15.0062
        float ValueGain = statusPtr->config.Gain_Value;
        nRet = MV_CC_SetGain(statusPtr->handler, ValueGain);
        if (MV_OK != nRet) {
            LOG(ERROR) << "Camera-HKVISION: Set Gain fail:" << nRet;
        }
    }

    // Detection network optimal package size(It only works for the GigE camera)
    if (statusPtr->deviceInfo.nTLayerType == MV_GIGE_DEVICE) {
        int nPacketSize = MV_CC_GetOptimalPacketSize(statusPtr->handler);
        if (nPacketSize > 0) {
            nRet = MV_CC_SetIntValue(statusPtr->handler, "GevSCPSPacketSize", nPacketSize);
            if (nRet != MV_OK) {
                LOG(ERROR) << "Camera-HKVISION: Set Packet Size fail nRet" << nRet;
            }
        } else {
            LOG(INFO) << "Camera-HKVISION: Get Packet Size fail, nRet" << nPacketSize;
        }
    }
    return 0;
}

int Cam_Close(int nCameraIndex) {
    int nRet = -1;
    camState *statusPtr = Cam_Get_State(nCameraIndex);

    if (statusPtr->grabbing) {
        Cam_Stop_Grabbing(nCameraIndex);
    }

    nRet = MV_CC_CloseDevice(statusPtr->handler);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: CloseDevice fail nRet" << nRet;
        return 2;
    }

    nRet = MV_CC_DestroyHandle(statusPtr->handler);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: DestroyHandle fail nRet" << nRet;
        return 3;
    }
    return 0;
}

int Cam_Start_Grabbing(int nCameraIndex) {

    int nRet = -1;
    camState *statusPtr = Cam_Get_State(nCameraIndex);

    nRet = MV_CC_StartGrabbing(statusPtr->handler);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: StartGrabbing fail nRet" << nRet;
        return 1;
    }

    //获取一帧数据的大小
    nRet = MV_CC_GetIntValue(statusPtr->handler, "PayloadSize", &(statusPtr->stIntvalue));
    if (nRet != MV_OK) {
        LOG(ERROR) << "Camera-HKVISION: Get PayloadSize fail nRet" << nRet;
        return 3;
    }

    statusPtr->pFrameBuf = (unsigned char *) malloc(statusPtr->stIntvalue.nCurValue);

    memset(&(statusPtr->stInfo), 0, sizeof(MV_FRAME_OUT_INFO));

    statusPtr->grabbing = true;

    return 0;
}

int Cam_Trigger_Software(int nCameraIndex) {

    int nRet = -1;
    camState *statusPtr = Cam_Get_State(nCameraIndex);

    if (!statusPtr->grabbing) {
        return 0;
    }

    nRet = MV_CC_SetCommandValue(statusPtr->handler, "TriggerSoftware");
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: Trigger software fail nRet" << nRet;
        return 1;
    }

    return 0;
}

int Cam_Stop_Grabbing(int nCameraIndex) {
    int nRet = -1;
    camState *statusPtr = Cam_Get_State(nCameraIndex);

    if (!statusPtr->grabbing) {
        return 0;
    }

    free(statusPtr->pFrameBuf);

    statusPtr->grabbing = false;

    nRet = MV_CC_StopGrabbing(statusPtr->handler);
    if (MV_OK != nRet) {
        LOG(ERROR) << "Camera-HKVISION: StopGrabbing fail nRet" << nRet;
        return 1;
    }

    return 0;
}