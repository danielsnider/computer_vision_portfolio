#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <fstream>
#include "contour_find.hpp"
using namespace std;
using namespace cv;

// Usage: display_image ImageToLoadAndDisplay cons thres scan_rate output_file

Mat IMG;
Mat IMGP;
Mat IMGD;

vector<ImgPiece> CutUpImageHorizontally(ImgPiece piece, int cons, int thres, int scan_rate);
vector<ImgPiece> CutUpImageVertically(ImgPiece piece, int cons, int thres, int scan_rate);

ImgPiece::ImgPiece(Mat I, int x_1, int y_1, int x_2, int y_2){
  //set MyImage vars
  img = I;
  x1 = x_1;
  y1 = y_1;
  x2 = x_2;
  y2 = y_2;
}

Mat DrawBoxes(Mat I, vector<ImgPiece> img_pieces){
    for (int i = 0; i < img_pieces.size(); ++i)
    {
        rectangle( I,
           Point( img_pieces[i].x1, img_pieces[i].y1 ),
           Point( img_pieces[i].x2, img_pieces[i].y2),
           Scalar( 0, 0, 255 ),
           1);
    }
    return I;
}

Mat PreProc(Mat I){
//Binarize images and remove noise to make processing images easier

  // accept only char (ie. 8-bit) typ matrices, and are not continous
  CV_Assert(I.depth() == CV_8U);

  //change to black and white
  cvtColor(I, I, CV_RGB2GRAY);
  adaptiveThreshold(I, I,1000,1,0,81,14);

  //dilate to remove stray black pixels
  int size = 2;
  Mat element = getStructuringElement( MORPH_ELLIPSE,
      Size( size + 1 , size + 1 ),
      Point( size, size ) );
  dilate( I, I, element );

  //erode
  erode( I, I, element ); //enlarge black surface area

  return I;
}

Mat PreProcIntegral(Mat I){
    // Grey, adaptive threshold, dilate, erode, set whites to 0, black otherwise, integral
    // Binarize images and remove noise to make processing images easier
    uchar* p;

    // accept only char (ie. 8-bit) typ matrices, and are not continous
    CV_Assert(I.depth() == CV_8U);

    //change to black and white
    cvtColor(I, I, CV_RGB2GRAY);
    adaptiveThreshold(I, I,1000,1,0,81,14);

    //dilate to remove stray black pixels
    int size = 2;
    Mat element = getStructuringElement( MORPH_ELLIPSE,
      Size( size + 1 , size + 1 ),
      Point( size, size ) );
    dilate( I, I, element );

    //erode
    erode( I, I, element ); //enlarge black surface area

    // Step 2: Convert whie pixels to 0 and other pixels to 1
     for (int y = 0; y < I.rows; y++){
        p = I.ptr<uchar>(y);
        for (int x = 0; x < I.cols; x++){
            if (p[x] == 255){
                p[x] = 0;
            }
            else {
                p[x] = 1;
            }
        }
    }

    // Step 3: Integrate to compute cumulative sums
    integral(I, I);

    return I;
}

Mat SetWhite0Else1(Mat I){
    uchar* p;

     for (int y = 0; y < I.rows; y++){
        p = I.ptr<uchar>(y);
        for (int x = 0; x < I.cols; x++){
            if (p[x] == 255){
                p[x] = 0;
            }
            else {
                p[x] = 1;
            }
        }
    }
    return I;
}

int SumArea(Mat integral, int x1, int y1, int x2, int y2){
// Computes pixel sum of a given range rectangle from position (x1, y1) to (x2, y2)
// Input requires integrated matrix

    uchar* top_row;
    uchar* bot_row;
    int top_l, top_r, bot_l, bot_r, sum;

    // get pixel values
    top_row = integral.ptr<uchar>(y1);
    bot_row = integral.ptr<uchar>(y2);
    top_l = top_row[x1];
    top_r = top_row[x2];
    bot_l = bot_row[x1];
    bot_r = bot_row[x2];


    sum = bot_r + top_l - top_r - bot_l;

    return sum;
}

int CountNonWhitePixelsInArea(Mat I, int x1, int y1, int x2, int y2){
// Computes pixel sum of a given range rectangle from position (x1, y1) to (x2, y2)
// Input requires integrated matrix

    int* top_row;
    int* bot_row;
    int top_l, top_r, bot_l, bot_r, sum;

    // Step 4: Get the required pixel values
    // Explaination: http://www.aishack.in/2010/07/integral-images/
    top_row = I.ptr<int>(y1);
    bot_row = I.ptr<int>(y2);
    top_l = top_row[x1];
    top_r = top_row[x2+1];
    bot_l = bot_row[x1];
    bot_r = bot_row[x2+1];

    // Step 5: Compute sum
    sum = bot_r + top_l - top_r - bot_l;

    return sum;
}

