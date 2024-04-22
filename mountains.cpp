#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <omp.h>
using namespace cv;
using namespace std;


void to_grayscale(Mat& source_mat);
void to_sepia(Mat& source_mat);
void to_negative(Mat& source_mat);
void to_contours(Mat& grayscale_mat);


int main() {
    Mat img = cv::imread("D:/lab1os/mountains.jpg", IMREAD_COLOR);

    if (img.empty()) {
        cout << "Error! Failed to open img file" << endl;
        return -1;
    }

    Mat gray = img.clone();
    Mat sepia = img.clone();
    Mat negative = img.clone();
    Mat contours = img.clone();

#pragma omp parallel sections num_threads(4)
    {
#pragma omp section
        {
            to_grayscale(gray);
        }
#pragma omp section
        {
            to_sepia(sepia);
        }
#pragma omp section
        {
            to_negative(negative);
        }
#pragma omp section
        {
            to_contours(contours);
        }
    }

    imshow("Original img", img);
    imshow("Gray img", gray);
    imshow("Sepia img", sepia);
    imshow("Negative img", negative);
    imshow("Contour img", contours);

    waitKey(0);
    destroyAllWindows();
    return 0;
}



void to_grayscale(Mat& source_mat) {
    int rows = source_mat.rows;
    int cols = source_mat.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Vec3b pixel = source_mat.at<Vec3b>(i, j);
            uchar grayValue = (pixel[0] + pixel[1] + pixel[2]) / 3;
            source_mat.at<Vec3b>(i, j) = Vec3b(grayValue, grayValue, grayValue);
        }
    }
}

void to_sepia(Mat& source_mat) {
    int rows = source_mat.rows;
    int cols = source_mat.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Vec3b pixel = source_mat.at<Vec3b>(i, j);
            int sepiaR = (int)(0.393 * pixel[0] + 0.769 * pixel[1] + 0.189 * pixel[2]);
            int sepiaG = (int)(0.349 * pixel[0] + 0.686 * pixel[1] + 0.168 * pixel[2]);
            int sepiaB = (int)(0.272 * pixel[0] + 0.534 * pixel[1] + 0.131 * pixel[2]);
            source_mat.at<Vec3b>(i, j) = Vec3b(min(sepiaB, 255), min(sepiaG, 255), min(sepiaR, 255));
        }
    }
}

void to_negative(Mat& source_mat) {
    int rows = source_mat.rows;
    int cols = source_mat.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Vec3b pixel = source_mat.at<Vec3b>(i, j);
            source_mat.at<Vec3b>(i, j) = Vec3b(255 - pixel[0], 255 - pixel[1], 255 - pixel[2]);
        }
    }
}

void to_contours(Mat& source_mat) {
    Mat grayImg;
    cvtColor(source_mat, grayImg, COLOR_BGR2GRAY);
    Mat edges = Mat(source_mat.rows, source_mat.cols, CV_8U);

    for (int i = 1; i < grayImg.rows - 1; i++) {
        for (int j = 1; j < grayImg.cols - 1; j++) {
            float gx = grayImg.at<uchar>(i + 1, j + 1) + 2 * grayImg.at<uchar>(i, j + 1) + grayImg.at<uchar>(i - 1, j + 1) - grayImg.at<uchar>(i + 1, j - 1) - 2 * grayImg.at<uchar>(i, j - 1) - grayImg.at<uchar>(i - 1, j - 1);
            float gy = grayImg.at<uchar>(i + 1, j + 1) + 2 * grayImg.at<uchar>(i + 1, j) + grayImg.at<uchar>(i + 1, j - 1) - grayImg.at<uchar>(i - 1, j - 1) - 2 * grayImg.at<uchar>(i - 1, j) - grayImg.at<uchar>(i - 1, j + 1);
            edges.at<uchar>(i, j) = 255 - sqrt(pow(gx, 2) + pow(gy, 2));
        }
    }
    source_mat = edges.clone();
}