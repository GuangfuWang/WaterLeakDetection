#include "model.h"
#include "detection.h"
#include "config.h"
#include <string>
#include <iostream>

namespace water_leak{

class InferModel {
public:
	InferModel(SharedRef<Config>& config) {
		mDeploy = createSharedRef<WaterLeakDetection>(config);
	}
public:
	SharedRef<WaterLeakDetection> mDeploy;
};

static void *GenModel(SharedRef<Config>& config) {
	InferModel *model = new InferModel(config);
	return reinterpret_cast<void *>(model);
}

cvModel* Allocate_Algorithm(cv::Mat &input_frame, int algID, int gpuID){
	cv::cuda::setDevice(gpuID);

	std::string file;
	if(Util::checkFileExist("./water_leak_detection.yaml"))
		file = "./water_leak_detection.yaml";
	else if(Util::checkFileExist("../config/water_leak_detection.yaml")){
		file = "../config/water_leak_detection.yaml";
	}else{
		std::cerr<<"Cannot find YAML file!"<<std::endl;
	}
	auto config = createSharedRef<Config>(0, nullptr,file);
	config->IMAGE_SHAPE = {input_frame.channels(), input_frame.rows, input_frame.cols};
	auto *ptr = new cvModel();
	ptr->FrameNum = 0;
	ptr->Frameinterval = 0;
	ptr->countNum = 0;
	ptr->width = input_frame.cols;
	ptr->height = input_frame.rows;
	ptr->iModel = GenModel(config);
	// in case that user has not set the water_leak_detection.yaml file properly.
	return ptr;
}
void SetPara_Algorithm(cvModel *pModel,int algID){

}
void UpdateParams_Algorithm(cvModel *pModel){

}
void Process_Algorithm(cvModel *pModel, cv::Mat &input_frame){
	auto model = reinterpret_cast<InferModel *>(pModel->iModel);
	model->mDeploy->detect(input_frame, pModel->alarm);
	if(pModel->alarm)std::cout<<"detected!\n";
}
void Destroy_Algorithm(cvModel *pModel){
	if (pModel->iModel){
		auto model = reinterpret_cast<InferModel *>(pModel->iModel);
		delete model;
		model = nullptr;
	}
	if (pModel) {
		delete pModel;
		pModel = nullptr;
	}
	std::cout<<"Deconstruct the model done..."<<std::endl;
}
}
