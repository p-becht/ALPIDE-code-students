int AddressToColumn(int ARegion, int ADoubleCol, int AAddress) {
    int Column    = ARegion * 32 + ADoubleCol * 2;    // Double columns before ADoubleCol
    int LeftRight = ((AAddress % 4) < 2 ? 1:0);       // Left or right column within the double column
    
    Column += LeftRight;
    
    return Column;
}


int AddressToRow         (int ARegion, int ADoubleCol, int AAddress)
{
    // Ok, this will get ugly
    int Row = AAddress / 2;                // This is OK for the top-right and the bottom-left pixel within a group of 4
    if ((AAddress % 4) == 3) Row -= 1;      // adjust the top-left pixel
    if ((AAddress % 4) == 0) Row += 1;      // adjust the bottom-right pixel
    return Row;
}


int Hitmap(const char *fName) {
  int event, col, row, nhits;
  TH2F *hHitmap = new TH2F("hHitmap", "Hit map", 1024, -.5, 1023.5, 512, -.5, 511.5);
  FILE *fp = fopen(fName, "r");
  if (!fp) {
    std::cout << "Unable to open file " << fName <<std::endl;
    return -1;
  }
  TProfile *hCs = new TProfile("hCs", "Cluster Size", 512, -.5, 511.5);

  int ClusterSize = -1;
  int Lines = 0;
  while (fscanf (fp,"%d %d %d", &col, &row, &nhits) == 3) {
    int Column = AddressToColumn(col / 16, col % 16, row);
    int Row    = AddressToRow   (col / 16, col % 16, row);
    hHitmap->Fill(Column, Row, nhits);
  }


  gStyle->SetPalette(1);
  hHitmap->Draw("COLZ");
  //hCs->Draw();
  fclose(fp);
  return 1;
}


