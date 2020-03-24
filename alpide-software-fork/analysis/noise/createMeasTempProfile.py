#!/usr/bin/env python

import os
import sys



if len(sys.argv)-1 != 1:
    print "number of arguments not correct! please provide: "
    print "1) path of measurement"
    sys.exit()

else:
    meas_path = sys.argv[1]



f_out = open(meas_path + '/temp.log', 'w')
print >>f_out, 'RATE\tBUSY\tVCASN\tITHR\tTemp'

setting = ''
avg_temp = 0
n_settings = 0

f_run_log = open(meas_path + '/run.log', 'r')
for line in f_run_log:
    if len(setting.split())==5:
        setting = ''
    if 'RATE:' in line: 
        #print line.split()[1], line.split()[4]
        setting += line.split()[1] # rate
        setting += '\t'
        setting += line.split()[4] # busy
    if 'VCASN:' in line: 
        #print line.split()[1], line.split()[3]
        setting += '\t'
        setting += line.split()[1][0:len(line.split()[1])-1] # vcasn
        setting += '\t'
        setting += line.split()[3] # ithr
        #print setting
    if 'NTC' in line:
        #print line.split()[4] # temp
        if len(setting.split())==4:
            setting += '\t'
            setting += line.split()[4] # temp
            print setting
            print >>f_out, setting
            avg_temp+=float(line.split()[4])
            n_settings+=1

f_run_log.close()
f_out.close()


f_out_avg = open(meas_path + '/avg_temp.txt', 'w')
avg_temp /= n_settings
print avg_temp
print >>f_out_avg, avg_temp

f_out_avg.close()















