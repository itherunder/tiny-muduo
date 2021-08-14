#!/bin/sh

#会把所有命令打印出来
set -x

SOURCE_DIR=`pwd`
#把bulid目录放到工程最外层
BUILD_DIR=${BUILD_DIR:-../build}

mkdir -p $BUILD_DIR \
    && cd $BUILD_DIR \
    && cmake $SOURCE_DIR \
    && make $* #$*表示可以传参
