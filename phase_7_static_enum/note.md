好,進下一個主題。在你之前的清單裡,我建議 **enum class**——理由是它跟你已經很熟的東西(class、運算子多載裡的 `==`)銜接很快,而且馬上能接回你的職涯場景:你碩論裡有「閉集/開集測試」「USM/SDU 資料集」這種「一組固定選項」的概念,enum class 正是用來表達這種東西的標準工具。

---

## Enum Class:表達「一組固定選項」的安全方式

**先看問題:沒有 enum 之前,大家怎麼表達「一個東西只能是幾個固定狀態之一」?**

最原始的爛做法是用數字代表狀態:

```cpp
int status = 0;   // 0 = 等待中, 1 = 執行中, 2 = 完成, 3 = 失敗
```

問題很明顯,而且你應該很熟——這跟你之前學 `optional` 時學過的「magic number 很危險」是同一個道理:

- `0`、`1`、`2`、`3` 本身**沒有任何意義**,讀程式碼的人完全看不出 `status == 2` 是什麼意思,要去翻註解
- 容易打錯:`status = 5`(根本沒有 5 這個狀態)編譯器**不會**抓出來,因為 int 本來就能裝任何數字
- 不同地方可能用不同的數字對應(這個檔案 0=等待,那個檔案 0=完成),沒有強制力

**`enum class` 解決這個:給「一組固定選項」一個正式的型別,而不是借用 int。**

```cpp
enum class Status {
    Waiting,
    Running,
    Done,
    Failed
};

Status s = Status::Running;     // 必須寫 Status::,清楚知道這是哪個列舉的選項

if (s == Status::Done) {
    std::cout << "完成了";
}
```

逐塊看:

- `enum class Status { ... }` —— 定義一個叫 `Status` 的型別,它的值只能是 `{}` 裡列出的那幾個選項。
- `Status::Waiting`、`Status::Running` 等 —— 每個選項都要寫 `Status::` 開頭,**不能單獨寫 `Running`**——這是 `enum class`(相對舊式 `enum`)的關鍵改進,避免不同 enum 裡選項名字撞在一起時搞混。
- `Status s = Status::Running;` —— 宣告一個 `Status` 型別的變數,只能裝這四個選項之一,**不能塞別的東西進去**:

```cpp
Status s = 5;              // ❌ 編譯器直接擋下!5 不是 Status 的合法值
Status s = Status::Done;   // ✓ 只能用列舉裡定義的選項
```

對比 `int status = 5`(不會報錯但邏輯上是亂的),**`enum class` 讓編譯器替你守住「這個變數只能是這幾個合法狀態之一」**——這正是你之前學 `optional`、`nullptr` 時反覆看到的精神:**用型別本身去防止錯誤,而不是靠人記得「不要打錯數字」**。

**搭配你已經會的 `switch`(如果你還沒正式學過,這裡順便補一下語法):**

```cpp
switch (s) {
    case Status::Waiting:
        std::cout << "還在等待";
        break;
    case Status::Running:
        std::cout << "執行中";
        break;
    case Status::Done:
        std::cout << "完成";
        break;
    case Status::Failed:
        std::cout << "失敗";
        break;
}
```

`switch` 是「多選一」的判斷,比一連串 `if/else if` 更清楚。每個 `case` 對應一個可能的值,`break` 代表「這個分支結束,不要往下繼續執行」(忘記寫 `break` 是經典地雷,它會「掉落」執行到下一個 case,但這裡先不深究)。`enum class` 跟 `switch` 是天生一對——因為 enum 的選項是固定、有限的,`switch` 正好適合處理「列舉所有可能,各自怎麼辦」。

**對你職涯的實際用途:** 你碩論裡的「Closed-set / Open-set」「USM / SDU 資料集」「訓練策略 A/B/C」,這些都是「一組固定選項」的典型場景,實務寫程式碼時就會宣告:

```cpp
enum class Dataset { USM, SDU };
enum class TestProtocol { Closed, Open };
```

比起用字串(`"USM"`,容易打錯字、效能也較差)或數字(沒有意義),`enum class` 是更安全、更清楚、執行效率也更好的選擇——這也是面試官評估你「寫不寫得出乾淨的 C++」時會注意的細節。

---

驗收題,把這個套進一個小情境:

**定義一個 `enum class Direction { North, South, East, West };`,寫一個函式 `printDirection`,用 `switch` 印出對應的中文(北/南/東/西)。**

```cpp
#include <iostream>

enum class Direction { North, South, East, West };

void printDirection(Direction d) {
    // TODO: 用 switch,每個 case 印出對應中文,別忘了 break
}

int main() {
    printDirection(Direction::North);   // 北
    printDirection(Direction::East);    // 東
    return 0;
}
```

把 TODO 填出來。提示:`switch (d) { case Direction::North: std::cout << "北"; break; ... }`,四個 case 都要寫,每個都要 `break`。試試看。