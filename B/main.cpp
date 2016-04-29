#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

using namespace std;

#define m 100000000

void change(long long &x) {
    for (int i = 0; i < m; ++i)
        x++;
}

struct DifferentCacheLine {
    long long a;
    int c[16]; // 64 байта
    long long b;

    DifferentCacheLine() : a(0), b(0) {}
};

struct SameCacheLine {
    long long a;
    long long b;

    SameCacheLine() : a(0), b(0) {}
};

void changeVariables (long long &a, long long &b) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    vector<thread> v;
    v.push_back(thread(change, ref(a)));
    v.push_back(thread(change, ref(b)));
    v[0].join();
    v[1].join();
    end = std::chrono::system_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    cout << "cache line works for " << ms << " milliseconds\n";

}

int main() {
    cout << "hardware_concurrency: " << thread::hardware_concurrency() << "\n";
    cout << "Processor: Intel Pentium 2117U\n";
    cout << "Cache L1: 128 KB\n";
    cout << "Cache Line: 64 bites\n\n";

    SameCacheLine same;
    DifferentCacheLine different;

    changeVariables(same.a, same.b);
    changeVariables(different.a, different.b);

    return 0;
}
