/*
*/

#ifndef _CL_SHAPE_HPP
#define _CL_SHAPE_HPP

#include <iostream>
#include <vector>
#include <math/CL_LinearAlgebra.hpp>

using namespace std;
using namespace LinearAlgebra;

class CL_Shape{
protected:
	vector<vec3> vertices;
	vector<int> edgeList; //in form of indices
	vector<int> triangleList; //in form of indices
	vector<vec2> uvList;
	bool isIndexed;
public:
	CL_Shape();

	vector<vec3> getVertices();

	vector<double> getVerticesArray();

	vector<int> getTriangleList();

	vector<vec2> getUVList();

	vector<double> getUVListArray();

	bool getIsIndexed();

	/*
	* By default shapes are stored in vertices. This function creates indices. Vertices are store in vertices
	* while triangleList and edgeList contain indices.
	* It returns success(0) or errorCode(-1: vertices is empty -2: triangle cannot be formed as vertices are
	* not multiple of 3);
	*/
	int createIndices();

	/*
		Changes the orientation of the shape. It inverts counterclockwise triangle strip to clockwise and vice-versa
	*/
	void reverseOrientation();

	/*
		make u=1-u
	*/
	void flipU();

	/*
		make v=1-v;
	*/
	void flipV();
};

#endif