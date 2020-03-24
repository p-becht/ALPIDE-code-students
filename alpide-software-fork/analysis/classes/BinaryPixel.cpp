#include "Riostream.h"
#include "BinaryPixel.hpp"

using namespace std;

ClassImp(BinaryPixel)

//______________________________________________________________________
BinaryPixel::BinaryPixel():TObject(),
    fFlags(0)
{
    fCol=-1;
    fRow=-1;
} 

//______________________________________________________________________
BinaryPixel::BinaryPixel(Short_t col, Short_t row):TObject(),
    fFlags(0)
{
    fCol=col;
    fRow=row;
}

// assignment operator
//______________________________________________________________________
BinaryPixel& BinaryPixel::operator=(const BinaryPixel& orig) {
    fCol = orig.fCol;
    fRow = orig.fRow;
    fFlags = orig.fFlags;
    return *this;
}

//______________________________________________________________________
void BinaryPixel::Print(Option_t *) const {
    cout << "*****BinaryPixel*****" << endl
         << " Col/Row: (" << fCol << " / " << fRow << ")" << endl
         << " Hot: " << IsHot() << " Border: " << IsBorder() << endl
         << "*********************" << endl;
}

//______________________________________________________________________
void BinaryPixel::Reset() {
    fCol=-1;
    fRow=-1;
    fFlags=0;
}

//______________________________________________________________________
void BinaryPixel::SetFlag(UShort_t flag, Bool_t state) {
    fFlags = fFlags & ~(1<<flag);
    if(state) fFlags += (1<<flag);
}
