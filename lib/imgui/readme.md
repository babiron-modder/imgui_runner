# ライブラリ名
## 現行バージョン
1.89.1

## ダウンロード先
### リポジトリ
https://github.com/ocornut/imgui

### 最新バージョン
https://github.com/ocornut/imgui/releases/latest

### バイナリ
なし

## CMakeに書くこと
```
# ここは事前に宣言してあれば宣言不要
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE STRING "lib folder")

set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/imgui CACHE STRING "imgui")
include_directories(
    ${IMGUI_DIR}/include
)
add_executable(
    ${IMGUI_DIR}/src/imgui.cpp
    ${IMGUI_DIR}/src/imgui_demo.cpp
    ${IMGUI_DIR}/src/imgui_draw.cpp
    ${IMGUI_DIR}/src/imgui_tables.cpp
    ${IMGUI_DIR}/src/imgui_widgets.cpp
    ${IMGUI_DIR}/src/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/src/imgui_impl_opengl3.cpp
)
```


## 想定しているファイル構成
