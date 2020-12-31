// CameraDriver.cpp: 定义应用程序的入口点。
//

#include "../utils/easylogging++.h"
#include "opencv2/core.hpp"
#include "../utils/DataManage.h"

using namespace std;


INITIALIZE_EASYLOGGINGPP;

void initLogSystem(void)
{
    try
    {
        el::Configurations defaultConf;

        defaultConf.setToDefault();
        defaultConf.setGlobally(el::ConfigurationType::Enabled, "true");
        defaultConf.setGlobally(el::ConfigurationType::ToFile, "true");
        defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
        defaultConf.setGlobally(el::ConfigurationType::Filename, "./logs/Camera_%datetime{%Y%M%d}.log");

        defaultConf.parseFromText("*DEBUG:\n FORMAT = %datetime %level [%logger] [%func] [%loc] %msg");
        defaultConf.parseFromText("*DEBUG:\n Enabled = false");//关闭debug显示功能
        defaultConf.parseFromText("*ERROR:\n TO_STANDARD_OUTPUT = true");//将ERROR信息打印到terminal中
        defaultConf.parseFromText("*ERROR:\n FORMAT = %level [%logger] [%loc] %msg");

        el::Loggers::reconfigureLogger("default", defaultConf);

    }
    catch (...)
    {
        std::cout << "Exception: initLogSystem Failed";
    }
}

int main()
{
    initLogSystem();
	cout << "Hello CMake." << endl;

    DPCDataManage::GlobalDataInitial(); //初始化全局数据和相机

    //cv::waiteKey(0);
	return 0;
}
