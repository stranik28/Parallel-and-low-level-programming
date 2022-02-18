#include <iostream>
#include <thread>
#include <bits/stdc++.h>

using namespace std;
int a = 1000;

void matrix(vector<vector<int>> c, vector<vector<int>> b, vector<vector<int>> & res,int start, int end){
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < a; ++j) {
            for(int k = 0; k < a; k++)
                res[i][j] = c[i][k] * b[k][j];
        }
    }
}

int main() {
    vector<vector<int>> vec1(a, vector<int>(a, 1));
    vector<vector<int>> vec2(a,vector<int>(a, 2));
    vector<vector<int>> res1(a,vector<int>(a));
    vector<vector<int>> res2(a,vector<int>(a));
    int ih  = 0;
    for (auto &p : vec1) {
        for(auto &f: p){
            f = ++ih;
        }
    }
    ih  = 0;
    for (auto &p : vec2) {
        for(auto &f: p){
            f = ++ih;
        }
    }
    auto t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < a; ++i) {
        for (int j = 0; j < a; ++j) {
            for(int k = 0; k < a; k++)
                res1[i][j] = vec1[i][k] * vec2[k][j];
        }
    }
    auto t2 = chrono::high_resolution_clock::now();
    thread first(matrix,vec1,vec2,ref(res2),0,a/4);
    thread second(matrix,vec1,vec2,ref(res2),a/4,a/2);
    thread third(matrix,vec1,vec2,ref(res2),a/2,3*a/4);
    thread four(matrix,vec1,vec2, ref(res2),3*a/4,a);
    first.join();
    second.join();
    third.join();
    four.join();
    auto t3 = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::milliseconds>(t2-t1).count()<< " -singe core \n"
    cout << chrono::duration_cast<chrono::milliseconds>(t3-t2).count() << " -multi core";
}
