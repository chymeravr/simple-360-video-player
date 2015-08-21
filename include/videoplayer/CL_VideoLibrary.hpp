#ifndef _CL_VIDEOLIBRARY_HPP
#define _CL_VIDEOLIBRARY_HPP

#include<iostream>
#include<queue>
#include<mutex>
#include<thread>

using namespace std;

class CL_VideoLibrary{
protected:
	queue<void*> videoStreamQueue;
public:
	virtual void setVideoFilename(char *filename) = 0;
	virtual int init() = 0;
	virtual int getVideoResolutionWidth() = 0;
	virtual int getVideoResolutionHeight() = 0;
	virtual thread startVideoStreamThread() = 0;
	virtual void* getNextVideoFrame() = 0;
	virtual void close() = 0;
	virtual bool hasVideoStreamingFinished() = 0;
};

#endif