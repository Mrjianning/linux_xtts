### README

# 讯飞高品质版语音合成能力

## 项目简介

本项目实现了使用讯飞高品质版语音合成能力的示例。通过该示例，您可以将文本转换为高品质的语音输出，并保存为 WAV 文件。

## 运行教程

### 第一步：修改 `config.json` 中的 key

在运行项目之前，请确保在 `config.json` 文件中填入正确的 API Key 和其他配置信息。以下是 `config.json` 文件的示例格式：

```json
{
    "appID": "YOUR_APP_ID",
    "apiKey": "YOUR_API_KEY",
    "apiSecret": "YOUR_API_SECRET",
    "text": "你好，世界！",
    "vcn": "xiaoyan",
    "vcnModel": "xiaoyan",
    "language": 1,
    "speed": 50,
    "pitch": 50,
    "volume": 100,
    "textEncoding": "UTF-8",
    "sample_rate": 16000,
    "bits_per_sample": 16,
    "channels": 1
}
```

请将 `"YOUR_APP_ID"`、`"YOUR_API_KEY"` 和 `"YOUR_API_SECRET"` 替换为您从讯飞平台获得的实际值。

### 第二步：建立 build 目录并编译项目

1. 打开终端并导航到项目根目录。
2. 创建 `build` 目录：

```sh
mkdir build
```

3. 进入 `build` 目录：

```sh
cd build
```

4. 使用 CMake 配置项目：

```sh
cmake ..
```

5. 编译项目：

```sh
make
```

### 第三步：在 workspace 目录下运行可执行文件

1. 编译成功后，生成的可执行文件将位于 `workspace` 目录中。

2. 在 `workspace` 目录下运行：

```sh
cd ../workspace
```

3. 运行可执行文件：

```sh
./aikit_test
```

**注意**：将 `YourExecutableName` 替换为实际生成的可执行文件的名称。

运行后，程序将读取 `config.json` 中的配置信息，执行语音合成，并生成 `output.wav` 文件。