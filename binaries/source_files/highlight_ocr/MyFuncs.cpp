#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <boost/filesystem.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "MyFuncs.h"

namespace fs = boost::filesystem;
using namespace std;
using namespace cv;

vector<string> FilesystemLS(char* given_path)
{
  vector<std::string> files;
  std::string s_given_path = given_path;

  fs::path full_path( fs::initial_path<fs::path>() );
  full_path = fs::system_complete( fs::path( given_path ) );

  if ( !fs::exists( full_path ) )
  {
    cout << "\nNope. Sorry that directory does not exist at all." << endl;
    exit(1);
  }

  if ( fs::is_directory( full_path ) )
  {
    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
    {
      try
      {
        if ( fs::is_directory( dir_itr->status() ) )
        {
          true; //std::cout << dir_itr->path().filename() << " [directory]\n";
        }
        else if ( fs::is_regular_file( dir_itr->status() ) )
        {
          std::string full_file_name = s_given_path; //full relative file name
          full_file_name.append("/");
          full_file_name.append(dir_itr->path().filename().string());
          //cout << full_file_name;
          files.push_back(full_file_name);
        }
        else
        {
          true; //cout << dir_itr->path().filename() << " [other]\n";
        }
      }
      catch ( const exception & ex )
      {
        cout << dir_itr->path().filename() << " " << ex.what() << endl;
      }
    }
  }
  else // must be a file
  {
    cout << "\nNope. That was a file, I need a directory as an argument." << "\n";    
    exit(2);
  }
  
  return files;
}

Mat KeepHighlightsOnly(Mat img, int r, int g, int b)
// the order of r, g, b is incorrect, reality is that opencv stores channels in bgr format
{
  if (img.channels() != 3) {
    cout << "\nNope. That image didn't seem to be rgb tri-channel." << "\n";    
    exit(3);
  }

  int channels = img.channels();
  int nRows = img.rows;
  int nCols = img.cols * channels;
  uchar* p; // will store row of pixels

  int j;
  int count = 0;
  int start, end;
  int row = 0;

  // Scan the image downwards looking for the specified rgb 
  while (row < nRows) {
    

    p = img.ptr<uchar>(row);
    count = 0;

    //count the number of highlighed pixels in a row
    for (int i = 0; i < nCols-3; i = i + 3)
    {
      if (p[i] == r && p[i+1] == g && p[i+2] == b){ // if pixel colour matches
        count++;
      }
    }

    //if no highlighted pixels are found, set the row white
    if (count == 0 ){
      for (int i = 0; i < nCols; i++)
      {
        p[i] = 255;
      }
    }
    // else keep track of the start and continue
    else {
      start = row;
    }
    
    //if highlighted pixels are found, continue until a row with none is found
    while (count > 0)
    {
      count = 0;

      if (row+1 < nRows) {       
        row++;
        p = img.ptr<uchar>(row);
        
        for (int i = 0; i < nCols-3; i = i + 3)
        {
          if (p[i] == r && p[i+1] == g && p[i+2] == b){ // if pixel colour matches
            count++;
          }
        }
      }
      
      // now that a row with no highlight is found, keep track and determine how far to the right the highlight is
      if (count == 0){
        row--;
        end = row;
        p = img.ptr<uchar>(end);

        // for each pixel in row
        for (int i = 0; i < nCols-3; i = i + 3)
        {           
          // Assuming the bottom row of pixels of the highlight are always the exact same colour...
          // Scanning from left to right in the bottow consecutive row of a highlighted area,
          // If a highlight is not present, set that pixel and all the ones above until the top of 
          // the highlighted area white. 
          if (p[i] != r || p[i+1] != g || p[i+2] != b){
            for (int j = start; j <= end; ++j) // should finish with 'p' pointing to 'end' position like in the outer for loop
            {
              p = img.ptr<uchar>(j);
              p[i] = 255;
              p[i+1] = 255;
              p[i+2] = 255;
            }
          }
        }
      }
    }
    row++;
  }

  //remove any highlighting, leaving only the text that was highlighted
  for (int row = 0; row < nRows; ++row)
  {
    p = img.ptr<uchar>(row);
    for (int i = 0; i < nCols-3; i = i + 3)
    {
      if (p[i] == r && p[i+1] == g && p[i+2] == b){ // if pixel colour matches
        p[i] = 255;
        p[i+1] = 255;
        p[i+2] = 255;
      }
    }
  }

  return img;
}

void DisplayImage(Mat img){
  imshow("img", img);
  waitKey();
  destroyWindow("img");
}

std::string OCR(Mat img) {
//Tesseract OCR one image at a time and return string result

  char cmd[] = "tesseract -l eng temp.jpg temp"; // Tesseract system command
  std::string text = "";

  
  imwrite("temp.jpg", img); 
  
  if (system(cmd) == 0) {
    // ## Following only gets one line of text
    // ifstream in("temp.txt");
    // getline(in, text);
    // in.close();

    // ## Following doesn't work
    // ifstream t("temp.txt");
    // string text((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    system("cat temp.txt");
  } 
  else {
    cout << "Error: tesseract command failed\n";
  }

  return text;
}

void cleanUp() {
  system("rm temp.*");
}

