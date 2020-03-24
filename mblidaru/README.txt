Folder structure: check the git; it is identical. decide on a place from where you run scripts (maybe make a folder?). think programming-wise relative to that folder/ or to the Data folder under software/Data.
Threshold/Noise Scans: see git
Config file: see git
thresh.py: see git

Plotting:
1. BB vs Thr (x axis BB, y axis Thr (in electrons)). Remember 1DAC = 10 electrons
2. (BB,VCASN,ITHR) vs Thr (x axis the 3-pair values. y axis Thr).
3. FHR vs Thr (y axis FHR; log scale). 

For calculating values: use fits (eg, S-curve fit to get 50% value). Hint: error function.
For thresh.py (understand what it does and be able to explain/comment code) and if it does not do a gaussian fit to get the mean, think of a way to do it. Be able to motivate the use of a Gaussian!!! Looking at mean THR and its error, what do we see remarkable? Explain!
For FHR: why do we expect fake hits in normal operation of the chip? Why do we do this study? What generates fake signals?

Anything else we've missed and you need? Ask on Telegram :) 
