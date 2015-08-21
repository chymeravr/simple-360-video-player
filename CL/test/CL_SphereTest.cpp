#include <shapes/CL_Sphere.hpp>

#include <gmock/gmock.h>
using ::testing::Eq;
#include <gtest/gtest.h>
using ::testing::Test;

class CL_SphereTest : public ::testing::Test
{
protected:
	CL_SphereTest(){}
	~CL_SphereTest(){}

	virtual void SetUp(){}
	virtual void TearDown(){}

	CL_Sphere sphere;
};

TEST(CL_SphereTest, initialVertexSizeEmpty)
{
	CL_Sphere sphere;
	EXPECT_EQ(0, sphere.getVertices().size());
}

TEST(CL_SphereTest, degreeZeroCreateUnitSphereTest){
	CL_Sphere sphere;
	int returnValue = sphere.createUnitSphere(0);
	vector<vec3> vertices = sphere.getVertices();
	EXPECT_EQ(0, returnValue); //no error
	EXPECT_EQ(8, vertices.size()/3); //number of triangles
	
	for (int i = 0; i < vertices.size(); i++){
		EXPECT_DOUBLE_EQ(1, vertices[i].length());
	}
}

TEST(CL_SphereTest, degreeFourCreateUnitSphereTest){
	CL_Sphere sphere;
	int returnValue = sphere.createUnitSphere(4);
	vector<vec3> vertices = sphere.getVertices();
	EXPECT_EQ(0, returnValue); //no error
	EXPECT_EQ(8 * pow(4,4), vertices.size() / 3); //number of triangles 8*4^4
	for (int i = 0; i < vertices.size(); i++){
		EXPECT_DOUBLE_EQ(1, vertices[i].length());
	}
}

TEST(CL_SphereTest, reverseOrientationWithoutIndexingTest){
	CL_Sphere sphere1;
	CL_Sphere sphere2;
	sphere1.createUnitSphere(1);
	sphere2.createUnitSphere(1);
	sphere2.reverseOrientation();
	vector<vec3> vertices1 = sphere1.getVertices();
	vector<vec3> vertices2 = sphere2.getVertices();
	for (int i = 0; i < vertices1.size(); i = i + 3){
		EXPECT_DOUBLE_EQ(vertices1[i][0], vertices2[i][0]);
		EXPECT_DOUBLE_EQ(vertices1[i][1], vertices2[i][1]);
		EXPECT_DOUBLE_EQ(vertices1[i][2], vertices2[i][2]);

		EXPECT_DOUBLE_EQ(vertices1[i+1][0], vertices2[i+2][0]);
		EXPECT_DOUBLE_EQ(vertices1[i+1][1], vertices2[i+2][1]);
		EXPECT_DOUBLE_EQ(vertices1[i+1][2], vertices2[i+2][2]);

		EXPECT_DOUBLE_EQ(vertices1[i+2][0], vertices2[i+1][0]);
		EXPECT_DOUBLE_EQ(vertices1[i+2][1], vertices2[i+1][1]);
		EXPECT_DOUBLE_EQ(vertices1[i+2][2], vertices2[i+1][2]);
	}
}

TEST(CL_SphereTest, indexingTest){
	CL_Sphere sphere1;
	CL_Sphere sphere2;
	sphere1.createUnitSphere(1);
	sphere2.createUnitSphere(1);
	vector<vec3> vertices1 = sphere1.getVertices();
	sphere2.createIndices();
	vector<vec3> vertices2 = sphere2.getVertices();
	EXPECT_GT(vertices1.size(), vertices2.size());
	vector<int> triangles2 = sphere2.getTriangleList();
	for (int i = 0; i < triangles2.size(); i++){
		EXPECT_DOUBLE_EQ(vertices1[i][0], vertices2[triangles2[i]][0]);
		EXPECT_DOUBLE_EQ(vertices1[i][1], vertices2[triangles2[i]][1]);
		EXPECT_DOUBLE_EQ(vertices1[i][2], vertices2[triangles2[i]][2]);
	}
}

