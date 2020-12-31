#pragma once
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <io.h>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include "./DataType.h"
#include "json/json.h"
//
//#define GOOGLE_GLOG_DLL_DECL            // !!!!!!使用静态glog库用这个,不然会报错啊啊啊啊
//#define GLOG_NO_ABBREVIATED_SEVERITIES  // 与Windows.h的ERROR冲突，需进行此宏定义
//#include "glog/logging.h"
#include "./easylogging++.h"


template <typename T>
using SampleUniquePtr = std::unique_ptr<T, samplesCommon::InferDeleter>;

namespace MCCommonFunc {

    /// <summary>
    /// 泛型函数，设置pDestination处偏移位置offset处的int型数值value
    /// </summary>
    /// <param name="pDestination">基地址</param>
    /// <param name="offset">偏移地址</param>
    /// <param name="value">数值数值</param>
    template <typename T1>
    void SetValueOffset(char* pDestination, int offset, T1 value)
    {
        pDestination = pDestination + offset;
        *(T1*)pDestination = value;
    }

    template <typename T>
    inline T const& MaxAB(T const& A, T const& B)
    {
        return(A > B ? A : B);
    }

    template <typename T>
    inline T const& MinAB(T const& A, T const& B)
    {
        return(A < B ? A : B);
    }

    /// <summary>
    /// 获取指定路径下的所有/部分，完整路径/文件名
    /// </summary>
    /// <param name="fileDir"> 所要查找的目录，支持相对路径和绝对路径</param>
    /// <param name="vectorFileName"> 传入的引用，存储获取的文件信息 </param>
    /// <param name="formate"> 格式，筛选想要搜索的文件格式（默认获取所有类型文件eg: .jpg） </param>
    /// <param name="isGetFullPath"> 是否获取完整路径，fasle: 仅文件名； true: 完整路径</param>
    /// <returns> 处理结果</returns>
    static bool getFileNames(std::string fileDir, std::vector<std::string>& vectorFileName, std::string formate = "", bool isGetFullPath = false) {//默认fomrmate为所有类型
        //文件句柄
        intptr_t    hFile = 0;
        //文件信息
        struct _finddata_t fileinfo;
        std::string p;

        if ((hFile = _findfirst(p.assign(fileDir).append("\\*" + formate).c_str(), &fileinfo)) != -1) {
            do {
                std::string tempNames(fileinfo.name);

                //如果是目录,迭代之;否,则加入列表
                if ((fileinfo.attrib & _A_SUBDIR))                                                      //如果查找到的是文件夹
                {
                    if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)            //进入文件夹查找
                        getFileNames(p.assign(fileDir).append("\\").append(fileinfo.name), vectorFileName, formate);
                }
                else                                                                                    //如果查找到的不是是文件夹
                {
                    if (isGetFullPath)
                        vectorFileName.push_back(p.assign(fileDir).append("\\").append(fileinfo.name)); //完整文件路径
                    else
                        vectorFileName.push_back(fileinfo.name);                                        //仅获取文件名称

                }
            } while (_findnext(hFile, &fileinfo) == 0);
            _findclose(hFile);
        }
        else
            return false;

