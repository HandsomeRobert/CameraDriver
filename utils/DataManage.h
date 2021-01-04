/**
 * An Program to Manage the Data Used in Inspector Program
 * Authors:Robert
 * Email:chenqi1024s@163.com
 * *******/

#ifndef DATAMANAGE_H
#define DATAMANAGE_H

#include <iostream>
#include <list>
#include <string.h>
#include <string>
//#include <unistd.h>	//linux getcwd() function
#include <direct.h>     //windows getcwd() headers
#include "./rapidxml/rapidxml.hpp"
#include "./rapidxml/rapidxml_utils.hpp"
#include "./rapidxml/rapidxml_print.hpp"
#include "../Camera/CameraBase.h"
#include "../Camera/HKCamera.h"


namespace DPCDataManage{

    using namespace std;
    using namespace rapidxml;
    
    typedef struct /*other ObjectInfo remain to be added*/
    {
	    int MachinID;
	    int ObjectID;
	
	    unsigned char *pImage;
	    bool AnlsResult;
    }ObjectInfo;							        //检测对象的处理结果数据内容
    
    typedef struct
    {
	    std::string Grabbers;
	    std::string CameraType;
	    std::string CameraSerial;
    }GrabberCameraInfo;							    //存储采集图像的SDK类型和相机类型，再根据CameraSerial来识别相机
       
    void GlobalDataInitial();						//所有全局变量初始化			
    
    //!******************内部使用的函数***********************
    void CamerasInitial();						    //相机初始化
    bool LoadCamerasSerialFromFile();					//从xml读取所有相机驱动接口和序列号serials	
    std::string GetProjectPath();					//获取当前工程的目录用以查找下级配置文件,CameraSetting.xml等
    
}

extern std::list<DPCGrabbers::CameraBase*>* G_listCameras;			            //全局变量:相机对象list
extern std::list<DPCDataManage::GrabberCameraInfo>* G_listGrabberCameraInfo;	//从xml文件读取的相机信息


#endif
