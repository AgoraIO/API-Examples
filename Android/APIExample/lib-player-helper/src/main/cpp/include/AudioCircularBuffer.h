/*
 *  Copyright (c) 2016 The Agora project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_CHAT_ENGINE_FILE_AUDIO_CIRCULAR_BUFFER_H_
#define WEBRTC_CHAT_ENGINE_FILE_AUDIO_CIRCULAR_BUFFER_H_

#include "scoped_ptr.h"
#include <string.h>
template <typename  Ty>

class AudioCircularBuffer {
    
  public:
    typedef Ty value;
    AudioCircularBuffer(uint32_t initSize, bool newWay)
    : pInt16BufferPtr(nullptr),
    bNewWayProcessing(newWay)
    {
        mInt16BufferLength = initSize;
        if (bNewWayProcessing) {
            pInt16BufferPtr = new value[sizeof(value) * mInt16BufferLength];
        }
        else {
            if (!pInt16Buffer.get()) {
                pInt16Buffer.reset(new value[sizeof(value) * mInt16BufferLength]);
            }
        }
    }
    
     ~AudioCircularBuffer()
    {
        if (pInt16BufferPtr) {
            delete [] pInt16BufferPtr;
            pInt16BufferPtr = nullptr;
        }
    }
    
    void Push(value* data, int length)
    {
        if (bNewWayProcessing) {
            // If the internal buffer is not large enough, first enlarge the buffer
            if (mAvailSamples + length > mInt16BufferLength) {
                int newLength = std::max(length + mAvailSamples + 960, 2 * mInt16BufferLength);
                value * tmpBuffer = new value[sizeof(value) * newLength];
                if (mReadPtrPosition + mAvailSamples > mInt16BufferLength) {
                    int firstCopyLength = mInt16BufferLength - mReadPtrPosition;
                    
                    memcpy(tmpBuffer, pInt16BufferPtr + mReadPtrPosition, sizeof(value) * firstCopyLength);
                    memcpy(tmpBuffer + firstCopyLength, pInt16BufferPtr, sizeof(value) * (mAvailSamples - firstCopyLength));
                }
                else {
                    memcpy(tmpBuffer, pInt16BufferPtr + mReadPtrPosition, sizeof(value) * mAvailSamples);
                }
                delete [] pInt16BufferPtr;
                
                // Construct the new internal array
                mInt16BufferLength = newLength;
                pInt16BufferPtr = tmpBuffer;
                mReadPtrPosition = 0;
                mWritePtrPosition = mAvailSamples;
                memcpy(pInt16BufferPtr + mWritePtrPosition, data, sizeof(value) * length);
                mWritePtrPosition += length;
            }
            else {
                int availSlots = mInt16BufferLength - mWritePtrPosition;
                if (availSlots < length) {
                    memcpy(pInt16BufferPtr + mWritePtrPosition, data, sizeof(value) * availSlots);
                    memcpy(pInt16BufferPtr, data + availSlots, sizeof(value) * (length - availSlots));
                }
                else {
                    memcpy(pInt16BufferPtr + mWritePtrPosition, data, sizeof(value)*length);
                }
                mWritePtrPosition = IntModule(mWritePtrPosition, length, mInt16BufferLength);
            }
            mAvailSamples += length;
        }
        else {
            // If the internal buffer is not large enough, first enlarge the buffer
            if (length + mAvailSamples > mInt16BufferLength) {
                value * tmpBuffer = new value[sizeof(value) * mAvailSamples];
                memmove(tmpBuffer, &pInt16Buffer[mReadPtrPosition], sizeof(value)*mAvailSamples);
                
                mInt16BufferLength = (length + mAvailSamples) * 2;
                pInt16Buffer.reset(new value[sizeof(value) * mInt16BufferLength]);
                memmove(&pInt16Buffer[0], tmpBuffer, sizeof(value)*mAvailSamples);
                
                delete[] tmpBuffer;
                mReadPtrPosition = 0;
            }
            else {
                memmove(&pInt16Buffer[0], &pInt16Buffer[mReadPtrPosition], sizeof(value)*mAvailSamples);
            }
            
            memmove(&pInt16Buffer[mAvailSamples], data, sizeof(value)*length);
            mAvailSamples += length;
            mReadPtrPosition = 0;
        }
    }
    
    void Pop(value* data, int length)
    {
        if (bNewWayProcessing) {
            int availSlots = mInt16BufferLength - mReadPtrPosition;
            if (availSlots < length) {
                memcpy(data, pInt16BufferPtr + mReadPtrPosition, sizeof(value) * availSlots);
                memcpy(data + availSlots, pInt16BufferPtr, sizeof(value) * (length - availSlots));
            }
            else {
                memcpy(data, pInt16BufferPtr + mReadPtrPosition, sizeof(value)*length);
            }
            mReadPtrPosition = IntModule(mReadPtrPosition, length, mInt16BufferLength);
            mAvailSamples -= length;
        }
        else {
            memmove(data, &pInt16Buffer[mReadPtrPosition], sizeof(value)*length);
            mAvailSamples -= length;
            mReadPtrPosition += length;
        }
    }
    
    void Discard(int length)
    {
        if (bNewWayProcessing) {
            mReadPtrPosition = IntModule(mReadPtrPosition, length, mInt16BufferLength);
            mAvailSamples -= length;
        }
        else {
            mAvailSamples -= length;
            mReadPtrPosition += length;
        }
    }
    
    void Reset()
    {
        mAvailSamples = 0;
        mReadPtrPosition = 0;
        mWritePtrPosition = 0;
    }
    
    bool dataAvailable(uint32_t requireLength) {
        return mAvailSamples >= requireLength;
    }
    static uint32_t IntModule(uint32_t ptrIndex, int frmLength, int bufLength)
    {
        if (ptrIndex + frmLength >= bufLength) {
            return ptrIndex + frmLength - bufLength;
        }
        else {
            return ptrIndex + frmLength;
        }
    }
    uint32_t mAvailSamples = 0;
    uint32_t mReadPtrPosition = 0;
    uint32_t mWritePtrPosition = 0;
    uint32_t mInt16BufferLength;
    value* pInt16BufferPtr;
    AgoraRTC::scoped_array<value> pInt16Buffer;
    
  private:
    bool bNewWayProcessing;
    
  };
//ptrIndex = (ptrIndex + frmLength) % bufLength



#endif  // WEBRTC_CHAT_ENGINE_FILE_AUDIO_CIRCULAR_BUFFER_H_
