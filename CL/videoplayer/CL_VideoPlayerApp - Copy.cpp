#include<iostream>
#include<videoplayer/CL_VideoLibrary.hpp>
#include"CL_VideoLibraryFfmpegImpl.cpp"

#include<GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

extern "C"{
	#include<SDL.h>
	#include<SDL_opengl.h>
}

#include "common/shader.hpp"
#include <shapes/CL_Sphere.hpp>

using namespace std;

CL_VideoLibrary *videoLibrary;

//GL variables
GLuint vertexArrayId;
GLuint programId;
GLuint mvpId;
GLuint texture;
GLuint textureId;
GLuint vertexBuffer;
GLuint uvBuffer;
GLuint sphereIndexBuffer;
glm::mat4 mvp;

//Data variables
char videoFilename[] = "video.wmv";
/*static const GLfloat vertexBufferData[] = {
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f
};*/
GLfloat *vertexBufferData = NULL;
int vertexBufferDataSize = 0;
// Two UV coordinatesfor each vertex. They were created withe Blender.
/*static const GLfloat uvBufferData[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};*/
GLfloat *uvBufferData = NULL;
int uvBufferDataSize = 0;

unsigned short *sphereIndices = NULL;
int sphereIndicesSize = 0;
int videoResWidth = 0;
int videoResHeight = 0;

GLfloat * convertVec3VectorIntoGLFloatArray(vector<vec3> vec){
	GLfloat *floatArray = new GLfloat[vec.size() * 3];
	for (int i = 0; i < vec.size(); i++){
		floatArray[3 * i] = (GLfloat) vec[i][0];
		floatArray[3 * i + 1] = (GLfloat) vec[i][1];
		floatArray[3 * i + 2] = (GLfloat) vec[i][2];
	}
	return floatArray;
}

GLfloat * convertVec2VectorIntoGLFloatArray(vector<vec2> vec){
	GLfloat *floatArray = new GLfloat[vec.size() * 2];
	for (int i = 0; i < vec.size(); i++){
		floatArray[2 * i] = (GLfloat) vec[i][0];
		floatArray[2 * i + 1] = (GLfloat) vec[i][1];
	}
	return floatArray;
}

unsigned short *convertIntVecToUnsignedShortArray(vector<int> vec){
	unsigned short *shortArray = new unsigned short[vec.size()];
	for (int i = 0; i < vec.size(); i++){
		shortArray[i] = (unsigned short)vec[i];
	}
	return shortArray;
}

int initGL(){
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);
	programId = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	mvpId = glGetUniformLocation(programId, "MVP");

	// Load the texture using any two methods
	//texture = loadBMP_custom(textureFileName);
	// Get a handle for our "myTextureSampler" uniform
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoResWidth, videoResHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	textureId = glGetUniformLocation(programId, "myTextureSampler");

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexBufferDataSize, vertexBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvBufferDataSize, uvBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &sphereIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndicesSize * sizeof(unsigned short), sphereIndices, GL_STATIC_DRAW);

	return 0;
}

void setViewport(){
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 0, 0), // Location
		glm::vec3(0, 0, -1), // looks at
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 scale(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(0, 0, 0, 1)
		);
	// Our ModelViewProjection : multiplication of our 3 matrices
	mvp = projection * view * model * scale; // Remember, matrix multiplication is the other way around
//	mvp = projection * view * model;
}

void display(){
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	// Use our shader
	glUseProgram(programId);
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureId, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexBuffer);

	// Draw the triangle !
	//glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles
	//glDrawArrays(GL_TRIANGLES, 0, vertexBufferDataSize/3);
	glDrawElements(GL_TRIANGLES, 
		sphereIndicesSize, 
		GL_UNSIGNED_SHORT, 
		(void*)0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}



int main(int argc, char *argv[]){
	CL_Sphere sphere;
	sphere.createUnitSphere(6);
	sphere.createIndices();
	sphere.reverseOrientation();
	sphere.createAutomaticUVs();
	sphere.flipV();
	vertexBufferData = convertVec3VectorIntoGLFloatArray(sphere.getVertices());
	vertexBufferDataSize = sphere.getVertices().size() * 3;
	uvBufferData = convertVec2VectorIntoGLFloatArray(sphere.getUVList());
	uvBufferDataSize = sphere.getUVList().size() * 2;
	sphereIndices = convertIntVecToUnsignedShortArray(sphere.getTriangleList());
	sphereIndicesSize = sphere.getTriangleList().size();
	/*for (int i = 0; i < sphere.getVertices().size(); i++){
		cout << "i:" << i;
		cout << "(" << vertexBufferData[3 * i] << ",";
		cout << vertexBufferData[3 * i + 1] << ",";
		cout << vertexBufferData[3 * i + 2] << ") ";
		cout << "(" << uvBufferData[2 * i] << ",";
		cout << uvBufferData[2 * i + 1] << ")" << endl;
	}
	cin.get();*/
	SDL_Window *window = NULL;
	SDL_Event event;

	videoLibrary = new CL_VideoLibraryFfmpegImpl();
	videoLibrary->setVideoFilename(videoFilename);
	if (videoLibrary->init() == -1){
		return -1;
	}
	videoResWidth = videoLibrary->getVideoResolutionWidth();
	videoResHeight = videoLibrary->getVideoResolutionHeight();

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"Ffmpeg_opengl",
		2,//SDL_WINDOWPOS_UNDEFINED,
		2,//SDL_WINDOWPOS_UNDEFINED,
		videoResWidth, videoResHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!window){ /* Die if creation failed */
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	SDL_GL_CreateContext(window);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

	initGL();
	setViewport();

	thread videoStreamThread = videoLibrary->startVideoStreamThread();
	while (true){
		GLvoid *nextVideoFrame = videoLibrary->getNextVideoFrame();
		if (nextVideoFrame == NULL){
			if (videoLibrary->hasVideoStreamingFinished())
				break;
			continue;
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoResWidth, videoResHeight, GL_RGB, GL_UNSIGNED_BYTE, nextVideoFrame);
		display();
		SDL_GL_SwapWindow(window);
		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		default:
			break;
		}
	}
	videoStreamThread.join();
	/*
		Todo: free vertex buffer data and uvbufferdata
	*/
	return 0;
}