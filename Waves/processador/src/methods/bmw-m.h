#ifndef BMW_M_H_
#define BMW_M_H_

#include "utils/auxFunctions.h"

class BmwM{
public:
    
    vector<result> * processQueryBMW_twoLayers(Index *index, Index *sIndex, int &number_of_terms, vector<Term>  &terms, int &number_of_results, float &ideal_doc_score, int RESULT_HEAP_SIZE, int limiarType, int index_type){

        float scoret,maxposs;
        unsigned int did,minpivot,tempd;
        int f =0;
        unsigned int least_ft, least_i;
        int k = RESULT_HEAP_SIZE;
        int pos = 0;
        double score;
        int pivot;

        vector<result> *candidate_results;
        candidate_results = new vector<result>();
        candidate_results->reserve(RESULT_HEAP_SIZE);
    
        if (number_of_terms == 0)
            return new vector<result>();

        number_of_results = 0;
        float threshold = 0.0;
        unsigned int MAXD = std::numeric_limits<unsigned int>::max();

        PostingList **plists = getPostingLists(index, sIndex, terms, number_of_terms);
    
        if(plists == NULL)
            return new vector<result>();

        threshold = calcLimiar(terms, plists, number_of_terms, limiarType);

        number_of_terms +=number_of_terms;

        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            while((plists[i]->sizeList == 0) && (i<number_of_terms)) {
                delete plists[i];
                for(j=i ; j<number_of_terms-1;j++) {
                    plists[j] = plists[j+1]; 
                }
                number_of_terms--;
            }
        }
	
        if(number_of_terms == 0) {
            delete plists;
            return new vector<result>();
        }   

        for (int i = 0; i < number_of_terms; ++i) {
            plists[i]->pre= (float)3 * (float)(log( 1 + (float)(index->getNumberOfDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
            plists[i]->moveToFirst();
        }
    
        sortPostings(plists,number_of_terms);
            
        while( true ) {
            pivot = -1;
            scoret = 0.0f;
            for(int i = 0; i < number_of_terms; i ++) {
                scoret += plists[i]->listMaxScore;
                if( scoret > threshold) {
                    while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost)	 
                        i++;
                    pivot = i;
                    break;
                }
            }
    
            if(pivot == -1 || plists[pivot]->currPost == MAXD)
                break;
            
            maxposs = plists[pivot]->bufferSkipMaxScore[ plists[pivot]->atSkip];	
            did = plists[pivot]->currPost ;
            minpivot = plists[pivot]->bufferSkipDocId[ plists[pivot]->atSkip ] > MAXD ? MAXD : plists[pivot]->bufferSkipDocId[ plists[pivot]->atSkip ]+1;
    
            for(int i = 0; i < pivot; i++) {
                tempd = movepointer(plists[i], did);			
                if( minpivot >  tempd)
                    minpivot = tempd;
                if(plists[i]->checkblock < plists[i]->sizeSkip)
                    maxposs += plists[i]->bufferSkipMaxScore[ plists[i]->checkblock];
            }
    
            if( maxposs >= threshold ) {
                if( did == plists[0]->currPost ) {
                    score = 0.0f;
                    for(int i = 0 ; i < pivot + 1; i++) {
                        f = plists[i]->getFreq();
                        scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did), index_type);
                        score+=scoret;
                        maxposs -= ( plists[i]->bufferSkipMaxScore[ plists[i]->atSkip ] - scoret );
                        if( !(maxposs > threshold) )
                            break;
                    }
                    
                    for(int i = 0 ;i < pivot + 1; i++) {
                        plists[i]->skipToDoc(did + 1);
                    }
            
                    if (score >= threshold){
                        insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                    }
    
                    for(int i = pivot ; i > -1; i--){
                        popdown(plists, i, number_of_terms);
                    }
                }
                else {
                    least_ft = plists[0]->ft;
                    least_i = 0;
                    for(int i = 1; i < pivot ; i++) {
                        if( plists[i]->ft < least_ft &&  plists[i]->currPost != did ) {
                            least_ft = plists[i]->ft;
                            least_i = i;
                        }
                    }
                    plists[least_i]->skipToDoc(did );
                    popdown(plists,least_i, number_of_terms);
    
                }
            }
            else {		
                least_ft = plists[0]->ft;
                least_i = 0;
                for(int i = 1; i < pivot + 1; i++) {
                    if( plists[i]->ft < least_ft) {
                        least_ft = plists[i]->ft;
                        least_i = i;
                    }
                }
    
                if( pivot < number_of_terms - 1)
                    did = minpivot > plists[ pivot + 1 ]->currPost ? plists[ pivot + 1 ]->currPost : minpivot; 
                else
                    did = minpivot;
    
                if( did <= plists[pivot]->currPost)
                    did = plists[pivot]->currPost + 1;
                if(did > MAXD)
                    break;
            
                plists[least_i]->skipToDoc(did );
                popdown(plists, least_i, number_of_terms);	
            }
        }
            
        for (int i = 0; i < number_of_terms; ++i) {
            delete plists[i];
        }
    
