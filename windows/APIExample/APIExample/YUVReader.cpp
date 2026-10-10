#include "YUVReader.h"


YUVReader::YUVReader()
{
	char modulePath[MAX_PATH] = { 0 };
	DWORD pathLength = GetModuleFileNameA(nullptr, modulePath, MAX_PATH);
	if (pathLength > 0 && pathLength < MAX_PATH) {
		filePath.assign(modulePath, pathLength);
		size_t separator = filePath.find_last_of("\\/");
		if (separator != std::string::npos) {
			filePath.erase(separator + 1);
		}
		filePath += "sample.yuv";
	}
	else {
		filePath = "sample.yuv";
	}
}

bool YUVReader::start(IYUVCallback callback)
{
	if (thread != nullptr) {
		return true;
	}

	std::ifstream fileStream(filePath, std::ios::binary);
	if (!fileStream) {
		return false;
	}
	fileStream.close();

	isReading = true;
	this->callback = callback;
	thread = new std::thread(std::bind(&YUVReader::run, this));
	return true;
}

void YUVReader::stop()
{
	isReading = false;
	if (thread != nullptr) {
		thread->join();
		delete thread;
		thread = nullptr;
		this->callback = nullptr;
	}
}

void YUVReader::run()
{
	std::ifstream fileStream(filePath, std::ios::binary);

	if (!fileStream) {
		// failure
		std::cout << "fail to open the file" << std::endl;
	}
	else {
		// success
		fileStream.seekg(0, std::ios::beg);
		while (isReading) {
			if (fileStream.good()) {
				fileStream.read(reinterpret_cast<char*>(buffer), VIDEO_FRAME_SIZE);
				
				if (callback != nullptr) {
					callback(VIDEO_WIDTH, VIDEO_HEIGHT, buffer, VIDEO_FRAME_SIZE);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(VIDEO_FRAME_INTERVAL_MS));
			}
			else {
				fileStream.clear();
				fileStream.seekg(0, std::ios::beg);
			}
			
		}
	}
	
	fileStream.close();
}

