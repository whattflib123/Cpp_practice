#include <iostream>
#include <memory>
#include <vector>
#include <span>

class Frame {
    std::unique_ptr<char[]> buf;
    int id;
    int size;
public:
    Frame(int id, int size)
        : buf(std::make_unique<char[]>(size)), id(id), size(size) {
        std::cout << "alloc #" << id << '\n';
    } // 用 `make_unique` 不需要寫 dtor 也不會 leak
    int get_id()   const { return id; }
    int get_size() const { return size; }
    const char* data() const { return buf.get(); }
};


class FrameQueue {
    std::vector<Frame> frames;
public:
    FrameQueue() { frames.reserve(16); }

    void push(Frame&& f) {
        frames.push_back(std::move(f));
    }

    Frame pop() {
        // 提示：取 frames.back()，move 出來，再 pop_back()
        // 回傳那個 Frame
        Frame pop_frame = std::move(frames.back());
        frames.pop_back();

        return pop_frame;
    }

    bool empty() const { return frames.empty(); }
    size_t size()  const { return frames.size(); }
};


void process(std::span<const char> data) {
    std::cout << "processing " << data.size() << " bytes\n";
}


int main(){

    // 8-1 testing unique_ptr
    // Frame a(1, 64);
    // Frame b = std::move(a);
    // std::cout << "b id=" << b.get_id() << '\n';

    // // 8-2
    // FrameQueue q;
    // q.push(Frame(1, 64));
    // q.push(Frame(2, 64));
    // q.push(Frame(3, 64));

    // Frame f = q.pop();
    // std::cout << "popped id=" << f.get_id() << '\n';

    // // 8-3 
    // process(std::span<const char>(f.data(), f.get_size()));

    // 8-4 完整實做
    // capture
    FrameQueue q;

    for (int i = 0; i < 3; i++) {
        Frame f(i, 64);
        q.push(std::move(f));
    }

    // inference
    while (!q.empty()) {
        Frame f = q.pop();
        process(std::span<const char>(f.data(), f.get_size()));
    }



    return 0;
}

