#include "auxFunctions.h"

bool resultComparator(const result& x, const result& y) {
    if(fabs(x.score-y.score) > 0.00001)
        return x.score > y.score;
    return x.docId < y.docId;
}

bool resultDocIdComparator(const result& x, const result &y) {
    return x.docId < y.docId;
}

float calcScore(const float &pre, const float &idf, const float &freq, const float &norma, int index_type) {
    if(index_type == BM25_SCORE) {
        return ((pre * freq / ((float)(freq + 2 * ( 0.25 + 0.75 * (float)(norma)/(float)(130))))));
    }
    else if (index_type == VETORIAL_SCORE) {
        return (freq*idf*idf)/norma;
    }
    else {
        cerr<<"ERRO : Index_type INEXISTENTE "<<endl;
        exit(0);
    }
}

void insereNoRanking(const int SIZE_, int *pos, float *threshold, const result r_ , vector<result> *candidate_results ){
    if ( *pos < SIZE_ ) {  		  
        candidate_results->push_back( r_ ); 
        push_heap (candidate_results->begin(),candidate_results->end());
        *pos+=1;
        if( *pos==SIZE_){
            if( *threshold < (*candidate_results)[0].score){
                *threshold = (*candidate_results)[0].score;
            }
        }
    }
    else {
        pop_heap (candidate_results->begin(),candidate_results->end()); 
        candidate_results->pop_back();
        candidate_results->push_back(r_ ); 
        push_heap (candidate_results->begin(),candidate_results->end());
        
        if( *threshold < (*candidate_results)[0].score){
            *threshold = (*candidate_results)[0].score;
        }
    }
}

PostingList** getPostingLists(Index *index, vector<Term> &terms, int number_of_terms) {
    PostingList **plists = new PostingList*[number_of_terms];

    for (int i = 0; i < number_of_terms; ++i) {
        plists[i] = index->getPostingList(terms[i].term_id); 
        
        terms[i].idf = index->getTermIdf(terms[i].term_id);
        terms[i].min_score = index->getTermMinScore(terms[i].term_id);
        terms[i].max_score = index->getTermMaxScore(terms[i].term_id);
        terms[i].mil_score = index->getTermMilScore(terms[i].term_id);
    } 
    return plists;
}

PostingList** getPostingLists(Index *index, Index *sIndex, vector<Term> &terms, int number_of_terms) {
    PostingList **plists = new PostingList*[2*number_of_terms];

    for (int i = 0; i < number_of_terms; ++i) {
        plists[i] = index->getPostingList(terms[i].term_id); 
        
        terms[i].idf = index->getTermIdf(terms[i].term_id);
        terms[i].min_score = index->getTermMinScore(terms[i].term_id);
        terms[i].max_score = index->getTermMaxScore(terms[i].term_id);
        terms[i].mil_score = index->getTermMilScore(terms[i].term_id);
    }

    for (int i = 0; i < number_of_terms; ++i) {
        plists[i+number_of_terms] = sIndex->getPostingList(terms[i].term_id); 
    }
    return plists;
}

PostingList** getPostingLists(Index *index, Index *sIndex, Index *tIndex, vector<Term> &terms, int number_of_terms) {
    PostingList **plists = new PostingList*[3*number_of_terms];

    for (int i = 0; i < number_of_terms; ++i) {
        plists[i] = index->getPostingList(terms[i].term_id); 
        
        terms[i].idf = index->getTermIdf(terms[i].term_id);
        terms[i].min_score = index->getTermMinScore(terms[i].term_id);
        terms[i].max_score = index->getTermMaxScore(terms[i].term_id);
        terms[i].mil_score = index->getTermMilScore(terms[i].term_id);
    }

    for (int i = 0; i < number_of_terms; ++i) {
        plists[i+number_of_terms] = sIndex->getPostingList(terms[i].term_id); 
    }
	
    for (int i = 0; i < number_of_terms; ++i) {
        plists[i+(2*number_of_terms)] = tIndex->getPostingList(terms[i].term_id); 
    }
    
    return plists;
}


PostingList** secGetPostingLists(Index *index, vector<Term> &terms, int number_of_terms) {
    PostingList **plists = new PostingList*[number_of_terms];

    for (int i = 0; i < number_of_terms; ++i) {
        plists[i] = index->getPostingList(terms[i].term_id);
    }
    return plists;
}

