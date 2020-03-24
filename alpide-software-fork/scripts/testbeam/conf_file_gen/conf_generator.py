#!/usr/bin/env python

import os

####################################################################################################
### Settings class #################################################################################
####################################################################################################
class settings:
  def __init__( self ):
# file index number
    self.i_file=0
## settings
# list: values to be set
# prio: sequence of the different settings
##
# Vbb
    self.vbb_list=[]
    self.vbb_prio=[]
# Vcasn
    self.vcasn_list=[[]]
    self.vcasn_prio=[]
# Vcasn2
    self.vcasn2_list=[[]]
    self.vcasn2_prio=[]
# Ithr
    self.ithr_list=[]
    self.ithr_prio=[]
# Idb
    self.idb_list=[]
    self.idb_prio=[]
# Vaux/Vrst (VrstP/VrstD)
#    self.vaux_list=[[]]
    self.vrstp_list=[[]]
    self.vrstd_list=[[]]
#    self.vauxrst_prio=[]
    self.vrstp_prio=[]
    self.vrstd_prio=[]
# Vclip
    self.vclip_list=[]
    self.vclip_prio=[]
# Ireset
    self.ireset_list=[]
    self.ireset_prio=[]
# dut position and position related settings
    self.dut_pos_list=[]
    self.scs_list    =[]
# trigger delay for the DUTs
    self.trg_dly_list=[75]
# strobeBlength for the DUTs
    self.strobe_b_len_list=[ int(20) ] #25ns per unit

    self.n_events=int(120000)
# Number of consecutive triggers made by trigger generator after the particle income
    self.n_trig = int(-1)
# Period of consecutive triggers (in s)
    self.period_list =[ int(-1) ]
# Readout mode of the chip: triggered - 1 ; continuous - 2
    self.readout_mode = 1

  def generate_files( self ):
    for i_vbb in range(len(self.vbb_prio)):
      vbb_index=self.vbb_prio[i_vbb]
      vbb=self.vbb_list[vbb_index]
      for i_vcasn in range(len(self.vcasn_prio)):
        vcasn=self.vcasn_list[vbb_index][self.vcasn_prio[i_vcasn]]
        for i_ithr in range(len(self.ithr_prio)):
          ithr=self.ithr_list[self.ithr_prio[i_ithr]]
          for i_vrstd in range(len(self.vrstd_prio)):
            vrstd=self.vrstd_list[vbb_index][self.vrstd_prio[i_vrstd]]
            for i_vclip in range(len(self.vclip_prio)):
              vclip=self.vclip_list[vbb_index][self.vclip_prio[i_vclip]]
              for i_idb in range(len(self.idb_prio)):
                idb=self.idb_list[self.idb_prio[i_idb]]
                for i_dut_pos in range(len(self.dut_pos_list)):
                  dut_pos=self.dut_pos_list[i_dut_pos]
                  scs    =self.scs_list    [i_dut_pos]
                  for i_trg_dly in range(len(self.trg_dly_list)):
                    trg_dly=self.trg_dly_list[i_trg_dly]
                    for i_strobe_b_len in range(len(self.strobe_b_len_list)):
                      strobe_b_len=self.strobe_b_len_list[i_strobe_b_len]

                      for i_period in range(len(self.period_list)):
                        period = int(-1)
                        if self.period_list[i_period]>0.:
                          period = int((self.period_list[i_period])/25.e-9)

                        # obsolete fixed values
                        vrstp  = 0   # obsolete
                        ireset = 50 # obsolete

                        # arguments list
                        args   = (self.i_file, vbb, vcasn, vcasn+12, ithr, vrstp, vrstd, vclip, ireset, idb, dut_pos, scs, trg_dly, strobe_b_len, self.n_events, self.n_trig, period, self.readout_mode)
                        cmd    = "/bin/bash conf_gen_helper.sh %d %d %d %d %d %d %d %d %d %d %f %d %d %d %d %d %d %d" % args
                        os.system(cmd)
                        self.i_file+=1

####################################################################################################
####################################################################################################
# DUT = 292 for last sectors
s=settings()

s.n_events=60000

#### load the standard settings
# Vbb (V)
s.vbb_list=[0,3,6]
s.vbb_prio=[1,0]

# back-bias dependent Vcasn (DAC)
s.vcasn_list=[[ 41 ,44, 47, 50, 53],
              [95, 98, 102, 105, 108],
              [125, 128, 122,135, 138]]
s.vcasn_prio=[3]

# back-bias dependent Vcasn (DAC)  Now OBSOLETE (We use VCASN+12)
s.vcasn2_list=[[ 45, 52, 62, 63, 69],
              [123,129,117,141,147],
              [148,154,147,166,172]]
s.vcasn2_prio=[2]

# Ithr (DAC)
s.ithr_list=[20,30,40,51,60,70,80,90,100]
s.ithr_prio=[8, 7, 6, 5, 4, 3, 2, 1,  0]
#s.ithr_prio=[5]

