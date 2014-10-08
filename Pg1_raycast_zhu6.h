#ifndef _PG1_RAYCAST_ZHU6_H
#define _PG1_RAYCAST_ZHU6_H

#include "resource.h"

namespace calculate
{
	struct vw  //the coordinate x1,y1 in 2x2 front plane
	{
		float x1;
		float y1;
	};

	struct coefficient //the coefficient of parameter equation 
	{
		float xt;
		float yt;
		float zt;
	};

	struct coeff_para //the coefficient of parameter equation form 2 points
	{
		float x1;
		float x2;
		float y1;
		float y2;
		float z1;
		float z2;
	};

	struct coeff_plane_equa //the coefficient of triangle plane equation
	{
		float a;
		float b;
		float c;
		float d;
	};

	struct dpoint //the coordinate of a point
	{
		float x;
		float y;
		float z;
	};

}
#endif  // _PG1_RAYCAST_ZHU6_H
