//
// Created by wgf on 23-10-8.
//

#include <iostream>
#include "config.h"
#include "util.h"
#include <yaml-cpp/yaml.h>

namespace water_leak
{


void Config::LoadConfigFile(int argc, char **argv, const std::string &file)
{
	if (init)return;

	if (!Util::checkFileExist(file)) {
		std::cerr << "Config file for water leak non exists! Aborting..." << std::endl;
		return;
	}

	YAML::Node config;
	config = YAML::LoadFile(file);

	if (config["DATA"].IsDefined()) {
		auto model_node = config["DATA"];
		if (model_node["VIDEO_FILE"].IsDefined()) {
			VIDEO_FILE = model_node["VIDEO_FILE"].as<std::string>();
		}
	}
	else {
		std::cerr << "Please set VIDEO FILE, " << std::endl;
	}

	if (config["METHOD"].IsDefined()) {
		auto model_node = config["METHOD"];
		if (model_node["IMAGE_SHAPE"].IsDefined()) {
			IMAGE_SHAPE = model_node["IMAGE_SHAPE"].as<std::vector<int>>();
		}
		if (model_node["DETECTION_POS"].IsDefined()) {
			DETECTION_POS = model_node["DETECTION_POS"].as<std::vector<int>>();
		}
		if (model_node["FLOW_METHOD"].IsDefined()) {
			FLOW_METHOD = model_node["FLOW_METHOD"].as<int>();
		}
		if (model_node["LK_TERM_CRITERIA_MAX_COUNT"].IsDefined()) {
			LK_TERM_CRITERIA_MAX_COUNT = model_node["LK_TERM_CRITERIA_MAX_COUNT"].as<int>();
		}
		if (model_node["LK_TERM_CRITERIA_EPSILON"].IsDefined()) {
			LK_TERM_CRITERIA_EPSILON = model_node["LK_TERM_CRITERIA_EPSILON"].as<float>();
		}
		if (model_node["LK_MAX_CORNERS"].IsDefined()) {
			LK_MAX_CORNERS = model_node["LK_MAX_CORNERS"].as<int>();
		}
		if (model_node["LK_QUALITY_LEVEL"].IsDefined()) {
			LK_QUALITY_LEVEL = model_node["LK_QUALITY_LEVEL"].as<float>();
		}
		if (model_node["LK_MIN_DIST"].IsDefined()) {
			LK_MIN_DIST = model_node["LK_MIN_DIST"].as<int>();
		}
		if (model_node["LK_BLOCK_SIZE"].IsDefined()) {
			LK_BLOCK_SIZE = model_node["LK_BLOCK_SIZE"].as<int>();
		}
		if (model_node["LK_USE_HARRIS_DETECTOR"].IsDefined()) {
			LK_USE_HARRIS_DETECTOR = model_node["LK_USE_HARRIS_DETECTOR"].as<bool>();
		}
		if (model_node["LK_SIFT_K"].IsDefined()) {
			LK_SIFT_K = model_node["LK_SIFT_K"].as<float>();
		}
		if (model_node["LK_PLK_MAX_LEVEL"].IsDefined()) {
			LK_PLK_MAX_LEVEL = model_node["LK_PLK_MAX_LEVEL"].as<int>();
		}
		if (model_node["FB_PYR_SACLE"].IsDefined()) {
			FB_PYR_SACLE = model_node["FB_PYR_SACLE"].as<float>();
		}
		if (model_node["FB_LEVELS"].IsDefined()) {
			FB_LEVELS = model_node["FB_LEVELS"].as<int>();
		}
		if (model_node["FB_WINSIZE"].IsDefined()) {
			FB_WINSIZE = model_node["FB_WINSIZE"].as<int>();
		}
		if (model_node["FB_ITERATIONS"].IsDefined()) {
			FB_ITERATIONS = model_node["FB_ITERATIONS"].as<int>();
		}
		if (model_node["FB_POLY_N"].IsDefined()) {
			FB_POLY_N = model_node["FB_POLY_N"].as<int>();
		}
		if (model_node["FB_POLYSIGMA"].IsDefined()) {
			FB_POLYSIGMA = model_node["FB_POLYSIGMA"].as<float>();
		}
		if (model_node["FB_FLAGS"].IsDefined()) {
			FB_FLAGS = model_node["FB_FLAGS"].as<int>();
		}
		if (model_node["THRESHOLD_LEN"].IsDefined()) {
			THRESHOLD_LEN = model_node["THRESHOLD_LEN"].as<int>();
		}
		if (model_node["THRESHOLD"].IsDefined()) {
			THRESHOLD = model_node["THRESHOLD"].as<float>();
		}
		if (model_node["THRESHOLD_UPPER"].IsDefined()) {
			THRESHOLD_UPPER = model_node["THRESHOLD_UPPER"].as<float>();
		}
		if (model_node["ENABLE_MOVING_AVERAGE"].IsDefined()) {
			ENABLE_MOVING_AVERAGE = model_node["ENABLE_MOVING_AVERAGE"].as<bool>();
		}
		if (model_node["MOVING_LEN"].IsDefined()) {
			MOVING_LEN = model_node["MOVING_LEN"].as<unsigned int>();
		}
		if (model_node["USE_GPU"].IsDefined()) {
			USE_GPU = model_node["USE_GPU"].as<bool>();
		}
	}
	else {
		std::cerr << "Please set Hough Params, " << std::endl;
	}

	if (config["POST_PROCESS"].IsDefined()) {
		auto model_node = config["POST_PROCESS"];
		if (model_node["POST_TEXT"].IsDefined()) {
			POST_TEXT = model_node["POST_TEXT"].as<std::string>();
		}
		if (model_node["DRAW_LINE_WIDTH"].IsDefined()) {
			DRAW_LINE_WIDTH = model_node["DRAW_LINE_WIDTH"].as<int>();
		}
		if (model_node["DRAW_PLOT_RADIUS"].IsDefined()) {
			DRAW_PLOT_RADIUS = model_node["DRAW_PLOT_RADIUS"].as<int>();
		}
		if (model_node["DRAW_LINE_COLOR"].IsDefined()) {
			DRAW_LINE_COLOR = model_node["DRAW_LINE_COLOR"].as<std::vector<unsigned char>>();
		}
		if (model_node["DRAW_LINE_COLOR_THRES"].IsDefined()) {
			DRAW_LINE_COLOR_THRES = model_node["DRAW_LINE_COLOR_THRES"].as<std::vector<unsigned char>>();
		}
		if (model_node["TEXT_LINE_WIDTH"].IsDefined()) {
			TEXT_LINE_WIDTH = model_node["TEXT_LINE_WIDTH"].as<float>();
		}
		if (model_node["TEXT_FONT_SIZE"].IsDefined()) {
			TEXT_FONT_SIZE = model_node["TEXT_FONT_SIZE"].as<double>();
		}
		if (model_node["TEXT_LINE_COLOR"].IsDefined()) {
			TEXT_LINE_COLOR = model_node["TEXT_LINE_COLOR"].as<std::vector<unsigned char>>();
		}
		if (model_node["TEXT_OFFSET_X"].IsDefined()) {
			TEXT_OFFSET_X = model_node["TEXT_OFFSET_X"].as<unsigned int>();
		}
		if (model_node["TEXT_OFFSET_Y"].IsDefined()) {
			TEXT_OFFSET_Y = model_node["TEXT_OFFSET_Y"].as<unsigned int>();
		}
	}
	else {
		std::cerr << "Please set POST_PROCESS, " << std::endl;
	}
	std::cout<<"Parsing the Yaml config file done..."<<std::endl;
	init = true;

}
}
