#ifndef RESULT_H_
#define RESULT_H_

#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include "Vocabulario.h"
#include "core/Index.h"
#include "parser/parser.h"
#ifdef __APPLE__ 
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <pthread.h>
#include <set>

typedef struct result {
    
    int docId;
    int numberOfTerms;
    float score;
    float upperScore;

    bool operator<( const result & d ) const {
        return (fabs(score-d.score)<=0.0000001)?(docId>d.docId):(score > d.score);
    }
    
    bool operator>( const result & d ) const {
        return (fabs(score-d.score)<=0.0000001)?(docId<d.docId):(score < d.score);
    }
	
    bool operator<=( const result & d ) const {
        return (fabs(score-d.score)<=0.0000001)?1:(score > d.score);
    }
	
    bool operator<( const float & d ) const {
        return (fabs(score-d)<=0.0000001)?0:( score > d);
    }
	
    bool operator>( const float & d ) const {
        return (fabs(score-d)<=0.0000001)?0:(score > d);
    }
  
    result() {}

    result(int _docId, float _score, float _upperScore, int _numberOfTerms) {
        docId = _docId;
        score = _score;
        upperScore = _upperScore;
        numberOfTerms = _numberOfTerms;
    }

    result(int _docId, float _score, int _numberOfTerms) {
        docId = _docId;
        score = _score;
        upperScore = _score;
        numberOfTerms = _numberOfTerms;
    }

} result;

#endif
