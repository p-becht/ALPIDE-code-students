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

int ExtractBrokenPixels(const char *input_file, int expected_hits, const char* output_file = "broken_pixels.txt") {
  FILE *fp = fopen(input_file, "r");
  if (!fp) {
    std::cout << "Unable to open file " << input_file <<std::endl;
    return -1;
  }

  int Data [512][1024] = { 0 };

  int col, row, nhits;
  while (fscanf (fp,"%d %d %d", &col, &row, &nhits) == 3) {
    Data[col][row] = nhits;
  }
  fclose(fp);
  
  fp = fopen(output_file, "w");
  int pixels = 0;
  for (col=0; col<512; col++) {
    for (row=0; row<1024; row++) {
      if (Data[col][row] == expected_hits)
	continue;
      
      int region = ColToRegion(col*2);
      int dcol = ColToDoubleCol(col*2);
      int paddr = row;
      
      fprintf(fp, "%d %d %d\n", region, dcol, paddr);
      pixels++;
    }
  }
  fclose(fp);
  std::cout << "Wrote " << pixels << " broken pixels to file." << std::endl;

  return 1;
}
