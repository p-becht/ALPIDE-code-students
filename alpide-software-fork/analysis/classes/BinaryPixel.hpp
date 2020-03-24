// Modified by Miljenko Suljic, m.suljic@cern.ch
// Based on Jacobus W. van Hoorne Pixel class

#ifndef BINARYPIXEL_HPP
#define BINARYPIXEL_HPP

#include <TObject.h>

class BinaryPixel: public TObject {
  
private:

    Int_t    fCol;
    Int_t    fRow;
    UShort_t fFlags; // 0b1 = hot pixel, 0b10 = border pixel...
    
public:

    BinaryPixel();
    BinaryPixel(Short_t col, Short_t row);
    BinaryPixel& operator=(const BinaryPixel& orig);
    virtual ~BinaryPixel(){}
    
    Int_t    GetCol  () const {return fCol;}
    Int_t    GetRow  () const {return fRow;}
    UShort_t GetFlags() const {return fFlags;}
    Bool_t   GetFlag (UShort_t flag) {return fFlags & (1<<flag);}

    Bool_t   IsHot   () const {return fFlags & 1;}
    Bool_t   IsBorder() const {return fFlags & 2;}

    virtual void Print(Option_t *option="") const;
    void Reset();

    void Set(Int_t col, Int_t row) {fCol = col; fRow = row;}
    void SetCol(Int_t col) {fCol = col;}
    void SetRow(Int_t row) {fRow = row;}
    void SetFlag(UShort_t flag, Bool_t state);

    ClassDef(BinaryPixel,1);
};

#endif








