#include "FilterDesign.h"

transferFunct findTransferFunct()
{
	transferFunct transfer;
	double wc = shared.cutOff * (2.0 * M_PI);
	shared.wc = wc;
	if (shared.category == catState::BUTT)
	{
		transfer.numerator = numButtTrans(wc);
		transfer.denominator = denomButtTrans(wc);
	}
	else if(shared.category == catState::CHEB1)
	{
		transfer.numerator = numCheby1Trans(wc);
		transfer.denominator = denomCheby1Trans(wc);
	}
	else if(shared.category == catState::CHEB2)
	{
		transfer.numerator = numCheby2Trans(wc);
		transfer.denominator = denomCheby2Trans(wc);
	}
	else if(shared.category == catState::ELIP)
	{

	}
	else if(shared.category == catState::BESL)
	{

	}
	return transfer;
}

// Finds the numerator of the butterworth transfer function
std::string numButtTrans(double wc)
{
	double a;
	if (shared.type == typeState::LOW)
	{
		a = wc * wc * shared.gain;
		return std::to_string(a);
	}
	else if (shared.type == typeState::HIGH)
	{
		a = shared.gain;
		return std::to_string(a) + " * s^2";
	}
	shared.transfer.num = a;
}

// Finds the denominator of the butterworth transfer function
std::string denomButtTrans(double wc)
{
	double a;
	double b;
	a = M_SQRT2 * wc;
	b = wc * wc;
	return "s^2 + " + std::to_string(a) + " * s + " + std::to_string(b);
	shared.transfer.Adenom = a;
	shared.transfer.Bdenom = b;
}

void orderCheby1Calc()
{
    shared.epsilon = sqrt(pow(10, (shared.maxOsc / 10.0)) - 1.0);
	double lambda = sqrt(pow(10, (shared.stopAtt / 10.0)) - 1.0);
	double omegaP = 2 * M_PI * shared.cutOff;
    double omegaS = 2 * M_PI * shared.stopFreq;
	double omegaR = omegaS / omegaP;
	//order acosh(sqrt((10^(.1*abs(rs))-1)/(10^(.1*abs(rp))-1)))/acosh(WA)
	shared.order = acosh(lambda / shared.epsilon) / acosh(omegaR);
}

// Rounds 
void roundOrder()
{
	double roundedOrder = ceil(shared.order);
	while( shared.order < (roundedOrder - 0.05))
	{
		shared.maxOsc = shared.maxOsc - 0.1;
		orderCheby1Calc();
	}
	shared.order = roundedOrder;
}

// Finds the numerator of the Chebyshev 1 transfer function
std::string numCheby1Trans(double wc)
{
	double a;
	if (shared.type == typeState::LOW)
	{
		a = wc * wc * shared.gain;
		return std::to_string(a);
	}
	else if (shared.type == typeState::HIGH)
	{
		a = shared.gain;
		return std::to_string(a) + " * s^2";
	}
}

// Finds the denominator of the Chebyshev 1 transfer function
std::string denomCheby1Trans(double wc)
{
	std::vector<complex> poles;
	double multiplier = pow(2, shared.order - 1) * shared.epsilon;
	// Find the negative poles
	for (int m = 1; m <= shared.order; m++)
	{
		complex temp;
		double theta = (M_PI/2) * (2*m - 1)/shared.order;
		temp.re = -sinh((1/shared.order)*asinh(1/shared.epsilon))*sin(theta);
		temp.im = cosh((1/shared.order)*asinh(1/shared.epsilon))*cos(theta);
		poles.push_back(temp);
	}
	multiplyPoles(poles);
	return "bruv";
}

void multiplyPoles(std::vector<complex> poles)
{

}

// Finds the numerator of the Chebyshev 2 transfer function
std::string numCheby2Trans(double wc)
{
	orderCheby1Calc();
	double a;
	if (shared.type == typeState::LOW)
	{
		a = wc * wc * shared.gain;
		return std::to_string(a);
	}
	else if (shared.type == typeState::HIGH)
	{
		a = shared.gain;
		return std::to_string(a) + " * s^2";
	}
}

// Finds the denominator of the Chebyshev 2 transfer function
std::string denomCheby2Trans(double wc)
{
	double a;
	double b;
	a = std::sqrt(1 + shared.epsilon * shared.epsilon) * wc;
	b = shared.epsilon * shared.epsilon * wc * wc;
	return "s^2 + " + std::to_string(a) + " * s + " + std::to_string(b);
}