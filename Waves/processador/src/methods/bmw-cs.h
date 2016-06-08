#ifndef BMW_CS_H_
#define BMW_CS_H_

#include "utils/auxFunctions.h"

class BmwCS{
public:
    
    vector<result> * processQueryBMWCS(Index *index, Index *secIndex, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, PostingList** plists, int RESULT_HEAP_SIZE, int limiarType, int index_type){

        unsigned int MAXD = std::numeric_limits<unsigned int>::max();
        int pivot =0;
        unsigned int did,minpivot,tempd, f, least_ft, least_i;
        int k = RESULT_HEAP_SIZE;
        int pos =0;
        float score = 0.0f;
        float threshold =0.0f;
        float s, ms, ms_aux, scoret,maxposs, upper;
        s= ms= ms_aux= scoret= maxposs= upper=0;

        vector<result> *candidate_results;
        candidate_results = new vector<result>();
        candidate_results->reserve(RESULT_HEAP_SIZE);
        vector<result> *candidate = new vector<result>();
        candidate->reserve(RESULT_HEAP_SIZE*10);

        if (number_of_terms == 0) {
            return new vector<result>();
        }
            
        PostingList **secPlists = getPostingLists(secIndex, terms, number_of_terms);
        plists = getPostingLists(index, terms, number_of_terms);
            
        if(plists == NULL){
            return new vector<result>();
        }

        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            while((plists[i]->sizeList == 0) && (i<number_of_terms)) {
                delete plists[i];
                for(j=i ; j<number_of_terms-1;j++) {
                    plists[j] = plists[j+1];
                    secPlists[j] = secPlists[j+1];
                }
                number_of_terms--;
            }
        }
        
        if(number_of_terms == 0) {
            delete plists;
            delete secPlists;
            return new vector<result>();
        }
    
