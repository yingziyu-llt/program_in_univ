# 图像旋转作业

作者：林乐天 2300012154

## 安装

测试系统：Archlinux,gcc 13.2.1

```sh
g++ rotatebmp.cpp -o rotatebmp
```

测试系统：Windows11,mingw

```sh
g++ rotatebmp.cpp -o rotatebmp.exe
```

## 使用

### for linux

./rotatebmp src.bmp dst.bmp

### for Windows

./rotatebmp.exe src.bmp dst.bmp

由于未知问题，生成的图片不能用windows的图片查看器打开，但可以使用feh(测试用feh:feh 3.10.2)/vscode自带的图片预览功能打开，具体原因未知，解决方法未知。
