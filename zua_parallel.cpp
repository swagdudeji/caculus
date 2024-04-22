#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <thread>
#include <omp.h>
#include <chrono>


    using namespace std;
    using namespace cv;

    Mat pre_process_image_(Mat& source_img);
    void detect_face(Mat & img, CascadeClassifier & cascade, CascadeClassifier & nestedCascade, CascadeClassifier & smile_cascade, double scale);
    void detect_face_parallel(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nestedCascade, CascadeClassifier& smile_cascade, double scale);
    void draw_rectangle(Mat& source_img, Rect rectangle_, Scalar colour);
    vector <Rect> get_rectangles_by_cascade(Mat source_img, CascadeClassifier& cascade, double scale_factor, int min_neighbours, Size size);


    int main(int argc, const char** argv)
    {
        VideoCapture capture;
        Mat frame, image;
        CascadeClassifier face_cascade, eyes_cascade, smile_cascade;
        face_cascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml");
        eyes_cascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
        smile_cascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_smile.xml");
        double scale = 1;

        capture.open("D:\\ZUA.mp4");
        int frame_width = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH)); 
        int frame_height = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT)); 
        Size frame_size(frame_width, frame_height);
        int frames_per_second = 10;

        VideoWriter oVideoWriter("D:\\output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));
        if (capture.isOpened())
        {
            cout << "Someone bring me up the lobster" << endl;

            auto begin = std::chrono::steady_clock::now();

            for (int i = 0; i < 10; i++) // while (1)
            {
                capture >> frame;
                if (frame.empty())
                    break;
                Mat frame1 = frame.clone();

                detect_face_parallel(frame1, face_cascade, eyes_cascade, smile_cascade, scale);
                oVideoWriter.write(frame1);
                char c = (char)waitKey(10);

                if (c == 27 || c == 'q' || c == 'Q')
                    break;
            }

            auto end = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            std::cout << "The time: " << elapsed_ms.count() << " ms\n";

            oVideoWriter.release();
            cout << "SHEEEEEEEEEEEEESH";
        }
        else
            cout << "Could not Open Video/Camera! ";
        return 0;
    }

    Mat pre_process_image_(Mat& source_img) {
        Mat gray, image_blurred, resized_img;
        cvtColor(source_img, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, image_blurred, Size(3, 3), 0);
        double fx = 1;
        resize(image_blurred, resized_img, Size(), fx, fx, INTER_LINEAR);
        equalizeHist(resized_img, resized_img);
        return resized_img;
    }

    void draw_rectangle(Mat& source_img, Rect rectangle_, Scalar colour) {
        rectangle(source_img, Point(rectangle_.x, rectangle_.y), Point(rectangle_.x + rectangle_.width, rectangle_.y + rectangle_.height), colour, 4);
    }

    vector <Rect> get_rectangles_by_cascade(Mat source_img, CascadeClassifier& cascade, double scale_factor, int min_neighbours, Size size) {
        vector <Rect> rectangles;
        cascade.detectMultiScale(source_img, rectangles, scale_factor, min_neighbours, 0 | CASCADE_SCALE_IMAGE, size);
        return rectangles;
    }

    void detect_face(Mat & source_img, CascadeClassifier & face_cascade, CascadeClassifier & eyes_cascade, CascadeClassifier & smile_cascade, double scale)
    {
        Mat pre_processed_img = pre_process_image_(source_img);
        vector<Rect> faces, eyes, smiles;

        faces = get_rectangles_by_cascade(pre_processed_img, face_cascade, 1.1, 2, Size(30, 30));
        eyes = get_rectangles_by_cascade(pre_processed_img, eyes_cascade, 1.1, 2, Size(30, 30));
        smiles = get_rectangles_by_cascade(pre_processed_img, smile_cascade, 1.8, 22, Size(30, 30));

        for (Rect rect : faces) {
            draw_rectangle(source_img, rect, Scalar(0, 255, 0));
        }

        for (Rect rect : eyes) {
            draw_rectangle(source_img, rect, Scalar(0, 0, 255));
        }

        for (Rect rect : smiles) {
            draw_rectangle(source_img, rect, Scalar(255, 0, 0));
        }

        imshow("Face Detection", source_img);
    }

    void detect_face_parallel(Mat& source_img, CascadeClassifier& face_cascade, CascadeClassifier& eyes_cascade, CascadeClassifier& smile_cascade, double scale)
    {
        Mat pre_processed_img = pre_process_image_(source_img);
        vector<Rect> faces, eyes, smiles;

        const int processor_count = std::thread::hardware_concurrency();

#pragma omp parallel sections num_threads(processor_count)
        {
#pragma omp section 
            {
                cout << "section 1 start\n";
                faces = get_rectangles_by_cascade(pre_processed_img, face_cascade, 1.1, 2, Size(30, 30));
                cout << "section 1 end\n";
            }
#pragma omp section 
            {
                cout << "section 2 start\n";
                eyes = get_rectangles_by_cascade(pre_processed_img, eyes_cascade, 1.1, 2, Size(30, 30));
                cout << "section 2 end\n";
            }
#pragma omp section 
            {
                cout << "section 3 start\n";
                smiles = get_rectangles_by_cascade(pre_processed_img, smile_cascade, 1.8, 22, Size(30, 30));
                cout << "section 3 end\n";
            }
        }

        for (Rect rect : faces) {
            draw_rectangle(source_img, rect, Scalar(0, 255, 0));
        }

        for (Rect rect : eyes) {
            draw_rectangle(source_img, rect, Scalar(0, 0, 255));
        }

        for (Rect rect : smiles) {
            draw_rectangle(source_img, rect, Scalar(255, 0, 0));
        }

        imshow("Face Detection", source_img);
    }

