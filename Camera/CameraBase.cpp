/*
 * Copyright 2019 <copyright holder> <email>
 */
#include "CameraBase.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

/*注意：由于若在此构造函数内调用ThreadGrabInitial由于此为抽象类，
 * 调用ThreadGrabInitial时CameraBase并未被完全初始化，会导致访问空对象pObject==>PCamera出现报错
 */
void DPCGrabbers::CameraBase::ThreadGrabInitial()
{
//    //创建图像获取线程
//    pthread_t imageGrab;
//    pthread_attr_t threadAttr;
//    sched_param threadParam;
//    pthread_attr_init(&threadAttr);//Initial the thread attributes
//    pthread_attr_getschedparam(&threadAttr, &threadParam);//get priority parameteres
//    threadParam.__sched_priority = 2;//set thread priority
//    pthread_attr_setschedparam(&threadAttr, &threadParam);//set the schedule parameteres
//    
////	if(!pthread_create(&tcpCycle_t, &threadAttr, ReceiveCycleTCP, NULL))   //try use function template
//    if(pthread_create(&imageGrab, &threadAttr, DPCGrabbers::CameraBase::ThreadImageGrab, this) != 0)   //try use function template
//    {
//	fprintf(stderr, "Create ImageGrabThread Failed error:%s(errno:%d)\n", strerror(errno), errno);
//	exit(0);
//    }

	DPCGrabbers::CameraBase::ThreadImageGrab(this);
}


//!****************循环抓取图像线程*******************************
void DPCGrabbers::CameraBase::ThreadImageGrab( void* pObject )
{
    DPCGrabbers::CameraBase *pCamera = (DPCGrabbers::CameraBase*)pObject;
    int image_count = 0;
    DPCGrabbers::GrabbedImage* pTempImageInfo = new DPCGrabbers::GrabbedImage;
    
    while(1)
    {
// 	std::cout<<"GetInto ThreadImageGrab while(1)"<<std::endl;
	
	if(pCamera->IsCameraGrabbing())
	{
	    pCamera->RetrivePicture(pTempImageInfo);

	    if(pCamera->IsGrabSucceed())
	    {
		// Access the image data.
		//cam_object->image_save(image_count, ptrGrabResult);
		// CImagePersistence::Save( ImageFileFormat_Png, "The_Grabbed_image.png", ptrGrabResult);
		
		const uint8_t *pImageBuffer = pTempImageInfo->pData;//pImageBuffer saves a only one dimension gray images
		cv::Mat opencvImage = cv::Mat(pTempImageInfo->Height, pTempImageInfo->Width, CV_8UC3, (uint8_t *)pTempImageInfo->pData);//convert pylon image to OpenCV Image
		//cv::imshow("GrabbedImage", opencvImage);
		
		
		cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl << endl;
		cout << "SizeX: " << pTempImageInfo->Width << endl;
		cout << "SizeY: " << pTempImageInfo->Height << endl;
		image_count++;
		
		printf("The_buffer_size_on_Grab:%d\n", (int)strlen((char*)pImageBuffer));
		//边缘检测
		//Edge_Detection((unsigned char *)pImageBuffer, pTempImageInfo->Width, pTempImageInfo->Height);    
	   
	    }  
	}	 
    }
}

