#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <optional>



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
        cv.notify_one();   // 通知等待中的 consumer：aka pop
    }

    std::optional<Frame> pop(){

        std::unique_lock<std::mutex> lk(mtx);  // unique_lock，cv 需要
        // cv.wait(lk, [this] { return !frames.empty(); });  // 等到非空
        cv.wait(lk, [this] { return !frames.empty() || done; });


        // 對應 set_done 的邏輯：
        if (frames.empty() && done) return std::nullopt;

        // 實際的 consumer 邏輯： pop
        Frame f = std::move(frames.back());
        frames.pop_back();

        return f;
    }

    void set_done() {
        {
            std::lock_guard<std::mutex> g(mtx);
            done = true;
        }
        cv.notify_all();  // 喚醒 consumer，讓它重新檢查條件
    }

    };



// std::mutex mtx;

// int counter = 0;

// void increment() {
//     std::cout << "thread id: " << std::this_thread::get_id() << '\n';
//     for (int i = 0; i < 10000; i++) {
//         counter++;
//     }
    
// }

int main() {
    // std::thread t1(increment);
    // std::thread t2(increment);
    // t1.join();
    // t2.join();
    // std::cout << "counter = " << counter << '\n';


    // ThreadSafeQueue q;
    // q.push(Frame(1, 64));
    // q.push(Frame(2, 64));
    // q.push(Frame(3, 64));

    // Frame f = q.pop();
    // std::cout << "popped id=" << f.get_id() << '\n';

    ThreadSafeQueue q;

    std::thread producer([&] {
        for (int i = 0; i < 5; i++) {
            q.push(Frame(i, 64));
            std::cout << "pushed " << i << '\n';
        }
        q.set_done();
    });
    


    std::thread consumer([&] {
        // for (int i = 0; i < 3; i++) {
        //     Frame f = q.pop();
        //     std::cout << "popped " << f.get_id() << '\n';
        // }
        while (true) {
            auto f = q.pop();
            if (!f) break;          // nullopt → 結束
            std::cout << "popped " << f->get_id() << '\n';
        }

    });

    producer.join();
    consumer.join();



}
