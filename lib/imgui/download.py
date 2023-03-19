# python3で実行が必要
VERSION = '1.89.1'
DOWNLOAD_URL = 'https://github.com/ocornut/imgui/archive/refs/tags/v' + VERSION + '.zip'
TEMP_ZIP_FILE = 'temp.zip'
LIB_NAME = 'imgui-' + VERSION

INCLUDE_FOLDER = 'include'
LIB_FOLDER = 'lib'
SRC_FOLDER = 'src'

# ====================
#   ダウンロード処理
# ====================
import requests
import zipfile
import os
import glob
import shutil

# 生成フォルダの準備
if not os.path.isdir(INCLUDE_FOLDER):
  os.mkdir(INCLUDE_FOLDER)
if not os.path.isdir(LIB_FOLDER):
  os.mkdir(LIB_FOLDER)
if not os.path.isdir(SRC_FOLDER):
  os.mkdir(SRC_FOLDER)

temp_data = requests.get(DOWNLOAD_URL).content
with open(TEMP_ZIP_FILE ,mode='wb') as f:
  f.write(temp_data)


# ====================
#  ライブラリ展開処理
# ====================
with zipfile.ZipFile(TEMP_ZIP_FILE) as temp_data:
  temp_data.extractall('.')

for file in glob.glob('/'.join([LIB_NAME, '*.h'])):
    shutil.copy(file, INCLUDE_FOLDER)

for file in glob.glob('/'.join([LIB_NAME, 'backends', '*.h'])):
    shutil.copy(file, INCLUDE_FOLDER)

for file in glob.glob('/'.join([LIB_NAME, 'misc', 'freetype', '*.h'])):
    shutil.copy(file, INCLUDE_FOLDER)

for file in glob.glob('/'.join([LIB_NAME, '*.cpp'])):
    shutil.copy(file, SRC_FOLDER)

for file in glob.glob('/'.join([LIB_NAME, 'backends', '*.cpp'])):
    shutil.copy(file, SRC_FOLDER)

for file in glob.glob('/'.join([LIB_NAME, 'misc', 'freetype', '*.cpp'])):
    shutil.copy(file, INCLUDE_FOLDER)

# 削除
os.remove(TEMP_ZIP_FILE)
shutil.rmtree(LIB_NAME)