        return true;
    }

    /// <summary>
    /// 从FilePathModel路径下读取.json文件，初始化系统参数 params
    /// </summary>
    /// <param name="params">初始化的参数信息</param>
    /// <returns>返回状态</returns>
    static bool initializeParams(std::vector<MCDataType::BaseParams>& params) {
        MCDataType::BaseParams     paramsTemp;
        std::vector<std::string> fileNamesTemp;
        std::ifstream            iStream;
        Json::Reader             reader;                // 解析json用Json::Reader   
        Json::Value              root;                  // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         
        string modelFilePath(MCDataType::FilePathModel);            // 将FilPathModel转换为string型的modelFilePath变量，以便后续操作。 

        if (!getFileNames(MCDataType::FilePathModel, fileNamesTemp, "networkConfig.json", false))    //没找到networkConfig.json文件，直接返回false
        {
            LOG(ERROR) << "找不到当前执行程序同级目录--MODEL_DATA/ 下的算法配置信息networkConfig.json文件";
            return false;
        }

        paramsTemp.dataDirs.push_back(MCDataType::FilePathModel);

        for (auto itr = fileNamesTemp.begin(); itr != fileNamesTemp.end(); ++itr) {
            iStream.open(modelFilePath + "\\" + itr->c_str(), ios::binary);
            if (!iStream.is_open()) {
                LOG(ERROR) << "ifstream引擎无法打开文件：-->" << *itr << " <--请查看是否文件被占用";
                return false;
            }

            if (!reader.parse(iStream, root)) {
                paramsTemp.modelFileName = root["AIModelName"].asCString();
                paramsTemp.batchSize = root["batchSize"].asInt();
                paramsTemp.dlaCore = root["dlaCore"].asFloat();
                paramsTemp.int8 = root["int8"].asBool();
                paramsTemp.fp16 = root["fp16"].asBool();
                paramsTemp.confThresh = root["confThresh"].asFloat();
                paramsTemp.nmsThresh = root["nmsThresh"].asFloat();

                auto tempRoot = root["inputTensorNames"];
                for (auto itrInput = tempRoot.begin(); itrInput != tempRoot.end(); ++itrInput) {
                    paramsTemp.inputTensorNames.emplace_back(itrInput->asCString());
                }
                tempRoot = root["outputTensorNames"];
                for (auto itrOutput = tempRoot.begin(); itrOutput != tempRoot.end(); ++itrOutput) {
                    paramsTemp.outputTensorNames.emplace_back(itrOutput->asCString());
                }

                tempRoot = root["objectClassNames"];
                paramsTemp.numClasses = tempRoot.size();
                for (auto itrTemp = tempRoot.begin(); itrTemp != tempRoot.end(); ++itrTemp) {
                    paramsTemp.objectClassNames.emplace_back(itrTemp->asCString());
                }
                ////后续附加文件解析
                //if (paramsTemp.modelFileName.find(".onnx") != paramsTemp.modelFileName.npos) {
                //    
                //}
                ////后续附加文件解析
                //if (paramsTemp.modelFileName.find(".uff") != paramsTemp.modelFileName.npos) {
                //}
                //后续附加文件解析
                //if (paramsTemp.modelFileName.find(".caffemodel") != paramsTemp.modelFileName.npos) {
                //    paramsTemp.reservedFileName1 = root["reservedFileName1"].asCString();//约定为神经网络结构xxx.prototxt，
                //    paramsTemp.reservedFileName2 = root["reservedFileName2"].asCString();//约定为分类种类信息xxx.txt
                //}

                params.emplace_back(paramsTemp);
            }
            else {
                LOG(ERROR) << "json引擎无法解析文件：-->" << *itr << " <--请查找问题";
                iStream.close();    //关闭文件流，
                return false;
            }

            iStream.close();        //关闭文件流
        }

        return true;
    }


    /// <summary>
    /// 利用Opencv在图像上画矩形框，并将图像保存到./images路径下
    /// </summary>
    /// <param name="image">图像</param>
    /// <param name="imageID">要保存的图像序号</param>
    /// <param name="drawBoxs">矩形框，x,y为矩形中心坐标</param>
    /// <param name="strideX">X向缩放的比例(eg: strideX = 2048/ 416-->2048为原图大小，416为AI模型输入大小)</param>
    /// <param name="strideY">Y向缩放的比例</param>
    /// <returns>结果</returns>
    static bool DrawPicture(cv::Mat& image, const int& imageID, std::vector<MCDataType::BoxesToJS>& drawBoxs, const float strideX, const float strideY)
    {
        //cv::Mat tempImage = cv::imread("./images/test.bmp");

        for (auto itr = drawBoxs.begin(); itr != drawBoxs.end(); ++itr)
        {
            cv::rectangle(image, cv::Point((itr->x - (itr->width / 2)) * strideX, ((itr->y - (itr->height / 2)) * strideY)), cv::Point((itr->x + (itr->width / 2)) * strideX, ((itr->y + (itr->height / 2)) * strideY)), cv::Scalar(0, 0, 255), 3);
            //if()

            //cv::putText(tempImage, );
        }

        //cv::namedWindow("TestOut Image", cv::WINDOW_GUI_NORMAL);
        //cv::imshow("TestOut Image", this->originImage);
        //cv::waitKey(0);
        std::string outputPath(std::string(MCDataType::ImagePathModel).append("/output"));
        cv::imwrite(outputPath.append(std::to_string(imageID) + ".jpg"), image);

        return true;
    }

    /// <summary>
    /// 利用TensorRT引擎，从./MODEL_DATA/目录下加载xxx.engine文件进行反序列化
    /// </summary>
    /// <param name="engine">模型名称</param>
    /// <param name="DLACore">是否开启DLA核加速</param>
    /// <param name="err">错误数据流</param>
    /// <returns>执行状态</returns>
    static ICudaEngine* LoadEngine(const std::string& engine, int DLACore, std::ostream& err)
    {
        std::ifstream engineFile("./MODEL_DATA/" + engine, std::ios::binary);
        if (!engineFile)
        {
            err << "Error opening engine file: " << engine << std::endl;
            return nullptr;
        }

        engineFile.seekg(0, engineFile.end);
        long int fsize = engineFile.tellg();
        engineFile.seekg(0, engineFile.beg);

        std::vector<char> engineData(fsize);
        engineFile.read(engineData.data(), fsize);
        if (!engineFile)
        {
            err << "Error loading engine file: " << engine << std::endl;
            return nullptr;
        }

        SampleUniquePtr<IRuntime> runtime{ createInferRuntime(gLogger.getTRTLogger()) };

        if (DLACore != -1)
        {
            runtime->setDLACore(DLACore);
        }

        return runtime->deserializeCudaEngine(engineData.data(), fsize, nullptr);
    }


    static void  swapBoxes(MCDataType::ObjectBox& boxA, MCDataType::ObjectBox& boxB)
    {
        MCDataType::ObjectBox tempBox{};
        tempBox = boxA; //浅拷贝就可以了
        boxA = boxB;
        boxB = tempBox;

    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="boxsClass">要分类的数据</param>
    /// <param name="probID"></param>
    /// <returns></returns>
    static bool BoxesSortFromProb_I(std::vector<MCDataType::ObjectBox>& boxsClass, const int probID)//只改地址
    {
        if (!boxsClass.size()) return false;//没有元素进来直接返回未处理false

        for (int i = 0; i < boxsClass.size() - 1; ++i)//bubble sort
        {
            for (int j = 0; j < boxsClass.size() - 1 - i; ++j)
            {
                if ((*(boxsClass[j].probs))[probID] < (*(boxsClass[j + 1].probs))[probID])
                {
                    swapBoxes(boxsClass[j], boxsClass[j + 1]);
                }
            }
        }

        //(*(boxsClass.begin()->probs))[probID] = 2;
        return true;

    }

    // ========================= Activation Function: ReLU =======================
    template<typename _Tp>
    static bool activation_function_SigMoid(_Tp* src, _Tp* dst, int length)
    {
        for (int i = 0; i < length; ++i) {
            dst[i] = 1 / (1 + exp(-src[i]));
        }

        return true;
    }

    static float get_iou_value(MCDataType::ObjectBox& bbox1, MCDataType::ObjectBox& bbox2)
    {
        //float iou = 0;
        float xx1, yy1, xx2, yy2;

        xx1 = max(bbox1.x - bbox1.width / 2, bbox2.x - bbox2.width / 2);//交集区左上角x,
        yy1 = max(bbox1.y - bbox1.height / 2, bbox2.y - bbox2.height / 2);//交集区左上角y,
        xx2 = min(bbox1.x + bbox1.width / 2, bbox2.x + bbox2.width / 2);//交集区右下角x,
        yy2 = min(bbox1.y + bbox1.height / 2, bbox2.y + bbox2.height / 2);//交集区右下角y,

        int insection_width, insection_height;

        insection_width = std::max(xx2 - xx1, float(0));
        insection_height = std::max(yy2 - yy1, float(0));


        float insection_area, union_area, iou;
        insection_area = (float)insection_width * insection_height;
        union_area = float(bbox1.width * bbox1.height + bbox2.width * bbox2.height) - insection_area;

        iou = insection_area / union_area;

        return iou;
    }



}

