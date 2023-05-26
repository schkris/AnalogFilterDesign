#define _USE_MATH_DEFINES
#include <cmath>
#include "Common.h"

struct complex
{
    double re;
    double im;
};

transferFunct findTransferFunct();

std::string numButtTrans(double wc);

std::string denomButtTrans(double wc);

void orderCheby1Calc();

void roundOrder();

void multiplyPoles(std::vector<complex> poles);

std::string numCheby1Trans(double wc);

std::string denomCheby1Trans(double wc);

std::string numCheby2Trans(double wc);

std::string denomCheby2Trans(double wc);