        delete[] plists;
    
        ideal_doc_score = threshold;
        return candidate_results;
    }


    vector<result> * processQueryBMW_threeLayers(Index *index, Index *sIndex, Index *tIndex, int &number_of_terms, vector<Term>  &terms,  int &number_of_results, float &ideal_doc_score, int RESULT_HEAP_SIZE, int limiarType, int index_type){

        float scoret,maxposs;
        unsigned int did,minpivot,tempd;
        int f=0;
        unsigned int least_ft, least_i;
        int k = RESULT_HEAP_SIZE;
    
        if (number_of_terms == 0)
            return new vector<result>();
    
        vector<result> *candidate_results;
        candidate_results = new vector<result>();
        candidate_results->reserve(RESULT_HEAP_SIZE);
    
        number_of_results = 0;
        float threshold = 0.0;
        unsigned int MAXD = std::numeric_limits<unsigned int>::max();
    
        PostingList **plists = getPostingLists(index, sIndex, tIndex, terms, number_of_terms);
        number_of_terms = 3*number_of_terms;
    
        if(plists == NULL)
            return new vector<result>();
    
        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            while((plists[i]->sizeList == 0) && (i<number_of_terms)) {
                delete plists[i];
                for(j=i ; j<number_of_terms-1;j++) {
                    plists[j] = plists[j+1];
                }
                number_of_terms--;
            }
        }
    
        if(number_of_terms == 0) {
            delete plists;
            return new vector<result>();
        }

        for (int i = 0; i < number_of_terms; ++i) {
            plists[i]->pre= (float)3 * (float)(log( 1 + (float)(index->getNumberOfDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
            plists[i]->moveToFirst();
        }
    
        int pos = 0;
        float score;
        int pivot;
    
        sortPostings(plists,number_of_terms);
    
        while( true ) {
            pivot = -1;
            score = 0.0f;
    
            for(int i = 0; i < number_of_terms; i ++) {
                score += plists[i]->listMaxScore;
                if( score > threshold) {
                    while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost)	 
                        i++;
                    pivot = i;
                    break;
                }
            }
    
            if(pivot == -1 || plists[pivot]->currPost == MAXD)
                break;
            
            maxposs = plists[pivot]->bufferSkipMaxScore[ plists[pivot]->atSkip];	
            did = plists[pivot]->currPost ;
            minpivot = plists[pivot]->bufferSkipDocId[ plists[pivot]->atSkip ] > MAXD ? MAXD : plists[pivot]->bufferSkipDocId[ plists[pivot]->atSkip ]+1;
    
            for(int i = 0; i < pivot; i++) {
                tempd = movepointer(plists[i], did);			
                if( minpivot >  tempd)
                    minpivot = tempd;
                if(plists[i]->checkblock < plists[i]->sizeSkip)
                    maxposs += plists[i]->bufferSkipMaxScore[ plists[i]->checkblock];
            }
    
            if( maxposs > threshold ) {
                if( did == plists[0]->currPost ) {
                    score = 0.0f;
                    for(int i = 0 ; i < pivot + 1; i++) {
                        f = plists[i]->getFreq();
                        scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                        score+=scoret;
                        maxposs -= ( plists[i]->bufferSkipMaxScore[ plists[i]->atSkip ] - scoret );
                        if( !(maxposs > threshold) )
                            break;
                    }
                    
                    for(int i = 0 ;i < pivot + 1; i++) {
                        plists[i]->skipToDoc(did + 1);
                    }
                    if (score >= threshold) {
                        insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                    }
    
                    for(int i = pivot ; i > -1; i--) {
                        popdown(plists, i, number_of_terms);
                    }
                }
                else {
                    least_ft = plists[0]->ft;
                    least_i = 0;    
                    for(int i = 1; i < pivot ; i++) {
                        if( plists[i]->ft < least_ft &&  plists[i]->currPost != did ) {
                            least_ft = plists[i]->ft;
                            least_i = i;
                        }
                    }
                    plists[least_i]->skipToDoc(did );
                    popdown(plists,least_i, number_of_terms);
                }
            }
            else {
                least_ft = plists[0]->ft;
                least_i = 0;
                
                for(int i = 1; i < pivot + 1; i++) {
                    if(plists[i]->ft < least_ft) {
                        least_ft = plists[i]->ft;
                        least_i = i;
                    }
                }
    
                if( pivot < number_of_terms - 1)
                    did = minpivot > plists[ pivot + 1 ]->currPost ? plists[ pivot + 1 ]->currPost : minpivot; 
                else
                    did = minpivot;
    
                if( did <= plists[pivot]->currPost)
                    did = plists[pivot]->currPost + 1;
                if(did > MAXD)
                    break;
                
                plists[least_i]->skipToDoc(did );
                popdown(plists, least_i, number_of_terms);	
            }
        }   
            
        for (int i = 0; i < number_of_terms; ++i) {
            delete plists[i];
        }
        
        delete[] plists;
        
        ideal_doc_score = threshold;
        return candidate_results;
    }
    
};
#endif
