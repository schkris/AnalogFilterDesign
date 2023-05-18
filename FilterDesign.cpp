#include "FilterDesign.h"

transferFunct findTransferFunct()
{
	transferFunct transfer;
	if (true /*Currently only finding Butterworth*/)
	{
		double wc = shared.cutOff * (2.0 * M_PI);
		transfer.numerator = numButtTrans(wc);
		transfer.denominator = denomButtTrans(wc);
	}
	return transfer;
}

// Finds the numerator of the butterworth transfer function
std::string numButtTrans(double wc)
{
	double a;
	if (shared.type == typeState::LOW)
	{
		a = wc * wc;
	}
	else if (shared.type == typeState::HIGH)
	{
		a = 1;
	}
	return std::to_string(shared.gain) + " * " + std::to_string(a) + " * s";
}

// Finds the denominator of the butterworth transfer function
std::string denomButtTrans(double wc)
{
	double a;
	double b;
	a = M_SQRT2 * wc;
	b = wc * wc;
	return "s^2 + " + std::to_string(a) + " * s + " + std::to_string(b);
}