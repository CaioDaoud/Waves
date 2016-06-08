#include "parser.h"

int* Parser::parseQuery(const char *_query, Vocabulario *_vocabulary, int *_numberOfTerms) {
    unsigned int k;
    char pal[WORD_SIZE+1], charBase;
    int *termIds = new int [1000];
    unsigned char c;
    int sizeTermIds;
    int id;

    memset(pal,0,WORD_SIZE+1);
    k=0;
    pal[k] = '\0';
    sizeTermIds = 0;

    while(*_query) {
        charBase = *_query;
        c =  convertWithoutAccent[(unsigned char) charBase];
        if(isalnum(c)) {
            if(k < WORD_SIZE) {
                pal[k] = tolower(c);
                k++;
            }
        }
        else if(k>0) {
            pal[k] = '\0';
            id = _vocabulary->getId((char*)pal, k, 0);
            if(id != -1) {
                char tmp[50];
                    strncpy(tmp,(char*)pal,k);
                    tmp[k]=0;
                    termIds[sizeTermIds++] = id;
            }
                
            k = 0;
        }
            
        _query++;
    }
        
    if(k>0) {
        pal[k] = '\0';
        id = _vocabulary->getId((char*)pal, k, 0);
        if(id != -1) {
            char tmp[50];
            strncpy(tmp,(char*)pal,k);
            tmp[k]=0;
            termIds[sizeTermIds++] = id;
        }
    }
        
    *_numberOfTerms = sizeTermIds;
    return termIds;
}
