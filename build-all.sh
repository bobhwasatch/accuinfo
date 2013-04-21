#!/bin/sh

echo "#define INVERTED 1" > style.h
./waf clean build
if [ $? -eq 0 ]
then
    mv build/accuinfo.pbw ./accuwhite.pbw
fi

echo "#define INVERTED 0" > style.h
./waf clean build
if [ $? -eq 0 ]
then
    mv build/accuinfo.pbw ./accuinfo.pbw
fi

