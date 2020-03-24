#!/usr/bin/env python

# Dump used currents at regular intervals
# Written by Jan.Fiete.Grosse-Oetringhaus@cern.ch
# (C) CERN 2014

from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals
from future_builtins import *
from ctypes import *

import os
import sys
import random
import math
import argparse
import time

import smtplib
from email.mime.text import MIMEText

# command line arguments
parser = argparse.ArgumentParser(description='Script to dump currents, noise and thresholds')
parser.add_argument('-b', type=int, help='connect to the indicated DAQ board (enumeration on the USB) (default 0)', default=0, metavar='BOARD')
parser.add_argument('-id', type=int, help='connect to the indicated DAQ board (enumeration by the debug PIN jumpers)', default=-1, metavar='ID')
parser.add_argument('-alerts', help='send alerts if currents drop below thresholds', action='store_true')
arguments = parser.parse_args()

# connect to device
palpidefs = None

try:
  palpidefs = CDLL("libpalpidefs.so")
except OSError:
  palpidefs = CDLL("../pALPIDEfs-software/libpalpidefs.so")

nboards = palpidefs.Init()
use_board = arguments.b
if arguments.id >= 0:
  use_board = palpidefs.SearchDAQBoard(arguments.id)
  if use_board < 0:
    print("ERROR: DAQ board with jumper setting %d not found" % arguments.id)
    sys.exit(-1)  
  
if use_board >= nboards:
  print("ERROR: DAQ board at position %d not found" % arguments.b)
  sys.exit(-1)

palpidefs.ActivateDAQBoard(use_board)

counter = 0
while 1:
  print("Time: %d %s" % (time.time(), time.ctime()))
  palpidefs.ReadAllADCs()
  analog_current = palpidefs.ReadAnalogI()
  digital_current = palpidefs.ReadDigitalI()

  palpidefs.DumpConfiguration()
  palpidefs.ReadAllChipDacs()
  
  #if (counter % 15 == 0):
  palpidefs.Characterize(1, 0)
  
  if (arguments.alerts and (digital_current < 100)):
    # alert
    print("Alert current too low!")

    msg = MIMEText("")

    me = "jgrosseo@cern.ch"
    dest = "41764875459@mail2sms.cern.ch,jgrosseo@cern.ch,freidt@cern.ch"

    msg['Subject'] = "WARNING current is %d" % digital_current
    msg['From'] = me
    msg['To'] = dest

    # Send the message via our own SMTP server, but don't include the
    # envelope header.
    s = smtplib.SMTP('cernmx.cern.ch')
    #s.set_debuglevel(True)
    s.sendmail(me, [ dest ], msg.as_string())
    s.quit()
    
    time.sleep(60)
    
    # try to recover
    print("Time: %d %s" % (time.time(), time.ctime()))
    print("Trying to reboot chip...")
    palpidefs.ShutDown(-1)
    time.sleep(1)
    palpidefs.ActivateDAQBoard(use_board)

  time.sleep(60)
  counter += 1
  
palpidefs.ShutDown(-1)

sys.exit(0)      
