#pragma once

#include <thread>
#include <functional>
extern "C" {
#include <libavformat/avformat.h>
}



typedef std::function<void(unsigned char*, int, bool, int)> IVideoExtractorCallback;


class VideoExtractor
{
public:
	const char* VIDEO_URL = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4";

	VideoExtractor();
	~VideoExtractor();

	void Start(IVideoExtractorCallback callback);

	void Stop();

private:
	IVideoExtractorCallback callback = nullptr;
	std::thread* thread = nullptr;
	bool isRunning = false;

	void run();
	double r2d(AVRational r);
};
