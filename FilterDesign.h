#define _USE_MATH_DEFINES
#include <cmath>
#include "Common.h"

struct complex
{
    double re;
    double im;
};

transferFunct findTransferFunct();

void calcOrder();

void orderButtCalc();

void orderCheby1Calc();

void orderCheby2Calc();

void orderElipCalc();

void roundOrder();

std::string numButtTrans(double wc);

std::string denomButtTrans(double wc);

void multiplyPoles(std::vector<complex> poles);

std::string numCheby1Trans(double wc);

std::string denomCheby1Trans(double wc);

std::string numCheby2Trans(double wc);

std::string denomCheby2Trans(double wc);
