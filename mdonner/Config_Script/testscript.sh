#!/bin/bash

VALUE1=$(cat Config.txt | grep 'Value1')
VALUE2=$(cat Config.txt | grep 'VCASN')
VALUE3=$(cat Config.txt | grep 'VCASN2')
VALUE4=$(cat Config.txt | grep 'ITHR')

echo "Running Threshold scan with the following values:"
echo "$VALUE1"
echo "$VALUE2"
echo "$VALUE3"
echo "$VALUE4"
