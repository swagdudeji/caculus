//
// Created by debilian_user on 12/03/24.
//

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <thread>
#include "omp.h"
using namespace std;


vector<double> generate_point(int r);
bool is_in_circle(vector<double> point, int r);

int main() {
    int r = 10;
    double points_count = 1000000;
    double points_in_circle_count = 0;
#pragma omp parallel for num_threads(MAX)
    for (int i = 0; i < points_count; i++) {
        if (is_in_circle(generate_point(r), r)) {
#pragma omp atomic
            points_in_circle_count += 1;
        }
    }
    cout << 4 * (points_in_circle_count / points_count);
}

vector<double> generate_point(int r) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> distrib(-r, r);
    vector<double> point;

    generate_n(back_inserter(point), 2, [&distrib, &gen] {return distrib(gen); });
    return point;
    //copy(point.begin(), point.end(), ostream_iterator<double>(cout, " "));
}

bool is_in_circle(vector<double> point, int r) {
    double v = point[0] * point[0] + point[1] * point[1];
    if (point[0] * point[0] + point[1] * point[1] <= r * r) { return true; }
    else { return false; }
}