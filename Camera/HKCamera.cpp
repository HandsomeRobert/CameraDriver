#include "HKCamera.h"

bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
        printf("Camera Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chSerialNumber);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
        printf("Device Number: %d\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.nDeviceNumber);
    }
    else
    {
        printf("Not support.\n");
    }

    return true;
}

void __stdcall ImageCallBackEx(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)//BGR格式图像
{
    if (pFrameInfo)
    {
        DPCGrabbers::HKCamera* pCamera = (DPCGrabbers::HKCamera*)pUser;
        printf("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n",
            pFrameInfo->nWidth, pFrameInfo->nHeight, pFrameInfo->nFrameNum);
        
        DPCGrabbers::GrabbedImage tempImage;
        tempImage.Width  = pFrameInfo->nWidth;
        tempImage.Height = pFrameInfo->nHeight;
        tempImage.pData = (byte*)malloc(pFrameInfo->nFrameLen);
        memcpy(tempImage.pData, pData, pFrameInfo->nFrameLen);

        cv::Mat testImage(pFrameInfo->nHeight,pFrameInfo->nWidth, CV_8UC3, pData, 0);
        cv::imwrite(string("./Test") + std::to_string(pFrameInfo->nFrameNum) + ".bmp", testImage);

        MV_SAVE_IMG_TO_FILE_PARAM pstSaveImage;
        pstSaveImage.pData = pData;
        pstSaveImage.enImageType = MV_Image_Bmp;
        pstSaveImage.enPixelType = PixelType_Gvsp_BGR8_Packed;

        pCamera->GrabbedImageBuffer->push_back(tempImage);
    }
}


DPCGrabbers::HKCamera::HKCamera():
    Para(new CameraParameters),
    CameraSerial(""),
    handle(NULL),
    pDeviceInfo(new MV_CC_DEVICE_INFO)
{
    this->CameraSerial = '0';
    this->GrabbedImageBuffer = new std::list<GrabbedImage>();
	this->Initial();
}

DPCGrabbers::HKCamera::HKCamera(std::string cameraSerials):
    Para(new CameraParameters),
    CameraSerial(""),
    handle(NULL),
    pDeviceInfo(new MV_CC_DEVICE_INFO)
{
    this->GrabbedImageBuffer = new std::list<GrabbedImage>();
	this->Initial();
}

DPCGrabbers::HKCamera::~HKCamera()
{
    if (!handle)
    {
        LOG(ERROR) << "handle is NULL, check whether handle is Created ?";
    }

    if(pDeviceInfo)
    this->StopGrabbing();                   //停止取流
    this->Close();                          //关闭相机
    int nRet = MV_CC_DestroyHandle(handle); //销毁句柄
    if (MV_OK != nRet)
    {
        LOG(ERROR) << "Destroy Handle fail! nRet [" << nRet << "]";
    }

    //清除资源
    this->handle = NULL;
	delete this->Para;
    delete this->pDeviceInfo;
}

int DPCGrabbers::HKCamera::Initial()
{
	this->LoadParametersFromFile("./Configuration/CameraSettings.xml");
	//this->SetParameters();

	try
	{
        int nRet = MV_OK;

        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (MV_OK != nRet)
        {
            LOG(ERROR) << "Enum Devices fail! nRet [" << nRet << "]";
            return MC_ERROR;
        }

        if (stDeviceList.nDeviceNum > 0)
        {
            for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
            {
                //pDeviceInfo = stDeviceList.pDeviceInfo[i];
                memcpy(pDeviceInfo, stDeviceList.pDeviceInfo[i], sizeof(MV_CC_DEVICE_INFO));//保留设备信息

                if (NULL == pDeviceInfo)
                {
                    break;
                }
                PrintDeviceInfo(pDeviceInfo);

                if (0 == strcmp(this->CameraSerial.c_str(), (char*)pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber))
                {
                    if (MV_CC_IsDeviceAccessible(pDeviceInfo, MV_ACCESS_Control))
                    {
                        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[i]);
                        if (MV_OK != nRet)
                        {
                            LOG(ERROR) << "Create Handle fail! nRet [" << nRet << "]";
                            return MC_ERROR;
                        }
                    }
                    else
                    {
                        LOG(ERROR) << "Camera Device is not accessable";
                        return MC_ERROR;
                    }
                }
            }

            if (NULL == handle)
            {
                LOG(ERROR) << "Create Camera Failed, Serial: " << this->CameraSerial << "  cannot be found" ;
            }
        }
        else
        {
            LOG(ERROR) << "Find No Devices!";
            return MC_ERROR;
        }

        this->Open();
        this->SetParameters(NULL);
        //nRet = MV_CC_FeatureSave(handle, "./FeatureFile.ini");//存储相机配置文件


        MVCC_INTVALUE stParam = { 0 };
        nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
        if (MV_OK != nRet)
        {
            LOG(ERROR) << "Get PayloadSize fail! nRet  [" << nRet << "]";
            return MC_ERROR;
        }


        //MV_CC_RegisterImageCallBackForBGR() //MV_CC_RegisterImageCallBackEx
        nRet = MV_CC_RegisterImageCallBackForBGR(handle, ImageCallBackEx, this);//生成BGR图像，handle
        if (MV_OK != nRet)
        {
            LOG(ERROR) << "Register Image CallBack fail! nRet [" << nRet << "]";
            return MC_ERROR;
        }

        this->StartGrabbing();

	}
	catch (...)//const std::exception&
	{
		LOG(ERROR) << "[ERROR] In DPCGrabbers::HKCamera::Initial";
	}
	
    return MC_OK;
}

