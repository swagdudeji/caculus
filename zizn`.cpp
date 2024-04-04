//
// Created by debilian_user on 13/03/24.
//
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <thread>
#include "omp.h"
using namespace std;

struct Cell {
    int current_status;
    int next_status;
};


void set_cell_next_status(vector <vector <Cell>>& field, int row, int column);
bool update_field(vector <vector <Cell>>& field);
void generate_field(vector <vector <Cell>>& field, int field_size);
void print_field(vector <vector <Cell>> field);
bool check_for_update(vector <vector <Cell>> field);

int main() {
    int field_size = 40;
    vector <vector <Cell>> field(field_size, vector <Cell>(field_size));
    generate_field(field, field_size);
    Sleep(1000);
    print_field(field);
    for (int i = 0; i < 50; i++) {
        if (!update_field(field)) { break; }
        print_field(field);
    }
}

void set_cell_next_status(vector <vector <Cell>>& field, int row, int column) {
    int alive_neighbours_count = 0;
    if (row > 0) {
        alive_neighbours_count += field[row - 1][column].current_status;
    }
    if (row + 1 < field[0].size()) {
        alive_neighbours_count += field[row + 1][column].current_status;
    }
    if (column > 0) {
        alive_neighbours_count += field[row][column - 1].current_status;
    }
    if (column + 1 < field[0].size()) {
        alive_neighbours_count += field[row][column + 1].current_status;
    }

    if (field[row][column].current_status == 1) {
        field[row][column].next_status = (alive_neighbours_count == 2 || alive_neighbours_count == 3) ? 1 : 0;
    }
    else {
        field[row][column].next_status = (alive_neighbours_count == 3) ? 1 : 0;
    }
}

bool update_field(vector <vector <Cell>>& field) {
#pragma omp parallel for num_threads(MAX)
    for (int i = 0; i < field.size(); i++) {
        for (int j = 0; j < field.size(); j++) {
            set_cell_next_status(field, i, j);
        }
    }
    if (!check_for_update(field)) { return false; }
#pragma omp parallel for num_threads(MAX)
    for (int i = 0; i < field.size(); i++) {
        for (int j = 0; j < field.size(); j++) {
            field[i][j].current_status = field[i][j].next_status;
        }
    }
    return true;
}

void generate_field(vector <vector <Cell>>& field, int field_size) {
    random_device rd;
    mt19937 gen(rd());
    double mu = 1.0;
    double sigma = 1.0;
    //std::lognormal_distribution<double> distrib(mu, sigma);
    uniform_int_distribution<> distrib(0, 1);
#pragma omp parallel for num_threads(MAX)
    for (int i = 0; i < field_size; i++) {
        vector <int> current_row_status;//(field_size);
        generate_n(back_inserter(current_row_status), 40, [&distrib, &gen] {return distrib(gen); });
        for (int j = 0; j < field_size; j++) {
            field[i][j].current_status = current_row_status[j];
        }
    }
}

void print_field(vector <vector <Cell>> field) {
    system("cls");
    for (int i = 0; i < field.size(); i++) {
        for (int j = 0; j < field[i].size(); j++) {
            if (field[i][j].current_status == 1) {
                cout << 0 << " ";
            }
            else { cout << "_ "; }
        }
        cout << "\n";
    }
    cout << "\n###############################################################################\n";
    Sleep(0.5);
}

bool check_for_update(vector <vector <Cell>> field) {
    bool res = false;
#pragma omp parallel for num_threads(MAX)
    for (int i = 0; i < field.size(); i++) {
        for (int j = 0; j < field[i].size(); j++) {
            if (field[i][j].current_status != field[i][j].next_status) { res = true; break; }
        }
    }
    return res;
}