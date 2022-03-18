#include <iostream>
#include <stdlib.h>
#include <vector>
#include <limits.h>
#include <thread>
#include <bits/stdc++.h>

using namespace std;

const int SIZE = 10000;

void deicstra(int vertex,vector<vector<int>> &graph,vector<int> &graph_with_weights){
    int weight = graph_with_weights[vertex];
    int a = graph[vertex][0];
    int b = graph[vertex][1];
    if(graph_with_weights[a] > ++weight){
        graph_with_weights[a] = weight;
        deicstra(a,graph,graph_with_weights);
    }
    if(graph_with_weights[b] > weight){
        graph_with_weights[b] = weight;
        deicstra(b,graph,graph_with_weights);
    }
    return;
}

int main(){
    srand(time(0));
    int n,m,vertex1,vertex2;
    n = SIZE;
    m = 2*n;
    cout << "Введите вершины для которых ищется растояние\n";
    cin >> vertex1 >> vertex2;
    vector<vector<int>> graph (n,vector<int>(2,0));
    vector<int> graph_with_weights (n, INT_MAX);
    vector<vector<int>> graph1 (n,vector<int>(2,0));
    vector<int> graph_with_weights1 (n, INT_MAX);
    graph_with_weights[0] = 0;
    for(int i = 0; i < n; i++){
        int a,b;
        a = rand()%n;
        b = rand()%n;
        graph[i][0] = a;
        graph[i][1] = b;
        graph1[i][0] = a;
        graph1[i][1] = b;
    }
    swap(graph[0],graph[vertex1]);
    swap(graph1[0],graph1[vertex1]);
    // for multithread
    auto t1 = chrono::high_resolution_clock::now();
    int h1,h2;
    h1 = graph[0][0];
    h2 = graph[0][1];
    graph_with_weights[h1] = 1;
    graph_with_weights[h2] = 1;
    thread first(deicstra,h1,ref(graph),ref(graph_with_weights));
    thread second(deicstra,h2,ref(graph),ref(graph_with_weights));
    first.join();
    second.join();
    auto t2 = chrono::high_resolution_clock::now();
    //for singlethread
    auto t3 = chrono::high_resolution_clock::now();
    deicstra(0,graph1,graph_with_weights1);
    auto t4 = chrono::high_resolution_clock::now();
    if(vertex1 == vertex2){
        cout << 0 << endl;
    }
    else if(graph_with_weights[vertex2] == INT_MAX){
        cout << "Нет пути" << endl;
    }
    else{
        cout << "Кратчайший путь: " << graph_with_weights[vertex2] << endl;
    }
    cout << chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<< " -multi core \n";
    cout << chrono::duration_cast<chrono::nanoseconds>(t4-t3).count() << " -single core";
    cout << endl;
}
