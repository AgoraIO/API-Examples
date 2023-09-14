#include <VideoExtractor.h>
#include <memory.h>



VideoExtractor::VideoExtractor()
{

}

VideoExtractor::~VideoExtractor()
{
	Stop();
}

void VideoExtractor::Start(IVideoExtractorCallback callback)
{
	if (thread == nullptr) {
		isRunning = true;
		this->callback = callback;
		thread = new std::thread(std::bind(&VideoExtractor::run, this));
	}
}

void VideoExtractor::Stop()
{
	isRunning = false;
	if (thread != nullptr) {
		thread->join();
		thread = nullptr;
		this->callback = nullptr;
	}
}

void VideoExtractor::run()
{
	int ret = avformat_network_init();

	AVFormatContext *avFormatCtx = nullptr;

	ret = avformat_open_input(&avFormatCtx, VIDEO_URL, nullptr, nullptr);

	ret = avformat_find_stream_info(avFormatCtx, nullptr);

	bool hasVideo = false;
	int width, height, fps;
	uint8_t* extradata = nullptr;
	int extradata_size = 0;
	for (int i = 0; i < avFormatCtx->nb_streams; i++)
	{
		AVStream* stream = avFormatCtx->streams[i];
		AVCodecParameters* codcpar = stream->codecpar;
		if (codcpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			hasVideo = true;
			width = codcpar->width;
			height = codcpar->height;
			fps = (int)r2d(stream->avg_frame_rate);
			extradata = codcpar->extradata;
			extradata_size = codcpar->extradata_size;
		}
	}

	if (hasVideo) {
		int videoStreamIndex = av_find_best_stream(avFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

		uint8_t prefix[] = { 0, 0, 0, 1 };
		int spsSize = extradata[7];
		uint8_t* sps = extradata + 8;
		int ppsSize = 4;
		uint8_t* pps = extradata + (extradata_size - ppsSize);
		extradata_size = 8 + spsSize + ppsSize;
		AVPacket* pkt = av_packet_alloc();
		while (isRunning)
		{
			ret = av_read_frame(avFormatCtx, pkt);
			if (ret != 0) {
				// Loop playback
				long long pos = r2d(avFormatCtx->streams[videoStreamIndex]->time_base);
				av_seek_frame(avFormatCtx, videoStreamIndex, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
				continue;
			}

			if (pkt->stream_index == videoStreamIndex) {
				uint8_t* data = pkt->data;
				int size = pkt->size;
				bool isKeyFrame = (pkt->flags & AV_PKT_FLAG_KEY) > 0;

				memcpy(data, prefix, 4);
 				if (isKeyFrame) {
					memcpy(data + extradata_size, data, size);
					memcpy(data + 4, sps, spsSize);
					memcpy(data + 4 + spsSize, prefix, 4);
					memcpy(data + 4 + spsSize + 4, pps, ppsSize);
					size = size + extradata_size;
				}

				if (callback != nullptr) {
					callback(data, size, isKeyFrame, fps);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
			}
		}

		av_packet_free(&pkt);
	}
	
	avformat_close_input(&avFormatCtx);
	avformat_network_deinit();
	
}

double VideoExtractor::r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / r.den;
}

