CXX      = g++
CXXFLAGS = -std=c++20 -Wall -I./include
TARGET   = app

all: $(TARGET)

$(TARGET): src/math_utils.cpp main.cpp
# 依賴宣告——告訴 make「要建出 app，需要這兩個檔案存在」。
#   作用：
#   - 這兩個檔案任何一個比 app 新（被修改過）→ 重新編譯
#   - 兩個都沒變 → make 說「無需做任何事」，跳過

	$(CXX) $(CXXFLAGS) src/math_utils.cpp main.cpp -o $(TARGET)
# 等同於 g++ -std=c++20 -Wall -I./include src/math_utils.cpp main.cpp -o app

clean:
	rm -f $(TARGET)