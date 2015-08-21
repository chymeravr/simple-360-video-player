#include<shapes/CL_Sphere.hpp>
/*
	Helper functions
*/

vector<vec3> tesselateTriangeForUnitSphere(vec3 a, vec3 b, vec3 c, int tesselN){
	if (tesselN == 0){
		vector<vec3> v(3);
		v[0] = a;
		v[1] = b;
		v[2] = c;
		return v;
	}
	vec3 ab = (a + b) / 2; //midpoints of triangle edges
	vec3 bc = (b + c) / 2;
	vec3 ca = (c + a) / 2;

	ab = ab *(1 / ab.length()); //intersection with unit sphere at origin
	bc = bc *(1 / bc.length());
	ca = ca *(1 / ca.length());

	vector<vec3> aTriangles = tesselateTriangeForUnitSphere(a, ab, ca, tesselN - 1);
	vector<vec3> bTriangles = tesselateTriangeForUnitSphere(b, bc, ab, tesselN - 1);
	vector<vec3> cTriangles = tesselateTriangeForUnitSphere(c, ca, bc, tesselN - 1);
	vector<vec3> bcTriangles = tesselateTriangeForUnitSphere(bc, ca, ab, tesselN - 1);

	vector<vec3> resTriangles = aTriangles;
	resTriangles.insert(resTriangles.end(), bTriangles.begin(), bTriangles.end()); //concatenate all the triangles
	resTriangles.insert(resTriangles.end(), cTriangles.begin(), cTriangles.end());
	resTriangles.insert(resTriangles.end(), bcTriangles.begin(), bcTriangles.end());

	return resTriangles;
}


/*
	Class functions
*/

/*
	Constructor
*/
CL_Sphere::CL_Sphere(){

}

/*
	Unit sphere is to be created using recursive polyhedron sub-division technique.
	It starts with inscribing a octahedron on the sphere. Each triangle is then recursively
	divided into four equal triangles by using midpoints of the triangle edges. Then for each
	of the new vertices, which is not on the sphere, a line is drawn intersecting the sphere.
	The coordinates of the newly formed triangle vertices are updated, which are on the sphere.
*/
int CL_Sphere::createUnitSphere(int tesselationDegree){
	//Initial octahedron
	vector<vec3> vList;
	//vList.push_back(vec3(1.0, 0.0, 0.0));//0
	vList.push_back(vec3(1.0, 0.0, 0.0));//0
	vList.push_back(vec3(0.0, 0.0, -1.0));//1
	vList.push_back(vec3(-1.0, 0.0, 0.0));//2=2_1 splitting vertex one into two
	vList.push_back(vec3(-errorThres, 0.0, 1.0-errorThres));//3
	vList.push_back(vec3(0.0, 1.0, 0.0));//4
	vList.push_back(vec3(0.0, -1.0, 0.0));//5
	vList.push_back(vec3(errorThres, 0.0, 1.0-errorThres));//6=1_2 splitting vertex one into two

	vector< vector<int> > tList;//By default computing clockwise
	tList.push_back(vector<int>({ 0, 1, 4 }));
	tList.push_back(vector<int>({ 1, 2, 4 }));
	tList.push_back(vector<int>({ 2, 3, 4 }));
	tList.push_back(vector<int>({ 6, 0, 4 }));
	tList.push_back(vector<int>({ 1, 0, 5 }));
	tList.push_back(vector<int>({ 2, 1, 5 }));
	tList.push_back(vector<int>({ 3, 2, 5 }));
	tList.push_back(vector<int>({ 0, 6, 5 }));
	//tList.push_back(vector<int>({6, 0, 4}));
	//tList.push_back(vector<int>({6, 5, 0}));
	//call tesselation function
	vector<vec3> resList;
	for (vector< vector<int> >::iterator it = tList.begin(); it < tList.end(); it++){
		double value = (*it)[0];
		int valueInt = (int)value;
		vec3 value1 = vList[valueInt];
		vector<vec3> tempList = tesselateTriangeForUnitSphere(vList[(*it)[0]], vList[(*it)[1]], vList[(*it)[2]], tesselationDegree);
		resList.insert(resList.end(), tempList.begin(), tempList.end());
	}
	vertices = resList;
	return 0;
}

void CL_Sphere::createAutomaticUVs(){
	uvList = vector<vec2>(vertices.size());
	int i = 0;
	for (vector<vec3>::iterator it = vertices.begin(); it < vertices.end(); it++){
		vec3 vertex = (*it);
		vec2 uv;
		double theta = asin(vertex[1]);
		uv[1] = (theta + M_PI / 2) / M_PI;

		double phi;
		double y2 = vertex[1] * vertex[1];
		if ((1-y2) < errorThresSq){
			double planarDistance = sqrt(vertex[0] * vertex[0] + vertex[2] * vertex[2]);
			if (planarDistance == 0){
				phi = - M_PI/2;
			}
			else if (planarDistance < errorThres){
				phi = vertex[2] * (M_PI / 2) / planarDistance;
			}
			else{
				phi = asin(vertex[2] / planarDistance);
			}
		}
		else{
			phi = vertex[2] / sqrt(1 - y2);
			if (phi > 1) //checking if due to numerical error argument is not out of bound
				phi = 1;
			if (phi < -1)
				phi = -1;
			phi = asin(phi);
		}

		if (vertex[0] <= 0){
			phi = 0.5 * M_PI - phi;
		}
		else{
			phi = 1.5 * M_PI + phi;
		}
		uv[0] = phi / (2 * M_PI);
		uvList[i++] = uv;
	}
}
