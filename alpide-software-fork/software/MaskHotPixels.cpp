#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <ctime>

int ColRowToAddress      (int AColumn, int ARow) {
    int LeftRight = AColumn % 2;     // 0 for left column, 1 for right
    int OddEven   = ARow    % 2;     // 0 for even rows, 1 for odd
    int Address   = ARow * 2;        // this is OK only for the bottom left-pixel within a group of 4
    
    if ((LeftRight == 0) && (OddEven == 0)) Address += 3;     // adjust top-left pixel
    if ((LeftRight == 1) && (OddEven == 0)) Address += 1;     // adjust top-right pixel
    if ((LeftRight == 1) && (OddEven == 1)) Address -= 2;     // adjust bottom-right pixel
    return Address;
}


int ColToRegion          (int AColumn) {
    int Region = AColumn / 32;
    return Region;
}


int ColToDoubleCol       (int AColumn) {
    int ColInRegion = AColumn % 32;
    int DoubleCol   = ColInRegion / 2;
    
    return DoubleCol;
}


int main(int argc, const char *argv[]) {
  int dcol, address, hits;
  int NMasked = 0;
  int cut     = atoi(argv[2]);
  FILE *hitfile = fopen (argv[1], "r");
  FILE *outfile = fopen ("mask.out", "w");

  if (!hitfile) {
    std::cout << "File " << argv[1] << " not found." << std::endl;
  }


  while (fscanf (hitfile, "%d %d %d", &dcol, &address, &hits) == 3) {
    if (hits > cut){
      int Region = dcol / 16;
      int DCol   = dcol % 16;
      std:: cout << "Pixel " << dcol << "/" <<  address << " => " << Region << "/" << DCol << "/" << address << std::endl; 
      fprintf(outfile, "%d %d %d\n", Region, DCol, address);
      NMasked ++;      
    }
  }
  std::cout << "Found " << NMasked << " pixels with more than " << cut << " hits." << std::endl;
  fclose(hitfile);
  fclose(outfile);
}
