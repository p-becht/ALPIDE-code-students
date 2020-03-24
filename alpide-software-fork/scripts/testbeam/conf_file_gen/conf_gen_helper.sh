#! /bin/bash -x

if [ "$#" -ne 18 ]
then
    echo "not enough arguments!"
    exit 1
fi
n_file=$1
vbb=$2
vcasn=$3
vcasn2=$4
ithr=$5
vrstp=$6
vrstd=$7
vclip=$8
ireset=$9
idb=${10}
dut_pos=$(printf "%0.2f" ${11})
scs=${12}
trg_dly=${13}
strobe_b_len=${14}
n_events=${15}
ntrig=${16}
period=${17}
readout_mode=${18}

#config_template="palpidefs_template.conf"
#config_template="palpidefs_template_pALPIDE-3_tracking_cont_int.conf"
config_template="palpidefs_template_pALPIDE-4_consistent.conf"
trg_dly_tracking=75
strobe_b_len_tracking=${strobe_b_len}
period_s=-1 # period in seconds

# strobe_b length derived from trigger period (in multiples of 25ns)
if [[ "${period}" -ne "-1" ]]
then
    if [[ "${strobe_b_len}" -eq "-1" ]]
    then
	strobe_b_len=$((${period} - 10))
    fi
    strobe_b_len_tracking=$((${period} - 10))

    period_s=$(printf "%e" $(echo "${period} * 0.000000025" | bc -l))

    trg_dly_tracking=0
fi

# generate folder structure
mkdir -p conf/palpidefs/conf

# hex strings for the XML file
vcasn_hex=$(printf "0x%X\n" ${vcasn})
vcasn2_hex=$(printf "0x%X\n" ${vcasn2})
ithr_hex=$(printf "0x%X\n" ${ithr})
ireset_hex=$(printf "0x%X\n" ${ireset})
vrstp_hex=$(printf "0x%X\n" ${vrstp})
vrstd_hex=$(printf "0x%X\n" ${vrstd})
vclip_hex=$(printf "0x%X\n" ${vclip})
idb_hex=$(printf "0x%X\n" ${idb})

# wide number string for the config file name
n_file_str=$(printf "%06d" $n_file)

# assemble file names
config_file='pALPIDE-4_'${n_file_str}'_VCASN'${vcasn}'_VCASN2'${vcasn2}'_ITHR'${ithr}'_VBB'${vbb}'_DUTpos'$dut_pos'_VRSTP'${vrstp}'_VRSTD'${vrstd}'_VCLIP'${vclip}'_IDB'${idb}'_IRESET'${ireset}'_TRGDLY'${trg_dly}'_NTrig'${ntrig}'_Period'${period}'.conf'
threshold_file='pALPIDE-4_threshold_VCASN'${vcasn}'_VCASN2'${vcasn2}'_ITHR'${ithr}'_VRSTP'${vrstp}'_VRSTD'${vrstd}'_VCLIP'${vclip}'_IDB'${idb}'_IRESET'${ireset}'.xml'

# EUDAQ config file
cp ${config_template} ${config_file}
sed -i -e 's/thresholdTmp/'${threshold_file}'/g' ${config_file}
sed -i -e 's/RunEventLimitTmp/'${n_events}'/g' ${config_file}
sed -i -e 's/VBBtmp/'${vbb}'/g' ${config_file}
sed -i -e 's/DUTposTmp/'${dut_pos}'/g' ${config_file}
sed -i -e 's/SCStmp/'${scs}'/g' ${config_file}
sed -i -e 's/ReadoutModeTmp/'${readout_mode}'/g' ${config_file}
sed -i -e 's/trgDlyTmp/'${trg_dly}'/g' ${config_file}
sed -i -e 's/trgDlyTrackingTmp/'${trg_dly_tracking}'/g' ${config_file}
sed -i -e 's/strbBlenTmp/'${strobe_b_len}'/g' ${config_file}
sed -i -e 's/strbBlenTrackingTmp/'${strobe_b_len_tracking}'/g' ${config_file}
sed -i -e 's/NTrigTmp/'${ntrig}'/g' ${config_file}
sed -i -e 's/PeriodTmp/'${period_s}'/g' ${config_file}
mv ${config_file} conf/

# XML config file
cp ./full_pALPIDE-4template.xml ${threshold_file}
sed -i -e 's/vcasnTmp/'${vcasn_hex}'/g' ${threshold_file}
sed -i -e 's/vcasn2Tmp/'${vcasn2_hex}'/g' ${threshold_file}
sed -i -e 's/ithrTmp/'${ithr_hex}'/g' ${threshold_file}
sed -i -e 's/vrstpTmp/'${vrstp_hex}'/g' ${threshold_file}
sed -i -e 's/vrstdTmp/'${vrstd_hex}'/g' ${threshold_file}
sed -i -e 's/vclipTmp/'${vclip_hex}'/g' ${threshold_file}
sed -i -e 's/idbTmp/'${idb_hex}'/g' ${threshold_file}
sed -i -e 's/iresetTmp/'${ireset_hex}'/g' ${threshold_file}
mv ${threshold_file} conf/palpidefs/conf

# remove sed tmp files (on MAC only?)
rm -f ${config_file}-e
rm -f ${threshold_file}-e
