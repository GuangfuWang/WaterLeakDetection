#pragma once
#include "util.h"
#include "config.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudafilters.hpp>


namespace water_leak
{
enum FlowType{
	LK = 0,
	FARNEBACK,
};
class WaterLeakDetection final
{
public:
	explicit WaterLeakDetection(SharedRef<Config>& config);
	~WaterLeakDetection();

	void detect(cv::Mat &curr_img, cv::Mat& orin, int &res);
private:
	void detectWithLK(cv::Mat &curr_img, cv::Mat& orin, int &res);
	void detectWithFB(cv::Mat &curr_img,cv::Mat& orin, int &res);
	void detectWithGPU(cv::Mat &curr_img, int &res);
	void detectWithCPU(cv::Mat &curr_img, int &res);
	void init();
	void plot(cv::Mat& img);
	void reset();
private:
	SharedRef<Config> m_config = nullptr;
	std::vector<float> m_prev_res;
	//for mask out the uninterested area.
	cv::Mat m_mask;
	cv::cuda::GpuMat m_cuda_mask;

	//for add the line, LK stuff.
	cv::Mat m_line_mask;
	cv::cuda::GpuMat m_cuda_prev;
	cv::Mat m_prev;
	cv::TermCriteria criteria;
	std::vector<uchar> m_status;
	std::vector<cv::Point2f> m_p0;
	std::vector<cv::Point2f> m_p1;
	std::vector<cv::Scalar> m_colors;

	//for FB stuff.
	cv::Ptr<cv::cuda::FarnebackOpticalFlow> m_fb;
	cv::Ptr<cv::cuda::Filter> m_gauss = cv::cuda::createGaussianFilter(0, 0, 
	cv::Size(5, 5), 1.5, 0); 

	bool m_init = false;
	bool need_roi = false;
	float resize_x = 2.0f;
	float resize_y = 2.0f;
	int reset_cnt = 0;
	int latency = 0;
	int alarm_cnt = 0;

};
}