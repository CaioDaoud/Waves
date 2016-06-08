#ifndef WAVES_H_
#define WAVES_H_

#include "utils/auxFunctions.h"

class Waves{
public:
    vector<result>  * processQueryWave(Index *index, Index *secIndex, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, PostingList** plists, int RESULT_HEAP_SIZE, int limiarType,int index_type){

        float s, ms, ms_aux, scoret,maxposs, upper;
        s=ms=ms_aux=scoret=maxposs=upper=0.0f;
        unsigned int did,minpivot,tempd, f, least_ft, least_i;
        did=minpivot=tempd=f=least_ft=least_i=0;
        unsigned int MAXD = std::numeric_limits<unsigned int>::max();

        float score;
        int pivot =0;
        int pos =0;
        int k = RESULT_HEAP_SIZE;

        float threshold = 0.0f;
        vector<result> *candidate_results;
        candidate_results = new vector<result>();
        candidate_results->reserve(RESULT_HEAP_SIZE);


        int numLis =0;
        if (number_of_terms == 0)
            return new vector<result>();
    
        plists = getPostingLists(index, terms, number_of_terms);
        PostingList **secPlists = getPostingLists(secIndex, terms, number_of_terms);
    
        if(plists == NULL)	{
            delete plists; 
            delete secPlists; 
            return new vector<result>();
        }

        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            threshold = threshold<plists[i]->listMinScore?plists[i]->listMinScore:threshold;
            ms_aux += plists[i]->listMinScore;
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
            delete secPlists; 
            return new vector<result>();
        }
    
