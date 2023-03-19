# python3で実行が必要

VERSION = '3.3.8'
DOWNLOAD_URL = 'https://github.com/glfw/glfw/releases/download/' + VERSION + '/glfw-' + VERSION + '.bin.WIN64.zip'

LIB_ZIP_FILE = 'glfw-' + VERSION + '.bin.WIN64.zip'
LIB_NAME = 'glfw-' + VERSION + '.bin.WIN64'

INCLUDE_FOLDER = 'include'
LIB_FOLDER = 'lib'
SRC_FOLDER = 'src'


# ====================
#   ダウンロード処理
# ====================
import requests
import zipfile
import os
import shutil

# 生成フォルダの準備
if not os.path.isdir(INCLUDE_FOLDER):
  os.mkdir(INCLUDE_FOLDER)
if not os.path.isdir(LIB_FOLDER):
  os.mkdir(LIB_FOLDER)
if not os.path.isdir(SRC_FOLDER):
  os.mkdir(SRC_FOLDER)

# Githubからダウンロード
temp_data = requests.get(DOWNLOAD_URL).content
with open(LIB_ZIP_FILE ,mode='wb') as f:
  f.write(temp_data)


# ====================
#  ライブラリ展開処理
# ====================
with zipfile.ZipFile(LIB_ZIP_FILE) as temp_data:
  temp_data.extractall('.')

# ファイル配置
shutil.copytree('/'.join([LIB_NAME, 'include']), INCLUDE_FOLDER, dirs_exist_ok=True)
shutil.copytree('/'.join([LIB_NAME, 'lib-mingw-w64']), LIB_FOLDER, dirs_exist_ok=True)

# 削除
os.remove(LIB_ZIP_FILE)
shutil.rmtree(LIB_NAME)


