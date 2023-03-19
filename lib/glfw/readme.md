# GLFW
## 現行バージョン
3.3.8

## ダウンロード先
### リポジトリ
https://github.com/glfw/glfw

### 最新バージョン
https://github.com/glfw/glfw/releases/latest

### バイナリ
https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip

## CMakeに書くこと
```
# ここは事前に宣言してあれば宣言不要
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE STRING "lib folder")

set(LIB_GLFW_DIR ${LIB_DIR}/glfw CACHE STRING "glfw")
include_directories(
    ${LIB_GLFW_DIR}/include
)
link_directories(
    ${LIB_GLFW_DIR}/lib
)

# glfwのリンクの設定
target_link_libraries(プロジェクト名 PUBLIC libglfw3dll.a)
target_link_libraries(プロジェクト名 PUBLIC glfw3.dll)
```


## 想定しているファイル構成
