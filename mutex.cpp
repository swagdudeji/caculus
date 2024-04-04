#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

int generate_rand_num(int r);
void m_max(int id);

vector<int> shared_vec;
mutex vec_mutex;
condition_variable vec_cv;
int curr_thread_id = 1;

int main() {
    int vec_len = 100;
    int random_rng = 10000;
    vector<thread> threads;

    for (int i = 0; i < vec_len; i++) {
        shared_vec.push_back(generate_rand_num(random_rng));
    }

    for (int i = 1; i <= thread::hardware_concurrency(); ++i) {
        threads.emplace_back(m_max, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}

int generate_rand_num(int r) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(-r, r);
    return distrib(gen);
}

void m_max(int id) {
    unique_lock<mutex> lock(vec_mutex);

    while (curr_thread_id != id) {
        vec_cv.wait(lock);
    }

    cout << "Thread " << id << " started" << endl;

    int vec_max = shared_vec[0];
    for (int i : shared_vec) {
        if (i > vec_max) { vec_max = i; }
    }

    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "max: " << vec_max << endl;
    cout << "Thread " << id << " completed\n" << endl;

    curr_thread_id = (curr_thread_id % thread::hardware_concurrency()) + 1;
    vec_cv.notify_all();
}