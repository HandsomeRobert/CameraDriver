#pragma once
#include <vector>
#include <string>
#include "../easylogging++.h"

//#define _HAS_STD_BYTE 0
typedef unsigned char byte;


namespace MCDataType {
    
    //typedef unsigned char byte;
    //全局变量的申明，不能定义！！！不然会多重包含错误
    extern const char* ImagePathModel;                         //相对&&绝对路径，仅声明，不要定义，会出现多重定义错误。 ！！！应设置为= "./images"
    extern const char* FilePathModel;                          //相对&&绝对路径, 仅声明，不要定义，会出现多重定义错误。  应设置为= "./MODEL_DATA"

    const int MaxObjectClasses = 50;        //定义默认最大允许多少个物体类别...

    const int IntBeginWord = 0x47424B50;    //报文头部 "PKBG"
    const int IntEndWord = 0x44454B50;      //报文尾部 "PKED"
    
    enum StatusCodes        //函数返回状态值
    {
        MC_OK        = 0,
        MC_ERROR     = 1,
        MC_Exception = 2
    };

    struct BaseParams {
        std::string modelFileName;                  //!< 模型文件名
        bool int8{ false };                         //!< Allow runnning the network in Int8 mode.
        bool fp16{ false };                         //!< Allow running the network in FP16 mode.
        int  batchSize{ 1 };                        //!< Number of inputs in a batch
        int  dlaCore{ -1 };                         //!< Specify the DLA core to run network on.
        int  numClasses{ 0 };                         //!< 识别的物体类别
        float confThresh{ 0.5 };                    //!< 置信值 阈值
        float nmsThresh{ 0.4 };                     //!< 非极大值抑制NMS的阈值
        std::vector<std::string> dataDirs;          //!< Directory paths where sample data files are stored
        std::vector<std::string> inputTensorNames;  //!< 输入Tensor名字
        std::vector<std::string> outputTensorNames; //!< 输出张量名字

        std::vector<std::string> objectClassNames;  //!< 待检测物体的名称，按下标进行对应，numClass为这个类别计数值
    };

    struct ImagesBuffers {
        //int width{ 0 };
        //int height{ 0 };
        //int channels{ 0 };
        uint64_t    dataBytes{ 0 };                     //!< 数据大小
        //int batchSize { 0 };//暂不支持batchSize
        byte* data = NULL;
    };

    enum TypeModelInferResult
    {
        NO_TYPE = 0,
        ONNXTYPE = 1,
        UFFTYPE = 2,
        CAFFETYPE,
        ENGINETYPE,
        OTHERTYPE
    };

    //const int MAX_DIMS = 8;
    struct OutputInferInfo {
        int typeResult{ 0 };                              //!< 输出模型的类别枚举型，TypeModelInferResult
        int nbDims{ 0 };                                  //!< The number of dimensions.
        int d[8]{ 0 };                                    //!< 最大8个The extent of each dimension.
        uint64_t    dataBytes{ 0 };                     //!< 数据大小

        byte* data;                                     //!< 数据域
    };
    struct DimsInfo
    {
        int nbDims{ 0 };                                //!< The number of dimensions.
        int d[8]{ 0 };                                  //!< 最大8个The extent of each dimension.
    };

    struct ObjectBox	//物体的检测框信息
    {
        float x;
        float y;
        float width;
        float height;
        float thresh;

        std::vector<float>* probs;
    };

    struct BoxesToJS	//专用于4个种类的
    {
        float x;
        float y;
        float width;
        float height;
        float objectTypeID;
        float confidenceValue;
    };
}
