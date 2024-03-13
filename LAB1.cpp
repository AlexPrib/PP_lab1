#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <time.h>
#include <omp.h>

using namespace std;
using namespace std::chrono;

class MatrixMultiplier {
private:
    int size;
public:
    MatrixMultiplier(int size) : size(size) {
        srand(time(NULL));
    }

    long long generateRandomMatrixToFile(const string& filename) {
        auto start = high_resolution_clock::now();

        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file: " << filename << endl;
            exit(1);
        }

        file << size << endl;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                file << rand() % 100 << " ";
            }
            file << endl;
        }

        file.close();

        auto stop = high_resolution_clock::now();
        return duration_cast<milliseconds>(stop - start).count();
    }

    /*void freeMatrix(int** matrix) {
        for (int i = 0; i < size; ++i) {
            delete[] matrix[i];
        }
        delete[] matrix;
    }*/
    void writeMatrixToFile(int** matrix, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file: " << filename << endl;
            exit(1);
        }

        file << size << endl;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                file << matrix[i][j] << " ";
            }
            file << endl;
        }

        file.close();
    }

    long long multiplyMatrices(int matrix_size, const string& filename1, const string& filename2, const string& filename_res) {
        auto start = high_resolution_clock::now();

        ifstream file1(filename1);
        if (!file1.is_open()) {
            cerr << "Unable to open files: " << filename1 << endl;
            exit(1);
        }

        ifstream file2(filename2);
        if (!file2.is_open()) {
            cerr << "Unable to open files: " << filename2 << endl;
            exit(1);
        }

        // Чтение размера матрицы из файла
        file1 >> matrix_size;
        file2 >> matrix_size;
        cout << "Размеры перемножаемых матриц: " << matrix_size << "x" << matrix_size << endl;

        int** matrix1 = new int* [size];
        int** matrix2 = new int* [size];
        int** res_matrix = new int* [size];
        for (int i = 0; i < size; ++i) {
            matrix1[i] = new int[size];
            matrix2[i] = new int[size];
            res_matrix[i] = new int[size];
            for (int j = 0; j < size; ++j) {
                file1 >> matrix1[i][j];
                file2 >> matrix2[i][j];
            }
        }
        #pragma omp parallel num_threads(2)
        {
            #pragma omp for
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    res_matrix[i][j] = 0;
                    for (int k = 0; k < size; ++k) {
                        res_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
                    }
                }
            }
        }

        writeMatrixToFile(res_matrix, "result_matrix.txt");
    

        file1.close();
        file2.close();

        for (int i = 0; i < size; ++i) {
            delete[] matrix1[i];
            delete[] matrix2[i];
            delete[] res_matrix[i];
        }
        delete[] matrix1;
        delete[] matrix2;
        delete[] res_matrix;

        auto stop = high_resolution_clock::now();
        return duration_cast<milliseconds>(stop - start).count();
    }
};


int main() 
{
    setlocale(LC_ALL, "Russian");
    int N = 500;
    
    while (N <= 2000) {
        string filename1 = "matrix1.txt";
        string filename2 = "matrix2.txt";
        string filename_res = "result_matrix.txt";

        for (int i = 0; i < 10; ++i) {
            MatrixMultiplier matrixMultiplier(N);

            cout << "Файлы с данными для матриц сгенерированы" << endl;

            long long generation_time = matrixMultiplier.generateRandomMatrixToFile(filename1);
            long long generation_time_2 = matrixMultiplier.generateRandomMatrixToFile(filename2);
            long long computation_time = matrixMultiplier.multiplyMatrices(N, filename1, filename2, filename_res);
            long long total_time = generation_time + computation_time;
            long long task_size = static_cast<long long>(N) * static_cast<long long>(N) * static_cast<long long>(N);

            cout << "Время генерации матриц: " << generation_time << " мс" << endl;
            cout << "Матрицы перемножены." << endl;
            cout << "Время умножения матриц: " << computation_time << " мс" << endl;
            cout << "Общее время выполнения: " << total_time << " мс" << endl;
            cout << "Объем задачи: " << task_size << endl;

            ofstream resultsFile("results.txt", ios::app);
            if (!resultsFile.is_open()) {
                cerr << "Unable to open file: results.txt" << endl;
                exit(1);
            }

            resultsFile << N << endl << generation_time << endl << computation_time << endl << total_time << endl << task_size << endl;
            resultsFile.close();
        }
        N += 100;
    }
    return 0;
}
