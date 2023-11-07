#include "detection.h"
#include "config.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/optflow.hpp>
#include <opencv2/cudawarping.hpp>
#include "opencv2/cudaarithm.hpp"
#include <sys/stat.h>

namespace water_leak
{

	void WaterLeakDetection::detectWithLK(cv::Mat &curr_img, cv::Mat &orin, int &res)
	{
		cv::Mat temp;
		cv::resize(curr_img, temp, cv::Size(curr_img.cols / resize_x, curr_img.rows / resize_y));
		if (!m_init)
		{
			init();
			m_line_mask = cv::Mat::zeros(temp.size(), temp.type());
			temp.copyTo(m_prev, m_mask);
			cv::cvtColor(m_prev, m_prev, cv::COLOR_BGR2GRAY);
			reset();
			res = 0;
			return;
		}
		if (m_config->USE_GPU)
		{
			cv::Mat process_img;
			temp.copyTo(process_img, m_mask);
			cv::cvtColor(process_img, process_img, cv::COLOR_BGR2GRAY);
			detectWithGPU(process_img, res);
			plot(temp);
			m_prev = process_img.clone();
			cv::resize(temp, curr_img, curr_img.size());
			m_status.clear();
			m_p1.clear();
			if (m_p1.size() > m_config->THRESHOLD_LEN)
			{
				m_prev_res.push_back(1);
			}
			float sum = 0.0f;
			for (int i = m_prev_res.size() - 1; i >= 0 && i > m_prev_res.size() - 5; i--)
			{
				sum += m_prev_res[i];
			}
			sum /= 4.0f;
			if (sum > m_config->THRESHOLD)
			{
				res = 1;
				std::cout << "detected!\n";
			}
			reset_cnt--;
			if (0 == reset_cnt)
			{
				reset();
			}
		}
		else
		{
			cv::Mat process_img;
			temp.copyTo(process_img, m_mask);
			cv::cvtColor(process_img, process_img, cv::COLOR_BGR2GRAY);
			detectWithGPU(process_img, res);
			plot(temp);
			m_prev = process_img.clone();
			cv::resize(temp, curr_img, curr_img.size());
			m_status.clear();
			m_p1.clear();
			if (m_p1.size() > m_config->THRESHOLD_LEN)
			{
				m_prev_res.push_back(1);
			}
			float sum = 0.0f;
			for (int i = m_prev_res.size() - 1; i >= 0 && i > m_prev_res.size() - 5; i--)
			{
				sum += m_prev_res[i];
			}
			sum /= 4.0f;
			if (sum > m_config->THRESHOLD)
			{
				res = 1;
			}
			reset_cnt--;
			if (0 == reset_cnt)
			{
				reset();
			}
		}
	}
	void WaterLeakDetection::detectWithFB(cv::Mat &curr_img, cv::Mat &orin, int &res)
	{
		res = 0;
		if (!m_init)
		{
			if (m_config->USE_GPU)
			{
				cv::cuda::GpuMat temp(curr_img);
				cv::cuda::resize(temp, temp, cv::Size(curr_img.cols / resize_x, curr_img.rows / resize_y));
				init();
				reset();
				if (need_roi)
				{
					temp.copyTo(m_cuda_prev, m_cuda_mask);
				}
				else
				{
					temp.copyTo(m_cuda_prev);
				}

				cv::cuda::cvtColor(m_cuda_prev, m_cuda_prev, cv::COLOR_BGR2GRAY);
				cv::cuda::equalizeHist(m_cuda_prev, m_cuda_prev);
			}
			else
			{
				cv::Mat temp;
				cv::resize(curr_img, temp, cv::Size(curr_img.cols / resize_x, curr_img.rows / resize_y));
				init();
				reset();
				if (need_roi)
				{
					temp.copyTo(m_prev, m_mask);
				}
				else
				{
					temp.copyTo(m_prev);
				}

				cv::cvtColor(m_prev, m_prev, cv::COLOR_BGR2GRAY);
			}
			return;
		}
		if (m_config->USE_GPU)
		{
			cv::cuda::GpuMat temp;
			cv::cuda::GpuMat tt(curr_img);
			cv::cuda::resize(tt, temp, cv::Size(curr_img.cols / resize_x, curr_img.rows / resize_y));
			cv::cuda::GpuMat process_img;
			if (need_roi)
				temp.copyTo(process_img, m_cuda_mask);
			else
			{
				temp.copyTo(process_img);
			}
			cv::cuda::cvtColor(process_img, process_img, cv::COLOR_BGR2GRAY);
			cv::cuda::equalizeHist(process_img, process_img);

			cv::cuda::GpuMat sub;
			cv::cuda::absdiff(m_cuda_prev, process_img, sub);
			cv::Mat sub_cpu;
			sub.download(sub_cpu);
			double abs_mean = cv::mean(sub_cpu)[0];
			if (abs_mean >= m_config->THRESHOLD_UPPER)
			{
				cv::cuda::GpuMat flow(m_cuda_prev.size(), CV_32FC2);				
				m_fb->calc(m_cuda_prev, process_img, flow);
				cv::cuda::GpuMat flow_parts[2];
				cv::cuda::split(flow, flow_parts);

				cv::cuda::GpuMat magnitude, angle, magn_norm;
				cv::cuda::cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
				cv::cuda::normalize(magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX, -1);
				cv::cuda::GpuMat const_norm(angle.rows, angle.cols, angle.type(),
											cv::Scalar::all((1.f / 360.f) * (180.f / 255.f)));
				cv::cuda::multiply(angle, const_norm, angle);
				// build hsv image
				cv::cuda::GpuMat _hsv[3], hsv8, bgr;
				_hsv[0] = angle;
				_hsv[1] = cv::cuda::GpuMat(angle.rows, angle.cols, CV_32F, cv::Scalar::all(1.0f));
				_hsv[2] = magn_norm;

				cv::cuda::GpuMat hsv(angle.rows, angle.cols, CV_32FC3);
				cv::cuda::merge(_hsv, 3, hsv);
				hsv.convertTo(hsv8, CV_8U, 255.0);
				cv::cuda::cvtColor(hsv8, bgr, cv::COLOR_HSV2BGR);

				cv::cuda::GpuMat bgr_;
				cv::cuda::cvtColor(bgr, bgr_, cv::COLOR_BGR2GRAY);
				// bgr.setTo(0, bgr<0.15f);
				cv::Mat flow_graph;
				bgr_.download(flow_graph);
				cv::Mat img;
				cv::cuda::resize(bgr, bgr, cv::Size(curr_img.cols, curr_img.rows));
				bgr.download(img);
				auto mean_score = cv::mean(flow_graph);

				// std::cout<<"Curr Mean: "<<mean_score[0]<<std::endl;

				m_prev_res.push_back(mean_score[0]);
				auto sum = 0.0f;
				if (m_prev_res.size() > m_config->MOVING_LEN * 2)
				{
					for (int i = m_prev_res.size() - 1; i >= 0 && i > m_prev_res.size() - 5; i--)
					{
						sum += m_prev_res[i];
					}
					sum /= 4.0f;
					m_prev_res.erase(m_prev_res.begin());
				}
				// std::cout << "Mean: " << sum << "/ Threshold: " << m_config->THRESHOLD << std::endl;
				if (sum > m_config->THRESHOLD)
				{
					latency += 2;
					if (latency >= 2 * m_config->ALARM_COUNT)
					{
						res = 1;
						latency = 0;
						alarm_cnt = 60;
					}
				}
				else
				{
					latency--;
					if (latency < 0)
						latency = 0;
				}
				if (alarm_cnt)
				{
					cv::add(orin, img, orin);
				}
				alarm_cnt--;
				if (alarm_cnt < 0)
					alarm_cnt = 0;
			}

			reset_cnt--;
			if (0 == reset_cnt)
			{
				reset();
				m_cuda_prev = process_img.clone();
			}
		}
		else
		{
			cv::Mat temp;
			cv::resize(curr_img, temp, cv::Size(curr_img.cols / resize_x, curr_img.rows / resize_y));
			cv::Mat process_img;
			if (need_roi)
			{
				temp.copyTo(process_img, m_mask);
			}
			else
				temp.copyTo(process_img);

			cv::cvtColor(process_img, process_img, cv::COLOR_BGR2GRAY);

			cv::Mat flow(m_prev.size(), CV_32FC2);
			cv::calcOpticalFlowFarneback(m_prev, process_img, flow,
										 m_config->FB_PYR_SACLE,
										 m_config->FB_LEVELS, m_config->FB_WINSIZE,
										 m_config->FB_ITERATIONS, m_config->FB_POLY_N,
										 m_config->FB_POLYSIGMA, m_config->FB_FLAGS);
			// visualization
			cv::Mat flow_parts[2];
			cv::split(flow, flow_parts);
			cv::Mat magnitude, angle, magn_norm;
			cv::cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
			cv::normalize(magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX);
			angle *= ((1.f / 360.f) * (180.f / 255.f));
			// build hsv image
			cv::Mat _hsv[3], hsv, hsv8, bgr;
			_hsv[0] = angle;
			_hsv[1] = cv::Mat::ones(angle.size(), CV_32F);
			_hsv[2] = magn_norm;
			cv::merge(_hsv, 3, hsv);
			hsv.convertTo(hsv8, CV_8U, 255.0);
			cv::cvtColor(hsv8, bgr, cv::COLOR_HSV2BGR);

			cv::Mat flow_graph, bgr_;
			bgr_ = bgr.clone();
			cv::cvtColor(bgr_, flow_graph, cv::COLOR_BGR2GRAY);
			auto mean_score = cv::mean(flow_graph);

			m_prev_res.push_back(mean_score[0]);
			auto sum = 0.0f;
			if (m_prev_res.size() > m_config->MOVING_LEN * 2)
			{
				for (int i = m_prev_res.size() - 1; i >= 0 && i > m_prev_res.size() - 5; i--)
				{
					sum += m_prev_res[i];
				}
				sum /= 4.0f;
				auto it = m_prev_res.begin();
				int cnt = 0;
				for (; it < m_prev_res.end() && cnt < 2; it++)
				{
					cnt++;
					m_prev_res.erase(it);
				}
			}
			// std::cout<<"Mean: "<<sum<<"/ Threshold: "<<m_config->THRESHOLD<<std::endl;
			// std::cout<<"Current: "<<mean_score[0]<<std::endl;
			if (sum > m_config->THRESHOLD)
			{
				latency += 2;
				if (latency >= 2 * m_config->ALARM_COUNT)
				{
					res = 1;
					latency = 0;
					alarm_cnt = 60;
					// std::cout << "detected!=======================\n";
				}
			}
			else
			{
				latency--;
				if (latency < 0)
					latency = 0;
			}
			if (alarm_cnt)
			{
				cv::resize(bgr, bgr, curr_img.size());
				cv::add(orin, bgr, orin);
			}
			// cv::add(temp, bgr, temp);
			// cv::resize(temp, curr_img, curr_img.size());
			if (alarm_cnt == 0)
				alarm_cnt = 0;
			reset_cnt--;
			m_prev = process_img.clone();
			if (0 == reset_cnt)
			{
				reset();
			}
		}
	}

