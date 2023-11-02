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
		if (model_node["TERM_CRITERIA_MAX_COUNT"].IsDefined()) {
			TERM_CRITERIA_MAX_COUNT = model_node["TERM_CRITERIA_MAX_COUNT"].as<int>();
		}
		if (model_node["TERM_CRITERIA_EPSILON"].IsDefined()) {
			TERM_CRITERIA_EPSILON = model_node["TERM_CRITERIA_EPSILON"].as<float>();
		}
		if (model_node["MAX_CORNERS"].IsDefined()) {
			MAX_CORNERS = model_node["MAX_CORNERS"].as<int>();
		}
		if (model_node["QUALITY_LEVEL"].IsDefined()) {
			QUALITY_LEVEL = model_node["QUALITY_LEVEL"].as<float>();
		}
		if (model_node["MIN_DIST"].IsDefined()) {
			MIN_DIST = model_node["MIN_DIST"].as<int>();
		}
		if (model_node["BLOCK_SIZE"].IsDefined()) {
			BLOCK_SIZE = model_node["BLOCK_SIZE"].as<int>();
		}
		if (model_node["USE_HARRIS_DETECTOR"].IsDefined()) {
			USE_HARRIS_DETECTOR = model_node["USE_HARRIS_DETECTOR"].as<bool>();
		}
		if (model_node["SIFT_K"].IsDefined()) {
			SIFT_K = model_node["SIFT_K"].as<float>();
		}
		if (model_node["PLK_MAX_LEVEL"].IsDefined()) {
			PLK_MAX_LEVEL = model_node["PLK_MAX_LEVEL"].as<int>();
		}
		if (model_node["THRESHOLD_LEN"].IsDefined()) {
			THRESHOLD_LEN = model_node["THRESHOLD_LEN"].as<int>();
		}
		if (model_node["THRESHOLD"].IsDefined()) {
			THRESHOLD = model_node["THRESHOLD"].as<float>();
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

	init = true;

}
}
