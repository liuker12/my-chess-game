# my-chess-game
# ♟️ C++ 象棋游戏 (Chess Game)

这是一个基于 **C++** 开发的象棋游戏项目。项目使用 **SFML (Simple and Fast Multimedia Library)** 处理图形界面与音效，并集成了强大的 **Stockfish** 象棋引擎，支持人机对弈功能。

---

## 🚀 功能特点

* **图形界面**：基于 SFML 实现的棋盘渲染与交互。
* **AI 对弈**：集成 Stockfish 引擎，提供高水平的电脑对手。
* **音效支持**：包含落子、吃子等基础音效。
* **项目结构清晰**：专为 Visual Studio 2022 开发环境配置。

---

## 🛠️ 如何在本地运行


### 1. 环境准备
* 安装 **Visual Studio 2022**（确保勾选了“使用 C++ 的桌面开发”）。
* 本项目依赖 **SFML 2.6.x**。

2. 配置运行环境
使用 Visual Studio 打开 test.sln。

关键步骤：确保项目根目录（或 Debug/Release 输出目录）下包含以下文件：

所有 sfml-*.dll 文件。

openal32.dll。

stockfish.exe (AI 引擎)。

images/ 文件夹（存放素材）。

点击 本地 Windows 调试器 编译并运行。
