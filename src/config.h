#pragma once
#include <string>
#include <vector>
namespace water_leak
{
/**
 * @brief config class for deployment. should not be derived.
 */
class Config final
{
public:
	Config(int argc, char **argv, const std::string &file)
	{
		LoadConfigFile(argc, argv, file);
	}
	/**
	 * @brief loading the config yaml file, default folder is ./config/infer_cfg.yaml
	 * @param argc terminal arg number.
	 * @param argv terminal arg values.
	 * @param file config file full path.
	 * @note priority: 1 Terminal; 2 Config file; 3 Compilation settings
	 */
	void LoadConfigFile(int argc, char **argv, const std::string &file);
public:

	std::string VIDEO_FILE = "";
	std::vector<int> IMAGE_SHAPE = {3, 608, 608};
	std::vector<int> DETECTION_POS = {500, 200, 800, 900};
	int FLOW_METHOD = 1;

	int LK_TERM_CRITERIA_MAX_COUNT = 10;
	float LK_TERM_CRITERIA_EPSILON = 0.03f;
	int LK_MAX_CORNERS = 100;
	float LK_QUALITY_LEVEL = 0.3f;
	int LK_MIN_DIST = 7;
	int LK_BLOCK_SIZE = 7;
	bool LK_USE_HARRIS_DETECTOR = false;
	float LK_SIFT_K = 0.04f;
	int LK_PLK_MAX_LEVEL = 2;

	float FB_PYR_SACLE = 0.5;
  	int FB_LEVELS = 3;
  	int FB_WINSIZE= 15;
  	int FB_ITERATIONS= 3;
  	int FB_POLY_N= 5;
  	float FB_POLYSIGMA= 1.2;
  	int FB_FLAGS= 0;

	int THRESHOLD_LEN = 10;
	float THRESHOLD = 135.0f;
	float THRESHOLD_UPPER = 160.0f;
	int RESET_COUNT = 100;
	int ALARM_COUNT = 20;
	bool ENABLE_MOVING_AVERAGE = true;
	unsigned int MOVING_LEN = 10;
	bool USE_GPU = true;

	std::string POST_TEXT = "Water Level Warning";
	int DRAW_LINE_WIDTH = 2;
	int DRAW_PLOT_RADIUS = 5;
	std::vector<unsigned char> DRAW_LINE_COLOR = {255, 0, 0};
	std::vector<unsigned char> DRAW_LINE_COLOR_THRES = {0, 0, 255};
	float TEXT_LINE_WIDTH = 1.2f;
	std::vector<unsigned char> TEXT_LINE_COLOR = {255, 0, 0};
	double TEXT_FONT_SIZE = 2.0;
	unsigned int TEXT_OFFSET_X = 400;
	unsigned int TEXT_OFFSET_Y = 50;
	std::string FONT_SRC = "";

	bool init = false;
};
}
