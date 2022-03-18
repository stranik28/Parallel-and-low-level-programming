#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <cmath>
#include <ppl.h>
#include <bits/stdc++.h>

using namespace std;

void init(vector<vector<int>> &uni, int n){
    int mid = n/2;
    #pragma omp for
    for(int i = mid; i < n; i++){
        int f = i - mid;
        uni[f][mid - f] = 8;
        uni[f][mid + f] = 8;
    }
}

void print(vector<vector<int>> &uni, int n){
    for(int i = 1; i < n/2+1; i++){
        for(int j = 0; j < n;j++)
            cout << uni[i][j] << " ";
        cout << endl;
    }
}

void generate_random_data(vector<vector<int>> &uni, int n){
    int u = 0;
    for(int i = 1; i < n/2+1; i++){
        for(int j = 0; j < n;j++){
            if(uni[i][j] == 8){
                u++;
                continue;
            }
            if(u%2 == 0)
                continue;
            uni[i][j] = (rand()%7 == 3)?1:0;
        }
    }
}

void generate(vector<vector<int>> &univ, int n){
    cout << "Start generate data..." << endl;
    init(univ,n);
    generate_random_data(univ,n);
    // print(univ,n);
    cout << "Success generate!!" << endl;
}

void check_and_replace(vector<vector<int>> &univ, int n, int l, int k){
    int from_x = (l - 15 < 0)?1:l-15;
    int to_x = (l + 15 > n/2)?n/2-1:l+15;
    int from_y = (k - 15 < 0)?1:k-15;
    int to_y = (k + 15 > n)?n-1:k+15;
    univ[l][k] = 9;
    #pragma omp for
    for(int i = from_x; i < to_x;i++){
        for(int j = from_y;j < to_y;j++){
            int x = from_x - i;
            int y = from_y - j;
            if(sqrt(x*x+y*y) > 15.0)
                continue;
            #pragma omp critical
            if(univ[i][j] == 1){
                check_and_replace(univ,n,i,j);
            }
        }
    }
}

void solve_omp(vector<vector<int>> &univ, int n){
    int groups = 0;
    for(int i = 0; i < n/2;i++){
        for(int j = 0; j < n;j++){
            if(univ[i][j] == 1){
                #pragma omp parallel
                check_and_replace(univ,n,i,j);
                groups++;
            }
            #pragma omp taskwait
        }
    }
    cout << groups << endl;
}

void solve_ppl(vector<vector<int>> &univ, int n){
int groups = 0;
    parallel_for(1, n/2, [](int i){
        parallel_for(0,n, [](int j){
            if(univ[i][j] == 1){
                #pragma omp parallel
                check_and_replace_ppl(univ,n,i,j);
                groups++;
            }
            #pragma omp taskwait
        }
    }
    cout << groups << endl;
}

void check_and_replace_ppl(vector<vector<int>> &univ, int n, int l, int k){
    int from_x = (l - 15 < 0)?1:l-15;
    int to_x = (l + 15 > n/2)?n/2-1:l+15;
    int from_y = (k - 15 < 0)?1:k-15;
    int to_y = (k + 15 > n)?n-1:k+15;
    univ[l][k] = 9;
    parallel_for(from_x, to_x, [](int i){
        parallel_for(from_y,to_y, [](int j){
            int x = from_x - i;
            int y = from_y - j;
            if(sqrt(x*x+y*y) > 15.0)
                continue;
            if(univ[i][j] == 1){
                check_and_replace_ppl(univ,n,i,j);
            }
        }
    }
}

int main(){
    srand(time(NULL));
    int n;
    cin >> n;
    vector<vector<int>> univ (n/2+1,vector<int>(n,0)); 
    vector<vector<int>> p = univ; 
    generate(univ,n);
    auto t1 = chrono::high_resolution_clock::now();
    solve_omp(univ,n);
    auto t2 = chrono::high_resolution_clock::now();
    solve_ppl(univ,n);
    auto t3 = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(t2-t1).count() << " -OMP" << endl;
    cout << chrono::duration_cast<chrono::nanoseconds>(t3-t2).count() << " -PPL" << endl;
}
