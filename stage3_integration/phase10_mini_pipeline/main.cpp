#include <iostream>
#include <cstdlib>   // malloc, free
#include <span>
#include <vector>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <queue>


struct Detection {
    float x1, y1, x2, y2, confidence;
};


class DmaBuffer {
    void* ptr;
    size_t size;
public:
    explicit DmaBuffer(size_t n)
        : ptr(malloc(n)), size(n) {
        // TODO: 印 "[DmaBuffer] alloc N bytes"
        std::cout << "[DmaBuffer] alloc " << n << " bytes\n";
    }

    ~DmaBuffer() {
        // TODO: 印 "[DmaBuffer] free"
        std::cout << "[DmaBuffer] free\n";
        free(ptr);
    }

    // copy 建構子
    DmaBuffer(const DmaBuffer&) = delete;
    DmaBuffer& operator=(const DmaBuffer&) = delete;

    DmaBuffer(DmaBuffer&& o) noexcept
        : ptr(o.ptr), size(o.size) {
        o.ptr = nullptr;
        o.size = 0;
        // TODO: 印 "[DmaBuffer] move"
        std::cout << "[DmaBuffer] move\n";

    }

    DmaBuffer& operator=(DmaBuffer&& o) noexcept {
        if (this != &o) {
            free(ptr);
            ptr = o.ptr; size = o.size;
            o.ptr = nullptr; o.size = 0;
        }
        return *this;
    }

    void* data() const { return ptr; }
    size_t bytes() const { return size; }
};


class Frame {
    std::unique_ptr<char[]> buf;
    int id, size;
public:
    Frame(int id, int size)
        : buf(std::make_unique<char[]>(size)), id(id), size(size) {}
    int get_id() const { return id; }

    char* data() const { return buf.get(); }
    int bytes() const { return size; }

};

class FrameQueue {
    std::queue<Frame> frames;
    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
public:
    void push(Frame&& f) { 
        {
            std::lock_guard<std::mutex> g(mtx);
            frames.push(std::move(f));
        }     
        cv.notify_one(); 
    }
    std::optional<Frame> pop() { 
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [this] { return !frames.empty() || done; });
        if (frames.empty() && done) return std::nullopt;
        Frame f = std::move(frames.front());
        frames.pop();
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



class InferenceEngine {
public:
    // 借用 frame 的 pixel data，不複製
    std::vector<Detection> run(std::span<float> pixels) {
        std::cout << "[Inference] processing " << pixels.size() << " floats\n";
        // 回傳 2 個假 Detection
        return { {0.1f,0.2f,0.5f,0.8f,0.9f}, {0.3f,0.1f,0.7f,0.6f,0.75f} };
    }
};



int main() {
    FrameQueue q;
    InferenceEngine engine;

    std::thread producer([&] {
        for (int i = 0; i < 5; i++) {
            q.push(Frame(i, 1228800));
            std::cout << "pushed frame " << i << '\n';
        }
        q.set_done();
    });

    std::thread consumer([&] {
        while (true) {
            auto f = q.pop();
            if (!f) break;
            auto results = engine.run(std::span<float>(
                reinterpret_cast<float*>(f->data()),
                f->bytes() / sizeof(float)
            ));
            std::cout << "frame " << f->get_id()
                    << ": " << results.size() << " detections\n";
        }
    });

    producer.join();
    consumer.join();
}
