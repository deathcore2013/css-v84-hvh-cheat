#include "Math.h"


void CalcAngles( Vector src, Vector dst, Vector &ang )
{
	double delta[ 3 ] = { src[ 0 ] - dst[ 0 ], src[ 1 ] - dst [ 1 ] , src[ 2 ] - dst[ 2 ] };
	double hyp = sqrt( delta[ 0 ] * delta[ 0 ] + delta[ 1 ] * delta[ 1 ] );

	ang[ 0 ] = ( float ) ( atan( delta[ 2 ] / hyp ) * M_RADPI );
	ang[ 1 ] = ( float ) ( atan( delta[ 1 ] / delta[ 0 ] ) * M_RADPI );
	ang[ 2 ] = 0.0f;

	if( delta[ 0 ] >= 0.0 ) ang[ 1 ] += 180.0f;
}

void CalcAngles( Vector src, Vector dst, QAngle &ang )
{
	double delta[ 3 ] = { src[ 0 ] - dst[ 0 ], src[ 1 ] - dst [ 1 ] , src[ 2 ] - dst[ 2 ] };
	double hyp = sqrt( delta[ 0 ] * delta[ 0 ] + delta[ 1 ] * delta[ 1 ] );

	ang[ 0 ] = ( float ) ( atan( delta[ 2 ] / hyp ) * M_RADPI );
	ang[ 1 ] = ( float ) ( atan( delta[ 1 ] / delta[ 0 ] ) * M_RADPI );
	ang[ 2 ] = 0.0f;

	if( delta[ 0 ] >= 0.0 ) ang[ 1 ] += 180.0f;
}

void MakeVector( QAngle pfIn, Vector &pfOut )
{
	float pitch;
	float yaw;
	float tmp;		
	
	pitch = ( float ) ( pfIn[ 0 ] * M_PI / 180);
	yaw = ( float ) ( pfIn[ 1 ] * M_PI / 180);
	tmp = ( float ) cos( pitch );
	
	pfOut[ 0 ] = ( float ) ( -tmp * -cos( yaw ) );
	pfOut[ 1 ] = ( float ) ( sin( yaw )*tmp );
	pfOut[ 2 ] = ( float ) -sin( pitch );
}

void MakeVector( QAngle pfIn, QAngle &pfOut )
{
	float pitch;
	float yaw;
	float tmp;		
	
	pitch = ( float ) ( pfIn[ 0 ] * M_PI / 180);
	yaw = ( float ) ( pfIn[ 1 ] * M_PI / 180);
	tmp = ( float ) cos( pitch );
	
	pfOut[ 0 ] = ( float ) ( -tmp * -cos( yaw ) );
	pfOut[ 1 ] = ( float ) ( sin( yaw )*tmp );
	pfOut[ 2 ] = ( float ) -sin( pitch );
}

float AngleDif( QAngle angle, Vector src, Vector dst )
{ 
	QAngle ang;
	Vector aim;
	float angDifference; 

	CalcAngles( src, dst, ang ); 
	MakeVector( angle, aim ); 
	MakeVector( ang, ang );      

	float mag_s = sqrt( ( aim[ 0 ] * aim[ 0 ] ) + ( aim[ 1 ] * aim[ 1 ] ) + ( aim[ 2 ] * aim[ 2 ] ) ); 
	float mag_d = sqrt( ( aim[ 0 ] * aim[ 0 ] ) + ( aim[ 1 ] * aim[ 1 ] ) + ( aim[ 2 ] * aim[ 2 ] ) ); 

	float u_dot_v = aim[ 0 ] * ang[ 0 ] + aim[ 1 ] * ang[ 1]  + aim[ 2 ] * ang[ 2 ]; 

	angDifference = acos( u_dot_v / ( mag_s*mag_d ) ) * ( 180.0f / M_PI ); 

	return angDifference; 
}

float DegToRad( float alpha )
{
	return ( float ) ( alpha / 180.0f * M_PI );
}

bool WorldToScreen( Vector &vOrigin, Vector &vScreen )
{
	const VMatrix& worldToScreen = g_pEngine->WorldToScreenMatrix( );

	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3];
	vScreen.z = 0;

	if( w > 0.001 )
	{
		float fl1DBw = 1 / w;
		vScreen.x = (x / 2) + ( 0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * fl1DBw) * x + 0.5);
		vScreen.y = (y / 2) - ( 0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * fl1DBw) * y + 0.5);
		return true;
	}

	return false;
}

bool CalculateRadarPoint( int Coords[ 2 ], int size, float localyaw, Vector localOrigin, Vector AbsOrigin, int range, float *Point)
{
	float middleX = ( float )( Coords[ 0 ] + ( size / 2.0f ) );
	float middleY = ( float )( Coords[ 1 ] + ( size / 2.0f ) );

	float cosyaw = cos( DegToRad( localyaw ) );
	float sinyaw = sin( DegToRad( localyaw ) );

	float mappointx = AbsOrigin.x - localOrigin.x;
	float mappointy = AbsOrigin.y - localOrigin.y;

	float newx = ( mappointy * cosyaw - mappointx * sinyaw ) / range;
	float newy = ( mappointx * cosyaw + mappointy * sinyaw ) / range;

	Point[ 0 ] = newx+middleX;
	Point[ 1 ] = -newy+middleY;

	if( Point[ 0 ] < middleX ) 
	{
		float tmp = middleX - Point[ 0 ];
		Point[ 0 ] = middleX + tmp;
	}
	else if( Point[ 0 ] > middleX )
	{
		float tmp = Point[ 0 ] - middleX;
		Point[ 0 ] = middleX - tmp;
	}

	if(Point[ 0 ] > Coords[ 0 ] && Point[ 0 ]< Coords[ 0 ] + size && Point[ 1 ] > Coords[ 1 ] && Point[ 1 ] < Coords[ 1 ] + size) return true;
	else return false;
}

float _VectorNormalize( Vector &vec )
{
	float radius = sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.f / ( radius + FLT_EPSILON );
	
	vec.x *= iradius;
	vec.y *= iradius;
	vec.z *= iradius;
	
	return radius;
}

float _QAngleNormalize( QAngle &vec )
{
	float radius = sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.f / ( radius + FLT_EPSILON );
	
	vec.x *= iradius;
	vec.y *= iradius;
	vec.z *= iradius;
	
	return radius;
}

float DotProduct( Vector a, Vector b ) 
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

/*void VectorTransform( float *in1, matrix3x4_t in2, Vector &out)
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
} */