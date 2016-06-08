#ifndef BMW_CSP_H_
#define BMW_CSP_H_

#include "utils/auxFunctions.h"

class BmwCSP{
public:
    
    vector<result> * processQueryBMWCSP(Index *index, Index *secIndex, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, PostingList** plists, int RESULT_HEAP_SIZE, int limiarType, int index_type){

        unsigned int MAXD = std::numeric_limits<unsigned int>::max();
        int pivot =0;
        unsigned int did,minpivot,tempd, f, least_ft, least_i;
        did=minpivot=tempd= f= least_ft= least_i=0;
        int k = RESULT_HEAP_SIZE;
        int pos =0;
        float score;
        float  s , ms, ms_aux, scoret, maxposs, upper;
        s = ms= ms_aux= scoret= maxposs= upper=0;

        vector<result> *candidate_results;
        candidate_results = new vector<result>();
        candidate_results->reserve(RESULT_HEAP_SIZE);
	
        vector<result> *candidate = new vector<result>();
        candidate->reserve(RESULT_HEAP_SIZE*10);

	
        if (number_of_terms == 0) {
            return new vector<result>();
        }
    
        float threshold =0.0f;

        PostingList **secPlists = getPostingLists(secIndex, terms, number_of_terms);
        plists = getPostingLists(index, terms, number_of_terms);
	
        if(plists == NULL){
            printf("###$$## plists == NULL");
            return new vector<result>();
        }

        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            while((plists[i]->sizeList == 0) && (i<number_of_terms)) {
                delete plists[i];
                delete secPlists[i];
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
            plists[i]->pre= (float)3 * (float)(log( 1 + (float)(index->getNumberOfDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
            plists[i]->moveToFirst();
            secPlists[i]->moveToFirst();
            ms_aux += plists[i]->listMinScore;	
        }
    
        threshold = calcLimiar(terms, plists, number_of_terms, limiarType);
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
                    while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost) {
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
                    
            for(int i = 0; i < pivot; i++){
                movepointer(plists[i], did);			
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
                    upper=score+ms;
                    if(score ==0)
                        exit(1);
                                    
                    if(score>=threshold){
                        insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                    }
            
                    if (upper >= threshold){
                        upper = 0.0;
                        for (int j = pivot+1; j < number_of_terms; j++){
                            if(secPlists[j]->sizeList > 0){
                                movepointer(secPlists[j], did);
                            if(secPlists[j]->checkblock < secPlists[j]->sizeSkip)
                                upper+=secPlists[j]->bufferSkipMaxScore[ secPlists[j]->checkblock];
                            }
                        }
                        upper+=score;
                        if(upper >= threshold){
                            candidate->push_back( result(did, score, upper, pivot+1/*, st*/) );
                        }
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
            else {
                least_ft = plists[0]->ft;
                least_i = 0;
                minpivot = plists[0]->bufferSkipDocId[ plists[0]->atSkip ]+1;
                minpivot = minpivot > MAXD ? MAXD : minpivot;
                
                for(int i = 1; i < pivot + 1; i++) {
                    tempd = plists[i]->bufferSkipDocId[plists[i]->checkblock]+1; //movepointer(plists[i], did);			
                    if( minpivot >  tempd)
                        minpivot = tempd;
                    
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
    
        for (int i = 0; i < number_of_terms; ++i) {	
            secPlists[i]->pre= plists[i]->pre;
        }
    
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
                if(it->score >=threshold)
                    insereNoRanking(k,  &pos, &threshold, *it, candidate_results);
                continue;
            }
            else {
                upper = 0.0f;
                score = 0.0f;
                
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
        
        if ( ms_aux >= threshold){	
            for (int i = 0; i < number_of_terms; ++i) {
                secPlists[i]->moveToFirst();
            }
                    
            int meio=0, ini = 0, fim=0;
            bool autoriza;
            score = 0.0f;
            pivot = 0;
            sortPostings(secPlists, number_of_terms);

            while( true ) {
                pivot = -1;
                score = 0.0f;
    
                for(int i = 0; i < number_of_terms; i ++) {
                    score += secPlists[i]->listMaxScore;
                    if( score >= threshold) {
                        while( i + 1 < number_of_terms && secPlists[i + 1]->currPost == secPlists[i]->currPost){	 
                            i++;
                        }
                        pivot = i;
                        break;
                    }
                }
    
                if(pivot == -1 || secPlists[pivot]->currPost == MAXD) 
                    break;
    
                maxposs = secPlists[pivot]->bufferSkipMaxScore[ secPlists[pivot]->atSkip];	
                did = secPlists[pivot]->currPost ;
                minpivot = secPlists[pivot]->bufferSkipDocId[ secPlists[pivot]->atSkip ] > MAXD ? MAXD : secPlists[pivot]->bufferSkipDocId[ secPlists[pivot]->atSkip ]+1;

                for(int i = 0; i < pivot; i++) {
                    tempd = movepointer(secPlists[i], did);			
                    if( minpivot >  tempd)
                        minpivot = tempd;
                    if(secPlists[i]->checkblock < secPlists[i]->sizeSkip)		
                        maxposs += secPlists[i]->bufferSkipMaxScore[ secPlists[i]->checkblock];
                }
    
                if( maxposs >= threshold ) {
                    if( did == secPlists[0]->currPost ){
                        score = 0.0f;
                        for(int i = 0 ; i < pivot + 1; i++) {
                            f = secPlists[i]->getFreq();
                            scoret = calcScore( secPlists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                            score+=scoret;
                        }
                        if (score >= threshold){
                            vector< result >::iterator it = s_candidate->begin();
                            autoriza = true, meio=0, ini = 0, fim = s_candidate->size();
                            while(ini<=fim){
                                meio = (fim+ini)/2;
                                if((unsigned)it[meio].docId == did){
                                    autoriza = false;
                                    break;
                                }
                                else if((unsigned)it[meio].docId < did){
                                    ini=meio+1;
                                }
                                else
                                    fim=meio-1;
                            }
    
                            if(autoriza){
                                insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                            }
                        }
    
                        for(int i = pivot ; i > -1; i--){
                            secPlists[i]->skipToDoc(did + 1);
                            popdown(secPlists, i, number_of_terms);
                        }
                    }
                    else {
                        least_ft = secPlists[0]->ft;
                        least_i = 0;
                        for(int i = 1; i < pivot ; i++){
                            if( secPlists[i]->ft < least_ft &&  secPlists[i]->currPost != did ){
                                least_ft = secPlists[i]->ft;
                                least_i = i;
                            }
                        }
                        secPlists[least_i]->skipToDoc(did );
                        popdown(secPlists,least_i, number_of_terms);
    
                    }
                }
                else {
                    least_ft = secPlists[0]->ft;
                    least_i = 0;
                    for(int i = 1; i < pivot + 1; i++) {
                        if( secPlists[i]->ft < least_ft) {
                            least_ft = secPlists[i]->ft;
                            least_i = i;
                        }
                    }
    
                    if( pivot < number_of_terms - 1)
                        did = minpivot > secPlists[ pivot + 1 ]->currPost ? secPlists[ pivot + 1 ]->currPost : minpivot; 
                    else
                        did = minpivot;
                    
                    if( did <= secPlists[pivot]->currPost)
                        did = secPlists[pivot]->currPost + 1;
                    if(did > MAXD)
                        break;
                    secPlists[least_i]->skipToDoc(did );
                    popdown(secPlists, least_i, number_of_terms);	
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
