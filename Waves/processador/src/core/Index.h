#ifndef INDEX_H_
#define INDEX_H_

#include "PostingList.h"
#include <vector>

class Index {

public:
    enum IndexType {TEXT, ANCHOR, TITLE, URL, UTI};
    Index(const char *_indexBasePath, IndexType _type,  int _topK);
    Index(const char *_indexBasePath, IndexType _type,  int _topK, const char *_basePath );
    virtual ~Index();
    
    unsigned char **indexPointer;
    unsigned char **skipPointer;
    const char *indexBasePath;
    const char *basePath;
    unsigned int numberOfDocs;
    unsigned int *ftArray;
    int *lastDocArray;
    int numberOfTerms;
    float *normArray;
    float *idfArray;
    float *maxScoreArray;
    float *minScoreArray;
    float *thousandScoreArray;
    FILE *idxFile;
    IDX   *idxArray;
    IndexType type;
    float getTermIdf(int _termId);
    float getDocNorm(int _docId);
    float getTermMinScore(int _termId);
    float getTermMaxScore(int _termId);
    float getTermMilScore(int _termId);
    unsigned int getNumberOfDocs();
    void loadToMainMemory();
    PostingList* getPostingList(int _termId);
};

#endif /* INDEX_H_ */
