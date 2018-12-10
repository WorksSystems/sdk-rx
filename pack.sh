#!/bin/bash

cd build
make distclean
make
cd -

if [ -e doxygen.conf ] ; then
doxygen doxygen.conf
cp arch.png html/
fi

mkdir sdk-rx
cp -r lib sdk-rx
cp -r inc sdk-rx
cp -r examples sdk-rx
if [ -e html ] ; then
cp -r html sdk-rx
fi

tar zcvf sdk-rx.tgz sdk-rx

rm -rf html
rm -rf sdk-rx
