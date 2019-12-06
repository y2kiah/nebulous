/*----==== FASTMATH.H ====----
	Author: Jeff Kiah
	Date:	4/9/2007
----------------------------*/

#include "FastMath.h"

///// FUNCTIONS /////

/*=============================================================================
	There is no error correction available for the precision arguement
	since all const values in the class are based on it so it is up to
	the programmer to supply a legal value. Anything under 10 or over 100
	should be considered out of the norm. In a debug build the program
	asserts the value of precision.
=============================================================================*/
/*FastMath::FastMath(uint precision) : Singleton<FastMath>(*this),
	ANGLE360(precision*360),
	ANGLE315(precision*315),
	ANGLE271(precision*271),
	ANGLE270(precision*270),
	ANGLE225(precision*225),
	ANGLE180(precision*180),
	ANGLE135(precision*135),
	ANGLE90(precision*90),
	ANGLE89(precision*89),
	ANGLE60(precision*60),
	ANGLE45(precision*45),
	ANGLE40(precision*40),
	ANGLE30(precision*30),
	ANGLE20(precision*20),
	ANGLE15(precision*15),
	ANGLE10(precision*10),
	ANGLE5(precision*5),
	ANGLE2(precision*2),
	ANGLE1(precision)
{
	_ASSERTE(precision >= 1);

	mAnglePrecision = precision;
	mAngleIncrement = 1.0f / (float)precision;

	// set up the lookup tables
	mSinTable = new float[ANGLE90];
	mTanTable = new float[ANGLE90];

	float angleFix = mAngleIncrement * DEGTORADf;
	for (uint a = 0; a < ANGLE90; ++a) {
		mSinTable[a] = sinf((float)a * angleFix);
		mTanTable[a] = tanf((float)a * angleFix);
	}
}

FastMath::~FastMath()
{
	delete [] mSinTable;
	delete [] mTanTable;
}*/