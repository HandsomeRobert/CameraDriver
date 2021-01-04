/**
 * An Program to Manage the Data Used in Inspector Program
 * Authors:Robert
 * Email:chenqi1024s@163.com
 *
 *******/

#include "DataManage.h"

std::list<DPCGrabbers::CameraBase*>				*G_listCameras;			    //全局变量:相机对象list
std::list<DPCDataManage::GrabberCameraInfo>		*G_listGrabberCameraInfo;	//从xml文件读取的相机信息

void DPCDataManage::GlobalDataInitial() 
{
	G_listCameras = new std::list<DPCGrabbers::CameraBase*>();
	G_listGrabberCameraInfo = new std::list<DPCDataManage::GrabberCameraInfo>();

	CamerasInitial();//初始相机，创建相机控制对象
}

bool DPCDataManage::LoadCamerasSerialFromFile()
{
    try
    {
		std::string camerasFilePath = GetProjectPath();
		camerasFilePath.append("/Configuration/CameraSettings.xml");

		rapidxml::file<> fdoc(camerasFilePath.c_str());
		xml_document<> doc;
		doc.parse<0>(fdoc.data());
	
		//Get root head
		xml_node<> *root = doc.first_node();
	
		for(xml_node<> *Grabbers = root->first_node("Grabbers"); Grabbers != NULL; Grabbers = Grabbers->next_sibling())
		{
			std::string GrabberName = Grabbers->first_attribute("type")->value();
	    
			for(xml_node<> *BaseCamera = Grabbers->first_node("BaseCamera"); BaseCamera != NULL; BaseCamera = BaseCamera->next_sibling())
			{
			xml_node<> *CameraSerial = BaseCamera->first_node("CameraSerial");
		
			GrabberCameraInfo cameraInfo;
			cameraInfo.Grabbers = GrabberName;
			cameraInfo.CameraType = BaseCamera->first_attribute("type")->value();
			cameraInfo.CameraSerial = CameraSerial->value();
		
			G_listGrabberCameraInfo->push_back(cameraInfo);
			}
		}
	
		return true;
    }
    catch(std::exception &e)
    {
		std::cout<<"Generic Exception Occured:"<<e.what()<<std::endl;
		return false;
    }
}

std::string DPCDataManage::GetProjectPath()
{
    try
	{
		char pCurPath[256];
		getcwd(pCurPath, 256);
		std::string filePath(pCurPath);	//此filePath定位到了build文件夹下，需定位到工程文件夹，所以要删除部分
		//std::string delBuffer = "/build";
		//int pos = filePath.find(delBuffer);
		//filePath = filePath.erase(pos, delBuffer.size());
	
		return filePath;
    }
    catch(std::exception &e){
		std::cout<<"Generic Exception Occured:"<<e.what()<<std::endl;
    }
}

void DPCDataManage::CamerasInitial()
{
    LoadCamerasSerialFromFile();
    
    for(auto itor = G_listGrabberCameraInfo->begin(); itor != G_listGrabberCameraInfo->end(); itor++)
    {
		if(!strcmp((*itor).CameraType.c_str(), "HKVisionCamera"))//执行海康相机初始化
		{
			DPCGrabbers::CameraBase *pTempCamera = new DPCGrabbers::HKCamera((*itor).CameraSerial);
			pTempCamera->ThreadGrabInitial();		//初始化捕获图像线程
	    
			G_listCameras->push_back(pTempCamera);		//最后如果要pop的话注意要释放指针对象的内存，pop不会调用对象的析构函数
	
		}	    
		else if (!strcmp((*itor).CameraType.c_str(), "BaslerCamera"))
			;//执行basler相机类型初始化    
		else if(!strcmp((*itor).CameraType.c_str(), "NETCamera"))
			;//执行NET类型相机初始化
		else if(!strcmp((*itor).CameraType.c_str(), "BRCamera"))
			;//执行B&R相机类型初始化
	}
}
