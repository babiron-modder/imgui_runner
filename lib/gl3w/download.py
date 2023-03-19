# python3.8で実行が必要

GIT_URL = 'https://github.com/skaslev/gl3w'
LIB_NAME = 'gl3w'

INCLUDE_FOLDER = 'include'
LIB_FOLDER = 'lib'
SRC_FOLDER = 'src'

# ====================
#   ダウンロード処理
# ====================
import os
import subprocess
import shutil
import sys

versions = sys.version_info
if versions.major!=3 or versions.minor<8:
  print('[ERROR]: This program needs Python 3.8')
  exit(1)

if os.path.isdir(LIB_NAME):
  print('[ERROR]:' ,LIB_NAME, 'フォルダの削除してください')
  exit(1)

# 生成フォルダの準備
if not os.path.isdir(INCLUDE_FOLDER):
  os.mkdir(INCLUDE_FOLDER)
if not os.path.isdir(LIB_FOLDER):
  os.mkdir(LIB_FOLDER)
if not os.path.isdir(SRC_FOLDER):
  os.mkdir(SRC_FOLDER)

# Githubからダウンロード
subprocess.run(['git', 'clone', GIT_URL, LIB_NAME])

# 権限を変更しておく
subprocess.run(['attrib', '-r', '-h', LIB_NAME+'/*', '/s', '/d'])
subprocess.run(['icacls', LIB_NAME, '/grant', 'Users:(OI)(CI)F', '/T'])

# ビルドを行う
os.chdir(LIB_NAME)
subprocess.call("gl3w_gen.py", shell=True)
os.chdir('../')

# ファイルをコピー配置
shutil.copy('/'.join([LIB_NAME, 'src', 'gl3w.c']), SRC_FOLDER)
shutil.copytree('/'.join([LIB_NAME, 'include']), INCLUDE_FOLDER, dirs_exist_ok=True)

# 削除
shutil.rmtree(LIB_NAME)




