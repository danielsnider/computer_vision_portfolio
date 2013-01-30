#include <iostream>

#include "MyImage.h"
#include "MyFuncs.h"

using std::cout;
using std::system;

/*
Program usage: $0 [image to process]
Program depenencies: OpenCV, Tesseract
*/

void QuizEye(char* arg)
{
  //Init env
  system("rm ../public/uploads/output.csv");

  // Load image and filename into object
  MyImage my_im(arg);

  // Allow user to crop image
  // my_im.img = UserDrawnSelection(my_im.img);

  // Image preprocessing (grey, adaptive threshhold, erode)
  my_im.img = PreProc(my_im.img);

  // Split terms column from definitions column
  // 1. Scan until black 2. Scan until white 3. Scan until black 4. Back up 5 then seperate
  my_im.img_pieces = SeperateTermDefV2(my_im.img);

  // Split lines of text into pieces
  // 1. Scan until black, save location, scan until white, repeat
  // 2. For each saved location find upper and lower white
  // 3. Store image pieces in array (that aren't oddly small)
  my_im.img_pieces = SeperateLinesV2(my_im.img_pieces);

  // OCR pieces and save systematically
  OCRtoStdOut(my_im.img_pieces);

  // Clean up
  cleanUp();
}


int main(int argc, char** argv)
{
  if( argc != 2)
    {
     cout << "Usage: $0 [image to process]\n\n";
     return -1;
    }

  QuizEye(argv[1]);
}
