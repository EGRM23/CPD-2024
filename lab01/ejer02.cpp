#include <iostream>
#include <ctime>
#include <chrono>
using namespace std;

int main()
{
    const int MAX = 256;
    int n = 0, salto = 32;
    double M1[MAX][MAX], M2[MAX][MAX], MR[MAX][MAX];
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    srand(time(0));

    //Inicializamos M1 Y M2
    for(int i = 0; i < MAX; i++)
    {
        for(int j = 0; j < MAX; j++)
        {
            M1[i][j] = 1 + rand() % 10000;
            M2[i][j] = 1 + rand() % 10000;
            MR[i][j] = 0;
        }
    }

    //Multiplicacion de matriz clásica
    while (n != MAX)
    {
        for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                MR[i][j] = 0;
        n += salto;

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n; k++)
                    MR[i][j] += M1[i][k]*M2[k][j];
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout << "MULTIPLICACION MATRIZ DE " << n << "x" << n << ": " << duration.count() << " ms" <<endl;
    }
}