#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

using namespace std;

#define m 50000000

void inc(long long &x) {
    for (int i = 0; i < m; ++i)
        x++;
}

struct DifferentCacheLine {
    long long a;
    int c[16]; // 64 �����
    long long b;

    DifferentCacheLine() : a(0), b(0) {}
};

struct CrossedCacheLine {
    long long a; // 8 ����
    int c[13]; // 52 �����
    long long b;

    CrossedCacheLine() : a(0), b(0) {}
};

struct SameCacheLine {
    long long a;
    long long b;

    SameCacheLine() : a(0), b(0) {}
};

int main() {
    SameCacheLine same;
    DifferentCacheLine different;

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    vector<thread> v;
    v.push_back(thread(inc, ref(same.a)));
    v.push_back(thread(inc, ref(same.b)));
    v[0].join();
    v[1].join();
    end = std::chrono::system_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    cout << "same cache line: works for " << ms << " milliseconds\n";


    start = std::chrono::system_clock::now();
    v.resize(0);
    v.push_back(thread(inc, ref(different.a)));
    v.push_back(thread(inc, ref(different.b)));
    v[0].join();
    v[1].join();
    end = std::chrono::system_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    cout << "different cache lines: works for " << ms << " milliseconds\n";

    return 0;
}
