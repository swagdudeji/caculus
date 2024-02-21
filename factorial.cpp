#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#include <iostream>
#include "mingw.thread.h"
#include "mingw.mutex.h"
#include <chrono>
using namespace std;

mutex myMutex;

void part_of_factorial(int start,int stop,  unsigned long long& result){ 
    int local_result = 1;

    if (start == 0){
        start = 1;
    }

    for (int i = start; i < stop; i++){
        local_result = local_result * i;
    }

    lock_guard<mutex> lock(myMutex); 
    result = result * local_result; 

}

int main(){
    const int processor_count = std::thread::hardware_concurrency(); 
    
    int factorial_arg; 
    cout << "\nEnter number\n";
    cin >> factorial_arg;
    auto begin_time = chrono::steady_clock::now();
    int step = factorial_arg / processor_count; 
    unsigned long long result = 1;

    if (step < 1){ 
        thread AloneWolf(part_of_factorial,1,factorial_arg+1,ref(result));
        AloneWolf.join();
    }

    else{
    
        thread t[processor_count];
        for(int i = 0; i < processor_count; i++){
            int start = step*i;
            int stop = step*(i+1);
            if ((i+1)==processor_count){ 
                stop = factorial_arg + 1;
            }
            t[i]= thread(part_of_factorial,start,stop,ref(result));
        }

        for(int i = 0; i < processor_count; i++){
            t[i].join();
        }
    }
    auto end_time = chrono::steady_clock::now();
    auto elapsed_ms = chrono::duration_cast<chrono::nanoseconds>(end_time - begin_time);
    cout << "Result:" << result << endl;
    cout << "Work time:" << elapsed_ms.count()/1000000 << "ms" << endl;
    return 0;
}