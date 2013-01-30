requireds OpnenCV to build

Jon can compile this with:
g++ -I"C:\Users\jonotron\src\opencv\build\include" -L"C:\Users\jonotron\src\opencv\build\x86\mingw\lib" MyImage.cpp MyFuncs.cpp main.cpp -lopencv_core243 -lopencv_highgui243 -lopencv_objdetect243 -lopencv_imgproc243 -o objectDetect
