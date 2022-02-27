#include <iostream>

using namespace std;

void threadCalculateString (double **arr, int size, int start, int end, boost::barrier& cur_barier, boost::barrier& phase1) {
    double m;
    for (int k = 1; k < size; ++k) {
        for (int j = k; j < end; ++j) {
            if ((j >= start) && (j < end)) {
                m = arr[j][k - 1] / arr[k - 1][k - 1];
                for (int i = 0; i < size + 1; ++i)
                    arr[j][i] = arr[j][i] - m * arr[k - 1][i];
            }
        }
        cur_barier.wait();
    }

    phase1.wait();

    for (int i = size - 2; i >= 0; i--) {
        if ((i >= start) && (i < end)) {
            for (int j = i + 1; j < size; j++)
                arr[i][size] -= arr[i][j] * arr[j][size];
            arr[i][size] /= arr[i][i];
        }
        cur_barier.wait();
    }

}

vector<double> getAnswers (double **arraySlau, int size, int MAX_THREAD) {
    cout << "Метод Гауса - с использованием thread:" << endl;
    auto start_t = std::chrono::high_resolution_clock::now();
    vector<double> _answers;
    vector<thread> threads;
    boost::barrier bar(MAX_THREAD), phase1(MAX_THREAD);

    double **arr = new double*[size];
    for (int i = 0; i < size; i++) {
        arr[i] = new double[size + 1];
        for (int j = 0; j < size + 1; j++) {
            arr[i][j] = arraySlau[i][j];
        }
    }

    int temp = size / (MAX_THREAD - 1);             //для определения интервалов для потоков

    for (int i = 0; i < (MAX_THREAD - 1); ++i) {
        int start = temp * i, end = temp * (i + 1);
        threads.emplace_back (threadCalculateString, ref(arr), size, start, end, boost::ref(bar), boost::ref(phase1));
    }

    int start, end;

    if (size % MAX_THREAD - 1 != 0)
    {
        double m;
        start = temp * (MAX_THREAD - 1);
        end = size;
        for (int k = 1; k < size; ++k)
        {

            for (int j = k; j < end; ++j)
            {
                if ((j >= start) && (j < end))
                {
                    m = arr[j][k - 1] / arr[k - 1][k - 1];
                    for (int i = 0; i < size + 1; ++i) {
                        arr[j][i] = arr[j][i] - m * arr[k - 1][i];
                    }
                }
            }
            bar.wait();
        }
    }

    arr[size - 1][size] /= arr[size - 1][size - 1];

    phase1.wait();

    if (size % MAX_THREAD - 1 != 0)
    {
        start = temp * (MAX_THREAD - 1);
        end = size;

        for (int i = size - 2; i >= 0; i--) {
            if ((i >= start) && (i < end)) {
                for (int j = i + 1; j < size; j++)
                    arr[i][size] -= arr[i][j] * arr[j][size];
                arr[i][size] /= arr[i][i];
            }
            bar.wait();
        }
    }

    for (auto& thread : threads) {
        if (thread.joinable()){
            thread.join();
        }
    }

    for (int i = 0; i < size; ++i)
        _answers.push_back(arr[i][size]);

    auto end_t = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
    cout << "Время = " << time << "мс." << endl;

    for (int i = 0; i < size; ++i)
        delete[] arr[i];
    delete[] arr;

    return _answers;
}

int main(){

}