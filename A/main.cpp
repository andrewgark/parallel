#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <vector>
#include <atomic>
#include <assert.h>

using namespace std;

#define N 4
#define M 100000000

class futex {
    atomic<int> id;

public:
    futex() : id(0) {}

    void lock() {
        int x = pthread_self();//this_thread::get_id();
        //int x = this_thread::get_id();
        int y = 0;
        while (!id.compare_exchange_weak(y, x)) {
            this_thread::yield();
            y = 0;
        }
    }

    void unlock() {
        int x = pthread_self();
        assert(x == id);
        id = 0;
    }
};

class acquireFutex {
    atomic<int> id;

public:
    acquireFutex() : id(0) {}

    void lock() {
        int x = pthread_self();//this_thread::get_id();
        //int x = this_thread::get_id();
        int y = 0;
        while (!id.compare_exchange_weak(y, x, memory_order_acquire)) {
            this_thread::yield();
            y = 0;
        }
    }

    void unlock() {
        int x = pthread_self();
        assert(x == id);
        id.store(0, memory_order_release);
    }
};

class relaxedFutex {
    atomic<int> id;

public:
    relaxedFutex() : id(0) {}

    void lock() {
        int x = pthread_self();//this_thread::get_id();
        //int x = this_thread::get_id();
        int y = 0;
        while (!id.compare_exchange_weak(y, x, memory_order_relaxed)) {
            this_thread::yield();
            y = 0;
        }
    }

    void unlock() {
        int x = pthread_self();
        assert(x == id);
        id.store(0, memory_order_relaxed);
    }
};

template<typename T>
void change(long long &a, T &mutex) {
    for (int i = 0; i < M; ++i) {
        mutex.lock();
        a++;
        mutex.unlock();
    }
    return;
}


template<typename T>
void changeVolatile(volatile long long &a, T &mutex) {
    for (int i = 0; i < M; ++i) {
        mutex.lock();
        a++;
        mutex.unlock();
    }
    return;
}


template<typename T>
void testMutex(T &mutex) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<thread> v;
    long long a = 0;
    for (int i = 0; i < N; ++i)
        v.push_back(thread(change<T>, ref(a), ref(mutex)));
    for (int i = 0; i < N; ++i)
        v[i].join();

    end = std::chrono::system_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    cout << "mutex works for " << ms << " milliseconds\n";
}


template<typename T>
void testVolatileMutex(T &mutex) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<thread> v;
    volatile long long a = 0;
    for (int i = 0; i < N; ++i)
        v.push_back(thread(changeVolatile<T>, ref(a), ref(mutex)));
    for (int i = 0; i < N; ++i)
        v[i].join();

    end = std::chrono::system_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    cout << "mutex works for " << ms << " milliseconds\n";
}

int main()
{
    futex f;
    acquireFutex af;
    relaxedFutex rf;

    testMutex(f);
    testMutex(af);
    testVolatileMutex(f);
    testVolatileMutex(af);
    testVolatileMutex(rf);
    return 0;
}
