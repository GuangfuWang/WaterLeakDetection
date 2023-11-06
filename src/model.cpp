#include "model.h"
#include "detection.h"
#include "config.h"
#include <string>
#include <opencv2/freetype.hpp>
#include <iostream>

namespace water_leak
{

	class InferModel
	{
	public:
		InferModel(SharedRef<Config> &config)
		{
			mDeploy = createSharedRef<WaterLeakDetection>(config);
			m_config = config;
			m_font = cv::freetype::createFreeType2();
			if (!Util::checkFileExist(config->FONT_SRC))
				std::cerr << "Font file not found!" << std::endl;
			else
				m_font->loadFontData(config->FONT_SRC, 0);
		}

	public:
		SharedRef<WaterLeakDetection> mDeploy;
		SharedRef<Config> m_config;
		cv::Ptr<cv::freetype::FreeType2> m_font = nullptr;
		cv::Mat m_roi;
	};

	static void *GenModel(SharedRef<Config> &config)
	{
		InferModel *model = new InferModel(config);
		return reinterpret_cast<void *>(model);
	}

	static cv::Mat genROI(const cv::Size s, const std::vector<int> &points, cv_Point *coords)
	{
		if (points.empty())
		{
			return {s, CV_8UC3, cv::Scalar::all(255)};
		}
		cv::Mat roi_img = cv::Mat::zeros(s, CV_8UC3);

		std::vector<std::vector<cv::Point>> contour;

		int sums = 0;
		for (auto &each : points)
		{
			std::vector<cv::Point> pts;
			for (int j = sums; j < each + sums; ++j)
			{
				pts.push_back(cv::Point(coords[j].x, coords[j].y));
			}
			sums += each;
			contour.push_back(pts);
		}
		sums = 0;
		for (auto &i : points)
		{
			cv::drawContours(roi_img, contour, sums, cv::Scalar::all(255), -1);
			sums++;
		}
		return roi_img;
	}

	cvModel *Allocate_Algorithm(cv::Mat &input_frame, int algID, int gpuID)
	{
		cv::cuda::setDevice(gpuID);

		std::string file;
		if (Util::checkFileExist("./water_leak_detection.yaml"))
			file = "./water_leak_detection.yaml";
		else if (Util::checkFileExist("../config/water_leak_detection.yaml"))
		{
			file = "../config/water_leak_detection.yaml";
		}
		else
		{
			std::cerr << "Cannot find YAML file!" << std::endl;
		}
		auto config = createSharedRef<Config>(0, nullptr, file);
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
	void SetPara_Algorithm(cvModel *pModel, int algID)
	{
		//for test.		
		auto model = reinterpret_cast<InferModel *>(pModel->iModel);
		auto roi = pModel->p;
		pModel->pointNum = {4};
		roi[0].x = 100;
		roi[0].y = 150;

		roi[1].x = 1500;
		roi[1].y = 150;

		roi[2].x = 1500;
		roi[2].y = 1000;

		roi[3].x = 100;
		roi[3].y = 1000;
	}
	void UpdateParams_Algorithm(cvModel *pModel)
	{
		auto model = reinterpret_cast<InferModel *>(pModel->iModel);
		auto roi = pModel->p;
		model->m_roi = genROI(cv::Size(pModel->width, pModel->height),
							  pModel->pointNum, roi);
	}
	void Process_Algorithm(cvModel *pModel, cv::Mat &input_frame)
	{
		auto model = reinterpret_cast<InferModel *>(pModel->iModel);
		auto roi = pModel->p;
		if (model->m_roi.empty())
		{
			model->m_roi = genROI(input_frame.size(), pModel->pointNum, roi);
		}
		cv::Mat removed_roi;
		auto config = model->m_config;
		input_frame.copyTo(removed_roi, model->m_roi);
		model->mDeploy->detect(removed_roi, input_frame,pModel->alarm);

		int sums = 0;
		for (auto &each : pModel->pointNum)
		{
			for (int j = sums; j < each + sums; ++j)
			{
				int k = j + 1;
				if (k == each + sums)
					k = sums;
				cv::line(input_frame, cv::Point(roi[j].x, roi[j].y),
						 cv::Point(roi[k].x, roi[k].y), cv::Scalar(255, 0, 0),
						 config->DRAW_LINE_WIDTH);
			}
			sums += each;
		}
		if (pModel->alarm && model->m_font)
		{
			model->m_font->putText(input_frame, config->POST_TEXT, cv::Point(config->TEXT_OFFSET_X, config->TEXT_OFFSET_Y),
								   config->TEXT_FONT_SIZE, cv::Scalar(config->DRAW_LINE_COLOR[0], config->DRAW_LINE_COLOR[1], config->DRAW_LINE_COLOR[2]),
								   (int)config->TEXT_LINE_WIDTH, 8, false);
		}
	}
	void Destroy_Algorithm(cvModel *pModel)
	{
		if (pModel->iModel)
		{
			auto model = reinterpret_cast<InferModel *>(pModel->iModel);
			delete model;
			model = nullptr;
		}
		if (pModel)
		{
			delete pModel;
			pModel = nullptr;
		}
		std::cout << "Deconstruct the model done..." << std::endl;
	}
}
