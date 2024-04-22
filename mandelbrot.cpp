#include <opencv2/opencv.hpp>
#include <mpi.h>

int mandelbrot(double cr, double ci, int max_iterations) {
    double zr = 0.0, zi = 0.0;
    int iterations = 0;

    while (zr * zr + zi * zi <= 4.0 && iterations < max_iterations) {
        double temp = zr * zr - zi * zi + cr;
        zi = 2 * zr * zi + ci;
        zr = temp;
        iterations++;
    }

    return iterations;
}

cv::Vec3b color(int iterations) {
    if (iterations >= 255) {
        return cv::Vec3b(0, 0, 0);
    }
    else {
        double r = (double)(iterations % 5 + 1) / 8.0 * 255;
        double g = (double)(iterations % 7 + 1) / 6.0 * 255;
        double b = iterations;
        return cv::Vec3b(r, g, b);
    }
}
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int width = 1920;
    int height = 1080;
    int max_iterations = 256;
    double xmin = -2.5;
    double xmax = 2.5;
    double ymin = -1.5;
    double ymax = 1.5;
    double zoom = 1.0;
    double dx = 0.0;
    double dy = 0.0;

    bool redraw = true;

    int rows_per_process = height / size;
    int start_row = rank * rows_per_process;
    int end_row = std::min((rank + 1) * rows_per_process, height);

    while (redraw) {
        cv::Mat sub_image(end_row - start_row, width, CV_8UC3, cv::Scalar(0, 0, 0));

        for (int j = start_row; j < end_row; j++) {
            for (int i = 0; i < width; i++) {
                double x = xmin + (xmax - xmin) * i / width / zoom + dx;
                double y = ymin + (ymax - ymin) * j / height / zoom + dy;


                int iterations = mandelbrot(x, y, max_iterations);

                sub_image.at<cv::Vec3b>(j - start_row, i) = color(iterations);

            }
        }

        cv::Mat final_image(height, width, CV_8UC3);
        MPI_Gather(sub_image.data, (end_row - start_row) * width * 3, MPI_UNSIGNED_CHAR,
            final_image.data, (end_row - start_row) * width * 3, MPI_UNSIGNED_CHAR,
            0, MPI_COMM_WORLD);


        if (rank == 0) {
            cv::namedWindow("Mandelbrot Fractal", cv::WINDOW_NORMAL);
            imshow("Mandelbrot Fractal", final_image);
            int key = cv::waitKey(10);
            switch (key) {
            case 'w':
                dy -= 0.25 * ymax / zoom;
                break;
            case 's':
                dy += 0.25 * ymax / zoom;
                break;
            case 'a':
                dx -= 0.25 * xmax / zoom;
                break;
            case 'd':
                dx += 0.25 * xmax / zoom;
                break;
            case 'e':
                zoom *= 1.25;
                break;
            case 'q':
                zoom /= 1.25;
                break;
            case 'c':
                cv::destroyAllWindows();
                redraw = false;
                MPI_Finalize();
                return 0;

            }
        }
        MPI_Bcast(&zoom, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&dx, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&dy, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&max_iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}