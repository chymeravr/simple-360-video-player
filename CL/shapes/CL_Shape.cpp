/*
*/

#include<shapes/CL_Shape.hpp>
#include<hash_map>

/*
	Helper Functions
*/
string convertVec3ToString(vec3 v){
	char str[256];
	sprintf_s(str, "x%fy%fz%f", v[0], v[1], v[2]);
	string s(str);
	return s;
}

/*
	Class Functions
*/
CL_Shape::CL_Shape(){
	this->isIndexed = false;
}

vector<vec3> CL_Shape::getVertices(){
	return this->vertices;
}

vector<double> CL_Shape::getVerticesArray(){
	vector<double> verticesArray(vertices.size() * 3);
	for (int i = 0; i < vertices.size();i++){
		verticesArray[3 * i] = vertices[i][0];
		verticesArray[3 * i + 1] = vertices[i][1];
		verticesArray[3 * i + 2] = vertices[i][2];
	}
	return verticesArray;
}

vector<int> CL_Shape::getTriangleList(){
	return this->triangleList;
}

vector<vec2> CL_Shape::getUVList(){
	return this->uvList;
}

vector<double> CL_Shape::getUVListArray(){
	vector<double> uvListArray(this->uvList.size() * 2);
	for (int i = 0; i < uvList.size(); i++){
		uvListArray[2 * i] = uvList[i][0];
		uvListArray[2 * i + 1] = uvList[i][1];
	}
	return uvListArray;
}

bool CL_Shape::getIsIndexed(){
	return this->isIndexed;
}

int CL_Shape::createIndices(){
	if (vertices.size() == 0)
		return -1;
	if (vertices.size() % 3 != 0)
		return -2;
	if (this->getIsIndexed())
		return 0;
	hash_map<string, int> hash;
	int index = 0;
	vector<vec3>::iterator it;
	vector<vec3> newVertices;
	triangleList = vector<int>();

	bool processUV = false;
	if (uvList.size() == vertices.size()){
		true;
	}
	vector<vec2> newUvList;
	vector<vec2>::iterator uvIt = uvList.begin();
	for (it = vertices.begin(); it < vertices.end(); it = it + 3){
		vec3 triangle;
		string a = convertVec3ToString(*it);
		string b = convertVec3ToString(*(it + 1));
		string c = convertVec3ToString(*(it + 2));
		if (hash.find(a) == hash.end()){//new value
			newVertices.push_back(*it);
			if (processUV)
				newUvList.push_back(*uvIt);
			hash[a] = index++;
		}
		if (hash.find(b) == hash.end()){
			newVertices.push_back(*(it + 1));
			if (processUV)
				newUvList.push_back(*(uvIt+1));
			hash[b] = index++;
		}
		if (hash.find(c) == hash.end()){
			newVertices.push_back(*(it + 2));
			if (processUV)
				newUvList.push_back(*(uvIt + 2));
			hash[c] = index++;
		}
		triangleList.push_back(hash[a]);
		triangleList.push_back(hash[b]);
		triangleList.push_back(hash[c]);
		if (processUV)
			uvIt +=	3;
	}
	vertices = vector<vec3>();
	vertices = newVertices;

	uvList = vector<vec2>();
	uvList = newUvList;
	this->isIndexed = true;
	return 0;
}

void CL_Shape::reverseOrientation(){
	if (this->getIsIndexed()){
		vector<int>::iterator it;
		for (it = triangleList.begin(); it < triangleList.end(); it = it + 3){
			int swap = *(it + 1);
			*(it + 1) = *(it + 2);
			*(it + 2) = swap;
		}
	}
	else{
		vector<vec3>::iterator it;
		for (it = vertices.begin(); it < vertices.end(); it = it + 3){
			vec3 swap = *(it + 1);
			*(it + 1) = *(it + 2);
			*(it + 2) = swap;
		}
	}
}

void CL_Shape::flipU(){
	vector<vec2>::iterator it;
	for (it = uvList.begin(); it < uvList.end(); it++){
		(*it)[0] = 1 - (*it)[0];
	}
}

void CL_Shape::flipV(){
	vector<vec2>::iterator it;
	for (it = uvList.begin(); it < uvList.end(); it++){
		(*it)[1] = 1 - (*it)[1];
	}
}