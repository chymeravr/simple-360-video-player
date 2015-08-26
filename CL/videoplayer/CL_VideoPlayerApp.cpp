#include <iostream>
#include <fstream>
#include <oculus/Win32_GLAppUtil.h>
#include <Kernel/OVR_System.h>

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include<videoplayer/CL_VideoLibrary.hpp>
#include"CL_VideoLibraryFfmpegImpl.cpp"

#include "common/shader.hpp"
#include <shapes/CL_Sphere.hpp>

using namespace std;
using namespace OVR;

glm::mat4 mvp;

int videoResWidth = 0;
int videoResHeight = 0;

bool debug = false;

ofstream logfile;

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

class ContainerSphere{
public:
	GLuint vertexArrayId;
	GLuint programId;
	GLuint mvpId;
	GLuint texture;
	GLuint textureId;
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint sphereIndexBuffer;
	GLfloat *vertexBufferData = NULL;
	int vertexBufferDataSize = 0;
	GLfloat *uvBufferData = NULL;
	int uvBufferDataSize = 0;
	CL_Sphere sphere;
	unsigned short *sphereIndices = NULL;
	int sphereIndicesSize = 0;

	int init(){
		// Initialize GLEW
		/*glewExperimental = true; // Needed for core profile
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			return -1;
		}*/
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glGenVertexArrays(1, &vertexArrayId);
		glBindVertexArray(vertexArrayId);
		programId = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
		if (debug)
			logfile << "Shaders loaded" << endl;
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
	void setup(){
		sphere.createUnitSphere(6);
		sphere.createIndices();
		//sphere.reverseOrientation();
		sphere.createAutomaticUVs();
		sphere.flipV();
		vertexBufferData = convertVec3VectorIntoGLFloatArray(sphere.getVertices());
		vertexBufferDataSize = sphere.getVertices().size() * 3;
		uvBufferData = convertVec2VectorIntoGLFloatArray(sphere.getUVList());
		uvBufferDataSize = sphere.getUVList().size() * 2;
		sphereIndices = convertIntVecToUnsignedShortArray(sphere.getTriangleList());
		sphereIndicesSize = sphere.getTriangleList().size();
		if (debug)
			logfile << "Sphere data loaded" << endl;
		this->init();
		if (debug)
			logfile << "GL init done" << endl;
	}

	void render(Matrix4f view, Matrix4f proj){
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
		// Use our shader
		glUseProgram(programId);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		Matrix4f combined = proj * view;
		glUniformMatrix4fv(mvpId, 1, GL_TRUE, (FLOAT*)&combined);
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
};

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	//debug file
	logfile.open("debug.log");
	/*
		Video library initializations and start
	*/

	CL_VideoLibrary *videoLibrary;
	char videoFilename[] = "video.wmv";
	videoLibrary = new CL_VideoLibraryFfmpegImpl();
	videoLibrary->setVideoFilename(videoFilename);
	if (videoLibrary->init() == -1){
		return -1;
	}
	logfile << "Video thread started" << endl;
	videoResWidth = videoLibrary->getVideoResolutionWidth();
	videoResHeight = videoLibrary->getVideoResolutionHeight();

	thread videoStreamThread = videoLibrary->startVideoStreamThread();

	/*
		Initializing oculus
	*/
	OVR::System::Init();

	// Initialise rift
	if (ovr_Initialize(nullptr) != ovrSuccess) { MessageBoxA(NULL, "Unable to initialize libOVR.", "", MB_OK); return 0; }
	ovrHmd HMD;
	ovrResult result = ovrHmd_Create(0, &HMD);
	if (!OVR_SUCCESS(result))
	{
		result = ovrHmd_CreateDebug(ovrHmd_DK2, &HMD);
	}

	if (!OVR_SUCCESS(result)) { MessageBoxA(NULL, "Oculus Rift not detected.", "", MB_OK); ovr_Shutdown(); return 0; }
	if (HMD->ProductName[0] == '\0') MessageBoxA(NULL, "Rift detected, display not enabled.", "", MB_OK);

	// Setup Window and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
	ovrSizei windowSize = { HMD->Resolution.w / 2, HMD->Resolution.h / 2 };
	if (!Platform.InitWindowAndDevice(hinst, Recti(Vector2i(0), windowSize), true, L"Oculus Room Tiny (GL)"))
		return 0;

	// Make eye render buffers
	TextureBuffer * eyeRenderTexture[2];
	DepthBuffer   * eyeDepthBuffer[2];
	for (int i = 0; i<2; i++)
	{
		ovrSizei idealTextureSize = ovrHmd_GetFovTextureSize(HMD, (ovrEyeType)i, HMD->DefaultEyeFov[i], 1);
		eyeRenderTexture[i] = new TextureBuffer(HMD, true, true, idealTextureSize, 1, NULL, 1);
		eyeDepthBuffer[i] = new DepthBuffer(eyeRenderTexture[i]->GetSize(), 0);
	}

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	ovrGLTexture* mirrorTexture;
	ovrHmd_CreateMirrorTextureGL(HMD, GL_RGBA, windowSize.w, windowSize.h, (ovrTexture**)&mirrorTexture);
	// Configure the mirror read buffer
	GLuint mirrorFBO = 0;
	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	ovrEyeRenderDesc EyeRenderDesc[2];
	EyeRenderDesc[0] = ovrHmd_GetRenderDesc(HMD, ovrEye_Left, HMD->DefaultEyeFov[0]);
	EyeRenderDesc[1] = ovrHmd_GetRenderDesc(HMD, ovrEye_Right, HMD->DefaultEyeFov[1]);

	ovrHmd_SetEnabledCaps(HMD, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

	// Start the sensor
	ovrHmd_ConfigureTracking(HMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection |
		ovrTrackingCap_Position, 0);

	// Turn off vsync to let the compositor do its magic
	wglSwapIntervalEXT(0);

	// Make scene - can simplify further if needed
	//Scene roomScene(false);

	if (debug)
		logfile << "OVR initializations done" << endl;
	ContainerSphere sphereCtr;
	sphereCtr.setup();

	bool isVisible = true;

	if (debug)
		logfile << "Sphere initialization done" << endl;
	// Main loop
	while (Platform.HandleMessages())
	{
		// Keyboard inputs to adjust player orientation
		static float Yaw(3.141592f);
		if (Platform.Key[VK_LEFT])  Yaw += 0.02f;
		if (Platform.Key[VK_RIGHT]) Yaw -= 0.02f;

		// Keyboard inputs to adjust player position
		static Vector3f Pos2(0.0f, 0.0f, 0.0f);
		if (Platform.Key['W'] || Platform.Key[VK_UP])     Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, -0.05f));
		if (Platform.Key['S'] || Platform.Key[VK_DOWN])   Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, +0.05f));
		if (Platform.Key['D'])                          Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.05f, 0, 0));
		if (Platform.Key['A'])                          Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.05f, 0, 0));
		//Pos2.y = ovrHmd_GetFloat(HMD, OVR_KEY_EYE_HEIGHT, Pos2.y);
		if (debug)
			logfile << "Position y: " << Pos2.y << endl;
		// Animate the cube
		static float cubeClock = 0;
		//roomScene.Models[0]->Pos = Vector3f(9 * sin(cubeClock), 3, 9 * cos(cubeClock += 0.015f));

		// Get eye poses, feeding in correct IPD offset
		ovrVector3f               ViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset,
			EyeRenderDesc[1].HmdToEyeViewOffset };
		ovrPosef                  EyeRenderPose[2];

		ovrFrameTiming   ftiming = ovrHmd_GetFrameTiming(HMD, 0);
		ovrTrackingState hmdState = ovrHmd_GetTrackingState(HMD, ftiming.DisplayMidpointSeconds);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);

		// Get next video frame
		GLvoid *nextVideoFrame = videoLibrary->getNextVideoFrame();
		if (nextVideoFrame == NULL){
			if (videoLibrary->hasVideoStreamingFinished())
				break;
			continue;
		}
		if (debug)
			logfile << "Next frame read" << endl;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sphereCtr.texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoResWidth, videoResHeight, GL_RGB, GL_UNSIGNED_BYTE, nextVideoFrame);

		if (isVisible)
		{
			for (int eye = 0; eye<2; eye++)
			{
				// Increment to use next texture, just before writing
				eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;

				// Switch to eye render target
				eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);

				// Get view and projection matrices
				Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
				Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(EyeRenderPose[eye].Orientation);
				Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
				Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
				Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[eye].Position);

				Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
				Matrix4f proj = ovrMatrix4f_Projection(HMD->DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_RightHanded);
				if (debug)
					logfile << "Shifted eye position: (" << shiftedEyePos.x << "," << shiftedEyePos.y << "," << shiftedEyePos.z << ")" << endl;
				// Render world
				//roomScene.Render(view, proj);
				sphereCtr.render(view, proj);
				if (debug)
					logfile << "Scene rendered for eye " << eye << endl;
				// Avoids an error when calling SetAndClearRenderSurface during next iteration.
				// Without this, during the next while loop iteration SetAndClearRenderSurface
				// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
				// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
				eyeRenderTexture[eye]->UnsetRenderSurface();
			}
		}

		// Do distortion rendering, Present and flush/sync

		// Set up positional data.
		ovrViewScaleDesc viewScaleDesc;
		viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
		viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
		viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

		ovrLayerEyeFov ld;
		ld.Header.Type = ovrLayerType_EyeFov;
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

		for (int eye = 0; eye < 2; eye++)
		{
			ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
			ld.Viewport[eye] = Recti(eyeRenderTexture[eye]->GetSize());
			ld.Fov[eye] = HMD->DefaultEyeFov[eye];
			ld.RenderPose[eye] = EyeRenderPose[eye];
		}

		ovrLayerHeader* layers = &ld.Header;
		ovrResult result = ovrHmd_SubmitFrame(HMD, 0, &viewScaleDesc, &layers, 1);
		isVisible = OVR_SUCCESS(result);

		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = mirrorTexture->OGL.Header.TextureSize.w;
		GLint h = mirrorTexture->OGL.Header.TextureSize.h;
		glBlitFramebuffer(0, h, w, 0,
			0, 0, w, h,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		SwapBuffers(Platform.hDC);
		if (debug)
			logfile << "Final rendering on oculus done" << endl;
	}
	videoStreamThread.join();
	if (debug)
		logfile << "videoStreamThread joined " << endl;
	glDeleteFramebuffers(1, &mirrorFBO);
	ovrHmd_DestroyMirrorTexture(HMD, (ovrTexture*)mirrorTexture);
	ovrHmd_DestroySwapTextureSet(HMD, eyeRenderTexture[0]->TextureSet);
	ovrHmd_DestroySwapTextureSet(HMD, eyeRenderTexture[1]->TextureSet);

	// Release
	ovrHmd_Destroy(HMD);
	ovr_Shutdown();
	Platform.ReleaseWindow(hinst);
	OVR::System::Destroy();
	logfile.close();
	return 0;
}



