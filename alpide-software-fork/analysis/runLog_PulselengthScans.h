// number of PulselengthScan runs done 
const int NUM_RUNS = 190;

// run date, run time, VCASN, ITH, STROBEBLENGTH_ANALOGUE, VBIAS, IDB
int RUN_INFO[NUM_RUNS][12] = {
// first measurements
{141120, 1653, 5, 5,  57,  51, 500, 0,  64, 0, 117,  50},  //  0 ## VCASN 57, ITH 51, delay 1-400,  1, ch_step 1, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V (short)
{141120, 1932, 5, 5,  57,  51, 100, 0,  64, 0, 117,  50},  //  1 ## VCASN 57, ITH 51, delay 1-400,  1, ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V (short)
{141120, 2013, 5, 5,  57,  51, 100, 0,  64, 0, 117,  50},  //  2 ## VCASN 57, ITH 51, delay 1-800,  2, ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V (short)
{141121, 1144, 5, 5,  57,  51, 100, 0,  64, 0, 117,  50},  //  3 ## VCASN 57, ITH 51, delay 1-800,  4, ch_step 1, STROBE_B length 100ns, 3.2us pulse, Vbias 0V (short)
{141121, 1154, 5, 5,  57,  51, 100, 0,  64, 0, 117,  50},  //  4 ## VCASN 57, ITH 51, delay 1-800,  4, ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V (short)
{141121, 1205, 5, 5,  57,  51, 100, 0,  64, 0, 117,  50},  //  5 ## VCASN 57, ITH 51, delay 1-1600, 4, ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V (short)
{141124, 1654, 5, 5,  57,  25, 500, 0,  64, 0, 117,  50},  //  6 ## VCASN 57, ITH 25, delay 1-1600, 4, ch_step 1, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V (short)
{141124, 1713, 5, 5,  57,  10, 500, 0,  64, 0, 117,  50},  //  7 ## VCASN 57, ITH 10, delay 1-1600, 4, ch_step 1, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V (short)
{141124, 1743, 5, 5,  57,  60, 500, 0,  64, 0, 117,  50},  //  8 ## VCASN 57, ITH 60, delay 1-1600, 4, ch_step 1, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V (short)
{141124, 2131, 5, 5,  57,  10, 100, 0,  64, 0, 117,  50},  //  9 ## VCASN 57, ITH 10, delay 1-1600, 4, ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V (short)
{141124, 2240, 5, 5,  57,  25, 100, 0,  64, 0, 117,  50},  // 10 ## VCASN 57, ITH 25, delay 1-1600, 4, ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V (short)

// default settings for -3V Vbias
{141125, 1908, 5, 5, 135,  51, 100, 3,  64, 0, 117,  50},  // 11 ## VCASN 135, ITH 51, delay 1-1600, 4,ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias -3V 
{141125, 2230, 5, 5, 135,  25, 100, 3,  64, 0, 117,  50},  // 12 ## VCASN 135, ITH 25, delay 1-1600, 4,ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias -3V 
{141126,  706, 5, 5, 135,  10, 100, 3,  64, 0, 117,  50},  // 13 ## VCASN 135, ITH 10, delay 1-1600, 4,ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias -3V 

{141201, 1148, 5, 5,  67,  51, 500, 0,  64, 0, 117,  50},  // 14 ## VCASN 67, ITH 51, delay 1-1600, 4, ch_step 1, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V

// effect of IDB on pulse length, combine with run #5  for full set
{141201, 1905, 5, 5,  57,  51, 100, 0,  96, 0, 117,  50},  // 15 ## VCASN 67, ITH 51, delay 1-1600, 8, ch_step 2, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V
{141202, 1653, 5, 5,  57,  51, 500, 0, 128, 0, 117,  50},  // 16 ## VCASN 57, ITH 51, delay 1-800, 8, ch_step 2, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V
{141202, 1714, 5, 5,  57,  51, 500, 0, 160, 0, 117,  50},  // 17 ## VCASN 57, ITH 51, delay 1-800, 8, ch_step 2, STROBE_B length 100ns, default pulse (25.6us), Vbias 0V

// ITH, VCASN scan for 0V: low, mediu, high threshold
{141203, 1407, 5, 5,  72,  71, 500, 0,  64, 0U, 117, 50},  // 18 ## VCASN 72, ITH 71, delay 1-800, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141203, 1537, 5, 5,  77,  99, 500, 0,  64, 0U, 117, 50},  // 19 ## VCASN 67, ITH 99, delay 1-800, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141203, 2214, 5, 5,  62,  71, 500, 0,  64, 0U, 117, 50},  // 20 ## VCASN 62, ITH 71, delay 1-800, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141203, 2220, 5, 5,  72, 110, 500, 0,  64, 0U, 117, 50},  // 21 ## VCASN 72, ITH 110, delay 1-800, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141203, 2226, 5, 5,  77,  61, 500, 0,  64, 0U, 117, 50},  // 22 ## VCASN 77, ITH 61, delay 1-2000, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141203, 2240, 5, 5,  82,  91, 500, 0,  64, 0U, 117, 50},  // 23 ## VCASN 82, ITH 91, delay 1-2000, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1605, 5, 5,  62,  19, 500, 0,  64, 0U, 117, 50},  // 24 ## VCASN 82, ITH 91, delay 1-2000, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1621, 5, 5,  67,  29, 500, 0,  64, 0U, 117, 50},  // 25 ## VCASN 82, ITH 91, delay 1-2000, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1636, 5, 5,  72,  42, 500, 0,  64, 0U, 117, 50},  // 26 ## VCASN 82, ITH 91, delay 1-2000, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1802, 5, 5,  57,  10,7500, 0,  64, 0U, 117, 50},  // 27 ## VCASN 57, ITH 10, delay 1-2200, 8, ch_step 2, STROBE_B length 7500ns, default pulse (25.6us), Vbias 0V, forgot to change back the STROBE_B_LENGTH...
{141204, 1826, 5, 5,  57,  10, 500, 0,  64, 0U, 117, 50},  // 28 ## VCASN 57, ITH 10, delay 1-2200, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
// extreme ITH, VCASN combinations with larger IDB
{141204, 1856, 5, 5,  72,  42, 500, 0,  128, 0, 117,  50},  // 29 # delay 1-2200, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1911, 5, 5,  77,  61, 500, 0,  128, 0, 117,  50},  // 30 # delay 1-2200, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1926, 5, 5,  72,  71, 500, 0,  128, 0, 117,  50},  // 31 # delay 1-2200, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1932, 5, 5,  77,  99, 500, 0,  128, 0, 117,  50},  // 32 # delay 1-2200, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V
{141204, 1939, 5, 5,  72, 110, 500, 0,  128, 0, 117,  50},  // 33 # delay 1-2200, 8, ch_step 2, STROBE_B length 500ns, default pulse (25.6us), Vbias 0V

// ITH, VCASN scan for -3V Vbias: low, medium, high threshold
// IDB = 64
// low threshold
{141207, 1738, 5, 5, 135,  10, 500, 3,  64, 0, 117,  50},  // 34 # delay 1-2000, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1754, 5, 5, 140,  20, 500, 3,  64, 0, 117,  50},  // 35 # delay 1-2000, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1809, 5, 5, 145,  25, 500, 3,  64, 0, 117,  50},  // 36 # delay 1-2000, 8, ch_step 2, default pulse (25.6us), Vbias -3V
// medium threshold
{141207, 1824, 5, 5, 125,  10, 500, 3,  64, 0, 117,  50},  // 37 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1833, 5, 5, 130,  15, 500, 3,  64, 0, 117,  50},  // 38 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1843, 5, 5, 135,  25, 500, 3,  64, 0, 117,  50},  // 39 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1852, 5, 5, 140,  40, 500, 3,  64, 0, 117,  50},  // 40 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1901, 5, 5, 145,  55, 500, 3,  64, 0, 117,  50},  // 41 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
// high threshold
{141207, 1910, 5, 5, 125,  45, 500, 3,  64, 0, 117,  50},  // 42 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1917, 5, 5, 130,  60, 500, 3,  64, 0, 117,  50},  // 43 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1923, 5, 5, 135,  80, 500, 3,  64, 0, 117,  50},  // 44 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 1929, 5, 5, 140, 100, 500, 3,  64, 0, 117,  50},  // 45 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V
// IDB = 128
// low threshold
{141207, 2104, 5, 5, 135,  10, 500, 3, 128, 0, 117,  50},  // 46 # delay 1-2000, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2120, 5, 5, 140,  20, 500, 3, 128, 0, 117,  50},  // 47 # delay 1-2000, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2135, 5, 5, 145,  25, 500, 3, 128, 0, 117,  50},  // 48 # delay 1-2000, 8, ch_step 2, default pulse (25.6us), Vbias -3V
// medium threshold
{141207, 2150, 5, 5, 125,  10, 500, 3, 128, 0, 117,  50},  // 49 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2200, 5, 5, 130,  15, 500, 3, 128, 0, 117,  50},  // 50 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2209, 5, 5, 135,  25, 500, 3, 128, 0, 117,  50},  // 51 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2218, 5, 5, 140,  40, 500, 3, 128, 0, 117,  50},  // 52 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2227, 5, 5, 145,  55, 500, 3, 128, 0, 117,  50},  // 53 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias -3V
// high threshold
{141207, 2237, 5, 5, 125,  45, 500, 3, 128, 0, 117,  50},  // 54 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2243, 5, 5, 130,  60, 500, 3, 128, 0, 117,  50},  // 55 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2249, 5, 5, 135,  80, 500, 3, 128, 0, 117,  50},  // 56 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V
{141207, 2255, 5, 5, 140, 100, 500, 3, 128, 0, 117,  50},  // 57 # delay 1-800, 8, ch_step 2, default pulse (25.6us), Vbias -3V

// -------------------------------------------------------
// fs 8-20 (1e13)
// ITH, VCASN scan for -3V Vbias: low, medium, high threshold
// trying for optimal working point values
// IDB = 64
{141211, 1140, 5, 5,  70,  35, 500, 0,  64, 0, 117,  50},  // 58 # delay 1-2200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{141211, 1157, 5, 5,  70,  35, 500, 0, 192, 0, 117,  50},  // 59 # delay 1-2200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
// weird pulse shapes seen for runs above -> run non irradiated chip with same settings
{141211, 1333, 5, 5,  70,  35, 500, 0, 192, 0, 117,  50},  // 60 # delay 1-2200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
// for comparison: 
// fs 8-37 (1e13)
{141211, 1412, 5, 5,  70,  35, 500, 0, 192, 0, 117,  50},  // 61 # delay 1-2200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
// the chip above does not show the effect... -> continue with this chip for stdies at 0V


// -------------------------------------------------------
// non irradiated chip as in first measurements -> chipID W1-25?
// full scan of VCASN, ITHR as done in dec2014 testbeam
// 0V
{150128, 1443, 5, 5,  57,  10, 100, 0,  64, 0, 117,  50},  // 62 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1448, 5, 5,  63,  10, 100, 0,  64, 0, 117,  50},  // 63 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1501, 5, 5,  69,  10, 100, 0,  64, 0, 117,  50},  // 64 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150128, 1513, 5, 5,  57,  20, 100, 0,  64, 0, 117,  50},  // 65 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1519, 5, 5,  63,  20, 100, 0,  64, 0, 117,  50},  // 66 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1524, 5, 5,  69,  20, 100, 0,  64, 0, 117,  50},  // 67 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150128, 1530, 5, 5,  57,  30, 100, 0,  64, 0, 117,  50},  // 68 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1643, 5, 5,  63,  30, 100, 0,  64, 0, 117,  50},  // 69 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1649, 5, 5,  69,  30, 100, 0,  64, 0, 117,  50},  // 70 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150128, 1541, 5, 5,  57,  40, 100, 0,  64, 0, 117,  50},  // 71 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1546, 5, 5,  63,  40, 100, 0,  64, 0, 117,  50},  // 72 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1552, 5, 5,  69,  40, 100, 0,  64, 0, 117,  50},  // 73 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150128, 1557, 5, 5,  57,  51, 100, 0,  64, 0, 117,  50},  // 74 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1603, 5, 5,  63,  51, 100, 0,  64, 0, 117,  50},  // 75 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1703, 5, 5,  69,  51, 100, 0,  64, 0, 117,  50},  // 76 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150128, 1708, 5, 5,  57,  70, 100, 0,  64, 0, 117,  50},  // 77 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1725, 5, 5,  63,  70, 100, 0,  64, 0, 117,  50},  // 78 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150128, 1804, 5, 5,  69,  70, 100, 0,  64, 0, 117,  50},   // 79 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

// -3V
{150128, 1814, 5, 5, 135,  10, 100, 3,  64, 0, 117,  50},  // 80 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1822, 5, 5, 141,  10, 100, 3,  64, 0, 117,  50},  // 81 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1828, 5, 5, 147,  10, 100, 3,  64, 0, 117,  50},  // 82 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150128, 1834, 5, 5, 135,  20, 100, 3,  64, 0, 117,  50},  // 83 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1839, 5, 5, 141,  20, 100, 3,  64, 0, 117,  50},  // 84 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1845, 5, 5, 147,  20, 100, 3,  64, 0, 117,  50},  // 85 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150128, 1850, 5, 5, 135,  30, 100, 3,  64, 0, 117,  50},  // 86 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1856, 5, 5, 141,  30, 100, 3,  64, 0, 117,  50},  // 87 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1901, 5, 5, 147,  30, 100, 3,  64, 0, 117,  50},  // 88 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150128, 1907, 5, 5, 135,  40, 100, 3,  64, 0, 117,  50},  // 89 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1912, 5, 5, 141,  40, 100, 3,  64, 0, 117,  50},  // 90 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 2133, 5, 5, 147,  40, 100, 3,  64, 0, 117,  50},  // 91 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150128, 2139, 5, 5, 135,  51, 100, 3,  64, 0, 117,  50},  // 92 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1922, 5, 5, 141,  51, 100, 3,  64, 0, 117,  50},  // 93 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1927, 5, 5, 147,  51, 100, 3,  64, 0, 117,  50},  // 94 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150128, 1933, 5, 5, 135,  70, 100, 3,  64, 0, 117,  50},  // 95 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1938, 5, 5, 141,  70, 100, 3,  64, 0, 117,  50},  // 96 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150128, 1944, 5, 5, 147,  70, 100, 3,  64, 0, 117,  50},  // 97 # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V


// -------------------------------------------------------
// non irradiated chip as in first measurements -> chipID W1-25
// scan vs IDB, rather detailed
// 0V
{150204, 2028, 5, 5,  63,  30, 100, 0,  64, 0, 117,  50},  // 98 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2032, 5, 5,  63,  30, 100, 0,  96, 0, 117,  50},  // 99 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2037, 5, 5,  63,  30, 100, 0, 128, 0, 117,  50},  //100 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2041, 5, 5,  63,  30, 100, 0, 160, 0, 117,  50},  //101 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2045, 5, 5,  63,  30, 100, 0, 196, 0, 117,  50},  //102 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 0V

// 0V
{150204, 2050, 5, 5,  63,  70, 100, 0,  64, 0, 117,  50},  //103 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2053, 5, 5,  63,  70, 100, 0,  96, 0, 117,  50},  //104 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2056, 5, 5,  63,  70, 100, 0, 128, 0, 117,  50},  //105 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2059, 5, 5,  63,  70, 100, 0, 160, 0, 117,  50},  //106 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150204, 2103, 5, 5,  63,  70, 100, 0, 196, 0, 117,  50},  //107 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 0V

// 3V
{150205,  820, 5, 5, 141,  30, 100, 3,  64, 0, 117,  50},  //108 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  824, 5, 5, 141,  30, 100, 3,  96, 0, 117,  50},  //109 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  829, 5, 5, 141,  30, 100, 3, 128, 0, 117,  50},  //110 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  833, 5, 5, 141,  30, 100, 3, 160, 0, 117,  50},  //111 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  837, 5, 5, 141,  30, 100, 3, 196, 0, 117,  50},  //112 # delay 1-1600, 8, ch_step 2, default pulse (25.6us), Vbias 3V

// 3V
{150205,  842, 5, 5, 141,  70, 100, 3,  64, 0, 117,  50},  //113 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  845, 5, 5, 141,  70, 100, 3,  96, 0, 117,  50},  //114 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  848, 5, 5, 141,  70, 100, 3, 128, 0, 117,  50},  //115 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  851, 5, 5, 141,  70, 100, 3, 160, 0, 117,  50},  //116 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150205,  855, 5, 5, 141,  70, 100, 3, 196, 0, 117,  50},  //117 # delay 1-1200, 8, ch_step 2, default pulse (25.6us), Vbias 3V

// -------------------------------------------------------
// irradiated chip w-6, 
// full scan of VCASN, ITHR as done in dec2014 testbeam
// -3V
// last col 9 array: 2 => 1e13!!
{150226, 1109, 5, 5, 135,  10, 100, 3,  64, 2, 125,  50},  //118  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1114, 5, 5, 141,  10, 100, 3,  64, 2, 125,  50},  //119  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1120, 5, 5, 147,  10, 100, 3,  64, 2, 125,  50},  //120  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150226, 1126, 5, 5, 135,  20, 100, 3,  64, 2, 125,  50},  //121  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1131, 5, 5, 141,  20, 100, 3,  64, 2, 125,  50},  //122  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1137, 5, 5, 147,  20, 100, 3,  64, 2, 125,  50},  //123  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150226, 1142, 5, 5, 135,  30, 100, 3,  64, 2, 125,  50},  //124  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1148, 5, 5, 141,  30, 100, 3,  64, 2, 125,  50},  //125  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1153, 5, 5, 147,  30, 100, 3,  64, 2, 125,  50},  //126  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150226, 1159, 5, 5, 135,  40, 100, 3,  64, 2, 125,  50},  //127  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1205, 5, 5, 141,  40, 100, 3,  64, 2, 125,  50},  //128  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1210, 5, 5, 147,  40, 100, 3,  64, 2, 125,  50},  //129  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150226, 1216, 5, 5, 135,  51, 100, 3,  64, 2, 125,  50},  //130  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1221, 5, 5, 141,  51, 100, 3,  64, 2, 125,  50},  //131  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1227, 5, 5, 147,  51, 100, 3,  64, 2, 125,  50},  //132  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

{150226, 1232, 5, 5, 135,  70, 100, 3,  64, 2, 125,  50},  //133  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1238, 5, 5, 141,  70, 100, 3,  64, 2, 125,  50},  //134  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V
{150226, 1243, 5, 5, 147,  70, 100, 3,  64, 2, 125,  50},  //135  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 3V

// 0V
{150226, 1311, 5, 5,  57,  10, 100, 0,  64, 2, 117,  50},  //136  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1317, 5, 5,  63,  10, 100, 0,  64, 2, 117,  50},  //137  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1323, 5, 5,  69,  10, 100, 0,  64, 2, 117,  50},  //138  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150226, 1328, 5, 5,  57,  20, 100, 0,  64, 2, 117,  50},  //139  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1334, 5, 5,  63,  20, 100, 0,  64, 2, 117,  50},  //140  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1340, 5, 5,  69,  20, 100, 0,  64, 2, 117,  50},  //141  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150226, 1345, 5, 5,  57,  30, 100, 0,  64, 2, 117,  50},  //142  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1351, 5, 5,  63,  30, 100, 0,  64, 2, 117,  50},  //143  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1356, 5, 5,  69,  30, 100, 0,  64, 2, 117,  50},  //144  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150226, 1402, 5, 5,  57,  40, 100, 0,  64, 2, 117,  50},  //145  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1407, 5, 5,  63,  40, 100, 0,  64, 2, 117,  50},  //146  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1413, 5, 5,  69,  40, 100, 0,  64, 2, 117,  50},  //147  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150226, 1418, 5, 5,  57,  51, 100, 0,  64, 2, 117,  50},  //148  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1424, 5, 5,  63,  51, 100, 0,  64, 2, 117,  50},  //149  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1429, 5, 5,  69,  51, 100, 0,  64, 2, 117,  50},  //150  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150226, 1435, 5, 5,  57,  70, 100, 0,  64, 2, 117,  50},  //151  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1440, 5, 5,  63,  70, 100, 0,  64, 2, 117,  50},  //152  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150226, 1446, 5, 5,  69,  70, 100, 0,  64, 2, 117,  50},   //153  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V



// -------------------------------------------------------
// non-irradiated chip w1-25 , IRESET measurement!!
// full scan of VCASN, ITHR as done in dec2014 testbeam
// 0V
// IRST=200 -> extreme!

{150227, 1016, 5, 5,  57,  10, 100, 0,  64, 2, 117, 200},  //154  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1032, 5, 5,  63,  10, 100, 0,  64, 2, 117, 200},  //155  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1048, 5, 5,  69,  10, 100, 0,  64, 2, 117, 200},  //156  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150227, 1105, 5, 5,  57,  20, 100, 0,  64, 2, 117, 200},  //157  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1121, 5, 5,  63,  20, 100, 0,  64, 2, 117, 200},  //158  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1137, 5, 5,  69,  20, 100, 0,  64, 2, 117, 200},  //159  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150227, 1153, 5, 5,  57,  30, 100, 0,  64, 2, 117, 200},  //160  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1209, 5, 5,  63,  30, 100, 0,  64, 2, 117, 200},  //161  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1225, 5, 5,  69,  30, 100, 0,  64, 2, 117, 200},  //162  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150227, 1241, 5, 5,  57,  40, 100, 0,  64, 2, 117, 200},  //163  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1257, 5, 5,  63,  40, 100, 0,  64, 2, 117, 200},  //164  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1313, 5, 5,  69,  40, 100, 0,  64, 2, 117, 200},  //165  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150227, 1329, 5, 5,  57,  51, 100, 0,  64, 2, 117, 200},  //166  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1345, 5, 5,  63,  51, 100, 0,  64, 2, 117, 200},  //167  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1401, 5, 5,  69,  51, 100, 0,  64, 2, 117, 200},  //168  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150227, 1417, 5, 5,  57,  70, 100, 0,  64, 2, 117, 200},  //169  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1433, 5, 5,  63,  70, 100, 0,  64, 2, 117, 200},  //170  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150227, 1449, 5, 5,  69,  70, 100, 0,  64, 2, 117, 200},  //171  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V


// 0V
// IRST=100
{150303, 1349, 5, 5,  57,  10, 100, 0,  64, 2, 117, 100},  //172  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1405, 5, 5,  63,  10, 100, 0,  64, 2, 117, 100},  //173  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1421, 5, 5,  69,  10, 100, 0,  64, 2, 117, 100},  //174  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150303, 1437, 5, 5,  57,  20, 100, 0,  64, 2, 117, 100},  //175  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1453, 5, 5,  63,  20, 100, 0,  64, 2, 117, 100},  //176  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1509, 5, 5,  69,  20, 100, 0,  64, 2, 117, 100},  //177  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150303, 1525, 5, 5,  57,  30, 100, 0,  64, 2, 117, 100},  //178  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1541, 5, 5,  63,  30, 100, 0,  64, 2, 117, 100},  //179  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1557, 5, 5,  69,  30, 100, 0,  64, 2, 117, 100},  //180  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150303, 1613, 5, 5,  57,  40, 100, 0,  64, 2, 117, 100},  //181  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1629, 5, 5,  63,  40, 100, 0,  64, 2, 117, 100},  //182  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1645, 5, 5,  69,  40, 100, 0,  64, 2, 117, 100},  //183  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150303, 1701, 5, 5,  57,  51, 100, 0,  64, 2, 117, 100},  //184  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1717, 5, 5,  63,  51, 100, 0,  64, 2, 117, 100},  //185  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1733, 5, 5,  69,  51, 100, 0,  64, 2, 117, 100},  //186  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V

{150303, 1749, 5, 5,  57,  70, 100, 0,  64, 2, 117, 100},  //187  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1805, 5, 5,  63,  70, 100, 0,  64, 2, 117, 100},  //188  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V
{150303, 1821, 5, 5,  69,  70, 100, 0,  64, 2, 117, 100}   //189  # delay 1-2100, 8, ch_step 2, default pulse (25.6us), Vbias 0V


};





// information string for irradiated sensors
string RUN_INFO_IRRAD[3] = {"non-irrad", "2.5e12", "1e13"};



