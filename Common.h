#include <iostream>
#include <vector>
#include <string>

#ifndef COMMON_H
#define COMMON_H

#ifndef typeState
typedef enum class typeState { LOW, HIGH, BAND, NOTCH, STATE };

typedef enum class topState { MFB, SK, TWIN};

typedef enum class catState { BUTT, CHEB1, CHEB2, BESL, ELIP};

struct transferFunct
{
	std::string numerator;
	std::string denominator;
	double num;
	double Adenom;
	double Bdenom;
	double Cdenom;
	double Ddenom;
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
	double epsilon;
};

extern commonVars shared;

#endif


#endif