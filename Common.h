#include <iostream>
#include <vector>
#include <string>

#ifndef COMMON_H
#define COMMON_H

#ifndef typeState
typedef enum class typeState { LOW, HIGH, BAND, NOTCH, STATE };

typedef enum class topState { MFB, SK, TWIN};

typedef enum class catState { BUTT, CHEB1, CHEB2, BESL, ELIP};

struct complex
{
    double re;
    double im;
};

struct firstOrderTransferFunct
{
	std::string numerator;
	std::string denominator;
	double n0;
	double d0;
	double d1;
};

struct secondOrderTransferFunct
{
	std::string numerator;
	std::string denominator;
	double n0;
	double d0;
	double d1;
	double d2;
};

struct transferFunct
{
	firstOrderTransferFunct firstOrder;
	secondOrderTransferFunct secondOrder1;
	secondOrderTransferFunct secondOrder2;
	std::vector<complex> poles;
	int order;
};

struct commonVars
{
	typeState type;
	topState topography;
	catState category;
	transferFunct transfer;
	// Parameter Inputs
	double gain;
	double cutOff;
	double stopFreq;
	double passAtt;
	double stopAtt;
	double maxOsc;
	// Calculated Values
	double order = 2.0;
	double wc;
	double ws;
	double epsilon;
};

extern commonVars shared;

#endif


#endif