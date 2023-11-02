
#include "../src/config.h"
#include "../src/detection.h"
#include "../src/model.h"

using namespace water_leak;

int main(int argc, char** argv){
	using path = std::filesystem::path;
	//prepare the input data.
	auto file = "/home/gpu/Downloads/datasets/water_leak/split/203.129-15-04-26-0-Trim.mp4";
	
	if(!Util::checkFileExist(file)){
		std::cerr<<"Given video is not exist..."<<std::endl;
		return 0;
	}
	auto in_path = path(file);
	cv::VideoCapture cap(in_path);
	cv::VideoWriter vw;
	path output_path = in_path.parent_path() / (in_path.stem().string() + ".result.mp4");
	vw.open(output_path,
			cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
			cap.get(cv::CAP_PROP_FPS),
			cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH),
					 cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
	cv::Mat img;
	bool init = false;
	cvModel * model = nullptr;
	while(cap.isOpened()){
		cap.read(img);
		if (img.empty())break;
		if(!init){
			init = true;
			model = Allocate_Algorithm(img,0,0);
			SetPara_Algorithm(model,0);
			UpdateParams_Algorithm(model);
		}
		Process_Algorithm(model,img);
		vw.write(img.clone());
	}
	cap.release();
	vw.release();
	Destroy_Algorithm(model);

	return 0;
}