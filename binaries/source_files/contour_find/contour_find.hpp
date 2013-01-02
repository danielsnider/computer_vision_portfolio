#ifndef __CONTOUR_FIND_H_INCLUDED__
#define __CONTOUR_FIND_H_INCLUDED__

#include "opencv2/core/core.hpp"
#include <vector>

using namespace cv;

class ImgPiece {
  public:
	  Mat img;
	  int x1;
	  int y1;
	  int x2;
	  int y2;

	  ImgPiece(Mat I, int x_1, int y_1, int x_2, int y_2);

  private:
};

vector<ImgPiece> CutUpImageHorizontally(ImgPiece piece, int cons, int thres, int scan_rate);
vector<ImgPiece> CutUpImageVertically(ImgPiece piece, int cons, int thres, int scan_rate);

#endif // __CONTOUR_FIND_H_INCLUDED__