TEST(CL_SphereTest, reverseOrientationWithIndexingTest){
	CL_Sphere sphere1;
	CL_Sphere sphere2;
	sphere1.createUnitSphere(1);
	sphere2.createUnitSphere(1);
	sphere2.reverseOrientation();
	vector<vec3> vertices1 = sphere1.getVertices();
	sphere2.createIndices();
	vector<vec3> vertices2 = sphere2.getVertices();
	EXPECT_GT(vertices1.size(), vertices2.size());
	vector<int> triangles2 = sphere2.getTriangleList();
	for (int i = 0; i < triangles2.size()/3; i++){
		EXPECT_DOUBLE_EQ(vertices1[3 * i][0], vertices2[triangles2[3 * i]][0]); //first vertext of triangle. first coordinate
		EXPECT_DOUBLE_EQ(vertices1[3 * i][1], vertices2[triangles2[3 * i]][1]); // second coord
		EXPECT_DOUBLE_EQ(vertices1[3 * i][2], vertices2[triangles2[3 * i]][2]); // third coord

		EXPECT_DOUBLE_EQ(vertices1[3 * i + 1][0], vertices2[triangles2[3 * i + 2]][0]);//second vertex of triangle
		EXPECT_DOUBLE_EQ(vertices1[3 * i + 1][1], vertices2[triangles2[3 * i + 2]][1]);
		EXPECT_DOUBLE_EQ(vertices1[3 * i + 1][2], vertices2[triangles2[3 * i + 2]][2]);

		EXPECT_DOUBLE_EQ(vertices1[3 * i + 2][0], vertices2[triangles2[3 * i + 1]][0]);//third vertex of triangle
		EXPECT_DOUBLE_EQ(vertices1[3 * i + 2][1], vertices2[triangles2[3 * i + 1]][1]);
		EXPECT_DOUBLE_EQ(vertices1[3 * i + 2][2], vertices2[triangles2[3 * i + 1]][2]);
	}
}

TEST(CL_SphereTest, sphereUVTestDegreeOne){
	CL_Sphere sphere;
	sphere.createUnitSphere(1);
	sphere.createAutomaticUVs();
	vector<vec3> vertices = sphere.getVertices();
	vector<vec2> uvList = sphere.getUVList();
	EXPECT_EQ(vertices.size(), uvList.size());
	for (int i = 0; i < uvList.size(); i++){
		EXPECT_GE(1.0, uvList[i][0]);
		EXPECT_LE(0.0, uvList[i][0]);
		EXPECT_GE(1.0, uvList[i][1]);
		EXPECT_LE(0.0, uvList[i][1]);
	}
}

TEST(CL_SphereTest, sphereUVTestDegreeFour){
	CL_Sphere sphere;
	sphere.createUnitSphere(4);
	sphere.createAutomaticUVs();
	vector<vec3> vertices = sphere.getVertices();
	vector<vec2> uvList = sphere.getUVList();
	EXPECT_EQ(vertices.size(), uvList.size());
	for (int i = 0; i < uvList.size(); i++){
		EXPECT_GE(1.0, uvList[i][0]);
		EXPECT_LE(0.0, uvList[i][0]);
		EXPECT_GE(1.0, uvList[i][1]);
		EXPECT_LE(0.0, uvList[i][1]);
	}
}

TEST(CL_SphereTest, sphereUVTestDegreeFourAfterIndexing){
	CL_Sphere sphere;
	sphere.createUnitSphere(4);
	sphere.createIndices();
	sphere.createAutomaticUVs();
	vector<vec3> vertices = sphere.getVertices();
	vector<vec2> uvList = sphere.getUVList();
	EXPECT_EQ(vertices.size(), uvList.size());
	for (int i = 0; i < uvList.size(); i++){
		EXPECT_GE(1.0, uvList[i][0]);
		EXPECT_LE(0.0, uvList[i][0]);
		EXPECT_GE(1.0, uvList[i][1]);
		EXPECT_LE(0.0, uvList[i][1]);
	}
}

TEST(CL_SphereTest, speedTestCreateSphereDegreeSix){
	CL_Sphere sphere;
	sphere.createUnitSphere(6);
}

TEST(CL_SphereTest, speedTestCreateSphereDegreeEight){
	CL_Sphere sphere;
	sphere.createUnitSphere(8);
}

TEST(CL_SphereTest, speedTestCreateUVAfterCreatingSphereDegreeSix){
	CL_Sphere sphere;
	sphere.createUnitSphere(6);
	sphere.createAutomaticUVs();
}

TEST(CL_SphereTest, speedTestCreateUVAfterCreatingSphereDegreeEight){
	CL_Sphere sphere;
	sphere.createUnitSphere(8);
	sphere.createAutomaticUVs();
}

TEST(CL_SphereTest, speedTestCreateIndicesAfterCreatingSphereDegreeSix){
	CL_Sphere sphere;
	sphere.createUnitSphere(6);
	sphere.createIndices();
}

TEST(CL_SphereTest, speedTestCreateUVsAfterCreatingIndicesDegreeSix){
	CL_Sphere sphere;
	sphere.createUnitSphere(6);
	sphere.createIndices();
	sphere.createAutomaticUVs();
}

TEST(CL_SphereTest, waitforoutput){
	cin.get();
}
