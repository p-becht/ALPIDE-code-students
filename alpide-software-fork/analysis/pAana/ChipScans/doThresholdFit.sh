#/bin/bash
#
BINEXE="FitThresholds.exe"
#
Identi="160310_141922"
PathIn="Data/B143_W7_R5"
#
CfgsIn="${PathIn}/ScanConfig_${Identi}.cfg"
FileIn="${PathIn}/ThresholdScan_${Identi}.dat"
#
if [ ! -f ${CfgsIn} ]
then
  echo "No cfg found!!!"
  exit 0
fi
#
${BINEXE} ${FileIn} $1
#
exit 0
