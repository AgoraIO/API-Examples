#include "YUVReader.h"


YUVReader::YUVReader()
{

}

void YUVReader::start(IYUVCallback callback)
{
	if (thread == nullptr) {
		isReading = true;
		this->callback = callback;
		thread = new std::thread(std::bind(&YUVReader::run, this));
	}
}

void YUVReader::stop()
{
	isReading = false;
	if (thread != nullptr) {
		thread->join();
		thread = nullptr;
		this->callback = nullptr;
	}
}

void YUVReader::run()
{
	std::ifstream fileStream(FILE_PATH);

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


