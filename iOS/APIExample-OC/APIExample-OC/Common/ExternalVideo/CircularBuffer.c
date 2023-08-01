//
//  CircularBuffer.c
//
//  Created by 罗亮富(Roen zxllf23@163.com) on 14-1-14.
//  Copyright (c) 2014年 All rights reserved.
//


#include "CircularBuffer.h"
#include <string.h>

struct s_circularBuffer{
    
    size_t size; //capacity bytes size
    size_t dataSize; //occupied data size
    size_t tailOffset; //head offset, the oldest byte position offset
    size_t headOffset; //tail offset, the lastest byte position offset
    void *buffer;

};

extern CircularBuffer CircularBufferCreate(size_t size)
{
    size_t totalSize = sizeof(struct s_circularBuffer) + size;
    void *p = malloc(totalSize);
    CircularBuffer buffer = (CircularBuffer)p;
    buffer->buffer = p + sizeof(struct s_circularBuffer);
    buffer->size = size;
    CircularBufferReset(buffer);
    return buffer;
}

void CircularBufferFree(CircularBuffer cBuf)
{
    CircularBufferReset(cBuf);
    cBuf->size = 0;
    cBuf->dataSize = 0;
    cBuf->buffer = NULL;
    free(cBuf);
}

void CircularBufferReset(CircularBuffer cBuf)
{
    cBuf->headOffset = -1;
    cBuf->tailOffset = -1;
    cBuf->dataSize = 0;
}

size_t CircularBufferGetCapacity(CircularBuffer cBuf) {
    
    return cBuf->size;
}

size_t CircularBufferGetSize(CircularBuffer cBuf)
{
    return cBuf->size;
}

size_t CircularBufferGetDataSize(CircularBuffer cBuf)
{
    return cBuf->dataSize;
}

void CircularBufferPush(CircularBuffer cBuf,void *src, size_t length)
{
    if(length == 0)
        return;

    size_t writableLen = length;
    void *pSrc = src;
    
    if(writableLen > cBuf->size)//in case of size overflow
    {
        size_t overFlowLen = writableLen - cBuf->size;
        writableLen = cBuf->size;
        pSrc = src + overFlowLen;
    }
    
    
    bool resetHead = false;
    //in case the circle buffer won't be full after adding the data
    if(cBuf->tailOffset+writableLen < cBuf->size)
    {
        memcpy(&cBuf->buffer[cBuf->tailOffset + 1], pSrc, writableLen);
        
        if((cBuf->tailOffset < cBuf->headOffset) && (cBuf->tailOffset+writableLen >= cBuf->headOffset) )
            resetHead = true;
        
        cBuf->tailOffset += writableLen;
    }
    else//in case the circle buffer will be overflow after adding the data
    {
        size_t remainSize = cBuf->size - cBuf->tailOffset - 1; //the remain size
        memcpy(&cBuf->buffer[cBuf->tailOffset+1], pSrc, remainSize);
        
        size_t coverSize = writableLen - remainSize; //size of data to be covered from the beginning
        memcpy(cBuf->buffer, pSrc+remainSize, coverSize);
        
        if(cBuf->tailOffset < cBuf->headOffset)
            resetHead = true;
        else
        {
            if(coverSize>cBuf->headOffset)
                resetHead = true;
        }
        
        cBuf->tailOffset = coverSize - 1;
    }
    
    if(cBuf->headOffset == -1)
        cBuf->headOffset = 0;
    
    if(resetHead)
    {
        if(cBuf->tailOffset+1 < cBuf->size)
            cBuf->headOffset = cBuf->tailOffset + 1;
        else
            cBuf->headOffset = 0;
        
        cBuf->dataSize = cBuf->size;
    }
    else
    {
        if(cBuf->tailOffset >= cBuf->headOffset)
            cBuf->dataSize = cBuf->tailOffset - cBuf->headOffset + 1;
        else
            cBuf->dataSize = cBuf->size - (cBuf->headOffset - cBuf->tailOffset - 1);
    }
}

size_t inter_circularBuffer_read(CircularBuffer cBuf, size_t length, void *dataOut, bool resetHead)
{
    if(cBuf->dataSize == 0 || length == 0)
        return 0;
    
    size_t rdLen = length;
    
    if(cBuf->dataSize < rdLen)
        rdLen = cBuf->dataSize;
    
    
    if(cBuf->headOffset <= cBuf->tailOffset)
    {
        if(dataOut)
            memcpy(dataOut, &cBuf->buffer[cBuf->headOffset], rdLen);
        
        if(resetHead)
        {
            cBuf->headOffset += rdLen;
            if(cBuf->headOffset > cBuf->tailOffset)
            {
                cBuf->headOffset = -1;
                cBuf->tailOffset = -1;
            }
        }
    }
    else
    {
        if(cBuf->headOffset+rdLen <= cBuf->size)
        {
            if(dataOut)
                memcpy(dataOut, &cBuf->buffer[cBuf->headOffset], rdLen);
            
            if(resetHead)
            {
                cBuf->headOffset += rdLen;
                if(cBuf->headOffset == cBuf->size)
                    cBuf->headOffset = 0;
            }
        }
        else
        {
            size_t frg1Len = cBuf->size - cBuf->headOffset;
            if(dataOut)
                memcpy(dataOut, &cBuf->buffer[cBuf->headOffset], frg1Len);
            
            size_t frg2len = rdLen - frg1Len;
            if(dataOut)
                memcpy(dataOut+frg1Len, cBuf->buffer, frg2len);
            
            if(resetHead)
            {
                cBuf->headOffset = frg2len;
                if(cBuf->headOffset > cBuf->tailOffset)
                {
                    cBuf->headOffset = -1;
                    cBuf->tailOffset = -1;
                }
            }
        }
    }
    
    if(resetHead)
        cBuf->dataSize -= rdLen;
    
    return rdLen;
}


size_t CircularBufferPop(CircularBuffer cBuf, size_t length, void *dataOut)
{
    return inter_circularBuffer_read(cBuf,length,dataOut,true);
}

size_t CircularBufferRead(CircularBuffer cBuf, size_t length, void *dataOut)
{
    return inter_circularBuffer_read(cBuf,length,dataOut,false);
}


//print circular buffer's content into str,
void CircularBufferPrint(CircularBuffer cBuf, bool hex)
{
    char *b = cBuf->buffer;
    size_t cSize = CircularBufferGetSize(cBuf);
    char *str = malloc(2*cSize+1);
    
    char c;
    
    for(size_t i=0; i<cSize; i++)
    {
        if(CircularBufferGetDataSize(cBuf) == 0)
        {
            c = '_';
        }
        else if (cBuf->tailOffset < cBuf->headOffset)
        {
            if(i>cBuf->tailOffset && i<cBuf->headOffset)
                c = '_';
            else
              c = b[i];
        }
        else
        {
            if(i>cBuf->tailOffset || i<cBuf->headOffset)
                c = '_';
            else
                c = b[i];
        }
        if(hex)
            sprintf(str+i*2, "%02X|",c);
        else
            sprintf(str+i*2, "%c|",c);
    }
    
    printf("CircularBuffer: %s <size %zu dataSize:%zu>\n",str,CircularBufferGetSize(cBuf),CircularBufferGetDataSize(cBuf));
    
    free(str);
}