//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//#include <chrono>
//
//void detectFaces(const cv::Mat& frame, cv::CascadeClassifier& faceCascade, std::vector<cv::Rect>& faces) {
//    cv::Mat gray;
//    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
//
//    faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));
//
//    for (const auto& face : faces) {
//        cv::rectangle(frame, face, cv::Scalar(255, 180, 0), 2);
//    }
//}
//
//void detectEyes(const cv::Mat& frame, const std::vector<cv::Rect>& faces, cv::CascadeClassifier& eyesCascade) {
//    for (const auto& face : faces) {
//        cv::Mat faceROI = frame(face);
//
//        std::vector<cv::Rect> eyes;
//        eyesCascade.detectMultiScale(faceROI, eyes, 1.1, 10, 0, cv::Size(20, 20));
//
//        for (const auto& eye : eyes) {
//            cv::Point center(face.x + eye.x + eye.width / 2, face.y + eye.y + eye.height / 2);
//            int radius = cvRound((eye.width + eye.height) * 0.25);
//            cv::circle(frame, center, radius, cv::Scalar(200, 200, 0), 2);
//        }
//    }
//}
//
//void detectSmiles(const cv::Mat& frame, const std::vector<cv::Rect>& faces, cv::CascadeClassifier& smileCascade) {
//    for (const auto& face : faces) {
//        cv::Mat faceROI = frame(face);
//
//        std::vector<cv::Rect> smiles;
//        smileCascade.detectMultiScale(faceROI, smiles, 2, 30, 0, cv::Size(10, 10));
//
//        for (const auto& smile : smiles) {
//            cv::rectangle(frame, cv::Point(face.x + smile.x, face.y + smile.y),
//                cv::Point(face.x + smile.x + smile.width, face.y + smile.y + smile.height),
//                cv::Scalar(255, 0, 0), 2);
//        }
//    }
//}
//
//int main() {
//    cv::CascadeClassifier faceCascade, eyesCascade, smileCascade;
//
//    faceCascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml");
//    eyesCascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
//    smileCascade.load("D:/OpenCV/opencv/sources/data/haarcascades/haarcascade_smile.xml");
//
//    cv::VideoCapture cap("D:/ZUA.mp4");
//    if (!cap.isOpened()) {
//        std::cerr << "error" << std::endl;
//        return -1;
//    }
//
//    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
//    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
//    double fps = cap.get(cv::CAP_PROP_FPS);
//
//    std::vector<cv::Mat> processedFrames;
//
//    auto start = std::chrono::steady_clock::now();
//
//    while (true) {
//        cv::Mat frame;
//        cap >> frame;
//        if (frame.empty()) {
//
//            break;
//        }
//
//        cv::Mat resizedFrame;
//        cv::resize(frame, resizedFrame, cv::Size(), 0.5, 0.5, cv::INTER_LANCZOS4);
//
//        std::vector<cv::Rect> faces;
//        detectFaces(resizedFrame, faceCascade, faces);
//
//        detectEyes(resizedFrame, faces, eyesCascade);
//
//
//        detectSmiles(resizedFrame, faces, smileCascade);
//
//        cv::resize(resizedFrame, resizedFrame, cv::Size(frameWidth, frameHeight));
//        processedFrames.push_back(resizedFrame.clone());
//    }
//
//    auto end = std::chrono::steady_clock::now();
//    std::chrono::duration<double> elapsed_seconds = end - start;
//    std::cout << "time: " << elapsed_seconds.count() << "s" << std::endl;
//
//    cap.release();
//
//    cv::VideoWriter video("D:/output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(frameWidth, frameHeight));
//    for (const auto& frame : processedFrames) {
//        video.write(frame);
//        cv::imshow("UwU", frame);
//
//        if (cv::waitKey(10) == 'q') {
//            break;
//        }
//    }
//
//    video.release();
//    cv::destroyAllWindows();
//
//    return 0;
//}
