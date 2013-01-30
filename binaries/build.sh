#!/bin/sh
gcc ./source_files/empty_wall_space/empty_wall_space.c -o ./empty_wall_space.out `pkg-config --cflags opencv` `pkg-config --libs opencv`
g++ -g ./source_files/contour_find/contour_find.cpp -o ./contour_find.out `pkg-config --cflags opencv` `pkg-config --libs opencv`
g++ -g ./source_files/highlight_ocr/MyFuncs.cpp ./source_files/highlight_ocr/main.cpp -o ./highlight_ocr.out `pkg-config --cflags opencv` `pkg-config --libs opencv`
g++ -g ./source_files/quiz_it/MyImage.cpp ./source_files/quiz_it/MyFuncs.cpp ./source_files/quiz_it/main.cpp -o ./quiz_it.out `pkg-config --cflags opencv` `pkg-config --libs opencv`

