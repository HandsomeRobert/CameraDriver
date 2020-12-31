#include "HKCamera.h"

DPCGrabbers::HKCamera::HKCamera():
	Para(new CameraParameters{}),
	cameraSerial("")
{
	this->Initial();

}

DPCGrabbers::HKCamera::HKCamera(std::string cameraSerials)
{
	this->Initial();
}

DPCGrabbers::HKCamera::~HKCamera()
{
	delete this->Para;

}

int DPCGrabbers::HKCamera::Initial()
{
	//this->LoadParametersFromXML();
	//this->SetParameters();
	return 0;
}

int DPCGrabbers::HKCamera::Open()
{
	return 0;
}

int DPCGrabbers::HKCamera::Close()
{
	return 0;
}

int DPCGrabbers::HKCamera::LoadParametersFromFile(const char* pFileName)
{

	return 0;
}

int DPCGrabbers::HKCamera::SetParameters(void* parameters)
{
	DPCGrabbers::CameraParameters* para = (DPCGrabbers::CameraParameters*)parameters;


	return 0;
}

int DPCGrabbers::HKCamera::SaveParametersToFile(const char* pFileName)
{
	return 0;
}

int DPCGrabbers::HKCamera::GetCameraInfo()
{
	return 0;
}

int DPCGrabbers::HKCamera::SetNumBuffers(int numberBuffers)
{
	return 0;
}

int DPCGrabbers::HKCamera::SetTransferTime(int transferTime)
{
	return 0;
}

int DPCGrabbers::HKCamera::SetExposureTime(int exposureTime)
{
	return 0;
}

int DPCGrabbers::HKCamera::SetGain(int gain)
{
	return 0;
}

int DPCGrabbers::HKCamera::SetStrobelInvert(bool strobelInvert)
{
	return 0;
}

int DPCGrabbers::HKCamera::SetROI(int offsetX, int offsetY, int width, int height, bool reverseX, bool reverseY)
{
	return 0;
}

int DPCGrabbers::HKCamera::RetrivePicture(GrabbedImage* pImage)
{
	return 0;
}

int DPCGrabbers::HKCamera::IsCameraGrabbing()
{
	return 0;
}

int DPCGrabbers::HKCamera::IsGrabSucceed()
{
	return 0;
}

int DPCGrabbers::HKCamera::SaveGrabbedImage()
{
	return 0;
}
