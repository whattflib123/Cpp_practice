#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <optional>
#include <memory>

class Frame {
    std::unique_ptr<char[]> buf;
    int id, size;
public:
    Frame(int id, int size)
        : buf(std::make_unique<char[]>(size)), id(id), size(size) {}
    int get_id() const { return id; }
};

class ThreadSafeQueue {
    std::vector<Frame> frames;
    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
public:
    ThreadSafeQueue() { frames.reserve(16); }

    void push(Frame&& f) {
        {
            std::lock_guard<std::mutex> g(mtx);
            frames.push_back(std::move(f));
        }
        cv.notify_one();
    }

    std::optional<Frame> pop() {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [this] { return !frames.empty() || done; });
        if (frames.empty() && done) return std::nullopt;
        Frame f = std::move(frames.back());
        frames.pop_back();
        return f;
    }

    void set_done() {
        {
            std::lock_guard<std::mutex> g(mtx);
            done = true;
        }
        cv.notify_all();
    }
};

int main() {
    ThreadSafeQueue q;

    std::thread producer([&] {
        for (int i = 0; i < 5; i++) {
            q.push(Frame(i, 64));
            std::cout << "pushed " << i << '\n';
        }
        q.set_done();
    });

    std::thread consumer([&] {
        while (true) {
            auto f = q.pop();
            if (!f) break;
            std::cout << "popped " << f->get_id() << '\n';
        }
    });

    producer.join();
    consumer.join();
}
