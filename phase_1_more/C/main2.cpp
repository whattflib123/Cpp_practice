#include <iostream>

class Logger{
    private:
        int log_count;
    
    public:
        Logger(){
            log_count = 0;
            std::cout << "=== log start ===" << std::endl;
        }
        ~Logger(){
            std::cout << "=== log end, total:"  << log_count << " ===" << std::endl; 
        }
        void log(){
            log_count = log_count + 1;
        }
};


int main() {
    {
        Logger lg;          // 印 "=== log start ==="
        lg.log();
        lg.log();
        lg.log();
    }   // lg 離開 scope → 印 "=== log end, total: 3 ==="

    std::cout << "main done" << std::endl;
    return 0;
}