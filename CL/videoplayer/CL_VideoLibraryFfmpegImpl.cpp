#include <videoplayer/CL_VideoLibrary.hpp>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
}

class CL_VideoLibraryFfmpegImpl :public CL_VideoLibrary{
private:
	char *videoFilename = "";
	int videoResWidth=0;
	int videoResHeight=0;
	AVFormatContext *pFormatCtx = NULL;
	int videoStreamChannel;
	AVCodecContext *pCodecCtxOrig = NULL;
	AVCodec *pCodec = NULL;
	AVCodecContext *pCodecCtx = NULL;
	AVFrame *pFrame = NULL;
	AVFrame *rgbFrame = NULL;
	SwsContext *swsCtx = NULL;
	AVPacket packet;
	mutex *videoStreamMutex;
	bool isStreamingFinished = false;
	
	void setVideoStreamMutex(){
		this->videoStreamMutex = new mutex();
	}
public:
	void setVideoFilename(char *filename){
		this->videoFilename = filename;
	}

	int init(){
		av_register_all();
		// Open video file
		int error = avformat_open_input(&pFormatCtx, videoFilename, NULL, NULL);
		if (error != 0)
			return -1; // Couldn't open file
		// Retrieve stream information
		if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
			return -1; // Couldn't find stream information

		// Dump information about file onto standard error
		av_dump_format(pFormatCtx, 0, "VideoLibraryFfmpegImpl", 0);

		// Find the first video stream
		videoStreamChannel = -1;
		for (int i = 0; i < pFormatCtx->nb_streams; i++){
			if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStreamChannel = i;
				break;
			}
		}
		if (videoStreamChannel == -1)
			return -1; // Didn't find a video stream

		// Get a pointer to the codec context for the video stream
		pCodecCtxOrig = pFormatCtx->streams[videoStreamChannel]->codec;
		// Find the decoder for the video stream
		pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
		if (pCodec == NULL) {
			fprintf(stderr, "Unsupported codec!\n");
			return -1; // Codec not found
		}

		// Copy context
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
			fprintf(stderr, "Couldn't copy codec context");
			return -1; // Error copying codec context
		}

		// Open codec
		if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
			return -1; // Could not open codec

		// Allocate video frame
		pFrame = av_frame_alloc();
		rgbFrame = av_frame_alloc();
		int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
			pCodecCtx->height);
		uint8_t *buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
		avpicture_fill((AVPicture *)rgbFrame, buffer, AV_PIX_FMT_RGB24,
			pCodecCtx->width, pCodecCtx->height);

		swsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
			PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);

		videoResWidth = pCodecCtx->width;
		videoResHeight = pCodecCtx->height;

		//initialize the queue
		videoStreamQueue = queue<void *>();
		setVideoStreamMutex();
		return 0; //all is well
	}

	thread startVideoStreamThread(){
		return thread([this] { this->videoStreamingLoop(); });
	}
	
	int getVideoResolutionWidth(){
		return this->videoResWidth;
	}

	int getVideoResolutionHeight(){
		return this->videoResHeight;
	}
	
	void videoStreamingLoop(){
		int frameFinished = 0;
		while (av_read_frame(pFormatCtx, &packet) >= 0){
			if (packet.stream_index == videoStreamChannel){//is this a packet from video stream
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet); //decode video frame
				if (frameFinished){ //did we get a video frame
					sws_scale(swsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, rgbFrame->data, rgbFrame->linesize);
					videoStreamMutex->lock();
					videoStreamQueue.push(rgbFrame->data[0]);
					videoStreamMutex->unlock();
					av_free_packet(&packet);
				}
			}
		}
		isStreamingFinished = true;
	}

	void *getNextVideoFrame(){
		void *retVal = NULL;
		this->videoStreamMutex->lock();
		if (!videoStreamQueue.empty()){
			retVal = videoStreamQueue.front();
			videoStreamQueue.pop();
		}
		this->videoStreamMutex->unlock();
		return retVal;
	}

	void close(){
		av_frame_free(&pFrame);
		av_frame_free(&rgbFrame);

		// Close the codec
		avcodec_close(pCodecCtx);
		avcodec_close(pCodecCtxOrig);

		// Close the video file
		avformat_close_input(&pFormatCtx);

		if (this->videoStreamMutex)
			free(this->videoStreamMutex);
	}

	bool hasVideoStreamingFinished(){
		return isStreamingFinished;
	}
};