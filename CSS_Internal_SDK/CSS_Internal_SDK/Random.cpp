#include "Random.h"

cRandom Random;

void cRandom::SetSeed( int iSeed )
{
	m_idum = ( ( iSeed < 0 ) ? iSeed : -iSeed );
	m_iy = 0;
}

int cRandom::GenerateRandomNumber()
{
	int j;
	int k;
	
	if( m_idum <= 0 || !m_iy )
	{
		if( - ( m_idum ) < 1 ) m_idum=1;
		else m_idum = -( m_idum );

		for( j = NTAB + 7; j >= 0; j-- )
		{
			k = ( m_idum ) / IQ;
			m_idum = IA * ( m_idum - k * IQ ) - IR * k;
			if( m_idum < 0 ) m_idum += IM;
			if( j < NTAB ) m_iv[ j ] = m_idum;
		}
		m_iy = m_iv[ 0 ];
	}
	k = ( m_idum ) / IQ;
	m_idum = IA * ( m_idum - k * IQ ) - IR * k;
	if( m_idum < 0 ) m_idum += IM;
	j = m_iy / NDIV;
	m_iy = m_iv[ j ];
	m_iv[ j ] = m_idum;

	return m_iy;
}

float cRandom::RandomFloat( float flLow, float flHigh )
{
	float fl = AM * GenerateRandomNumber( );
	if( fl > RNMX ) fl = RNMX;
	return ( fl * ( flHigh - flLow ) ) + flLow;
}
