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
};

struct commonVars
{
	typeState type;
	topState topography;
	catState category;
	transferFunct transfer;
	int cutOff;
	int gain;
};

extern commonVars shared;

#endif


#endif