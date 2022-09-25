#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
int main()
{
    std::string img_path = "c:\\users\\murtaza\\opencv\\sources\\samples\\data\\starry_night.jpg";
    cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cout << "Could not read the image";
        return 1;
    }
    cv::imshow("Display window", img);
    int k = cv::waitKey(0);
    if (k == 's') {
        cv::imwrite("Starry_night2.png", img);
    }
    return 0;
}
