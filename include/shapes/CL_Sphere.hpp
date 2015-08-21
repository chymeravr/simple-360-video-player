/*
*/
#ifndef _CL_SPHERE_HPP
#define _CL_SPHERE_HPP

#include <shapes/CL_Shape.hpp>

class CL_Sphere :public CL_Shape{
private:
	double errorThres = 0.000001;
	double errorThresSq = errorThres*errorThres;

public:
	CL_Sphere();

	/*
	* Creates a unit sphere.
	* Number of triangles used in the sphere is 8*(4^tesselatationDegree).
	*/
	int createUnitSphere(int tesselationDegree);
	void createAutomaticUVs();
};

#endif
