/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */
#include <fstream>
#include <ios>
#include <iostream>

#pragma pack( 1 )
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;

// 位图文件头定义;
struct BMPFileHeader {
    // 文件类型bfType单独定义
    DWORD bfSize;       // 文件大小
    WORD  bfReserved1;  // 保留字
    WORD  bfReserved2;  // 保留字
    DWORD bfOffBits;    // 从文件头到实际位图数据的偏移字节数
};

// 位图信息头定义
struct BMPInfoHeader {
    DWORD biSize;           // 信息头大小
    DWORD biWidth;          // 图像宽度
    DWORD biHeight;         // 图像高度
    WORD  biPlanes;         // 位平面数，必须为1
    WORD  biBitCount;       // 每像素位数
    DWORD biCompression;    // 压缩类型
    DWORD biSizeImage;      // 压缩图像大小字节数
    DWORD biXPelsPerMeter;  // 水平分辨率
    DWORD biYPelsPerMeter;  // 垂直分辨率
    DWORD biClrUsed;        // 位图实际用到的色彩数
    DWORD biClrImportant;   // 本位图中重要的色彩数
};

// 像素信息
struct DATA {
    BYTE blue;
    BYTE green;
    BYTE red;
};
// std::vector<DATA> data;
// std::vector<DATA> newData;

DATA *data, *newData;
int   getDiff( BMPInfoHeader& info ) {
    int DataSizePerline = ( info.biWidth * info.biBitCount + 31 ) / 8;
    DataSizePerline -= DataSizePerline % 4;
    return DataSizePerline - info.biWidth * info.biBitCount / 8;
}

void ReadImg( const std::string& filename, BMPFileHeader& fileHeader, BMPInfoHeader& infoHeader ) {
    std::ifstream file( filename, std::ios::binary );
    data = nullptr;
    if ( !file ) {
        std::cerr << "file not found" << std::endl;
        return;
    }
    WORD name;
    file.read( reinterpret_cast< char* >( &name ), sizeof( WORD ) );
    file.read( reinterpret_cast< char* >( &fileHeader ), sizeof( BMPFileHeader ) );
    if ( name != 0x4D42 ) {
        std::cerr << "not a bmp" << std::endl;
        return;
    }
    file.read( reinterpret_cast< char* >( &infoHeader ), sizeof( infoHeader ) );
    int diff           = getDiff( infoHeader );
    data               = new DATA[ infoHeader.biWidth * infoHeader.biHeight ];
    char* data_pointer = ( char* )&data[ 0 ];
    for ( int i = 0; i < infoHeader.biHeight; i++ ) {
        file.read( data_pointer, sizeof( DATA ) * infoHeader.biWidth );
        file.seekg( diff, std::ios_base::cur );
        data_pointer += sizeof( DATA ) * infoHeader.biWidth;
    }
    return;
}
void rotateImg( BMPFileHeader& fileHeader, BMPInfoHeader& infoHeader, const DATA* oriData ) {
    WORD new_height = infoHeader.biWidth;
    WORD new_width  = infoHeader.biHeight;
    newData         = new DATA[ new_height * new_width ];
    for ( int x = 0; x < new_height; x++ )
        for ( int y = 0; y < new_width; y++ ) {
            newData[ x * new_width + y ] = oriData[ y * infoHeader.biWidth + x ];
        }
    std::swap( infoHeader.biWidth, infoHeader.biHeight );
    //    infoHeader.biSize = ( infoHeader.biWidth * 24 + 31 ) / 8 * infoHeader.biHeight;
    //   fileHeader.bfSize = fileHeader.bfOffBits + infoHeader.biSize;
    return;
}
bool WriteImg( const std::string& filename, BMPFileHeader& fileHeader, BMPInfoHeader& infoHeader, const DATA* data ) {
    std::ofstream file( filename, std::ios::binary );
    if ( !file ) {
        std::cerr << "file not found" << std::endl;
        return false;
    }
    WORD type = 0x4D42;
    file.write( ( char* )&type, sizeof( WORD ) );
    file.write( reinterpret_cast< char* >( &fileHeader ), sizeof( fileHeader ) );
    file.write( reinterpret_cast< char* >( &infoHeader ), sizeof( infoHeader ) );

    int diff = getDiff( infoHeader );

    char* data_pointer = ( char* )&data[ 0 ];
    for ( int i = 0; i < infoHeader.biHeight; i++ ) {
        file.write( data_pointer, sizeof( DATA ) * infoHeader.biWidth );
        file.seekp( diff, std::ios_base::cur );
        data_pointer += sizeof( DATA ) * infoHeader.biWidth;
    }
    return true;
}
int main( int argc, char* argv[] ) {
    if ( argc != 3 ) {
        std::cerr << "usage:./rotatebmp src.bmp dest.bmp " << std::endl;
        return 1;
    }
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    ReadImg( argv[ 1 ], fileHeader, infoHeader );
    if ( data == nullptr ) {
        std::cerr << "can't open source file " << std::endl;
        return 1;
    }
    rotateImg( fileHeader, infoHeader, data );
    if ( !WriteImg( argv[ 2 ], fileHeader, infoHeader, newData ) ) {
        std::cerr << "write error " << std::endl;
        return 1;
    }
    return 0;
}
