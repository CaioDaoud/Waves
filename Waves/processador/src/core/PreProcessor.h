/*
 * QueryProcessor.h
 *
 *  Created on: 03/08/2010
 *      Author: felipe
 */

#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include <iostream>
#include <fstream>
#include <algorithm> 
#include <vector>
#include <sys/time.h>
#include <time.h> 
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <exception>
#include "structures/result.h"
#include "parser/parser.h"
#include "utils/auxFunctions.h"
#include "methods/wand.h"
#include "methods/bmw.h"
#include "methods/bmw-cs.h"
#include "methods/bmw-csp.h"
#include "methods/bmw-m.h"
#include "methods/waves.h"


using namespace std;
#ifndef __APPLE__
using namespace std::tr1;
#endif

class PreProcessor {
public:
    PreProcessor(int limiar, Index *_entireIndex, Index *_smallIndex, Index *_wholedocIndex, Index *_textIndex, Index *_titleIndex, Index *_anchorIndex, Index *_urlIndex, Vocabulario *_vocabulary, const char *_resultFile, int _processMode, int _indexType);
    ~PreProcessor();

    int getResultHeapSize();
    void setResultHeapSize(int _newSize);
    void setSecSIndex(Index *_SecSmallIndex);
    void processQuery(const string _query);
    void processQuery(const string _query, const string _secQuery); 

private:
    int topk;
    int processMode;
    int indexType;
    int formula;
    int limiarType;
    
    FILE *_fpReults;
    Vocabulario *vocabulary;
    Parser simpleQueryParser;
    Index *SecondSmallIndex;
    Index *entireIndex;
    Index *wholedocIndex;
    Index *smallIndex;
    Index *textIndex;
    Index *titleIndex;
    Index *anchorIndex;
    Index *urlIndex;
    
    Wand *wand;
    Bmw *bmw;
    BmwCS *bmwcs;
    BmwCSP *bmwcsp;
    BmwM *bmwm;
    Waves *waves;
    
    vector<Term>*   preProcessQuery(const string &_query, int &_size);
    inline void saveResultsToFile(vector<result>* _results, FILE *_fpResults, int _queryId);
    inline void showResults(vector<result>* _results);
    
};

#endif

