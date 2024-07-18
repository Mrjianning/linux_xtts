#include "XTTSClient.hpp"
#include <fstream>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

int main() {
    try {
        // 构建json
        std::ifstream i("config.json");
        json config;
        i >> config;

        // 开始计时
        auto start = high_resolution_clock::now();

        // 语言合成
        XTTSClient client;
        client.createXTTS(config);

         // 结束计时
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        // 打印执行时间
        std::cout << "===========================Execution time: " << duration.count() << " milliseconds=============================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; // 返回非零值表示错误
    }
    return 0; // 成功执行
}
