/*
#
# Copyright 2010 Daniel Snider
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

Displays your webcam with a video overlay in the position that would be best if it were being projected into your world.

Input: a video to play
Requirments: a webcam, VLC media player, xdotool
*/

#define THRESHOLD_VAL 0.1
#include <stdio.h>
#include "cv.h"
#include "highgui.h"

IplImage* findWhitespace(IplImage *reference, IplImage *template)
{
    IplImage    *result;
    IplImage    *color_result;
    CvRect      res;
    CvPoint     minloc, maxloc;
    double      minval, maxval;
    int         reference_width, reference_height;
    int         template_width, template_height;
    int         result_width, result_height;

    /* get image's properties */
    reference_width  = reference->width;
    reference_height = reference->height;
    template_width  = template->width;
    template_height = template->height;
    result_width  = reference_width - template_width + 1;
    result_height = reference_height - template_height + 1;

    /* result image of template match*/
    result = cvCreateImage( cvSize( result_width, result_height ), IPL_DEPTH_32F, 1 );
    color_result = cvCreateImage(cvSize(reference->width, reference->height), 8, 3);


    /* choose template matching method to be used */
    cvMatchTemplate( reference, template, result, CV_TM_SQDIFF );

    cvMinMaxLoc( result, &minval, &maxval, &minloc, &maxloc, 0 );

    if (minval < THRESHOLD_VAL) {
        // printf("Best match found at (%d, %d)\n", minloc.x, minloc.y);
        cvCvtColor(reference,color_result,CV_GRAY2RGB);
        cvRectangle( color_result,cvPoint( minloc.x, minloc.y ), cvPoint( minloc.x + template_width, minloc.y + template_height ), cvScalar( 0, 0, 255, 0 ), 1, 0, 0 );    //draw square

        // cvMoveWindow( "reference", 0, 0);//cvNamedWindow( "reference", CV_WINDOW_AUTOSIZE );    //create window
        //cvShowImage( "reference", reference );
        //cvMoveWindow( "reference", 0, 0);

        // res = cvRect(minloc.x, minloc.y, template_width, template_height);
        //printf("%d, %d, %d, %d\n", res.x, res.y, res.width, res.height);

        //// Move VLC window on linux to the found whitespace area
        // char sizecmd[200];
        // char mvcmd[200];

        // char wid[50];
        // char hei[50];
        // char vidx[50];
        // char vidy[50];

        // sprintf(wid,"%d",res.x);
        // sprintf(hei,"%d",res.y+25); //25 is the offset for window title bar.. maybe?
        // sprintf(vidx,"%d",res.width);
        // sprintf(vidy,"%d",res.height);

        // strcpy (sizecmd, "xdotool windowsize `xdotool search --onlyvisible --classname \"vlc\"` ");
        // strcat (sizecmd, vidx);
        // strcat (sizecmd, " ");
        // strcat (sizecmd, vidy);

        // strcpy (mvcmd, "xdotool windowmove `xdotool search --onlyvisible --classname \"vlc\"`  ");
        // strcat (mvcmd, wid);
        // strcat (mvcmd, " ");
        // strcat (mvcmd, hei);

        // printf("%s\n",sizecmd);
        // system(sizecmd);
        // printf("%s\n",mvcmd);
        // system(mvcmd);

        return color_result; //I can't find a way around the warning caused by this
    }

    else
    {
        int percentage;
        percentage = 83;

        if (((template->width*percentage)/100) < 10 || ((template->height*percentage)/100) < 10) {
            printf("Error: Your walls are too cluttered! Or image resolution too small\n"); //wow clean your house
            // cvNamedWindow( "referenc32e", CV_WINDOW_AUTOSIZE );
            // cvShowImage( "referenc32e", reference );
            // cvWaitKey( 0 );
            //exit(1);  //this might be useful...
        }

        else {
            IplImage *newtemp;
            //create white image, to be used as the colour and size to look for. i.e. the template
            newtemp = cvCreateImage( cvSize( ((template->width*percentage)/100), ((template->height*percentage)/100)), template->depth, template->nChannels);
            //set template white
            cvRectangle(newtemp, cvPoint(0,0),  cvPoint(newtemp->width, newtemp->height), cvScalarAll(255),CV_FILLED, 0, 0);
            findWhitespace(reference, newtemp);
        }
    }
}

IplImage* processFrame(IplImage* greyframe)
{
    IplImage *tpl; //whitespace to be found within image (tpl stands for template)
    IplImage *wspace; //the best whitespace found as a result of image analysis

    //create white image, to be used as the colour and size to look for. i.e. the template
    tpl = cvCreateImage( cvSize( greyframe->width, greyframe->height), greyframe->depth, greyframe->nChannels);

    //set template white
    cvRectangle(tpl, cvPoint(0,0),  cvPoint(greyframe->width, greyframe->height), cvScalarAll(255),CV_FILLED, 0, 0);

    cvAdaptiveThreshold (greyframe, greyframe, 1000, 0, 0, 3, 5); //dialect picture to white

    //find largest clear wall space
    wspace = findWhitespace(greyframe, tpl); //find whitespace

    /*
    */

    return wspace;
}

int playVideo(char vidpath[])
{
    char vidcmd[250];

    strcpy (vidcmd, "cvlc --quiet ");
    strcat (vidcmd, vidpath); //max vidpath length is 50
    strcat (vidcmd, " --video-x=100 --video-y=100 --width=100 --height=100");

    int pid;
    pid = fork();
    if (pid != 0)
    {
        sleep(2);
        system(vidcmd);
    }
    return pid;
}

void processCam(char vidpath[])
{
    CvCapture *capture = 0;
    IplImage  *frame = 0;
    int       key = 0;
    int pid;
    char kill[20], pidc[10];

    /* initialize camera */
    capture = cvCaptureFromCAM( 0 );

    /* always check */
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        exit(1);
    }

    pid = playVideo(vidpath);
    strcpy (kill, "kill ");
    sprintf(pidc,"%d",pid);
    strcat( kill, pidc);

    /* create a window for the video */
    cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );

    while( key != 'q' ) {
        /* get a frame */
        frame = cvQueryFrame( capture );

        /* always check */
        if( !frame ) break;

       frame = processFrame(frame);

        /* display current frame */
        cvShowImage( "result", frame );
        cvMoveWindow( "result", 0, 0);

        /* exit if user press 'q' */
        key = cvWaitKey( 10 );
    }

    /* free memory */
    system(kill);
    cvDestroyWindow( "result" );
    cvReleaseCapture( &capture );
}


int main( int argc, char** argv )
{
    if( argc != 3)
    {
        fprintf( stderr, "Usage: empty_wall_space [input image] [output image save name]");
        return -1;
    }
    // processCam(argv[1]);

    IplImage* frame;
    IplImage* processed_frame;
    frame = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
    /* check if image exists */
    if( frame == 0 ) {
        fprintf( stderr, "Cannot load file %s!\n", argv[1] );
        exit(1);
    }
    processed_frame = processFrame(frame);
    cvSaveImage(argv[2], processed_frame, 0);

    return 0;
}