int CountNonWhitePixelsInAreaV2(Mat I, int x1, int y1, int x2, int y2){
// Computes pixel sum of a given range rectangle from position (x1, y1) to (x2, y2)
// Input requires integrated matrix

    I = IMGP;

    int* top_row;
    int* bot_row;
    int top_l, top_r, bot_l, bot_r, sum;

    // Step 4: Get the required pixel values
    // Explaination: http://www.aishack.in/2010/07/integral-images/
    top_row = I.ptr<int>(y1);
    bot_row = I.ptr<int>(y2);
    top_l = top_row[x1];
    top_r = top_row[x2+1];
    bot_l = bot_row[x1];
    bot_r = bot_row[x2+1];

    // Step 5: Compute sum
    sum = bot_r + top_l - top_r - bot_l;

    return sum;
}

vector<int> ComputeColumnRegions(Mat I, int cons, int thres, int scan_rate){
// Return a vector of locations of where columns with some black pixels change to columns with no black pixels
// Function dependencies: Mat SetWhite0Else1(Mat I), int SumArea(Mat I, int x1, int y1, int x2, int y2)

    vector<int> col_boundries;
    int x1, y1, x2, y2;
    int sum = 0;
    int saved_sum;
    uchar* p;



    for (int x = cons; x < I.cols; x+=scan_rate)
    {
        x1 = x - cons; //integration added an extra column and row
        x2 = x;
        y1 = 0;
        y2 = I.rows-1;

        saved_sum = sum;
        sum = CountNonWhitePixelsInArea(I, x1, y1, x2, y2);

        if (saved_sum > thres && sum == 0)
        {
            col_boundries.push_back(x1+1);
        }
    }
    return col_boundries;
}

void Display2Piece(ImgPiece piece1, ImgPiece piece2){
    Mat I;
    I = IMG(Range(piece1.y1, piece1.y2), Range(piece1.x1, piece1.x2));
    imshow("1", I);
    I = IMG(Range(piece2.y1, piece2.y2), Range(piece2.x1, piece2.x2));
    imshow("2", I);
    waitKey();
}

void DisplayPiece(ImgPiece piece){
    Mat I;
    I = IMGD(Range(piece.y1, piece.y2), Range(piece.x1, piece.x2));
    imshow("piece", I);
    // imshow("piece", piece.img);
    waitKey();
}

// vector<ImgPiece> CutUpImageVertically(vector<ImgPiece> img_pieces, int cons, int thres, int scan_rate){
vector<ImgPiece> CutUpImageVertically(ImgPiece piece, int cons, int thres, int scan_rate){
// Return a vector of locations of where columns with some black pixels change to columns with no black pixels
// Function dependencies: Mat SetWhite0Else1(Mat I), int SumArea(Mat I, int x1, int y1, int x2, int y2)

    int sum = 0;
    int saved_sum;
    vector<ImgPiece> img_pieces;
    vector<ImgPiece> more_pieces;
    vector<ImgPiece> temp_img_pieces;
    int shifting_pos, shifting_pos_end;
    int left, top, bottom;

    Mat I = piece.img;

    top = 0; //top of image
    bottom = I.rows-1; //bottom of image
    left = 0;

    for (int x = cons; x < I.cols; x+=scan_rate)
    {
        shifting_pos = x - cons;
        shifting_pos_end = x;

        saved_sum = sum;
        sum = CountNonWhitePixelsInAreaV2(I, piece.x1+shifting_pos, piece.y1, piece.x1+shifting_pos_end, piece.y2);
        // sum = CountNonWhitePixelsInArea(I, shifting_pos, top, shifting_pos_end, bottom);

        if (saved_sum == 0 && sum > thres){
            left = shifting_pos;
        }

        else if (saved_sum > thres && sum == 0)
        {
            //Cut current image 'I' into two pieces
            Mat new_piece = I(Range(top, bottom), Range(left, shifting_pos_end));

            //Create ImgPiece objects out of pieces and keep track of their positions
            ImgPiece a_new_piece(new_piece, piece.x1+left, piece.y1, piece.x1+shifting_pos_end, piece.y2);

            // DisplayPiece(a_new_piece);

            //Add all pieces to vector
            temp_img_pieces.push_back(a_new_piece);

            left = shifting_pos;
        }
    }
    if (temp_img_pieces.size() <= 1){
        img_pieces.push_back(piece);
    }
    else {
        for (int x = 0; x < temp_img_pieces.size(); ++x)
        {
            more_pieces = CutUpImageHorizontally(temp_img_pieces[x], cons, thres, scan_rate);
            img_pieces.insert(img_pieces.end(), more_pieces.begin(), more_pieces.end());
        }
    }
    return img_pieces;
}

