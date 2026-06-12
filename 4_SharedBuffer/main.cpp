#include <iostream>

class SharedBuffer {
private:
    int* data;        // 共享的資料
    int* ref_count;   // 共享的計數器(在 heap 上)

public:
    // 建構子:第一個主人
    SharedBuffer(int value) {
        data = new int(value);
        // TODO 1: 配置 ref_count,初始值設為 1
        //   提示:跟上面 data 同樣的句型,值是 1
        ref_count = new int(1);
    }

    // Copy 建構子:有人複製我的時候會自動呼叫這個
    //   參數 const SharedBuffer& other = 「被複製的那個物件」
    SharedBuffer(const SharedBuffer& other) {
        // TODO 2: 
        //   (a) 把 other 的 data 指標抄過來(指向同一塊)
        //   (b) 把 other 的 ref_count 指標抄過來(指向同一個計數器)
        //   (c) 計數 +1   ← 提示:要改的是「計數器裡的值」,不是指標本身
        //                    用 *ref_count 去存取那個值

        // 這裡我大卡特卡 完全寫不出來
        data = other.data;
        ref_count = other.ref_count;

        // *ref_count = ref_count + 1; // 錯
        //           ^^^^^^^^^ 這是地址(像 0x7ffe…),加 1 變成隔壁地址,
        //           然後把這個亂七八糟的地址值塞進計數器 → 計數完全壞掉
        *ref_count = *ref_count + 1;        // copy:右邊應該是 *ref_count

    }

    // 解構子
    ~SharedBuffer() {
        // TODO 3:
        //   (a) 計數 -1   (一樣用 *ref_count 改值)
        //   (b) if 計數變成 0:
        //          delete data;
        //          delete ref_count;
        //          印 "buffer freed"
        //       else:
        //          印 "ref count now: X"  (X 是現在的計數)

        // *ref_count = ref_count - 1; // 錯
        *ref_count = *ref_count - 1;        // 解構:右邊應該是 *ref_count

        //if (ref_count == 0){ 
        //  ^^^^^^^^^ 在問「這根指標的地址是不是 0(nullptr)」
        //  但你想問的是「計數器裡的值是不是 0」→ 完全問錯問題
        //  ref_count 永遠不是 0(它是有效地址),所以這個 if 永遠不成立 → 永遠不會釋放 → 記憶體洩漏
        if (*ref_count == 0){
            delete data;
            delete ref_count;
            std::cout << "buffer freed" << std::endl;
        }
        else{
            // std::cout << "ref count now: " << ref_count << std::endl; // 錯
            std::cout << "ref count now: " << *ref_count << std::endl;
        }
    }

    // 回傳目前計數
    int count() {
        // TODO 4: 回傳計數器裡的值(不是指標,是值)

        // return ref_count; // 錯
        return *ref_count;
    }
};

int main() {
    SharedBuffer a(42);
    std::cout << a.count() << "\n";
    {
        SharedBuffer b = a;
        std::cout << a.count() << "\n";
    }
    std::cout << a.count() << "\n";
    return 0;
}