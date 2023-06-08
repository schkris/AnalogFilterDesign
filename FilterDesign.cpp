#include "FilterDesign.h"

transferFunct findTransferFunct()
{
	transferFunct transfer;
	double wc = shared.cutOff * (2.0 * M_PI);
	double ws = shared.stopFreq * (2.0 * M_PI);
	shared.wc = wc;
	shared.ws = ws;
	shared.order = ceil(shared.order);
	if (shared.category == catState::BUTT)
	{
		transfer = findButterTrans(wc);
	}
	if(shared.category == catState::CHEB1)
	{
		transfer = findCheb1Trans();
	}
	else if(shared.category == catState::CHEB2)
	{
		transfer = findCheb2Trans();
	}
	else if(shared.category == catState::ELIP)
	{

	}
	else if(shared.category == catState::BESL)
	{

	}
	return transfer;
}

void calcOrder()
{
	if(shared.category == catState::BUTT)
	{
		orderButtCalc();
	}
	else if(shared.category == catState::CHEB1)
	{
		orderCheby1Calc();
	}
	else if(shared.category == catState::CHEB2)
	{
		orderCheby2Calc();
	}
	else if(shared.category == catState::ELIP)
	{
		orderElipCalc();
	}
	else if(shared.category == catState::BESL)
	{

	}
}

void orderButtCalc()
{
	double omegaP = 2 * M_PI * shared.cutOff;
    double omegaS = 2 * M_PI * shared.stopFreq;
	if(shared.type == typeState::LOW)
	{
		shared.order = (log(pow(10, (shared.stopAtt / 10.0)) - 1.0))/(2*log(omegaS/omegaP));
	}
	else if(shared.type == typeState::HIGH)
	{
		shared.order = (log(pow(10, (shared.stopAtt / 10.0)) - 1.0))/(2*log(omegaP/omegaS));
	}
}

void orderCheby1Calc()
{
	double gammaDenom = sqrt(pow(10, (shared.passAtt / 10.0)) - 1.0);
	double gammaNum = sqrt(pow(10, (shared.stopAtt / 10.0)) - 1.0);
	double gamma = gammaNum / gammaDenom;
	double omegaP = 2 * M_PI * shared.cutOff;
    double omegaS = 2 * M_PI * shared.stopFreq;
	double omegaR;
	if(shared.type == typeState::LOW)
	{
		// Correct
		omegaR = omegaS / omegaP;
	}
	else if(shared.type == typeState::HIGH)
	{
		// Check High
		omegaR = omegaP / omegaS;
	}
	shared.epsilon = gammaDenom;
	shared.order = acosh(gamma) / acosh(omegaR);
}

void orderCheby2Calc()
{
	double omegaP = 2 * M_PI * shared.cutOff;
    double omegaS = 2 * M_PI * shared.stopFreq;
	double omegaR;
	double A = pow(10, (shared.stopAtt / 20.0));
	shared.epsilon = sqrt(pow(10, (shared.passAtt / 10.0)) - 1.0);
	if(shared.type == typeState::LOW)
	{
		// Correct
		omegaR = omegaS / omegaP;
	}
	else if(shared.type == typeState::HIGH)
	{
		// Check High
		omegaR = omegaP / omegaS;
	}
	shared.order = acosh((sqrt(pow(A, 2) - 1.0))/shared.epsilon) / acosh(omegaR);
	shared.epsilon = 1.0 / sqrt(pow(10, (shared.stopAtt / 10.0)) - 1.0);
}

void orderElipCalc()
{
	double omegaP = 2 * M_PI * shared.cutOff;
    double omegaS = 2 * M_PI * shared.stopFreq;
	double D = (pow(10, (shared.stopAtt / 10.0)) - 1.0)/(pow(10, (shared.passAtt / 10.0)) - 1.0);
	double k = omegaP/omegaS;
	double kprime = sqrt(1.0 - pow(k, 2));
	double qnum = 1.0-sqrt(kprime);
	double qdenom = 1.0+sqrt(kprime);
	double qnauth = (0.5)*(qnum/qdenom);
	double q = qnauth + 2 * pow(qnauth, 5) + 15 * pow(qnauth, 9) + 150 * pow(qnauth, 13);
	shared.order = log(16*D)/log(1/q);
}

// Rounds the order by modifying the oscillation or stop frequency for efficiency
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

