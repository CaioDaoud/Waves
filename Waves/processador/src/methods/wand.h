#ifndef WAND_H_
#define WAND_H_

#include "utils/auxFunctions.h"

class Wand{
public:
    
    vector<result>* processQueryWAND(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, int RESULT_HEAP_SIZE, int limiarType, int index_type){
        unsigned int MAXD = std::numeric_limits<unsigned int>::max();
        unsigned int did;
        int k = RESULT_HEAP_SIZE;
        int f;
        unsigned int least_ft, least_i;
        float scoret;
        float score;
        float threshold = 0.0;
        int pivot;
        int pos =0;
	
        vector<result> *candidate_results;
        candidate_results = new vector<result>();
        candidate_results->reserve(RESULT_HEAP_SIZE);

        if (number_of_terms == 0)
            return new vector<result>();
    
        PostingList **plists = getPostingLists(index, terms, number_of_terms);
    
        if(plists == NULL)
            return new vector<result>();

        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            while((i<number_of_terms) && (plists[i]->sizeList == 0)) {
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
	
        threshold = calcLimiar(terms, plists, number_of_terms, limiarType);
        sortPostings(plists, number_of_terms);
	
        while( true ) {
            pivot = -1;
            scoret = 0.0f;
            
            for(int i = 0; i < number_of_terms; i ++){
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
        
            if( plists[pivot]->currPost == plists[0]->currPost ) {
                did = plists[ 0 ]->currPost;
                scoret = 0.0f;
                score = 0.0f;
			
                for(int i = 0 ; i < pivot+1; i++) {
                    f = plists[i]->getFreq();
                    scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                    score += scoret;	
                }
    
                if(score>=threshold){
                    insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                }
    
                for(int i = pivot ; i > -1; i--) {
                    plists[i]->skipToDoc(did + 1);
                    popdown(plists, i, number_of_terms);
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
                } while(plists[i]->currPost != plists[pivot]->currPost);
			
                plists[least_i]->skipToDoc(plists[pivot]->currPost);
                popdown(plists, least_i, number_of_terms);
            }
        }

	
        for (int i = 0; i < number_of_terms; ++i) {
            delete plists[i];
        }
    
        delete[] plists;
        return candidate_results;
    }

};
#endif
