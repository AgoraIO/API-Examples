#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include <direct.h>
#include <Windows.h>


typedef std::function<void(int, int, unsigned char*, int)> IYUVCallback;

class YUVReader
{

public:
	const static int VIDEO_WIDTH = 320;
	const static int VIDEO_HEIGHT = 180;
	const static int VIDEO_FRAME_SIZE = VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2;
	const static int VIDEO_FRAME_RATE = 15;
	const static int VIDEO_FRAME_INTERVAL_MS = 1000 / VIDEO_FRAME_RATE;


	YUVReader();

	bool start(IYUVCallback callback);

	void stop();

private:
	

	IYUVCallback callback = nullptr;
	std::thread* thread = nullptr;
	bool isReading = false;
	std::string filePath;
	unsigned char buffer[VIDEO_FRAME_SIZE]{0};

	void run();
};
