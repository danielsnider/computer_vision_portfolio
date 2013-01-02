#ifndef __FUNCS_H_INCLUDED__
#define __FUNCS_H_INCLUDED__

#include "opencv2/core/core.hpp"
#include <vector>
#include <string>

using namespace cv;

vector<string> FilesystemLS(char* given_path);
Mat KeepHighlightsOnly(Mat img, int r, int g, int b);
void DisplayImage(Mat img);
std::string OCR(Mat img);
void cleanUp();


#endif // __FUNCS_H_INCLUDED__ 