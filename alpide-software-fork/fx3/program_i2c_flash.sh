#!/bin/bash

./download_fx3 -t RAM -i cyfxflashprog.img
sleep 1
./download_fx3 -t I2C -i SlaveFifoSync.img