int DPCGrabbers::HKCamera::Open()
{
    if (!handle)
    {
        LOG(ERROR) << "handle is NULL, check whether handle is Created ?";
        return MC_ERROR;
    }

    int nRet = MV_CC_OpenDevice(handle);
    if (MV_OK != nRet)
    {
        LOG(ERROR) << "Open Device fail! nRet [" << nRet << "]";
        return MC_ERROR;
    }

	return MC_OK;
}

int DPCGrabbers::HKCamera::Close()
{
    if (!handle)
    {
        LOG(ERROR) << "handle is NULL, check whether handle is Created ?";
        return MC_ERROR;
    }

    int nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        printf("Close Device fail! nRet [0x%x]\n", nRet);
        return MC_ERROR;
    }

	return MC_OK;
}

int DPCGrabbers::HKCamera::StartGrabbing()
{
    int nRet = 0;
    if (!handle)
    {
        LOG(ERROR) << "handle is NULL, check whether handle is Created ?";
        return MC_ERROR;
    }

    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        LOG(ERROR) << "Start Grabbing fail! nRet [" << nRet << "]";
        return MC_ERROR;
    }

    return MC_OK;
}

int DPCGrabbers::HKCamera::StopGrabbing()
{
    int nRet = 0;
    if (!handle)
    {
        LOG(ERROR) << "handle is NULL, check whether handle is Created ?";
        return MC_ERROR;
    }

    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        LOG(ERROR) << "Stop Grabbing fail!nRet[" << nRet << "]";
        return MC_ERROR;
    }

    return MC_OK;
}

int DPCGrabbers::HKCamera::LoadParametersFromFile(const char* pFileName)
{
    file<> fdoc(pFileName);
    xml_document<> doc;
    doc.parse<0>(fdoc.data());


    /*Get root head*/
    xml_node<>* root = doc.first_node("CameraSettings");
    xml_node<>* Grabbers = root->first_node("Grabbers");
    xml_node<>* BaseCamera = Grabbers->first_node("BaseCamera");
    xml_node<>* GrabbersChild = BaseCamera->first_node();

    this->ConfigurationID = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->GrabberName     = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->CameraID        = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->CameraName      = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->CameraSerial    = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->CameraModelName = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->CameraDeviceID  = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->CameraIndex     = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->ConfigFile      = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->CameraIP        = GrabbersChild->value(); GrabbersChild = GrabbersChild->next_sibling();
    this->NumBuffers      = stoi(GrabbersChild->value()); GrabbersChild = GrabbersChild->next_sibling();
    this->TransferTime    = stoi(GrabbersChild->value()); GrabbersChild = GrabbersChild->next_sibling();
    /*Configuration Node*/
    xml_node<>* ConfigurationChild = GrabbersChild->first_node();
    this->ExposureTime    = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->Gain            = stoi(ConfigurationChild->value()); ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->TriggerSource   = ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->TriggerEdge     = ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->Strobe1Invert   = CameraBase::StringToBool(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->Strobe1Delay    = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->Strobe1Length   = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->ScanMode        = ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->WhiteBalanceR   = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->WhiteBalanceG   = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->WhiteBalanceB   = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->BlackLevel      = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->Gamma           = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->OffsetX         = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->OffsetY         = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->Width           = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->Height          = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->ReverseX        = CameraBase::StringToBool(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->ReverseY        = CameraBase::StringToBool(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->TriggerSourceLine = ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->TriggerEdgeLine   = ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->TriggerDelayLine  = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->ShaftEncoderLineSourcePhaseA = ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->ShaftEncoderLineSourcePhaseB = ConfigurationChild->value(); ConfigurationChild = ConfigurationChild->next_sibling();
    this->ShaftEncoderMultiplier       = stoi(ConfigurationChild->value()); ConfigurationChild = ConfigurationChild->next_sibling();
    this->ShaftEncoderDivider          = stoi(ConfigurationChild->value());


	return MC_OK;
}

int DPCGrabbers::HKCamera::SetParameters(void* parameters)
{
	DPCGrabbers::CameraParameters* para = (DPCGrabbers::CameraParameters*)parameters;
    int nRet = 0;

    if (this->pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
        if (nPacketSize > 0)
        {
            nRet = MV_CC_SetIntValue(handle, "GevSCPSPacketSize", nPacketSize);
            if (nRet != MV_OK)
            {
                LOG(WARNING) << "Warning: Set Packet Size fail nRet[" << nRet << "]";
            }
        }
        else
        {
            LOG(WARNING) << "Warning: Get Packet Size fail nRet[" << nRet << "]";
        }
    }

    nRet = MV_CC_SetEnumValue(handle, "TriggerMode", MV_TRIGGER_MODE_OFF);
    if (MV_OK != nRet)
    {
        LOG(ERROR) << "Set Trigger Mode fail!  nRet [" << nRet << "]";
        return MC_ERROR;
    }



	return MC_OK;
}

int DPCGrabbers::HKCamera::SaveParametersToFile(const char* pFileName)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::GetCameraInfo()
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::SetNumBuffers(int numberBuffers)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::SetTransferTime(int transferTime)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::SetExposureTime(int exposureTime)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::SetGain(int gain)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::SetStrobelInvert(bool strobelInvert)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::SetROI(int offsetX, int offsetY, int width, int height, bool reverseX, bool reverseY)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::RetrivePicture(GrabbedImage* pImage)
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::IsCameraGrabbing()
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::IsGrabSucceed()
{
	return MC_OK;
}

int DPCGrabbers::HKCamera::SaveGrabbedImage()
{
	return MC_OK;
}
