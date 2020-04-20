#!/bin/bash

VALUE1=$(cat Config.cfg | grep 'Value1')
VALUE2=$(cat Config.cfg | grep 'VCASN')
VALUE3=$(cat Config.cfg | grep 'VCASN2')
VALUE4=$(cat Config.cfg | grep 'ITHR')

echo "Running Threshold scan with the following values:"
echo "$VALUE1"
echo "$VALUE2"
echo "$VALUE3"
echo "$VALUE4"
