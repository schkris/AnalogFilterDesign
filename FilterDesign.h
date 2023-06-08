#define _USE_MATH_DEFINES
#include <cmath>
#include "Common.h"

const int DECIMAL_PLACES = 3;

struct sepPoles
{
    std::vector<complex> conjugates;
    std::vector<complex> standAlone;
};

transferFunct findTransferFunct();

void calcOrder();

void orderButtCalc();

void orderCheby1Calc();

void orderCheby2Calc();

void orderElipCalc();

void roundOrder();

transferFunct findButterTrans(double wc);

transferFunct findButtPoles(transferFunct transfer, double wc);

transferFunct findCheb1Trans();

transferFunct findCheb1Poles(transferFunct transfer);

transferFunct findCheb2Trans();

transferFunct findCheb2Poles(transferFunct transfer);

sepPoles findConjugates(transferFunct transfer);