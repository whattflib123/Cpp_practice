#include <iostream>
#include <algorithm>  // std::fill

void dump(const unsigned char* buf, int width, int height, int stride) {
    // TODO: 印出整塊 buf（含 padding）
    // 外圈 y = 0..height-1，內圈 x = 0..stride-1
    // 有效像素印數字，padding（x >= width）印 "FF"
    // 格式不限，能看出哪裡是 padding 就好
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < stride; x++)
        {
            if (x >= width) {
                std::cout << "FF ";
            }
            else std::cout << (int)buf[y*stride + x] << " ";
            
        }
        std::cout << std::endl;
        
    }
    std::cout << "dump sizeof(buf) = " << sizeof(buf) << '\n';

    
}

double average(const unsigned char* buf, int width, int height, int stride) {
    // TODO: 只加有效像素（x < width），回傳平均
    // 誤用 stride 當內圈上限 → 平均被 0xFF 拉高
    double sum = 0.0f;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            sum = buf[i*stride + j] + sum;
        }
    }
    sum = sum / (width * height);
    return sum;
}

int main() {
    int width = 6, height = 4, stride = 8;

    double sum = 0.0f;

    // TODO 1: new 配置 stride * height bytes，填 0xFF
    unsigned char* buf = new unsigned char[stride * height];
    // TODO 2: pointer arithmetic 填有效像素，值 = y * 10 + x
    std::fill(buf, buf + stride * height, 0xFF);  // 整塊哨兵
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            buf[i*stride + j] = 10 * i + j;
        }
        
    }
    
    // TODO 3: 印 sizeof(buf)（這裡）

    dump(buf, width, height, stride);

    // TODO 4: 印 average 結果
    sum = average(buf, width, height, stride);
    std::cout << sum << '\n';

    std::cout << "main sizeof(buf) = " << sizeof(buf) << '\n';


    // TODO 5: 故意存取第 height 行 → 看 ASan 報什麼 → 再註解掉
    // buf[height * stride] = 0;

    // TODO 6: delete[]
    delete[] buf; 

    return 0;
}