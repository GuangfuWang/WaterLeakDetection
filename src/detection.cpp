#include "detection.h"
#include "config.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/optflow.hpp>
#include <sys/stat.h>

namespace water_leak
{


void WaterLeakDetection::detect(cv::Mat &curr_img, int &res)
{
	cv::Mat temp;
	cv::resize(curr_img, temp, cv::Size(curr_img.cols / resize_x, curr_img.rows / resize_y));
	if (!m_init) {
		init();
		m_line_mask = cv::Mat::zeros(temp.size(), temp.type());
		temp.copyTo(m_prev, m_mask);
		cv::cvtColor(m_prev, m_prev, cv::COLOR_BGR2GRAY);
		reset();
		res = 0;
		return;
	}
	if (m_config->USE_GPU) {
		cv::Mat process_img;
		temp.copyTo(process_img, m_mask);
		cv::cvtColor(process_img, process_img, cv::COLOR_BGR2GRAY);
		detectWithGPU(process_img, res);
		plot(temp);
		m_prev = process_img.clone();
		cv::resize(temp, curr_img, curr_img.size());
		m_status.clear();
		m_p1.clear();
		if (m_p1.size() > m_config->THRESHOLD_LEN) {
			m_prev_res.push_back(1);
		}
		float sum = 0.0f;
		for (int i = m_prev_res.size() - 1; i >= 0 && i > m_prev_res.size() - 5; i--) {
			sum+=m_prev_res[i];
		}
		sum/=4.0f;
		if(sum>m_config->THRESHOLD){
			res = 1;
			std::cout<<"detected!\n";
		}
		reset_cnt--;
		if(0==reset_cnt){
			reset();
		}
	}
	else {
		cv::Mat process_img;
		temp.copyTo(process_img, m_mask);
		cv::cvtColor(process_img, process_img, cv::COLOR_BGR2GRAY);
		detectWithGPU(process_img, res);
		plot(temp);
		m_prev = process_img.clone();
		cv::resize(temp, curr_img, curr_img.size());
		m_status.clear();
		m_p1.clear();
		if (m_p1.size() > m_config->THRESHOLD_LEN) {
			m_prev_res.push_back(1);
		}
		float sum = 0.0f;
		for (int i = m_prev_res.size() - 1; i >= 0 && i > m_prev_res.size() - 5; i--) {
			sum+=m_prev_res[i];
		}
		sum/=4.0f;
		if(sum>m_config->THRESHOLD){
			res = 1;
		}
		reset_cnt--;
		if(0==reset_cnt){
			reset();
		}
	}

}

// the image is gray/resized/masked.
void WaterLeakDetection::detectWithGPU(cv::Mat &curr_img, int &res)
{
	std::vector<cv::Point2f> curr_p;
	std::vector<float> err;
	cv::calcOpticalFlowPyrLK(m_prev, curr_img, m_p0, m_p1, m_status, err,
							 cv::Size(15, 15), m_config->PLK_MAX_LEVEL, criteria);
}
void WaterLeakDetection::detectWithCPU(cv::Mat &curr_img, int &res)
{
	std::vector<cv::Point2f> curr_p;
	std::vector<float> err;
	cv::calcOpticalFlowPyrLK(m_prev, curr_img, m_p0, m_p1, m_status, err,
							 cv::Size(15, 15), m_config->PLK_MAX_LEVEL, criteria);
}

WaterLeakDetection::WaterLeakDetection(SharedRef<Config>& config)
{
	reset_cnt = m_config->RESET_COUNT;
	m_config = config;
}
void WaterLeakDetection::init()
{
	if (m_init)return;
	auto h = m_config->IMAGE_SHAPE[1];
	auto w = m_config->IMAGE_SHAPE[2];
	auto rect = m_config->DETECTION_POS;
	m_mask = cv::Mat::zeros(h / resize_y, w / resize_x, CV_8UC3);
	cv::rectangle(m_mask, cv::Rect(rect[0] / resize_x,
								   rect[1] / resize_y,
								   (rect[2] - rect[0]) / resize_x,
								   (rect[3] - rect[1]) / resize_y),
				  cv::Scalar(255,255,255), -1);

	criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS),
								m_config->TERM_CRITERIA_MAX_COUNT, m_config->TERM_CRITERIA_EPSILON);
	cv::RNG rng;
	for (int i = 0; i < m_config->MAX_CORNERS; i++) {
		int r = rng.uniform(0, 256);
		int g = rng.uniform(0, 256);
		int b = rng.uniform(0, 256);
		m_colors.push_back(cv::Scalar(r, g, b));
	}
	m_init = true;
}

//the img here should only be resized. without gray/masked.
void WaterLeakDetection::plot(cv::Mat &img)
{
	std::vector<cv::Point2f> good_new;
	cv::Mat temp = img.clone();
	auto mask = m_line_mask.clone();
	for (uint i = 0; i < m_p0.size(); i++) {
		if (m_status[i] == 1) {
			good_new.push_back(m_p1[i]);
			// plot the trajectory.
			line(mask, m_p1[i], m_p0[i], m_colors[i], m_config->DRAW_LINE_WIDTH);
			circle(temp, m_p1[i], m_config->DRAW_PLOT_RADIUS, m_colors[i], -1);
		}
	}
	cv::add(mask, temp, img);
	m_p0 = good_new;
}
void WaterLeakDetection::reset()
{
	cv::goodFeaturesToTrack(m_prev, m_p0, m_config->MAX_CORNERS,
							m_config->QUALITY_LEVEL, m_config->MIN_DIST,
							cv::Mat(), m_config->BLOCK_SIZE, m_config->USE_HARRIS_DETECTOR,
							m_config->SIFT_K);
	reset_cnt = m_config->RESET_COUNT;
}

WaterLeakDetection::~WaterLeakDetection() = default;
}