// Finds the numerator of the butterworth transfer function
transferFunct findButterTrans(double wc)
{
	sepPoles seperate;
	transferFunct transfer;
	transfer.order = shared.order;
	transfer = findButtPoles(transfer, wc);
	seperate = findConjugates(transfer);
	std::string coefn0;
	std::string coefd0;
	std::string coefd1;
	std::string coefd2;
	if(transfer.order == 1)
	{
		transfer.firstOrder.d1 = 1.0;
		transfer.firstOrder.d0 = -seperate.standAlone[0].re;
		if(shared.type == typeState::LOW)
		{
			transfer.firstOrder.n0 = shared.gain * wc;
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			transfer.firstOrder.n0 = shared.gain;
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s";
		}
		coefd0 = std::to_string(transfer.firstOrder.d0);
		coefd1 = std::to_string(transfer.firstOrder.d1);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		transfer.firstOrder.denominator = coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 2)
	{
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im);
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = shared.gain * wc * wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			transfer.secondOrder1.n0 = shared.gain;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 3)
	{
		// First Order
		transfer.firstOrder.d1 = 1.0;
		transfer.firstOrder.d0 = -seperate.standAlone[0].re;
		if(shared.type == typeState::LOW)
		{
			transfer.firstOrder.n0 = sqrt(shared.gain) * wc;
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			transfer.firstOrder.n0 = sqrt(shared.gain);
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s";
		}
		coefd0 = std::to_string(transfer.firstOrder.d0);
		coefd1 = std::to_string(transfer.firstOrder.d1);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		transfer.firstOrder.denominator = coefd1 + " * s" + " + " + coefd0;
		// Second Order
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im);
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = sqrt(shared.gain) * wc * wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			transfer.secondOrder1.n0 = sqrt(shared.gain);
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 4)
	{
		// First Second Order
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im);
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = sqrt(shared.gain) * wc * wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			transfer.secondOrder1.n0 = sqrt(shared.gain);
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;

		// Second Second Order
		transfer.secondOrder2.d2 = 1.0;
		transfer.secondOrder2.d1 = -2.0 * seperate.conjugates[1].re;
		transfer.secondOrder2.d0 = seperate.conjugates[1].re * seperate.conjugates[1].re + abs(seperate.conjugates[1].im) * abs(seperate.conjugates[1].im);
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder2.n0 = sqrt(shared.gain) * wc * wc;
			coefn0 = std::to_string(transfer.secondOrder2.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder2.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			transfer.secondOrder2.n0 = sqrt(shared.gain);
			coefn0 = std::to_string(transfer.secondOrder2.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder2.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder2.d0);
		coefd1 = std::to_string(transfer.secondOrder2.d1);
		coefd2 = std::to_string(transfer.secondOrder2.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder2.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	return transfer;
}

transferFunct findButtPoles(transferFunct transfer, double wc)
{
	for(int m = 1; m <= transfer.order; m++)
	{
		complex temp;
		double internal = (2.0*m + (double)transfer.order - 1.0)/(2.0 * (double)transfer.order);
		temp.re = wc * cos(M_PI * internal);
		temp.im = wc * sin(M_PI * internal);
		// Fix rounding error
		if(temp.im > -0.0000000001 && temp.im < 0.0000000001)
		{
			temp.im = 0.0;
		}
		transfer.poles.push_back(temp);
	}
	return transfer;
}

transferFunct findCheb1Trans()
{
	sepPoles seperate;
	transferFunct transfer;
	transfer.order = shared.order;
	transfer = findCheb1Poles(transfer);
	seperate = findConjugates(transfer);
	double denomMultiplier = shared.epsilon * (pow(2.0, shared.order - 1));
	std::string coefn0;
	std::string coefd0;
	std::string coefd1;
	std::string coefd2;
	if(transfer.order == 1)
	{
		transfer.firstOrder.n0 = shared.gain / denomMultiplier;
		transfer.firstOrder.d1 = 1.0;
		transfer.firstOrder.d0 = -seperate.standAlone[0].re;
		if(shared.type == typeState::LOW)
		{
			transfer.firstOrder.n0 = transfer.firstOrder.n0 * shared.wc;
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s";
		}
		coefd0 = std::to_string(transfer.firstOrder.d0);
		coefd1 = std::to_string(transfer.firstOrder.d1);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
	coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		transfer.firstOrder.denominator = coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 2)
	{
		transfer.secondOrder1.n0 = shared.gain / denomMultiplier;
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = (seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = transfer.secondOrder1.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 3)
	{
		// First Order
		transfer.firstOrder.n0 = sqrt(shared.gain) / sqrt(denomMultiplier);
		transfer.firstOrder.d1 = 1.0;
		transfer.firstOrder.d0 = -seperate.standAlone[0].re;
		if(shared.type == typeState::LOW)
		{
			transfer.firstOrder.n0 = transfer.firstOrder.n0 * shared.wc;
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s";
		}
		coefd0 = std::to_string(transfer.firstOrder.d0);
		coefd1 = std::to_string(transfer.firstOrder.d1);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		transfer.firstOrder.denominator = coefd1 + " * s" + " + " + coefd0;
		// Second Order
		transfer.secondOrder1.n0 = sqrt(shared.gain) / sqrt(denomMultiplier);
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = (seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = transfer.secondOrder1.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 4)
	{
		// First Second Order
		transfer.secondOrder1.n0 = sqrt(shared.gain) / sqrt(denomMultiplier);
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = (seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = transfer.secondOrder1.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;

		// Second Second Order
		transfer.secondOrder2.n0 = sqrt(shared.gain) / sqrt(denomMultiplier);
		transfer.secondOrder2.d2 = 1.0;
		transfer.secondOrder2.d1 = -2.0 * seperate.conjugates[1].re;
		transfer.secondOrder2.d0 = (seperate.conjugates[1].re * seperate.conjugates[1].re + abs(seperate.conjugates[1].im) * abs(seperate.conjugates[1].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder2.n0 = transfer.secondOrder2.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder2.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder2.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder2.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder2.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder2.d0);
		coefd1 = std::to_string(transfer.secondOrder2.d1);
		coefd2 = std::to_string(transfer.secondOrder2.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder2.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	return transfer;
}

transferFunct findCheb1Poles(transferFunct transfer)
{
	double alpha = (1.0/transfer.order) * asinh((1.0/shared.epsilon));
	for(int m = 1; m <= transfer.order; m++)
	{
		complex temp;
		temp.re = -shared.wc * sin((M_PI_2 * (((2.0 * m) - 1.0) / transfer.order))) * sinh(alpha);
		temp.im = shared.wc * cos((M_PI_2 * (((2.0 * m) - 1.0) / transfer.order))) * cosh(alpha);
		// Fix rounding error
		if(temp.im > -0.0000000001 && temp.im < 0.0000000001)
		{
			temp.im = 0.0;
		}
		transfer.poles.push_back(temp);
	}
	return transfer;
}

transferFunct findCheb2Trans()
{
	sepPoles seperate;
	transferFunct transfer;
	transfer.order = shared.order;
	transfer = findCheb2Poles(transfer);
	seperate = findConjugates(transfer);
	double denomMultiplier = shared.epsilon * (pow(2.0, shared.order - 1));
	std::string coefn0;
	std::string coefd0;
	std::string coefd1;
	std::string coefd2;
	if(transfer.order == 1)
	{
		transfer.firstOrder.n0 = shared.gain * denomMultiplier;
		transfer.firstOrder.d1 = 1.0;
		transfer.firstOrder.d0 = -seperate.standAlone[0].re;
		if(shared.type == typeState::LOW)
		{
			transfer.firstOrder.n0 = transfer.firstOrder.n0 * shared.wc;
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s";
		}
		coefd0 = std::to_string(transfer.firstOrder.d0);
		coefd1 = std::to_string(transfer.firstOrder.d1);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		transfer.firstOrder.denominator = coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 2)
	{
		transfer.secondOrder1.n0 = shared.gain * denomMultiplier;
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = (seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = transfer.secondOrder1.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 3)
	{
		// First Order
		transfer.firstOrder.n0 = sqrt(shared.gain) * sqrt(denomMultiplier);
		transfer.firstOrder.d1 = 1.0;
		transfer.firstOrder.d0 = -seperate.standAlone[0].re;
		if(shared.type == typeState::LOW)
		{
			transfer.firstOrder.n0 = transfer.firstOrder.n0 * shared.wc;
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.firstOrder.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.firstOrder.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s";
		}
		coefd0 = std::to_string(transfer.firstOrder.d0);
		coefd1 = std::to_string(transfer.firstOrder.d1);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		transfer.firstOrder.denominator = coefd1 + " * s" + " + " + coefd0;
		// Second Order
		transfer.secondOrder1.n0 = sqrt(shared.gain) * sqrt(denomMultiplier);
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = (seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = transfer.secondOrder1.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	else if(transfer.order == 4)
	{
		// First Second Order
		transfer.secondOrder1.n0 = sqrt(shared.gain) * sqrt(denomMultiplier);
		transfer.secondOrder1.d2 = 1.0;
		transfer.secondOrder1.d1 = -2.0 * seperate.conjugates[0].re;
		transfer.secondOrder1.d0 = (seperate.conjugates[0].re * seperate.conjugates[0].re + abs(seperate.conjugates[0].im) * abs(seperate.conjugates[0].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder1.n0 = transfer.secondOrder1.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder1.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder1.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder1.d0);
		coefd1 = std::to_string(transfer.secondOrder1.d1);
		coefd2 = std::to_string(transfer.secondOrder1.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder1.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;

		// Second Second Order
		transfer.secondOrder2.n0 = sqrt(shared.gain) * sqrt(denomMultiplier);
		transfer.secondOrder2.d2 = 1.0;
		transfer.secondOrder2.d1 = -2.0 * seperate.conjugates[1].re;
		transfer.secondOrder2.d0 = (seperate.conjugates[1].re * seperate.conjugates[1].re + abs(seperate.conjugates[1].im) * abs(seperate.conjugates[1].im));
		if(shared.type == typeState::LOW)
		{
			transfer.secondOrder2.n0 = transfer.secondOrder2.n0 * shared.wc * shared.wc;
			coefn0 = std::to_string(transfer.secondOrder2.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder2.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
		}
		else if(shared.type == typeState::HIGH)
		{
			coefn0 = std::to_string(transfer.secondOrder2.n0);
			coefn0 = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1);
			transfer.secondOrder2.numerator = coefn0.substr(0 , coefn0.find('.') + DECIMAL_PLACES + 1) + " * s^2";
		}
		coefd0 = std::to_string(transfer.secondOrder2.d0);
		coefd1 = std::to_string(transfer.secondOrder2.d1);
		coefd2 = std::to_string(transfer.secondOrder2.d2);
		coefd0 = coefd0.substr(0 , coefd0.find('.') + DECIMAL_PLACES + 1);
		coefd1 = coefd1.substr(0 , coefd1.find('.') + DECIMAL_PLACES + 1);
		coefd2 = coefd2.substr(0 , coefd2.find('.') + DECIMAL_PLACES + 1);
		transfer.secondOrder2.denominator = coefd2 + " * s^2" + " + " + coefd1 + " * s" + " + " + coefd0;
	}
	return transfer;
}

transferFunct findCheb2Poles(transferFunct transfer)
{
	// Inverse Chebyshev poles are found by finding the inverse magnitude and using the fact the Q factors are equal
	double alpha = (1.0/transfer.order) * asinh((1.0/shared.epsilon));
	for(int m = 1; m <= transfer.order; m++)
	{
		complex temp;
		complex tempInverse;
		double magnitude;
		double magnitudeInverse;
		double Q;
		double Qim;
		bool negative;
		negative = false;
		temp.re = sin((M_PI_2 * (((2.0 * m) - 1.0) / transfer.order))) * sinh(alpha);
		temp.im = cos((M_PI_2 * (((2.0 * m) - 1.0) / transfer.order))) * cosh(alpha);
		if(temp.im < 0.0)
		{
			negative = true;
		}
		// Fix rounding error
		if(temp.im > -0.0000000001 && temp.im < 0.0000000001)
		{
			temp.im = 0.0;
		}
		magnitude = sqrt(temp.re * temp.re + temp.im * temp.im);
		magnitudeInverse = 1.0 / magnitude;
		Q = magnitude / (2 * temp.re); // Q value
		tempInverse.re = magnitudeInverse / ( 2 * Q );
		tempInverse.im = sqrt(magnitudeInverse * magnitudeInverse - tempInverse.re * tempInverse.re);
		if(negative)
		{
			tempInverse.im = -tempInverse.im;
		}
		tempInverse.re = tempInverse.re * -shared.wc;
		tempInverse.im = tempInverse.im * shared.wc;
		transfer.poles.push_back(tempInverse);
	}
	return transfer;
}

sepPoles findConjugates(transferFunct transfer)
{
    sepPoles result;
    std::vector<complex> temp = transfer.poles;
	std::vector<int> matchedIndices;

    for (int i = 0; i < temp.size(); i++)
    {
        if (temp[i].im != 0)
        {
            bool foundConjugate = false; // Flag to track if a conjugate is found

            for (int j = i + 1; j < temp.size(); j++)
            {
                if (std::abs(temp[i].re - temp[j].re) < 1e-10 && std::abs(temp[i].im - (-temp[j].im)) < 1e-10)
                {
                    result.conjugates.push_back(temp[i]);
					matchedIndices.push_back(i);
					matchedIndices.push_back(j);
                    foundConjugate = true;
                    break;
                }
            }
			bool isAlreadyMatched = std::find(matchedIndices.begin(), matchedIndices.end(), i) != matchedIndices.end();
            if (!foundConjugate && !isAlreadyMatched)
            {
                result.standAlone.push_back(temp[i]);
            }
        }
        else
        {
            result.standAlone.push_back(temp[i]);
        }
    }

    return result;
}