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
//#define GOOGLE_GLOG_DLL_DECL            // !!!!!!ʹ�þ�̬glog�������,��Ȼ�ᱨ��������
//#define GLOG_NO_ABBREVIATED_SEVERITIES  // ��Windows.h��ERROR��ͻ������д˺궨��
//#include "glog/logging.h"
#include "./easylogging++.h"


template <typename T>
using SampleUniquePtr = std::unique_ptr<T, samplesCommon::InferDeleter>;

namespace MCCommonFunc {

    /// <summary>
    /// ���ͺ���������pDestination��ƫ��λ��offset����int����ֵvalue
    /// </summary>
    /// <param name="pDestination">����ַ</param>
    /// <param name="offset">ƫ�Ƶ�ַ</param>
    /// <param name="value">��ֵ��ֵ</param>
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
    /// ��ȡָ��·���µ�����/���֣�����·��/�ļ���
    /// </summary>
    /// <param name="fileDir"> ��Ҫ���ҵ�Ŀ¼��֧�����·���;���·��</param>
    /// <param name="vectorFileName"> ��������ã��洢��ȡ���ļ���Ϣ </param>
    /// <param name="formate"> ��ʽ��ɸѡ��Ҫ�������ļ���ʽ��Ĭ�ϻ�ȡ���������ļ�eg: .jpg�� </param>
    /// <param name="isGetFullPath"> �Ƿ��ȡ����·����fasle: ���ļ����� true: ����·��</param>
    /// <returns> ������</returns>
    static bool getFileNames(std::string fileDir, std::vector<std::string>& vectorFileName, std::string formate = "", bool isGetFullPath = false) {//Ĭ��fomrmateΪ��������
        //�ļ����
        intptr_t    hFile = 0;
        //�ļ���Ϣ
        struct _finddata_t fileinfo;
        std::string p;

        if ((hFile = _findfirst(p.assign(fileDir).append("\\*" + formate).c_str(), &fileinfo)) != -1) {
            do {
                std::string tempNames(fileinfo.name);

                //�����Ŀ¼,����֮;��,������б�
                if ((fileinfo.attrib & _A_SUBDIR))                                                      //������ҵ������ļ���
                {
                    if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)            //�����ļ��в���
                        getFileNames(p.assign(fileDir).append("\\").append(fileinfo.name), vectorFileName, formate);
                }
                else                                                                                    //������ҵ��Ĳ������ļ���
                {
                    if (isGetFullPath)
                        vectorFileName.push_back(p.assign(fileDir).append("\\").append(fileinfo.name)); //�����ļ�·��
                    else
                        vectorFileName.push_back(fileinfo.name);                                        //����ȡ�ļ�����

                }
            } while (_findnext(hFile, &fileinfo) == 0);
            _findclose(hFile);
        }
        else
            return false;

        return true;
    }

    /// <summary>
    /// ��FilePathModel·���¶�ȡ.json�ļ�����ʼ��ϵͳ���� params
    /// </summary>
    /// <param name="params">��ʼ���Ĳ�����Ϣ</param>
    /// <returns>����״̬</returns>
    static bool initializeParams(std::vector<MCDataType::BaseParams>& params) {
        MCDataType::BaseParams     paramsTemp;
        std::vector<std::string> fileNamesTemp;
        std::ifstream            iStream;
        Json::Reader             reader;                // ����json��Json::Reader   
        Json::Value              root;                  // Json::Value��һ�ֺ���Ҫ�����ͣ����Դ����������͡���int, string, object, array         
        string modelFilePath(MCDataType::FilePathModel);            // ��FilPathModelת��Ϊstring�͵�modelFilePath�������Ա���������� 

        if (!getFileNames(MCDataType::FilePathModel, fileNamesTemp, "networkConfig.json", false))    //û�ҵ�networkConfig.json�ļ���ֱ�ӷ���false
        {
            LOG(ERROR) << "�Ҳ�����ǰִ�г���ͬ��Ŀ¼--MODEL_DATA/ �µ��㷨������ϢnetworkConfig.json�ļ�";
            return false;
        }

        paramsTemp.dataDirs.push_back(MCDataType::FilePathModel);

        for (auto itr = fileNamesTemp.begin(); itr != fileNamesTemp.end(); ++itr) {
            iStream.open(modelFilePath + "\\" + itr->c_str(), ios::binary);
            if (!iStream.is_open()) {
                LOG(ERROR) << "ifstream�����޷����ļ���-->" << *itr << " <--��鿴�Ƿ��ļ���ռ��";
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
                ////���������ļ�����
                //if (paramsTemp.modelFileName.find(".onnx") != paramsTemp.modelFileName.npos) {
                //    
                //}
                ////���������ļ�����
                //if (paramsTemp.modelFileName.find(".uff") != paramsTemp.modelFileName.npos) {
                //}
                //���������ļ�����
                //if (paramsTemp.modelFileName.find(".caffemodel") != paramsTemp.modelFileName.npos) {
                //    paramsTemp.reservedFileName1 = root["reservedFileName1"].asCString();//Լ��Ϊ������ṹxxx.prototxt��
                //    paramsTemp.reservedFileName2 = root["reservedFileName2"].asCString();//Լ��Ϊ����������Ϣxxx.txt
                //}

                params.emplace_back(paramsTemp);
            }
            else {
                LOG(ERROR) << "json�����޷������ļ���-->" << *itr << " <--���������";
                iStream.close();    //�ر��ļ�����
                return false;
            }

            iStream.close();        //�ر��ļ���
        }

        return true;
    }


    /// <summary>
    /// ����Opencv��ͼ���ϻ����ο򣬲���ͼ�񱣴浽./images·����
    /// </summary>
    /// <param name="image">ͼ��</param>
    /// <param name="imageID">Ҫ�����ͼ�����</param>
    /// <param name="drawBoxs">���ο�x,yΪ������������</param>
    /// <param name="strideX">X�����ŵı���(eg: strideX = 2048/ 416-->2048Ϊԭͼ��С��416ΪAIģ�������С)</param>
    /// <param name="strideY">Y�����ŵı���</param>
    /// <returns>���</returns>
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
    /// ����TensorRT���棬��./MODEL_DATA/Ŀ¼�¼���xxx.engine�ļ����з����л�
    /// </summary>
    /// <param name="engine">ģ������</param>
    /// <param name="DLACore">�Ƿ���DLA�˼���</param>
    /// <param name="err">����������</param>
    /// <returns>ִ��״̬</returns>
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
        tempBox = boxA; //ǳ�����Ϳ�����
        boxA = boxB;
        boxB = tempBox;

    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="boxsClass">Ҫ���������</param>
    /// <param name="probID"></param>
    /// <returns></returns>
    static bool BoxesSortFromProb_I(std::vector<MCDataType::ObjectBox>& boxsClass, const int probID)//ֻ�ĵ�ַ
    {
        if (!boxsClass.size()) return false;//û��Ԫ�ؽ���ֱ�ӷ���δ����false

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

        xx1 = max(bbox1.x - bbox1.width / 2, bbox2.x - bbox2.width / 2);//���������Ͻ�x,
        yy1 = max(bbox1.y - bbox1.height / 2, bbox2.y - bbox2.height / 2);//���������Ͻ�y,
        xx2 = min(bbox1.x + bbox1.width / 2, bbox2.x + bbox2.width / 2);//���������½�x,
        yy2 = min(bbox1.y + bbox1.height / 2, bbox2.y + bbox2.height / 2);//���������½�y,

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

