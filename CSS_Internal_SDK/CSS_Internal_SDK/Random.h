#ifndef RANDOM_H
#define RANDOM_H

#include <math.h>

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define MAX_RANDOM_RANGE 0x7FFFFFFFUL

#define AM (1.0/IM)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

class cRandom
{
private:
	int		m_idum;
    int		m_iy;
   	int		m_iv[ NTAB ];
	int		GenerateRandomNumber( );
public:
	void	SetSeed( int iSeed );
    float	RandomFloat( float flMinVal = 0.0f, float flMaxVal = 1.0f );
};
extern cRandom Random;

#endif