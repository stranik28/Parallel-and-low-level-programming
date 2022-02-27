#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <chrono>
#include <thread>
#include <condition_variable>

typedef unsigned long long int ull;

using namespace std;

vector<vector<ull>> generateRandomGraph(int, int, bool);
vector<vector<ull>> generateIsomorphGraph(vector<vector<ull>>);
bool checkIsomorphGraphs(const vector<vector<ull>>&, const vector<vector<ull>>&);
bool checkEvenMatrixes(const vector<vector<ull>>& adjMatrix1,
                       const vector<vector<ull>>& adjMatrix2,
                       const vector<int>& perm);

mutex lock;
bool gpisTrue;

unsigned long fact(size_t n) {
    unsigned long res = 1;

    for(int i = 1; i <= n; ++i) {
        res *= i;
    }

    return res;
}


vector<vector<ull>> generateIsomorphGraph(vector<vector<ull>> adjacencyMatrix) {
    ull numOfMix = (adjacencyMatrix.size() / 2) + (rand() % (adjacencyMatrix.size() / 2));
    ull c;

    if(rand() % 2) {
        for (int i = 0; i < numOfMix; ++i) {
            c = (i + rand()) % adjacencyMatrix.size();

            for (int j = 0; j < adjacencyMatrix.size(); ++j) {
                swap(adjacencyMatrix[i][j], adjacencyMatrix[c][j]);
            }
        }
    }
    else {
        for (int j = 0; j < numOfMix; ++j) {
            c = (j + rand()) % adjacencyMatrix.size();

            for (auto & i : adjacencyMatrix) {
                swap(i[j], i[c]);
            }
        }
    }

    return adjacencyMatrix;
}


vector<vector<ull>> generateRandomGraph(int numOfVerteces, int numOfEdges, bool completeGraph) {
    vector<vector<ull>> adjacencyMatrix(numOfVerteces, vector<ull>(numOfVerteces, 0));

    if(numOfEdges > 0 || completeGraph) {
        for (int i = 0; i < numOfVerteces; ++i) {
            for (int j = i; j < numOfVerteces; ++j) {
                if (rand() % 2 || completeGraph) {
                    adjacencyMatrix[i][j] = 1;
                    numOfEdges--;
                } else
                    adjacencyMatrix[i][j] = 0;

                if (i == j)
                    adjacencyMatrix[i][j] = 0;

                if (numOfEdges <= 0 && !completeGraph)
                    break;
            }
            if (numOfEdges <= 0 && !completeGraph)
                break;
        }
    }

    return adjacencyMatrix;
}

bool checkIsomorphGraphs(const vector<vector<ull>>& adjMatrix1,
                         const vector<vector<ull>>& adjMatrix2) {
    vector<int> perm(adjMatrix1.size());
    iota(perm.begin(), perm.end(), 0);

    auto thrdsNum = thread::hardware_concurrency();
    vector<thread> thrds(thrdsNum);
    gpisTrue = false;
    for (unsigned int i = 0; i < fact(perm.size()); i += thrdsNum) {
        for (thread& thrd : thrds) {
            thrd = thread(checkEvenMatrixes,
                               ref(adjMatrix1),
                               ref(adjMatrix2),
                               ref(perm));
            if(!next_permutation(perm.begin(), perm.end()))
                break;
        }
        for (thread& thrd : thrds) {
            if(thrd.joinable()) thrd.join();
        }
        if(gpisTrue)
            return true;
    }

    return false;
}

bool checkEvenMatrixes(const vector<vector<ull>>& adjMatrix1,
                       const vector<vector<ull>>& adjMatrix2,
                       const vector<int>& perm) {
    bool f1 = true;
    for (int i = 0; i < perm.size() && f1; ++i) {
        for (int j = 0; j < perm.size(); ++j) {
            if (adjMatrix1[i][j] != adjMatrix2[perm[i]][j]) {
                f1 = false;
                break;
            }
        }
    }

    if(f1) {
        {
            unique_lock<mutex> locker(lock);
            gpisTrue = true;
        }
        return true;
    }

    bool f2 = true;
    for (int j = 0; j < perm.size() && f2; ++j) {
        for (int i = 0; i < perm.size(); ++i) {
            if (adjMatrix1[i][j] != adjMatrix2[i][perm[j]]) {
                f2 = false;
                break;
            }
        }
    }

    if(f2) {
        {
            unique_lock<mutex> locker(lock);
            gpisTrue = true;
        }
    }

    return f2;
}

int main() {
    const int maxNumOfVerteces = 8;
    const int numOfGraphGenerations = 8;

    srand(time(NULL));

    vector<vector<ull>> adjM1;
    vector<vector<ull>> adjM2;
    int maxNumOfEdges;
    int numOfEdges;
    ull time;
    for (int numOfVerteces = 2; numOfVerteces <= maxNumOfVerteces; ++numOfVerteces) {
        time = 0;
        for (int gen_i = 0; gen_i < numOfGraphGenerations; ++gen_i) {
            maxNumOfEdges = (numOfVerteces * (numOfVerteces - 1)) / 2;
            numOfEdges = rand() % maxNumOfEdges;

            adjM1 = generateRandomGraph(numOfVerteces, numOfEdges, false);
            adjM2 = generateIsomorphGraph(adjM1);

            auto start = chrono::high_resolution_clock::now();
            checkIsomorphGraphs(adjM1, adjM2);
            auto end = chrono::high_resolution_clock::now();
            time += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }
        cout << "Avg time for multi thread " << numOfVerteces << " vertices graph is: "
                  << time/numOfGraphGenerations << " nanoseconds" << '\n';
    }

    return 0;
}