	void WaterLeakDetection::detect(cv::Mat &curr_img, cv::Mat &orin, int &res)
	{
		if (m_config->FLOW_METHOD == FlowType::FARNEBACK)
		{
			detectWithFB(curr_img, orin, res);
		}
		else
			detectWithLK(curr_img, orin, res);
	}

	// the image is gray/resized/masked.
	void WaterLeakDetection::detectWithGPU(cv::Mat &curr_img, int &res)
	{
		std::vector<cv::Point2f> curr_p;
		std::vector<float> err;
		cv::calcOpticalFlowPyrLK(m_prev, curr_img, m_p0, m_p1, m_status, err,
								 cv::Size(15, 15), m_config->LK_PLK_MAX_LEVEL, criteria);
	}
	void WaterLeakDetection::detectWithCPU(cv::Mat &curr_img, int &res)
	{
		std::vector<cv::Point2f> curr_p;
		std::vector<float> err;
		cv::calcOpticalFlowPyrLK(m_prev, curr_img, m_p0, m_p1, m_status, err,
								 cv::Size(15, 15), m_config->LK_PLK_MAX_LEVEL, criteria);
	}

	WaterLeakDetection::WaterLeakDetection(SharedRef<Config> &config)
	{
		m_config = config;
		reset_cnt = m_config->RESET_COUNT;
	}
	void WaterLeakDetection::init()
	{
		if (m_init)
			return;
		auto h = m_config->IMAGE_SHAPE[1];
		auto w = m_config->IMAGE_SHAPE[2];
		auto rect = m_config->DETECTION_POS;
		m_mask = cv::Mat::zeros(h / resize_y, w / resize_x, CV_8UC3);
		cv::rectangle(m_mask, cv::Rect(rect[0] / resize_x, rect[1] / resize_y, (rect[2] - rect[0]) / resize_x, (rect[3] - rect[1]) / resize_y),
					  cv::Scalar(255, 255, 255), -1);
		if (m_config->USE_GPU)
		{
			m_cuda_mask.upload(m_mask);
			m_fb = cv::cuda::FarnebackOpticalFlow::create(m_config->FB_LEVELS,
														  m_config->FB_PYR_SACLE, false, m_config->FB_WINSIZE,
														  m_config->FB_ITERATIONS, m_config->FB_POLY_N,
														  m_config->FB_POLYSIGMA, m_config->FB_FLAGS);
		}
		if (m_config->FLOW_METHOD == FlowType::LK)
		{
			criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS),
										m_config->LK_TERM_CRITERIA_MAX_COUNT, m_config->LK_TERM_CRITERIA_EPSILON);
			cv::RNG rng;
			for (int i = 0; i < m_config->LK_MAX_CORNERS; i++)
			{
				int r = rng.uniform(0, 256);
				int g = rng.uniform(0, 256);
				int b = rng.uniform(0, 256);
				m_colors.push_back(cv::Scalar(r, g, b));
			}
		}

		m_init = true;
	}

	// the img here should only be resized. without gray/masked.
	void WaterLeakDetection::plot(cv::Mat &img)
	{
		if (m_config->FLOW_METHOD == FlowType::FARNEBACK)
		{
		}
		else
		{
			std::vector<cv::Point2f> good_new;
			cv::Mat temp = img.clone();
			auto mask = m_line_mask.clone();
			for (uint i = 0; i < m_p0.size(); i++)
			{
				if (m_status[i] == 1)
				{
					good_new.push_back(m_p1[i]);
					// plot the trajectory.
					line(mask, m_p1[i], m_p0[i], m_colors[i], m_config->DRAW_LINE_WIDTH);
					circle(temp, m_p1[i], m_config->DRAW_PLOT_RADIUS, m_colors[i], -1);
				}
			}
			cv::add(mask, temp, img);
			m_p0 = good_new;
		}
	}
	void WaterLeakDetection::reset()
	{
		if (m_config->FLOW_METHOD == FlowType::LK)
		{
			cv::goodFeaturesToTrack(m_prev, m_p0, m_config->LK_MAX_CORNERS,
									m_config->LK_QUALITY_LEVEL, m_config->LK_MIN_DIST,
									cv::Mat(), m_config->LK_BLOCK_SIZE, m_config->LK_USE_HARRIS_DETECTOR,
									m_config->LK_SIFT_K);
		}
		reset_cnt = m_config->RESET_COUNT;
	}

	WaterLeakDetection::~WaterLeakDetection() = default;
}