#s.ithr_list=[30,51,80,100]
#s.ithr_prio=[0,1,2,3]
#s.ithr_list=[20,30,40,51,60,70,80,90,100]
#s.ithr_prio=[6, 3, 1, 8, 7, 5, 4, 2,  0]

# Idb (DAC)
s.idb_list=[29]
s.idb_prio=[ 0]

# back-bias dependent Vaux/Vrst (DAC)
#s.vaux_list=[[117],[117],[117]]  # Obsolete
#s.vrstp_list=[[117],[117],[117]] # Obsolete
#s.vrstp_prio=[0]                 # Obsolete
s.vrstd_list=[[147],[147],[170]]
s.vrstd_prio=[0]

# Vclip (DAC)
s.vclip_list=[[0], [60],[100]]
s.vclip_prio=[0]

# IRESET (DAC) # obsolete
#s.ireset_list=[100]
#s.ireset_prio=[0]

# dut position and position related settings (mm)
#s.dut_pos_list=[97,295] # 97 = in the beam, 295 out of the beam
#s.scs_list    =[ 0,  1] # 0  = no S-Curve scan, 1 = do S-Curve scan
s.dut_pos_list=[173] # not used for noise measurements anymore, just for positioning wrt the beam
s.scs_list    =[  0] # 0  = no S-Curve scan, 1 = do S-Curve scan
#


## Ithr scan
#############################################
s.generate_files()

#s.ithr_list=[40,60,70,90]
#s.ithr_prio=[0,1,2,3]
#s.generate_files()
# dut position and position related settings (mm)
#s.dut_pos_list=[97,295] # 97 = in the beam, 295 out of the beam
#s.scs_list    =[ 0,  1] # 0  = no S-Curve scan, 1 = do S-Curve scan
#s.ithr_list=[30,51,80,100]
#s.ithr_prio=[0,1,2,3]
#
#s.dut_pos_list=[182] # not used for noise measurements anymore, just for positioning wrt the beam
#s.scs_list    =[  0] # 0  = no S-Curve scan, 1 = do S-Curve scan
#


## Ithr scan
#############################################
s.strobe_b_len_list=[int(-1)]
s.period_list=[1.e-5]
s.generate_files()
#s.ithr_list=[40,60,70,90]
#s.ithr_pro=[0,1,2,3]
#s.generate_files()

#s.i_file+=13

## Add two Vcasn points
#############################################
s.i_file=300
s.vbb_prio=[1,0]  # -3V only
s.ithr_prio=[3,8] # only Ithr=51DAC and 100DAC
s.vcasn_prio=[0,1,2,4]
s.generate_files()

## delay scan
#############################################
s.i_file=800
s.vbb_prio=[1,0,2]
s.vcasn_prio=[3]
s.ithr_prio=[3]
s.trg_dly_list=[0,40,80,120,160,200,240,280,320,360,400]
s.generate_files()

## continuous integration
#########################################################
s.i_file=1200
s.n_events=int(600000)
s.vbb_prio=[1,0]
s.vcasn_prio=[3]
s.ithr_prio=[8,3]
s.n_trig=int(100)
s.trg_dly_list=[0]
s.strobe_b_len_list=[int(-1)]
s.period_list=[5.e-5, 2.e-5, 1.e-5, 5.e-6, 2.e-6, 1.e-6]
#s.generate_files()

s.i_file=1400
s.ithr_prio=[8,3]
s.readout_mode=2
s.generate_files()

s.i_file=1600
s.readout_mode=1
s.vbb_prio=[1]
s.strobe_b_len_list=[40, 80, 200, 300, 320, 340, 360, 380]
s.period_list=[1.e-5]
s.generate_files()


s.i_file=1800
s.strobe_b_len_list=[int(-1)]
s.trg_dly_list=[0,40,80,120,160,200,240,280,320,360]
s.generate_files()

## extend Vcasn to lower values
#############################################
#s.i_file=74

#s.vcasn_list=[[21, 27, 33, 39, 45, 51,57, 63, 69],
#              [ 99,105,111,117,123,129,135,141,147],
#              [124,130,136,142,148,154,160,166,172]]
#s.vbb_prio=[1,2]
#s.vcasn_prio=[0,1,2,3]
#s.ithr_prio=[8] # only Ithr=70DAC
#s.generate_files()

## delay scan
#############################################
#s.i_file +=100
#s.n_events=60000
#s.vbb_prio=[0,1,2]
#s.vcasn_prio=[2]
#s.vcasn2_prio=[2]
#s.ithr_prio=[3]
#s.trg_dly_list=[0,31,111,151,191,231,271,311,351,391,431]
#s.trg_dly_list=[0,50,125,200]
#s.generate_files()

#s.dut_pos_list=[173]
#s.generate_files()


s.i_file-=0 # remove the offset before calculate the time amount

print "%d config files produced" % s.i_file
print "Estimated measurement time %d min (%0.1f h) assuming 120 min per config file" % (s.i_file*120., s.i_file*120./60.)