vector<ImgPiece> CutUpImageHorizontally(ImgPiece piece, int cons, int thres, int scan_rate){
// Return a vector of locations of where columns with some black pixels change to columns with no black pixels
// Function dependencies: Mat SetWhite0Else1(Mat I), int SumArea(Mat I, int x1, int y1, int x2, int y2)

    int sum = 0;
    int saved_sum;
    vector<ImgPiece> img_pieces;
    vector<ImgPiece> more_pieces;
    vector<ImgPiece> temp_img_pieces;
    int shifting_pos, shifting_pos_end;
    int left, right, top;

    Mat I = piece.img;

    top = 0; //top of image
    left = 0;
    right = I.cols-1;


    for (int x = cons; x < I.rows; x+=scan_rate)
    {
        shifting_pos = x - cons;
        shifting_pos_end = x;

        saved_sum = sum;
        sum = CountNonWhitePixelsInAreaV2(I, piece.x1, piece.y1+shifting_pos, piece.x2, piece.y1+shifting_pos_end);
        // sum = CountNonWhitePixelsInArea(I, left, shifting_pos, right, shifting_pos_end);

        if (saved_sum == 0 && sum > thres){
            top = shifting_pos;
        }

        else if (saved_sum > thres && sum == 0)
        {
            //Cut current image 'I' into two pieces
            Mat new_piece = I(Range(top, shifting_pos_end), Range(left, right));

            //Create ImgPiece objects out of pieces and keep track of their positions
            ImgPiece a_new_piece(new_piece, piece.x1, piece.y1+top, piece.x2, piece.y1+shifting_pos_end);

            // DisplayPiece(a_new_piece);

            //Add all pieces to vector
            temp_img_pieces.push_back(a_new_piece);

            top = shifting_pos;
        }
    }
    if (temp_img_pieces.size() <= 1){
        img_pieces.push_back(piece);
    }
    else {
        for (int x = 0; x < temp_img_pieces.size(); ++x)
        {
            more_pieces = CutUpImageVertically(temp_img_pieces[x], cons, thres, scan_rate);
            img_pieces.insert(img_pieces.end(), more_pieces.begin(), more_pieces.end());
        }
    }
    return img_pieces;
}

int main( int argc, char** argv )
{
    if( argc != 6)
    {
     cout <<"Usage: display_image ImageToLoadAndDisplay cons thres scan_rate output_file" << endl;
     return -1;
    }

    Mat I;
    Mat wboxes;
    vector<int> col_boundries;
    vector<ImgPiece> img_pieces; //create vector of image pieces to store the divided parts of the image.
    Point pt1;
    Point pt2;
    int cons, thres, scan_rate;
    char* input_file;
    char* output_file;

    input_file = argv[1];
    cons = atoi(argv[2]);
    thres = atoi(argv[3]);
    scan_rate = atoi(argv[4]);
    output_file = argv[5];

    I = imread(input_file, CV_LOAD_IMAGE_COLOR);
    if (I.empty()){
        cout << "\nCannot open image!" << endl;
        exit(6);
    }

    IMG = PreProc( I);
    IMGD = I;
    IMGP = PreProcIntegral(I);

    col_boundries = ComputeColumnRegions(PreProcIntegral(I), cons, thres, scan_rate);

    for (int i = 0; i < col_boundries.size(); ++i)
    {
        pt1 = Point(col_boundries[i], 0);
        pt2 = Point(col_boundries[i], I.rows);
        line(I, pt1, pt2, Scalar( 0, 0, 255 ));
    }


    ImgPiece img_piece(PreProcIntegral(I), 0, 0, I.cols-1, I.rows-1); //create first piecie as whole image
    img_pieces = CutUpImageVertically(img_piece, cons, thres, scan_rate); // Initiate main program

    if (img_pieces.size() <= 1){
        img_pieces = CutUpImageHorizontally(img_piece, cons, thres, scan_rate);
    }

    wboxes = DrawBoxes(IMG, img_pieces);
    imwrite(output_file, wboxes);

    return 0;
}