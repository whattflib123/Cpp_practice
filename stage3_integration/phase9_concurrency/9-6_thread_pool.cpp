#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <queue>
#include <functional>

class ThreadPool {
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
    bool stop = false;
    std::mutex mtx;
    std::vector<std::thread> workers;

public:
    explicit ThreadPool(int n_threads) {
        for (int i = 0; i < n_threads; ++i) {
            workers.emplace_back([this] { worker_loop(); });
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> g(mtx);
            tasks.push(std::move(task));
        }
        cv.notify_one();
    }

    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lk(mtx);
                cv.wait(lk, [this] { return !tasks.empty() || stop; });
                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();   // 鎖外執行，不佔用鎖的時間
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> g(mtx);
            stop = true;
        }
        cv.notify_all();   // 全部喚醒，每個 worker 檢查 stop 後退出
        for (auto& t : workers) t.join();
    }
};

int main() {
    ThreadPool pool(3);
    for (int i = 0; i < 5; i++) {
        pool.enqueue([i] {
            std::cout << "task " << i << " on thread "
                      << std::this_thread::get_id() << '\n';
        });
    }
}
