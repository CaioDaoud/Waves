/*
 * PostingList.cpp
 *
 *  Created on: 21/09/2010
 *      Author: felipe
 */

#include "PostingList.h"

PostingList::PostingList() {
    this->currPost = 0;
    this->sizeSkip = 0;
    this->sizeList = 0;
    this->atSkip = 0;
}

PostingList::~PostingList() {}

int PostingList::getFreq() {
    if (freqDecoded) {
        return bufferFreqs[atBlock];
    } else {
        freqDecoded = 1;
        bufferFreqs = (unsigned int *) out;
        return bufferFreqs[atBlock];
    }
}

void PostingList::Init(unsigned int *_atList, long _sizeList, long _listBegin) {
    atList = (unsigned int*) _atList + (_listBegin / sizeof(unsigned int));
    out = (unsigned int*) atList;
    ini = out;
    sizeList = _sizeList;
    currPost = 0;
}

void PostingList::skipInit(unsigned int *_atList, long _sizeList, long _listBegin, char *_atSkip, long _sizeSkip, long _skipBegin, int _termId) {
    Init(_atList, _sizeList, _listBegin);
    this->termId = _termId;
    bufferSkipDocId = (unsigned int*) (_atSkip + _skipBegin);
    bufferSkipOffset = (long*) (_atSkip + _skipBegin + sizeof(unsigned int) * _sizeSkip);
    bufferSkipBitAt = (unsigned char*) (_atSkip + _skipBegin + (sizeof(unsigned int) + sizeof(long)) * _sizeSkip);
    bufferSkipMaxScore = (float*) (_atSkip + _skipBegin + (sizeof(unsigned int) + sizeof(long) + sizeof(unsigned char)) * _sizeSkip);
    atSkip = 0;
    sizeSkip = _sizeSkip;
}

int PostingList::END_OF_LIST_DOC_ID() {
    return std::numeric_limits<unsigned int>::max();
}

void PostingList::endOfList(){
        currPost = END_OF_LIST_DOC_ID();
}

void PostingList::skipToDoc(unsigned int _docId) {
    if (_docId > endOfBlock) {
        if (atSkip == sizeSkip - 1) {
            currPost = END_OF_LIST_DOC_ID();
            return;
        }
        while (bufferSkipDocId[atSkip] < _docId) {
            atSkip++;
            if (atSkip == sizeSkip) {
                    currPost = END_OF_LIST_DOC_ID();
                    return;
            }
        }
    
        sizeBlock = 0;
        atBlock = -1;
        freqDecoded = 0;
        out = (unsigned int*) (&atList[bufferSkipOffset[atSkip - 1]]);
        bufferDocs[0] = (*(unsigned int *) out);
        out++;
            
        
        endOfBlock = bufferSkipDocId[atSkip];
        
        while (bufferDocs[sizeBlock] < endOfBlock) {
            sizeBlock++;
            bufferDocs[sizeBlock] = (*(unsigned int *) out);
            out++;
        }
        
        sizeBlock++;
        bufferDocs[sizeBlock] = END_OF_LIST_DOC_ID();
    }
    
    atBlock++;
    while (bufferDocs[atBlock] < _docId){	
        atBlock++;
    }
    
    currPost = bufferDocs[atBlock];
    return;
}

void PostingList::moveToFirst() {
    out = ini;
    checkblock = 0;
    if (sizeSkip <= 0) {
        currPost = END_OF_LIST_DOC_ID();
        return;
    }
    sizeBlock = 0;
    freqDecoded = 0;
    bufferDocs[0] = (*(unsigned int*) out);

    out++;
    if (sizeSkip == 0) {
        printf("nunca deve acontecer %ld  First %u ft %u\n", sizeList, bufferDocs[0], ft);
        exit(1);
    } else {
        endOfBlock = bufferSkipDocId[0];
    }
    
    while (bufferDocs[sizeBlock] < endOfBlock) {
        sizeBlock++;
        bufferDocs[sizeBlock] = (*(unsigned int*) out);
        out++;
        if (sizeBlock > 127) {
            printf("erro na descomprime!\n");
            exit(1);
        }
    }

    sizeBlock++;
    bufferDocs[sizeBlock] = END_OF_LIST_DOC_ID();
    atBlock = 0;
    currPost = bufferDocs[0];
    atSkip = 0;
}
