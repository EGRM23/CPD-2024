#include <iostream>
#include <ctime>
#include <chrono>
using namespace std;

int main()
{
    const int MAX = 500;
    double A[MAX][MAX], x[MAX], y[MAX];
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    srand(time(0));

    //Initialize A and x, assign y = 0
    for(int i = 0; i < MAX; i++)
    {
        for(int j = 0; j < MAX; j++)
            A[i][j] = 1 + rand() % 10000;
        x[i] = 1 + rand() % 1000;
        y[i] = 0;
    }

    //First pair of loops
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX; i++)
        for (int j = 0; j < MAX; j++)
            y[i] += A[i][j]*x[j];
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cout<< "LOOP 1: " << duration.count() << " ms" <<endl;

    //Assign y = 0
    for(int i = 0; i < MAX; i++)
        y[i] = 0;
    

    //Second pair of loops
    start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < MAX; j++)
        for (int i = 0; i < MAX; i++)
            y[i] += A[i][j]*x[j];
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cout<< "LOOP 2: " << duration.count() << " ms" <<endl;
}