void sortPostings(PostingList** mplists,  int mplistsSize) {
    int i, j;
    PostingList *plistTemp;
	
    for(i = 1; i < mplistsSize; i++)    {
        plistTemp= mplists[i];
        j = i-1;
        while(j>=0 && plistTemp->currPost < mplists[j]->currPost ) {
            mplists[j+1] = mplists[j];
            j = j-1;
        }
        mplists[j+1] = plistTemp;
    }
}

float calcLimiar(vector<Term> &terms, PostingList **plists, int &number_of_terms, int limiarType) {
    float limiar =0.0;
    
    switch (limiarType){	
        case 0 :
            return 0;
            break;
        case 1 :
            for (int i = 0; i < number_of_terms; ++i) {	
                if(terms[i].mil_score > limiar)	
                    limiar = terms[i].mil_score;
            }
            break;
        case 2:
            for (int i = 0; i < number_of_terms; ++i) {	
                if(plists[i]->listMinScore > limiar)
                    limiar = plists[i]->listMinScore;
            }
            break;
	
        default:
            printf("------------MODO DE LIMIAR INICIAL INEXISTENTE-----------------[%d]\n",limiarType);
            exit(0);
    }
    limiar -= 0.0001;
    return limiar;
}

void popdown(PostingList **plists, int i, int num) {
    PostingList *temp = plists[i];
    int j = i + 1;
    while( j < num && temp->currPost > plists[j]->currPost) {
        plists[j-1] = plists[j];
        j++;
    }
    plists[j - 1] = temp;
}

void sortPostingsCS(PostingList** mplists, PostingList** mSplists, int mplistsSize) {
    int i, j;
    PostingList *plistTemp;
    PostingList *SplistTemp;

    for(i = 1; i < mplistsSize; i++)    {
        plistTemp= mplists[i];
        SplistTemp= mSplists[i];
        j = i-1;
        while(j>=0 && plistTemp->currPost < mplists[j]->currPost ){
            mplists[j+1] = mplists[j];
            mSplists[j+1] = mSplists[j];
            j = j-1;
        }
        mplists[j+1]  = plistTemp;
        mSplists[j+1] = SplistTemp;
    }
}

void sortPostingsCS(PostingList** mplists, PostingList** mSplists, PostingList** mTplists, int mplistsSize) {
    int i, j;
    PostingList *plistTemp;
    PostingList *SplistTemp;
    PostingList *tplistTemp;

    for(i = 1; i < mplistsSize; i++)    {
        plistTemp= mplists[i];
        SplistTemp= mSplists[i];
        tplistTemp= mTplists[i];
        j = i-1;
        while(j>=0 && plistTemp->currPost < mplists[j]->currPost ){
            mplists[j+1]  = mplists[j];
            mSplists[j+1] = mSplists[j];
            mTplists[j+1] = mTplists[j];
            j = j-1;
        }
        mplists[j+1]  = plistTemp;
        mSplists[j+1] = SplistTemp;
        mTplists[j+1] = tplistTemp;
    }
}

void popdownCS(PostingList **plists, PostingList **splists, int i, int num) {
    PostingList *temp  = plists[i];
    PostingList *stemp = splists[i];
    int j = i + 1;
    while( j < num && temp->currPost > plists[j]->currPost) {
        plists[j-1]  = plists[j];
        splists[j-1] = splists[j];
        j++;
    }
    plists[j - 1] = temp;
    splists[j - 1] = stemp;
}

void popdownCS(PostingList **plists, PostingList **splists, PostingList **tplists, int i, int num) {
    PostingList *temp  = plists[i];
    PostingList *stemp = splists[i];
    PostingList *ttemp = tplists[i];
    int j = i + 1;
    while( j < num && temp->currPost > plists[j]->currPost) {
        plists[j-1]  = plists[j];
        splists[j-1] = splists[j];
        tplists[j-1] = tplists[j];
        j++;
    }
    plists[j - 1] = temp;
    splists[j - 1] = stemp;
    tplists[j - 1] = ttemp;
}

unsigned int movepointer(PostingList *plist, unsigned int did){
    while (did > plist->bufferSkipDocId[plist->checkblock]) {
        plist->checkblock ++;
    }	
    return plist->bufferSkipDocId[plist->checkblock] + 1 ;
}
