# GL3W
## 現行バージョン
---(不明)

## ダウンロード先
### リポジトリ
https://github.com/skaslev/gl3w

### 最新バージョン
https://github.com/skaslev/gl3w

### バイナリ
---(なし)

## CMakeに書くこと
```
# ここは事前に宣言してあれば宣言不要
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE STRING "lib folder")

set(LIB_GL3W_DIR ${LIB_DIR}/gl3w CACHE STRING "gl3w")
include_directories(
    ${LIB_GL3W_DIR}/include
)
link_directories(
    ${LIB_GL3W_DIR}/lib
)
add_executable(
    ${LIB_GL3W_DIR}/src/gl3w.c
)
```


## 想定しているファイル構成
