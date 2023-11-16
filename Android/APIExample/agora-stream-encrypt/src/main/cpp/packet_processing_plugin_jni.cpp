#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include <vector>
#include <algorithm>

#include "./include/agora/IAgoraMediaEngine.h"
#include "./include/agora/IAgoraRtcEngine.h"

#include "./include/packet_processing_plugin_jni.h"

/**stream data frame listener*/
class AgoraRTCPacketObserver : public agora::rtc::IPacketObserver {
 public:
  AgoraRTCPacketObserver() {
    __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "AgoraRTCPacketObserver0");
    m_txAudioBuffer.resize(2048);
    m_rxAudioBuffer.resize(2048);
    m_txVideoBuffer.resize(2048);
    m_rxVideoBuffer.resize(2048);
    __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "AgoraRTCPacketObserver1");
  }

  /**Occurs when the local user sends an audio packet.
   * @param packet The sent audio packet.
   * @return
   *   true: The audio packet is sent successfully.
   *   false: The audio packet is discarded.*/
  virtual bool onSendAudioPacket(Packet &packet) {
    __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "onSendAudioPacket0");
    int i;
    //encrypt the packet
    const unsigned char *p = packet.buffer;
    const unsigned char *pe = packet.buffer + packet.size;

    for (i = 0; p < pe && i < m_txAudioBuffer.size(); ++p, ++i) {
      m_txAudioBuffer[i] = *p ^ 0x55;
    }
    //assign new buffer and the length back to SDK
    packet.buffer = &m_txAudioBuffer[0];
    packet.size = i;
    return true;
  }

  /**Occurs when the local user sends a video packet.
   * @param packet The sent video packet.
   * @return
   *   true: The video packet is sent successfully.
   *   false: The video packet is discarded.*/
  virtual bool onSendVideoPacket(Packet &packet) {
    __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "onSendAudioPacket1%d", 1);
    int i;
    //encrypt the packet
    const unsigned char *p = packet.buffer;
    const unsigned char *pe = packet.buffer + packet.size;
    for (i = 0; p < pe && i < m_txVideoBuffer.size(); ++p, ++i) {
      m_txVideoBuffer[i] = *p ^ 0x55;
    }
    //assign new buffer and the length back to SDK
    packet.buffer = &m_txVideoBuffer[0];
    packet.size = i;
    return true;
  }

  /**Occurs when the local user receives an audio packet.
   * @param packet The received audio packet.
   * @return
   *   true: The audio packet is received successfully.
   *   false: The audio packet is discarded.*/
  virtual bool onReceiveAudioPacket(Packet &packet) {
    __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "onReceiveAudioPacket0");
    int i = 0;
    //decrypt the packet
    const unsigned char *p = packet.buffer;
    const unsigned char *pe = packet.buffer + packet.size;
    for (i = 0; p < pe && i < m_rxAudioBuffer.size(); ++p, ++i) {
      m_rxAudioBuffer[i] = *p ^ 0x55;
    }
    //assign new buffer and the length back to SDK
    packet.buffer = &m_rxAudioBuffer[0];
    packet.size = i;
    return true;
  }

  /**Occurs when the local user receives a video packet.
   * @param packet The received video packet.
   * @return
   *   true: The video packet is received successfully.
   *   false: The video packet is discarded.*/
  virtual bool onReceiveVideoPacket(Packet &packet) {
    __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "onReceiveAudioPacket1");
    int i = 0;
    //decrypt the packet
    const unsigned char *p = packet.buffer;
    const unsigned char *pe = packet.buffer + packet.size;

    for (i = 0; p < pe && i < m_rxVideoBuffer.size(); ++p, ++i) {
      m_rxVideoBuffer[i] = *p ^ 0x55;
    }
    //assign new buffer and the length back to SDK
    packet.buffer = &m_rxVideoBuffer[0];
    packet.size = i;
    return true;
  }

 private:
  std::vector<unsigned char> m_txAudioBuffer; //buffer for sending audio data
  std::vector<unsigned char> m_txVideoBuffer; //buffer for sending video data

  std::vector<unsigned char> m_rxAudioBuffer; //buffer for receiving audio data
  std::vector<unsigned char> m_rxVideoBuffer; //buffer for receiving video data
};

static AgoraRTCPacketObserver s_packetObserver;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
  JNIEnv *env = NULL;
  jint result = -1;

  if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
    return result;
  }

  assert(env != NULL);
  result = JNI_VERSION_1_6;
  return result;
}

JNIEXPORT void JNICALL
Java_io_agora_api_streamencrypt_PacketProcessor_doRegisterProcessing
    (JNIEnv *env, jclass clazz, jlong rtcEngineHandler) {

  agora::rtc::IRtcEngine *rtcEngine = reinterpret_cast<agora::rtc::IRtcEngine *>(rtcEngineHandler);
  __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "doRegisterProcessing0");
  if (!rtcEngine) return;
  __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "doRegisterProcessing1");
/**Registers a packet observer.
 * The Agora SDK allows your application to register a packet observer to receive callbacks for
 * voice or video packet transmission.
 * @param obsrver Pointer to the registered packet observer.
 * @return
 *   0: Success.
 *   < 0: Failure.
 * PS:
 *   The size of the packet sent to the network after processing should not exceed 1200 bytes,
 *      otherwise, the packet may fail to be sent.
 *   Ensure that both receivers and senders call this method, otherwise, you may meet undefined
 *      behaviors such as no voice and black screen.
 *   When you use CDN live streaming, recording or storage functions, Agora doesn't recommend
 *      calling this method.*/
  int code = rtcEngine->registerPacketObserver(&s_packetObserver);
  __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "%d", code);
}

JNIEXPORT void JNICALL
Java_io_agora_api_streamencrypt_PacketProcessor_doUnregisterProcessing
    (JNIEnv *env, jclass clazz, jlong rtcEngineHandler) {
  agora::rtc::IRtcEngine *rtcEngine = reinterpret_cast<agora::rtc::IRtcEngine *>(rtcEngineHandler);
  if (!rtcEngine) return;
  __android_log_print(ANDROID_LOG_INFO, "agoraencryption", "doUnregisterProcessing");
  rtcEngine->registerPacketObserver(nullptr);
}

#ifdef __cplusplus
}
#endif