#ifndef MATH_H
#define MATH_H

#include "stdafx.h"
#include "Globals.h"

#define M_RADPI 57.295779513082f

bool	CalculateRadarPoint( int Coords[ 2 ], int size, float localyaw, Vector localOrigin, Vector AbsOrigin, int range, float *Point );
float	DegToRad( float alpha );
void	CalcAngles( Vector src, Vector dst, Vector &ang );
void	CalcAngles( Vector src, Vector dst, QAngle &ang );
void	MakeVector( QAngle pfIn, QAngle &pfOut );
void	MakeVector( QAngle pfIn, Vector &pfOut );
float	AngleDif( QAngle angle, Vector src, Vector dst );
bool    WorldToScreen( Vector &vOrigin, Vector &vScreen );
float	_VectorNormalize( Vector &vec );
float   _QAngleNormalize( QAngle &vec );
//void	VectorTransform( Vector in1, matrix3x4_t in2, Vector &out );
float	DotProduct( Vector a, Vector b );
#endif