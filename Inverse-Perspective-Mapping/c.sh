g++ -c MatrixReaderWriter.cpp 
g++ -c -I/usr/include/opencv4/ Pers.cpp
g++ -I/usr/include/opencv4/ MatrixReaderWriter.o Pers.o -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -o Pers
