#include <iostream>

class RequestTracker{

    private:
        int* counter;

    public:
        RequestTracker(){
            std::cout << "Resuest started" << std::endl;
            counter = new int();
        }
        ~RequestTracker(){
            if (counter != nullptr){
                std::cout << "Request finished, total logs = "<< *counter << std::endl;
            }
            
        }

        void log(){
            *counter = *counter + 1;
        }

        int count(){
            return *counter;
        }

};


int main()
{
    RequestTracker tracker;

    tracker.log();
    tracker.log();
    tracker.log();

    std::cout << tracker.count() << '\n';
}