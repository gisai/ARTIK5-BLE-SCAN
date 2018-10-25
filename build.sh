#! /bin/bash
gcc ARTIK.c -o SCAN -I/usr/include/arm-linux-gnueabihf/artik/base -I/usr/include/arm-linux-gnueabihf/artik/bluetooth -lartik-sdk-base
