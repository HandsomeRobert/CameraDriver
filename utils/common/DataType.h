#pragma once
#include <vector>
#include <string>
#include "../easylogging++.h"

//#define _HAS_STD_BYTE 0
typedef unsigned char byte;


namespace MCDataType {
    
    //typedef unsigned char byte;
    //ȫ�ֱ��������������ܶ��壡������Ȼ����ذ�������
    extern const char* ImagePathModel;                         //���&&����·��������������Ҫ���壬����ֶ��ض������ ������Ӧ����Ϊ= "./images"
    extern const char* FilePathModel;                          //���&&����·��, ����������Ҫ���壬����ֶ��ض������  Ӧ����Ϊ= "./MODEL_DATA"

    const int MaxObjectClasses = 50;        //����Ĭ�����������ٸ��������...

    const int IntBeginWord = 0x47424B50;    //����ͷ�� "PKBG"
    const int IntEndWord = 0x44454B50;      //����β�� "PKED"
    
    enum StatusCodes        //��������״ֵ̬
    {
        MC_OK        = 0,
        MC_ERROR     = 1,
        MC_Exception = 2
    };

    struct BaseParams {
        std::string modelFileName;                  //!< ģ���ļ���
        bool int8{ false };                         //!< Allow runnning the network in Int8 mode.
        bool fp16{ false };                         //!< Allow running the network in FP16 mode.
        int  batchSize{ 1 };                        //!< Number of inputs in a batch
        int  dlaCore{ -1 };                         //!< Specify the DLA core to run network on.
        int  numClasses{ 0 };                         //!< ʶ����������
        float confThresh{ 0.5 };                    //!< ����ֵ ��ֵ
        float nmsThresh{ 0.4 };                     //!< �Ǽ���ֵ����NMS����ֵ
        std::vector<std::string> dataDirs;          //!< Directory paths where sample data files are stored
        std::vector<std::string> inputTensorNames;  //!< ����Tensor����
        std::vector<std::string> outputTensorNames; //!< �����������

        std::vector<std::string> objectClassNames;  //!< �������������ƣ����±���ж�Ӧ��numClassΪ���������ֵ
    };

    struct ImagesBuffers {
        //int width{ 0 };
        //int height{ 0 };
        //int channels{ 0 };
        uint64_t    dataBytes{ 0 };                     //!< ���ݴ�С
        //int batchSize { 0 };//�ݲ�֧��batchSize
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
        int typeResult{ 0 };                              //!< ���ģ�͵����ö���ͣ�TypeModelInferResult
        int nbDims{ 0 };                                  //!< The number of dimensions.
        int d[8]{ 0 };                                    //!< ���8��The extent of each dimension.
        uint64_t    dataBytes{ 0 };                     //!< ���ݴ�С

        byte* data;                                     //!< ������
    };
    struct DimsInfo
    {
        int nbDims{ 0 };                                //!< The number of dimensions.
        int d[8]{ 0 };                                  //!< ���8��The extent of each dimension.
    };

    struct ObjectBox	//����ļ�����Ϣ
    {
        float x;
        float y;
        float width;
        float height;
        float thresh;

        std::vector<float>* probs;
    };

    struct BoxesToJS	//ר����4�������
    {
        float x;
        float y;
        float width;
        float height;
        float objectTypeID;
        float confidenceValue;
    };
}
