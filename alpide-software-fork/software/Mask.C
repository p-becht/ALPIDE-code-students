void Mask (int Bit) {
    int NMasked = 0;
    const char fName[20];
    sprintf(fName, "mask_bit%d.dat", Bit);
    int MaskPattern = 0x1 << Bit;

    std::cout << std::hex << "Masking bit " << MaskPattern << std::dec << std::endl;
    FILE *fp = fopen(fName, "w");
    for (int ireg = 0; ireg < 32; ireg ++) {
        for (int idcol = 0; idcol < 16; idcol ++) {
            for (int iadd = 0; iadd < 1024; iadd ++) {
	      if (!(iadd & MaskPattern)) continue;
                NMasked ++;
	        fprintf(fp, "%d %d %d\n", ireg, idcol, iadd);
            }
        }
    }
    fclose(fp);
    std::cout << "Masked " << NMasked << " Pixels" << std::endl;
}
