g++ -c MatrixReaderWriter.cpp 
g++ -c -I/usr/include/opencv4/ Panorama.cpp
g++ -I/usr/include/opencv4/ MatrixReaderWriter.o Panorama.o -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -o panorama
