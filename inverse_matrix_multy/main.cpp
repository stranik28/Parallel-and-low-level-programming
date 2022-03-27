#include <vector>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <stdexcept>
#include <chrono>

void omp_set_num_threads(
   int num_threads = 4
);


double getDeterminant(const std::vector<std::vector<double>> vect) {
    if(vect.size() != vect[0].size()) {
        throw std::runtime_error("Matrix is not quadratic");
    } 
    int dimension = vect.size();

    if(dimension == 0) {
        return 1;
    }

    if(dimension == 1) {
        return vect[0][0];
    }

    //Formula for 2x2-matrix
    if(dimension == 2) {
        return vect[0][0] * vect[1][1] - vect[0][1] * vect[1][0];
    }

    double result = 0;
    int sign = 1;
    for(int i = 0; i < dimension; i++) {
		
        //Submatrix
        std::vector<std::vector<double>> subVect(dimension - 1, std::vector<double> (dimension - 1));
        for(int m = 1; m < dimension; m++) {
            int z = 0;
            #pragma omp for
            for(int n = 0; n < dimension; n++) {
                if(n != i) {
                    subVect[m-1][z] = vect[m][n];
                    z++;
                }
            }
        }

        //recursive call
        result = result + sign * vect[0][i] * getDeterminant(subVect);
        sign = -sign;
    }

    return result;
}

std::vector<std::vector<double>> getCofactor(const std::vector<std::vector<double>> vect) {
    if(vect.size() != vect[0].size()) {
        throw std::runtime_error("Matrix is not quadratic");
    } 

    std::vector<std::vector<double>> solution(vect.size(), std::vector<double> (vect.size()));
    std::vector<std::vector<double>> subVect(vect.size() - 1, std::vector<double> (vect.size() - 1));
    for(std::size_t i = 0; i < vect.size(); i++) {
        for(std::size_t j = 0; j < vect[0].size(); j++) {

            int p = 0;
            for(size_t x = 0; x < vect.size(); x++) {
                if(x == i) {
                    continue;
                }
                int q = 0;
                #pragma omp for
                for(size_t y = 0; y < vect.size(); y++) {
                    if(y == j) {
                        continue;
                    }

                    subVect[p][q] = vect[x][y];
                    q++;
                }
                p++;
            }
            solution[i][j] = pow(-1, i + j) * getDeterminant(subVect);
        }
    }
    return solution;
}



std::vector<std::vector<double>> getTranspose(const std::vector<std::vector<double>> matrix1) {

    //Transpose-matrix: height = width(matrix), width = height(matrix)
    std::vector<std::vector<double>> solution(matrix1[0].size(), std::vector<double> (matrix1.size()));
    //Filling solution-matrix
    for(size_t i = 0; i < matrix1.size(); i++) {
        #pragma omp for
        for(size_t j = 0; j < matrix1[0].size(); j++) {
            solution[j][i] = matrix1[i][j];
        }
    }
    return solution;
}

std::vector<std::vector<double>> getInverse(const std::vector<std::vector<double>> vect) {
    if(getDeterminant(vect) == 0) {
        throw std::runtime_error("Determinant is 0");
    } 

    double d = 1.0/getDeterminant(vect);
    std::vector<std::vector<double>> solution(vect.size(), std::vector<double> (vect.size()));
    for(size_t i = 0; i < vect.size(); i++) {
        #pragma omp for
        for(size_t j = 0; j < vect.size(); j++) {
            solution[i][j] = vect[i][j]; 
        }
    }
    solution = getTranspose(getCofactor(solution));
    for(size_t i = 0; i < vect.size(); i++) {
        #pragma omp for
        for(size_t j = 0; j < vect.size(); j++) {
            solution[i][j] *= d;
        }
    }

    return solution;
}

int main() {
    std::vector<std::vector<double>> matrix(19, std::vector<double> (19));
	// for(int i = 0; i < n;i++){
	// 	for(int j = 0; j < n; j++){
	// 		matrix[i][j] = (i+j)*1.0;
	// 	}
	// }
    matrix = {
        {2,2,3,9,12,12,99},
		{3,2,23,9,12,14,89},
		{4,64,3,213,12,14,65},
		{1,2,1233,9,12,5,45},
		{1,2,66,9,12,66,11},
		{12,2,3,9,43,14,9},
		{312,2,32,9,43,14,213}
    };
	auto t1 = std::chrono::high_resolution_clock::now();
    getInverse(matrix);
	auto t2 = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << " -multy core \n";
    return 0;
}