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
        # Ithr
        self.ithr_list=[]
        self.ithr_prio=[]
        # Idb
        self.idb_list=[]
        self.idb_prio=[]
        # Vaux/Vrst
        self.vaux_list=[[]]
        self.vrst_list=[[]]
        self.vauxrst_prio=[]
        # dut position and position related settings
        self.dut_pos_list=[]
        self.scs_list    =[]
        # trigger delay for the DUTs
        #self.trg_dly=int(75)
        self.trg_dly_list=[75]
        # strobeBlength for the DUTs
        self.strbB_len=int(20)
        self.n_events=int(120000)

    def generate_files( self ):
        for i_vbb in range(len(self.vbb_prio)):
            vbb_index=self.vbb_prio[i_vbb]
            vbb=self.vbb_list[vbb_index]
            for i_vcasn in range(len(self.vcasn_prio)):
                vcasn=self.vcasn_list[vbb_index][self.vcasn_prio[i_vcasn]]
                for i_ithr in range(len(self.ithr_prio)):
                    ithr=self.ithr_list[self.ithr_prio[i_ithr]]
                    for i_vauxrst in range(len(self.vauxrst_prio)):
                        vaux=self.vaux_list[vbb_index][self.vauxrst_prio[i_vauxrst]]
                        vrst=self.vrst_list[vbb_index][self.vauxrst_prio[i_vauxrst]]
                        for i_idb in range(len(self.idb_prio)):
                            idb=self.idb_list[self.idb_prio[i_idb]]
                            for i_dut_pos in range(len(self.dut_pos_list)):
                                dut_pos=self.dut_pos_list[i_dut_pos]
                                scs    =self.scs_list    [i_dut_pos]
                                for i_trg_dly in range(len(self.trg_dly_list)):
                                    trg_dly=self.trg_dly_list[i_trg_dly]

                                    args   =(self.i_file, vbb, vcasn, ithr, vaux, vrst, idb, dut_pos, scs, trg_dly , self.strbB_len, self.n_events)
                                    cmd    ="/bin/bash conf_gen_helper.sh %d %d %d %d %d %d %d %f %d %d %d %d"%args
                                    os.system(cmd)
                                    self.i_file+=1

####################################################################################################
####################################################################################################

s=settings()

s.n_events=50000

#### load the standard settings
# Vbb (V)
s.vbb_list=[0,3,6]
s.vbb_prio=[0,1,2]

# back-bias dependent Vcasn (DAC)
s.vcasn_list=[[ 45, 52, 57, 63, 69],
              [123,129,135,141,147],
              [148,154,160,166,172]]
s.vcasn_prio=[2]

# Ithr (DAC)
s.ithr_list=[10,15,20,30,40,51,60,70,80,90,100]
s.ithr_prio=[10, 9, 8, 7, 6, 5, 4, 3, 2, 1,  0]

# Idb (DAC)
s.idb_list=[64]
s.idb_prio=[ 0]

# back-bias dependent Vaux/Vrst (DAC)
s.vaux_list=[[117],[117],[117]]
s.vrst_list=[[117],[117],[117]]
s.vauxrst_prio=[0]

# dut position and position related settings (mm)
#s.dut_pos_list=[97,295] # 97 = in the beam, 295 out of the beam
#s.scs_list    =[ 0,  1] # 0  = no S-Curve scan, 1 = do S-Curve scan
s.dut_pos_list=[179.5] # not used for noise measurements anymore, just for positioning wrt the beam
s.scs_list    =[  0] # 0  = no S-Curve scan, 1 = do S-Curve scan
#


## Ithr scan
#############################################
s.generate_files()

s.i_file+=13

## Add two Vcasn points at Vbb=-3V
#############################################
s.vbb_prio=[1,2]  # -3V only
s.ithr_prio=[8] # only Ithr=70DAC
s.vcasn_prio=[0,1,3,4]
s.generate_files()

## delay scan
#############################################
#s.n_events=6000
#s.vbb_prio=[1]
#s.vcasn_prio=[2]
#s.ithr_prio=[7]
#s.trg_dly_list=[0,31,111,151,191,231,271,311,351,391,431]
#s.generate_files()

## extend Vcasn to lower values
#############################################
s.i_file=74

s.vcasn_list=[[21, 27, 33, 39, 45, 51,57, 63, 69],
              [ 99,105,111,117,123,129,135,141,147],
              [124,130,136,142,148,154,160,166,172]]
s.vbb_prio=[1,2]
s.vcasn_prio=[0,1,2,3]
s.ithr_prio=[8] # only Ithr=70DAC
s.generate_files()

## delay scan
#############################################
s.n_events=50000
s.vbb_prio=[1,2]
s.vcasn_prio=[6]
s.ithr_prio=[10]
s.trg_dly_list=[0,31,111,151,191,231,271,311,351,391,431]
s.generate_files()



s.i_file-=0 # remove the offset before calculate the time amount
print "%d config files produced" % s.i_file
print "Estimated measurement time %d min (%0.1f h) assuming 120 min per config file" % (s.i_file*120., s.i_file*120./60.)
