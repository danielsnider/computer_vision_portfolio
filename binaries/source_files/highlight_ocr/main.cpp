#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

#include "MyFuncs.h"

using std::cout;
using std::system;

using namespace cv;

void KindleHighlightOCR(char* folder, char* hi_r, char* hi_g, char* hi_b)
{
  vector<string> images;
  Mat img;
  int r, g, b;

  //search directory for images
  images = FilesystemLS(folder);

  // Convert colour of highlight from char to int
  r = atoi(hi_r);
  g = atoi(hi_g);
  b = atoi(hi_b);

  cout << "iOS Kindle highlight screenshot to text program version 1.0\n";
  cout << "Author: Daniel Snider (2012) - GNU General Public License\n";
  cout << "Loading images from folder: " << folder << "\n";
  cout << "Colour of highlight is: " << r << "," << g << "," << b << "\n";
  cout << "Number of images found is: " << images.size() << "\n\n";

  for (int i = 0; i < images.size(); ++i)
  {
    cout << "Operating on image: " << images[i] << "\n";

    // Load image
    img = imread(images[i], 1);
    if (img.empty())
    {
      cout << "\nCannot open image!\n";
      exit(1);
    }

    // Other than text highlighted in a given colour make everything white
    img = KeepHighlightsOnly(img, r, g, b);

    // OCR pieces with Tesseract and cout results
    OCR(img);

    // Display image and wait for key
    //DisplayImage(img);

    // Clean up
    cleanUp();
  }
}

int main(int argc, char** argv)
{
  cout << "Usage: $0 [folder containing images] [colour of highlighted colour, red intensity] [green intensity] [blue intensity]\n";
  cout << "A common iOS kindle highlight colour is (251,226,152).\n";

  KindleHighlightOCR(argv[1], argv[2], argv[3], argv[4]);
}
