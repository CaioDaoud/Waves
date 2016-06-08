#ifndef POSTINGLIST_H_
#define POSTINGLIST_H_

extern unsigned int t1;
extern unsigned int t2;
extern unsigned int mm;
extern unsigned int total_clock;

#include <limits>
#include "../../libs-iw/indexer/include/IndiceInvertido.h"
#include "../../libs-iw/indexer/include/prefixinmem.h"

typedef struct doc {
    unsigned int tf;
    unsigned int docId;
    doc(){}
    doc(unsigned int _docId, unsigned int _tf) : tf(_tf), docId(_docId) {}
} doc;

class PostingList {

public:
    PostingList();
    virtual ~PostingList();
    
    char freqDecoded;
    unsigned char *bufferSkipBitAt;
    int termId;
    int atBlock;
    int checkblock;
    int currFreq;
    int atSkip;
    int sizeSkip;
    unsigned int bufferDocs[513];
    unsigned int *bufferFreqs;
    unsigned int sizeBlock;
    unsigned int *atList;
    unsigned int currPost;
    unsigned int *bufferSkipDocId;
    unsigned int endOfBlock;
    unsigned int ft;
    long int sizeList;
    long int *bufferSkipOffset;
    float *bufferSkipMaxScore;
    float listMaxScore;
    float listMinScore;
    float listThousandScore;
    float pre;
    _tipo_buffer *out, *ini;
    
    int END_OF_LIST_DOC_ID();
    int getFreq();
    void skipInit(unsigned int *_atList, long _sizeList, long _listBegin, char *_atSkip, long _sizeSkip, long _skipBegin, int _termId);
    void Init(unsigned int *_atList, long _tam_lista, long _inicio_lista);
    void endOfList();
    void skipToDoc(unsigned int _docId);
    void moveToFirst();

};

#endif /* POSTINGLIST_H_ */
