#include <bits/stdc++.h>

using namespace std;

const int sz = 1000;

void multiply(vector<int>a,vector<int>b,vector<int>&c,int start,int end){
    for (int i = start; i < end; ++i) {
        c[i] = a[i]*b[i];
    }
}

int main() {
    vector<int> vector1(sz,3);
    vector<int> vector2(sz,9);
    vector<int> vector3(sz);
    auto t1 = chrono::high_resolution_clock::now();
    thread first(multiply,vector1,vector2,ref(vector3),0,sz/4);
    thread second(multiply,vector1,vector2,ref(vector3),sz/4,sz/2);
    thread third(multiply,vector1,vector2,ref(vector3),sz/2,3*sz/4);
    thread four(multiply,vector1,vector2,ref(vector3),3*sz/4,sz);
    first.join();
    second.join();
    third.join();
    four.join();
    auto t2 = chrono::high_resolution_clock::now();
    multiply(vector1,vector2,vector3,0,sz);
    auto t3 = chrono::high_resolution_clock::now();
    cout << endl << chrono::duration_cast<chrono::nanoseconds>(t2-t1).count() << " multicore mode" <<  endl;
    cout << endl << chrono::duration_cast<chrono::nanoseconds>(t3-t2).count() << " single core mode" <<  endl << endl;
    return 0;
}