        for (int i = 0; i < number_of_terms; ++i) {
            plists[i]->pre= (float)3 * (float)(log( 1 + (float)(index->getNumberOfDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
            plists[i]->moveToFirst();
            secPlists[i]->moveToFirst();
        }
            
        threshold = calcLimiar(terms, plists, number_of_terms,limiarType);
        sortPostingsCS(plists, secPlists, number_of_terms);
    
        while( true ){
            pivot = -1;
            s = 0.0f;
            ms = ms_aux;
            upper = 0.0f;
    
            for(int i = 0; i < number_of_terms; i ++){
                s  += plists[i]->listMaxScore;
                ms -= plists[i]->listMinScore;
                upper = s + ms;
    
                if( upper > threshold){
                    while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost){	 
                        i++; ms -= plists[i]->listMinScore;
                    }
                    pivot = i;
                    break;
                }					
            }
	
            if(pivot == -1 || plists[pivot]->currPost == MAXD)
                break; 
        
            maxposs = plists[pivot]->bufferSkipMaxScore[ plists[pivot]->atSkip];	
            did = plists[pivot]->currPost ;
                    
            for(int i = 0; i < pivot; i++){
                movepointer(plists[i], did);
                if(plists[i]->checkblock < plists[i]->sizeSkip){
                    maxposs += plists[i]->bufferSkipMaxScore[ plists[i]->checkblock];
                }
                else {
                    plists[i]->endOfList();
                    popdownCS(plists, secPlists, i, number_of_terms);
                    pivot--;
                }	
            }
            
            upper = maxposs + ms;
            
            if(upper >= threshold ) {
                numLis =0;
                upper=maxposs;
    
                for (int j = pivot+1; j < number_of_terms; j++){
                    if(secPlists[j]->sizeList > 0){
                        movepointer(secPlists[j], did);
                        if(secPlists[j]->checkblock < secPlists[j]->sizeSkip)
                            maxposs+=secPlists[j]->bufferSkipMaxScore[ secPlists[j]->checkblock];
                    }
                }
    
                upper=upper+maxposs;
        
                if(upper >= threshold ){
                    if( did == plists[0]->currPost ){
                        score = 0.0f;
                        for(int i = 0 ; i <= pivot; i++) {
                            f = plists[i]->getFreq();
                            scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                            score+=scoret;
                            numLis++;
                            upper -= ( plists[i]->bufferSkipMaxScore[ plists[i]->checkblock] - scoret );
    
                            if( !(upper > threshold) )
                                break;
                        }
                        
                        upper=score+maxposs;
                        if(score ==0)
                            exit(1);
                                    
                        if (upper > threshold){
                            scoret=0.0f;
                            
                            for (int j = pivot+1; j < number_of_terms; j++){
                                if(secPlists[j]->sizeList > 0){
                                    if(secPlists[j]->currPost < did) {
                                        secPlists[j]->skipToDoc(did);
                                    }
                
                                    if(secPlists[j]->currPost == did){
                                        numLis++;
                                        f = secPlists[j]->getFreq(); 
                                        scoret = calcScore( plists[j]->pre, terms[j].idf , (float)(f), index->getDocNorm(did),index_type);
                                        score+=scoret;
                                    }
                                }
                            }

                            if(score>=threshold){
                                insereNoRanking(k,  &pos, &threshold, result(did, score, numLis), candidate_results);
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
            else {
                least_ft = plists[0]->ft;
                least_i = 0;
                minpivot = plists[0]->bufferSkipDocId[ plists[0]->atSkip ]+1;
                minpivot = minpivot > MAXD ? MAXD : minpivot;
    
                for(int i = 1; i < pivot + 1; i++) {
                    tempd = plists[i]->bufferSkipDocId[plists[i]->checkblock]+1;			
                    if( minpivot >  tempd)
                        minpivot = tempd;
                    if( plists[i]->ft < least_ft) {
                        least_ft = plists[i]->ft;
                        least_i = i;
                    }
                }
    
                if( pivot < number_of_terms - 1){
                    did = minpivot > plists[ pivot + 1 ]->currPost ? plists[ pivot + 1 ]->currPost : minpivot; 
                }
                else {
                    did = minpivot;
                }
    
                if( did <= plists[pivot]->currPost){
                    did = plists[pivot]->currPost + 1;
                }
                
                if(did > MAXD)
                    break;
            
                plists[least_i]->skipToDoc(did );
                popdownCS(plists, secPlists, least_i, number_of_terms);	
            }	
        }
	
        if (ms_aux > threshold) {
            vector<result> *candidate_Sort = NULL;

            for (int i = 0; i < number_of_terms; ++i) {
                secPlists[i]->pre = plists[i]->pre;
                secPlists[i]->moveToFirst();
            }
    
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
		
            int meio=0;
            int ini = 0;
            int fim=0;
            bool autoriza;
            score =0.0f;
            pivot=0;

            sortPostings(secPlists, number_of_terms);

            while( true ) {
                pivot = -1;
                score = 0.0f;
                for(int i = 0; i < number_of_terms; i ++) {
                    score += secPlists[i]->listMaxScore;
                    if( score > threshold) {
                        while( i + 1 < number_of_terms && secPlists[i + 1]->currPost == secPlists[i]->currPost){
                            i++; ms -= plists[i]->listMinScore;
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
        
                if( maxposs > threshold ) {
                    if( did == secPlists[0]->currPost ) {
                        score = 0.0f;
                        for(int i = 0 ; i < pivot + 1; i++) {
                            f = secPlists[i]->getFreq();
                            scoret = calcScore( secPlists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                            score+=scoret;
                        }
                        for(int i = 0 ;i < pivot + 1; i++) {
                            secPlists[i]->skipToDoc(did + 1);
                        }

                        if (score >= threshold) {
                            vector<result>::iterator it;
                            if(candidate_Sort == NULL){
                                candidate_Sort = new vector<result>();
                                for (it = candidate_results->begin(); it!= candidate_results->end(); it++){
                                    candidate_Sort->push_back(*it);
                                }
                                sort(candidate_Sort->begin(), candidate_Sort->end(), &resultDocIdComparator);								 
                            }
                            it = candidate_Sort->begin();
                            autoriza = true;
                            meio=0;
                            ini = 0;
                            fim = candidate_Sort->size();
                            
                            while(ini<=fim) {
                                meio = (fim+ini)/2;
                                if((unsigned)it[meio].docId == did){
                                    autoriza = false;
                                    break;
                                }
                                else if((unsigned)it[meio].docId < did) {
                                    ini=meio+1;
                                } 
                                else
                                    fim=meio-1;
                            }
                            
                            if(autoriza){
                                insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                            }
                        }
    
                        for(int i = pivot ; i > -1; i--) {
                            popdown(secPlists, i, number_of_terms);
                        }
                    }
                    else {
                        least_ft = secPlists[0]->ft;
                        least_i = 0;
                        for(int i = 1; i < pivot ; i++){
                            if( secPlists[i]->ft < least_ft &&  secPlists[i]->currPost != did ) {
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
            delete plists[i];
        }
    
        for (int i = 0; i < number_of_terms; ++i) {
            delete secPlists[i];
        }
    
        delete[] secPlists;
        delete[] plists;

        return candidate_results;
    }

    vector<result> * processQueryTWave(Index *index, Index *secIndex, Index *thirIndex, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, PostingList** plists, int RESULT_HEAP_SIZE, int limiarType, int index_type){

        float s, ms,  scoret,maxposs, upper;
        s= ms=  scoret=maxposs= upper=0.0f;
        float sec_max =0.0f;
        unsigned int minpivot,tempd, f, least_ft, least_i;
        minpivot=tempd=f=least_ft=least_i =0;
	
        int k = RESULT_HEAP_SIZE;
        unsigned int MAXD = std::numeric_limits<unsigned int>::max();
        float score=0.0f;
        float thir_max=0.0f;
        unsigned int did= 0;
        int pivot =0;
        int listId[number_of_terms];
        int n_Thir=0;
        int pos=0;
        float threshold = 0.0f;
        vector<result> *candidate_results;
        candidate_results = new vector<result>();
        candidate_results->reserve(RESULT_HEAP_SIZE);
    
        int numLis =0;
        if (number_of_terms == 0)
            return new vector<result>();
            
        plists  = getPostingLists(index, terms, number_of_terms);
        PostingList **secPlists  = secGetPostingLists(secIndex, terms, number_of_terms);
        PostingList **thirPlists = secGetPostingLists(thirIndex, terms, number_of_terms);
    
        if(plists == NULL)
            return new vector<result>();
            
        for (int i = 0; i < number_of_terms; ++i) {
            int j;
            threshold = threshold<plists[i]->listMinScore?plists[i]->listMinScore:threshold;
            while((plists[i]->sizeList == 0) && (i<number_of_terms)) {
                delete plists[i];
                for(j=i ; j<number_of_terms-1;j++) {
                    plists[j] = plists[j+1];
                    secPlists[j] = secPlists[j+1];
                    thirPlists[j] = thirPlists[j+1];
                }
                number_of_terms--;
            }
        }
    
        if(number_of_terms == 0) {
            delete plists; 
            delete secPlists; 
            delete thirPlists; 
            return new vector<result>();
        }
    
        threshold = calcLimiar(terms, plists, number_of_terms, limiarType);
            
        sec_max =0;
        for (int i = 0; i < number_of_terms; ++i) {
            plists[i]->pre= (float)3 * (float)(log( 1 + (float)(index->getNumberOfDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
            plists[i]->moveToFirst();   
            secPlists[i]->moveToFirst();  
            thirPlists[i]->moveToFirst();
            sec_max += plists[i]->listMinScore;
        }
    
        sortPostingsCS(plists, secPlists, thirPlists, number_of_terms);
    
        while( true ){
            pivot = -1;
            s = 0.0f;
            ms = sec_max;
            upper=0.0f;
    
            for(int i = 0; i < number_of_terms; i ++){
                s  += plists[i]->listMaxScore;
                ms -= plists[i]->listMinScore;
                upper = s + ms;
    
                if( upper > threshold){
                    while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost){ 
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
            
            maxposs = plists[pivot]->bufferSkipMaxScore[ plists[pivot]->atSkip];
            did = plists[pivot]->currPost ;
                    
            for(int i = 0; i < pivot; i++){
                movepointer(plists[i], did);			
                if(plists[i]->checkblock < plists[i]->sizeSkip)
                    maxposs += plists[i]->bufferSkipMaxScore[ plists[i]->checkblock]; 
            }
                    
            upper = maxposs + ms;
            if(upper > threshold ){
                numLis =0;
                if( did == plists[0]->currPost ){
                    score = 0.0f;
                    for(int i = 0 ; i <= pivot; i++) {
                        f = plists[i]->getFreq();
                        scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                        score+=scoret;
                        numLis++;
                        upper -= ( plists[i]->bufferSkipMaxScore[ plists[i]->atSkip ] - scoret );
                        if( !(upper > threshold) )
                            break;
                    }
                    if(score ==0)
                        exit(1);
                    upper =score+ms;
    
                    if (upper > threshold){
                        upper=0.0f;
                        
                        for (int j = pivot+1; j < number_of_terms; j++){
                            if(secPlists[j]->sizeList > 0){
                                movepointer(secPlists[j], did);
                                if(secPlists[j]->checkblock < secPlists[j]->sizeSkip)
                                    upper+=secPlists[j]->bufferSkipMaxScore[ secPlists[j]->checkblock];
                            }
                        }
    
                        upper= upper+score;
                    
                        if (upper > threshold){
                            n_Thir=0;
                            scoret=0.0f;
                            upper = 0.0f;
                            for (int j = pivot+1; j < number_of_terms; j++){
                                if(secPlists[j]->sizeList > 0){
                                    if(secPlists[j]->currPost < did) {
                                        secPlists[j]->skipToDoc(did);
                                    }
                                    if(secPlists[j]->currPost == did){
                                        numLis++;
                                        f = secPlists[j]->getFreq(); 
                                        scoret = calcScore( plists[j]->pre, terms[j].idf , (float)(f),index->getDocNorm(did),index_type);
                                        score+=scoret;
                                    }
                                    else {
                                        listId[n_Thir]=j;
                                        n_Thir++;
                                        upper += secPlists[j]->listMinScore;
                                    }
                                }
                            }	
                            if (score+upper >= threshold){
                                if(n_Thir>0){
                                    for (int j = 0; j < n_Thir; j++){
                                        if(thirPlists[listId[j]]->sizeList > 0){
                                            movepointer(thirPlists[listId[j]], did);
                                            
                                            /*if(thirPlists[listId[j]]->checkblock < thirPlists[listId[j]]->tam_skip)
                                                thirPlists[listId[j]]->buffer_skip_max_score[ thirPlists[listId[j]]->checkblock];*/
                                            if(thirPlists[listId[j]]->currPost < did) 
                                                thirPlists[listId[j]]->skipToDoc(did);
                                            if(thirPlists[listId[j]]->currPost == did){
                                                numLis++;
                                                f = thirPlists[listId[j]]->getFreq(); 
                                                scoret = calcScore( plists[listId[j]]->pre, terms[listId[j]].idf , (float)(f),index->getDocNorm(did),index_type);
                                                score += scoret;
                                            }
                                        }
                                    }
                                }
                                if(score>=threshold){
                                    insereNoRanking(k,  &pos, &threshold, result(did, score, numLis), candidate_results);
                                }
                            }
                        }
                    }
    
                    for(int i = pivot ; i > -1; i--){
                        plists[i]->skipToDoc(did + 1);
                        popdownCS(plists, secPlists, thirPlists, i, number_of_terms);
                    }
                }
                else {
                    least_ft = plists[0]->ft;
                    least_i = 0;
                    int i = 0; 
                    do {
                        if(plists[i]->ft < least_ft){
                            least_ft = plists[i]->ft;
                            least_i = i;
                        }				
                        i++;
                    } while(plists[i]->currPost != plists[pivot]->currPost);
                    plists[least_i]->skipToDoc(did);
                    popdownCS(plists, secPlists, thirPlists, least_i, number_of_terms);			
                }
            }
            else{
                minpivot = plists[0]->bufferSkipDocId[ plists[0]->atSkip ]+1;
                minpivot = minpivot > MAXD ? MAXD : minpivot;
                least_ft = plists[0]->ft;
                least_i = 0;
                
                for(int i = 1; i < pivot + 1; i++) {
                    tempd = plists[i]->bufferSkipDocId[plists[i]->checkblock]+1;
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
                popdownCS(plists, secPlists, thirPlists, least_i, number_of_terms);
    
            }
        }
        
        vector<result> *candidate_Sort =NULL;
        bool autoriza;
    
        if (sec_max > threshold) {
            upper=0.0f;
            for (int i = 0; i < number_of_terms; ++i) {
                secPlists[i]->pre = plists[i]->pre; 
                secPlists[i]->moveToFirst(); 
                thirPlists[i]->moveToFirst(); 
            
                if(thirPlists[i]->sizeList > 0)
                    thir_max += secPlists[i]->listMinScore;
            }
    
            ms = thir_max;
        
            for (int i = 0; i < number_of_terms; ++i) {
                int j;
                while((secPlists[i]->sizeList == 0) && (i<number_of_terms)) {
                    delete secPlists[i];
                    delete thirPlists[i];
                    for(j=i ; j<number_of_terms-1;j++) {
                        secPlists[j] = secPlists[j+1];
                        thirPlists[j] = thirPlists[j+1];
                    }
                    number_of_terms--;
                }
            }
                    
            int meio=0;
            int ini = 0;
            int fim=0;;
            score =0.0f;
            pivot=0;
                    
            sortPostingsCS(secPlists, thirPlists, number_of_terms);
            if(number_of_terms>0)
                    
            while( true ){
                ms = thir_max;
                pivot = -1;
                s = 0.0f;
    
                for(int i = 0; i < number_of_terms; i ++){
                    s  += secPlists[i]->listMaxScore;
                    ms -= secPlists[i]->listMinScore;
                    upper = s + ms;
    
                    if( upper > threshold){
                        while( i + 1 < number_of_terms && secPlists[i + 1]->currPost == secPlists[i]->currPost) { 
                            i++; 
                            ms -= secPlists[i]->listMinScore;
                        }
                        pivot = i;
                        break;
                    }	
                }
    
                if(pivot == -1 || secPlists[pivot]->currPost == MAXD)
                    break; 
            
                maxposs = secPlists[pivot]->bufferSkipMaxScore[ secPlists[pivot]->atSkip];	
                did = secPlists[pivot]->currPost ;
            
                for(int i = 0; i < pivot; i++) {
                    movepointer(secPlists[i], did);			
                    if(secPlists[i]->checkblock < secPlists[i]->sizeSkip)
                        maxposs += secPlists[i]->bufferSkipMaxScore[ secPlists[i]->checkblock];
                }
                upper = maxposs + ms;		
    
                if(upper > threshold ){
                    numLis =0;
                    if( did == secPlists[0]->currPost ){
                        score = 0.0f;
                        for(int i = 0 ; i <= pivot; i++) {
                            f = secPlists[i]->getFreq();
                            scoret = calcScore( secPlists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                            score+=scoret;
                            numLis++;
    
                            upper -= ( secPlists[i]->bufferSkipMaxScore[ secPlists[i]->atSkip ] - scoret );
    
                            if( !(upper > threshold) )
                                break;
                        }
                        if(score ==0)
                                exit(1);
    
                        upper=score+ms;
            
                        if (upper > threshold){
                            upper=0.0f;

                            for (int j = pivot+1; j < number_of_terms; j++){
                                if(thirPlists[j]->sizeList > 0){
                                    movepointer(thirPlists[j], did);
                                    if(thirPlists[j]->checkblock < thirPlists[j]->sizeSkip)
                                        upper+=thirPlists[j]->bufferSkipMaxScore[ thirPlists[j]->checkblock];
                                }
                            }
    
                            upper= upper+score;
                                                    
                            if (upper > threshold){
                                n_Thir=0;
                                scoret=0.0f;
                                for (int j = pivot+1; j < number_of_terms; j++){
                                    if(thirPlists[j]->sizeList > 0){
                                        if(thirPlists[j]->currPost < did) {
                                            thirPlists[j]->skipToDoc(did);
                                        }
                                        if(thirPlists[j]->currPost == did){
                                            numLis++;
                                            f = thirPlists[j]->getFreq(); 
                                            scoret = calcScore( secPlists[j]->pre, terms[j].idf , (float)(f),index->getDocNorm(did),index_type);
                                            score+=scoret;
                                        }   
                                    }
                                }	
                        
                                if(score >= threshold){
                                    vector< result >::iterator it;
                                    if(candidate_Sort == NULL ){
                                        candidate_Sort = new vector<result>();
                                        for (it = candidate_results->begin(); it!= candidate_results->end(); it++){
                                            candidate_Sort->push_back(*it);
                                        }
                                        sort(candidate_Sort->begin(), candidate_Sort->end(), &resultDocIdComparator);		
                                    }
                            
                                    it = candidate_Sort->begin();
                                    autoriza = true;
                                    meio=0;
                                    ini = 0;
                                    fim = k-1;
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
                                        insereNoRanking(k,  &pos, &threshold, result(did, score, numLis), candidate_results);
                                    }
                                }
                            }
                        }
    
                        for(int i = pivot ; i > -1; i--){
                            secPlists[i]->skipToDoc(did + 1);
                            popdownCS(secPlists, thirPlists, i, number_of_terms);
                        }
                    }
                    else {
                        least_ft = secPlists[0]->ft;
                        least_i = 0;
                        int i = 0; 
                        
                        do {
                            if(secPlists[i]->ft < least_ft){
                                least_ft = secPlists[i]->ft;
                                least_i = i;
                            }				
                            i++;
                        } while(secPlists[i]->currPost != did);
                        secPlists[least_i]->skipToDoc(did);
                        popdownCS( secPlists, thirPlists, least_i, number_of_terms);			
                    }
                }
                else {
                    least_ft = secPlists[0]->ft;
                    least_i = 0;
                    minpivot = secPlists[0]->bufferSkipDocId[ secPlists[0]->atSkip ]+1;
                    minpivot = minpivot > MAXD ? MAXD : minpivot;
            
                    for(int i = 1; i < pivot + 1; i++){
                        tempd = secPlists[i]->bufferSkipDocId[secPlists[i]->checkblock]+1;	
                        if( minpivot >  tempd)
                            minpivot = tempd;
                        if( secPlists[i]->ft < least_ft){
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
                    popdownCS(secPlists, thirPlists, least_i, number_of_terms);
                }
            }
        }
    
        if (thir_max > threshold) {
            vector<result> *candidate_Sort =NULL;
        
            for (int i = 0; i < number_of_terms; ++i) {
                thirPlists[i]->pre = secPlists[i]->pre; 
                thirPlists[i]->moveToFirst();
            }
        
            for (int i = 0; i < number_of_terms; ++i) {
                int j;
                while((thirPlists[i]->sizeList == 0) && (i<number_of_terms)) {
                    delete thirPlists[i];
                    for(j=i ; j<number_of_terms-1;j++) {
                        thirPlists[j] = thirPlists[j+1];
                    }
                    number_of_terms--;
                }
            }
        
            score =0.0f;
            pivot=0;
    
            sortPostings(thirPlists, number_of_terms);
            if(number_of_terms>0)
            
            while( true ) {
                pivot = -1;
                score = 0.0f;
                for(int i = 0; i < number_of_terms; i ++) {
                    score += thirPlists[i]->listMaxScore;
                    if( score > threshold) {
                        while( i + 1 < number_of_terms && thirPlists[i + 1]->currPost == thirPlists[i]->currPost)	 
                            i++;
                        pivot = i;
                        break;
                    }
                }
                
                if(pivot == -1 || thirPlists[pivot]->currPost == MAXD)   
                    break;
                
                maxposs = thirPlists[pivot]->bufferSkipMaxScore[ thirPlists[pivot]->atSkip];	
                did = thirPlists[pivot]->currPost ;
                            
                for(int i = 0; i < pivot; i++) {
                    movepointer(thirPlists[i], did);
                    if(plists[i]->checkblock < plists[i]->sizeSkip)
                    maxposs += thirPlists[i]->bufferSkipMaxScore[ thirPlists[i]->checkblock];
                }
        
                if( maxposs > threshold ) {
                    if( did == thirPlists[0]->currPost ) {
                        score = 0.0f;
                        for(int i = 0 ; i < pivot + 1; i++) {
                            f = thirPlists[i]->getFreq();
                            scoret = calcScore( thirPlists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did),index_type);
                            score+=scoret;
                        }
                        
                        for(int i = 0 ;i < pivot + 1; i++) {
                            thirPlists[i]->skipToDoc(did + 1);
                        }
                        
                        if (score >= threshold){
                            vector< result >::iterator it;
                            if(candidate_Sort == NULL ){
                                candidate_Sort = new vector<result>();
                                for (it = candidate_results->begin(); it!= candidate_results->end(); it++){
                                    candidate_Sort->push_back(*it);
                                }
                                sort(candidate_Sort->begin(), candidate_Sort->end(), &resultDocIdComparator);		
                            }
                                                
                            it = candidate_Sort->begin();
                            autoriza = true;
                            int meio=0;
                            int ini = 0;
                            int fim = k-1;
                            
                            while(ini<=fim){
                                meio = (fim+ini)/2;
                                if((unsigned)it[meio].docId == did){
                                    autoriza = false;
                                    break;
                                }
                                else if((unsigned)it[meio].docId < did) {
                                    ini=meio+1;
                                } 
                                else
                                    fim=meio-1;
                            }
                            
                            if(autoriza){
                                insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
                            }
                        }
    
                        for(int i = pivot ; i > -1; i--) {
                            popdown(thirPlists, i, number_of_terms);
                        }
                    }
                    else {
                        least_ft = thirPlists[0]->ft;
                        least_i = 0;
                        int i = 0; 
                        do {
                            if(thirPlists[i]->ft < least_ft){
                                least_ft = thirPlists[i]->ft;
                                least_i = i;
                            }				
                            i++;
                        } while(thirPlists[i]->currPost != did);
                        
                        thirPlists[least_i]->skipToDoc(did );
                        popdown(thirPlists,least_i, number_of_terms);
                    }
                }
                else {
                    least_ft = thirPlists[0]->ft;
                    least_i = 0;
                    minpivot = thirPlists[0]->bufferSkipDocId[ thirPlists[0]->atSkip ]+1;
                    minpivot = minpivot > MAXD ? MAXD : minpivot;
            
                    for(int i = 1; i < pivot + 1; i++){
                        tempd = thirPlists[i]->bufferSkipDocId[thirPlists[i]->checkblock]+1;			
                        if( minpivot >  tempd)
                            minpivot = tempd;
                        if( thirPlists[i]->ft < least_ft){
                            least_ft = thirPlists[i]->ft;
                            least_i = i;
                        }
                    }
    
                    if( pivot < number_of_terms - 1)
                        did = minpivot > thirPlists[ pivot + 1 ]->currPost ? thirPlists[ pivot + 1 ]->currPost : minpivot; 
                    else
                        did = minpivot;
    
                    if( did <= thirPlists[pivot]->currPost)
                        did = thirPlists[pivot]->currPost + 1;
                    if(did > MAXD)
                        break;
                            
                    thirPlists[least_i]->skipToDoc(did );
                    popdown(thirPlists, least_i, number_of_terms);	
                }
            }
        }
	
        delete candidate_Sort;

        for (int i = 0; i < number_of_terms; ++i) {
            delete secPlists[i];	
        }
        delete[] secPlists;
        
        for (int i = 0; i < number_of_terms; ++i) {	
            delete thirPlists[i];
        }
        delete[] thirPlists;
        
        for (int i = 0; i < number_of_terms; ++i) {	
            delete plists[i];
        }
        delete[] plists;
    
        return candidate_results;
    }

};
#endif
