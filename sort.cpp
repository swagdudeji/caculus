#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <thread>
#include "omp.h"
using namespace std;

void merge(int *arr,int first, int last);
void mergeSort(int *arr,int first, int last);
void mergeSortUnparallel(int *arr,int first, int last);
int check(int *arr1, int *arr2, int size);

int mai(){
    int size = 100000;
    int arr_parallel[size], arr_unparallel[size];
    for (int i = 0; i < size; i++){
        arr_parallel[i] = rand();
        arr_unparallel[i] = arr_parallel[i];
    }
    auto begin_p = chrono::steady_clock::now();
    mergeSort(arr_parallel, 0, size - 1);
    auto end_p = chrono::steady_clock::now();
    auto time_p = chrono::duration_cast<std::chrono::milliseconds>(end_p - begin_p);
    auto begin_u = chrono::steady_clock::now();
    mergeSortUnparallel(arr_unparallel, 0, size - 1);
    auto end_u = chrono::steady_clock::now();
    auto time_u = chrono::duration_cast<std::chrono::milliseconds>(end_u - begin_u);
    cout << check(arr_parallel, arr_unparallel, size) << " P_TIME: " << time_p.count() << " U_TIME: " << time_u.count();

    return 0;
}

void merge(int *arr,int first, int last){
    if (first==last) return;
    int *arr2 = new int[last-first+1];
    int start = first;
    int middle = (last+first)/2;
    int finish = middle+1;
    for (int i=first; i<=last; ++i){
        if ((start<=middle) && ((finish>last) || arr[start]<arr[finish])){
            arr2[i-first]=arr[start];
            ++start;
        }
        else{
            arr2[i-first]=arr[finish];
            ++finish;
        }
    }
    for(int i=first;i<=last;++i){
        arr[i]=arr2[i-first];
    }
    delete[] arr2;
};
void mergeSort(int *arr,int first, int last){
    if (first<last){
#pragma omp parallel num_threads(4)
#pragma omp parallel sections shared(arr)
        {
#pragma omp section
            {
                mergeSort(arr,first, (first+last)/2);
            }
#pragma omp section
            {
                mergeSort(arr,(first+last)/2+1, last);
            }
#pragma omp barrier
        }
        merge(arr,first,last);
    }
};

void mergeSortUnparallel(int *arr,int first, int last){
    if (first<last){
        {
            {
                mergeSort(arr,first, (first+last)/2);
            }
            {
                mergeSort(arr,(first+last)/2+1, last);
            }
        }
        merge(arr,first,last);
    }
};

int check(int *arr1, int *arr2, int size){
    for (int i = 0; i < size; i++){
        if (arr1[i] != arr2[i]){
            return 0;
        }
    }
    return 1;
}
