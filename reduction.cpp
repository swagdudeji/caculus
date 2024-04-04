#include <iostream>
#include <thread>
#include <random>
#include <vector>
#include <algorithm>
#include <iterator>
#include <omp.h>

using namespace std;

int generate_rand_num(int r);

int main() {
    int max_num_threads = omp_get_max_threads();
    vector <int> vec;
    int vec_len = 10;
    int rand_rng = 100;
    for (int i = 0; i < vec_len; i++) {
        vec.push_back(generate_rand_num(rand_rng));
    }
    int sum = 0;

    cout << "threads: " << max_num_threads << "\n";
    cout << "elements: ";
    for (int i : vec) {
        cout << i << ", ";
    }
    cout << "\n";

#pragma omp parallel num_threads(max_num_threads) reduction(+:total_sum)
    for (int i : vec) {
        sum += i;
    }

    cout << "Sum: " << sum << endl;

    return 0;
}

int generate_rand_num(int r) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(-r, r);
    return distrib(gen);
}