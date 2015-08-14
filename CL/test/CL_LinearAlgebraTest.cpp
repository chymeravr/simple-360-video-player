#include<iostream>

#include<math/CL_LinearAlgebra.hpp>

using namespace LinearAlgebra;
using namespace std;

int main(){
	mat44 matrix1(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	mat44 matrix2(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2);
	mat44 matrix3 = matrix1 + matrix2;
	matrix3.print(stdout, "add");
	(matrix1*matrix2).print(stdout, "mutl");
	cin.get();
	return 0;
}