        for (int i = 0; i < number_of_terms; ++i) {
            plists[i]->pre = secPlists[i]->pre = (float)3 * (float)(log( 1 + (float)(index->getNumberOfDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
            plists[i]->moveToFirst();   
            secPlists[i]->moveToFirst();
            ms_aux += plists[i]->listMinScore;	
        }
    
        sortPostingsCS(plists, secPlists, number_of_terms);
            
        while( true ){
            pivot = -1;
            s = 0.0f;
            ms = ms_aux;
            upper=0.0f;
    
            for(int i = 0; i < number_of_terms; i ++){
                s  += plists[i]->listMaxScore;
                ms -= plists[i]->listMinScore;
                upper = s + ms;
    
                if( upper >= threshold){
                    while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost)	 {
                        i++;
                        ms -= plists[i]->listMinScore;
                    }
                    pivot = i;
                    break;
                }					
            }
    
            if(pivot == -1 || plists[pivot]->currPost == MAXD){
                break;
            }
            did = plists[pivot]->currPost ;
                    
            maxposs = plists[pivot]->bufferSkipMaxScore[ plists[pivot]->atSkip];	
            minpivot = plists[pivot]->bufferSkipDocId[ plists[pivot]->atSkip ] > MAXD ? MAXD : plists[pivot]->bufferSkipDocId[ plists[pivot]->atSkip ]+1;
            for(int i = 0; i < pivot; i++){
                tempd = movepointer(plists[i], did);			
                if( minpivot >  tempd)
                    minpivot = tempd;
                if(plists[i]->checkblock < plists[i]->sizeSkip)
                    maxposs += plists[i]->bufferSkipMaxScore[ plists[i]->checkblock];
            }
                                    
            upper = maxposs + ms;
    
            if(upper >= threshold ){
                if( did == plists[0]->currPost ){
                    score = 0.0f;
                    for(int i = 0 ; i < pivot + 1; i++) {
                        f = plists[i]->getFreq();
                        scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                        score+=scoret;
                        upper -= ( plists[i]->bufferSkipMaxScore[ plists[i]->atSkip ] - scoret );
    
                        if( !(upper >= threshold) )
                            break;
                    }
                
                    if(score ==0)
                        exit(1);
                
                    if(score>=threshold){
                        insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                    }
                
                    upper = score+ms;
                    if (upper >= threshold){
                        if(candidate->size() > (unsigned) RESULT_HEAP_SIZE && (*candidate)[0].upperScore <= threshold) {
                            pop_heap (candidate->begin(),candidate->end()); 
                            candidate->pop_back();
                        }
                        candidate->push_back( result(did, score, upper, pivot+1/*, st*/) );
                        push_heap(candidate->begin(),candidate->end());
                    }
    
                    for(int i = pivot ; i > -1; i--){
                        plists[i]->skipToDoc(did + 1);
                        popdownCS(plists, secPlists, i, number_of_terms);
                    }
                }
                else {
                    least_ft = plists[0]->ft;
                    least_i = 0;
                    int i =0;
                    do {
                        if(plists[i]->ft < least_ft){
                            least_ft = plists[i]->ft;
                            least_i = i;
                        }				
                        i++;
                        } while(plists[i]->currPost != did);
            
                    plists[least_i]->skipToDoc(did);
                    popdownCS(plists, secPlists, least_i, number_of_terms);					
                }
            }
            else{
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
                popdownCS(plists, secPlists, least_i, number_of_terms);	
            }
        }
        
        vector<result> *s_candidate = new vector<result>();
        s_candidate->reserve(RESULT_HEAP_SIZE*10);
            
        for(vector<result>::iterator it = candidate->begin(); it!= candidate->end(); it++) {
            if (threshold <= it->upperScore ){ s_candidate->push_back(*it);}
        }
        delete(candidate);
        sort(s_candidate->begin(), s_candidate->end(), &resultDocIdComparator);
    
        pos = 0;
        score =0.0f;
        (*candidate_results).clear();
    
        int totalTerm = number_of_terms;
            
        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            while((secPlists[i]->sizeList == 0) && (i<number_of_terms)) {
                delete secPlists[i];
                for(j=i ; j<number_of_terms-1;j++) {
                    secPlists[j] = secPlists[j+1];
                }
                number_of_terms--;
            }
        }
    
        for (int i = 0; i < number_of_terms; ++i) {
            secPlists[i]->moveToFirst();
        }
            
        vector< result >::iterator it;
        for(it = s_candidate->begin(); it!= s_candidate->end(); it++){
            if(it->numberOfTerms == totalTerm)	{	
                if(it->score >= threshold)
                    insereNoRanking(k,  &pos, &threshold, *it, candidate_results);
                continue;
            }
            else {
                upper = 0.0f;
                score = 0.0f;
                for (int j = 0; j < number_of_terms; j++){
                    movepointer(secPlists[j], it->docId);
                                
                    if(secPlists[j]->checkblock < secPlists[j]->sizeSkip)
                        upper+=secPlists[j]->bufferSkipMaxScore[ secPlists[j]->checkblock];
                }
            
                if(upper+it->score >= threshold){
                    for (int j = 0; j < number_of_terms; j++){
                        if(secPlists[j]->currPost < (unsigned)it->docId) {
                            secPlists[j]->skipToDoc(it->docId);
                        }
                        if(secPlists[j]->currPost == (unsigned)it->docId){
                            it->numberOfTerms++;
                            f = secPlists[j]->getFreq(); 
                            scoret = calcScore( secPlists[j]->pre, terms[j].idf , (float)(f),index->getDocNorm(it->docId),index_type);
                            score +=scoret;
                        }
                    }
                    it->score += score;
                    if(it->score >= threshold){
                        insereNoRanking(k,  &pos, &threshold, *it, candidate_results);
                    }
                }
            }
        }
    
        for (int i = 0; i < number_of_terms; ++i) { 	
            delete secPlists[i];	
        }
        delete[] secPlists;
            
        for (int i = 0; i < number_of_terms; ++i) {	
            delete plists[i];	
        }
            
        delete[] plists;
        delete(s_candidate);
            
        return candidate_results;
    }
    
};
#endif
