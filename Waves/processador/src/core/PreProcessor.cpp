#include "PreProcessor.h"
 
extern float totalTime;
extern struct timespec start_r, stop_r;

#define SHOW_TIME 1
#define SHOW_RESULTS 1

#define WAND_MODE 1
#define BMW_MODE 2
#define BMWCSP_MODE 3
#define BMWCAMDAS_MODE 4
#define WAVE_MODE 5
#define thirWAVES_MODE 6
#define thirBMW_MODE 7
#define BMWCS_MODE 8
#define NO_MODE 0

using namespace std;
PostingList **plistsGlob = NULL;

PreProcessor::PreProcessor(int _limiar, Index *_entireIndex, Index *_smallIndex, Index *_wholedocIndex, Index *_textIndex, Index *_titleIndex, Index *_anchorIndex, Index *_urlIndex, Vocabulario *_vocabulary, const char *_resultFile, int _processMode, int _indexType)  {
    this->limiarType = _limiar;
    this->entireIndex = _entireIndex;
    this->wholedocIndex = _wholedocIndex;
    this->smallIndex = _smallIndex;
    this->textIndex = _textIndex;
    this->anchorIndex = _anchorIndex;
    this->titleIndex = _titleIndex;
    this->urlIndex = _urlIndex;
    this->formula = 0;
    this->vocabulary = _vocabulary;
    this->_fpReults = fopen(_resultFile, "w");
    this->processMode = _processMode;
    this->indexType = _indexType;
}

PreProcessor::~PreProcessor() {
    fclose(this->_fpReults);
}

void PreProcessor::processQuery(const string _query) {
    int numberOfTerms;
    vector<Term> *terms;
    int numberOfResults;
    float idealDocScore = 0.0f;
    float time = 0.0f;

    vector<result> *tResults = NULL;
    terms = preProcessQuery(_query, numberOfTerms);
	
    cout<<"Number_of_terms "<<numberOfTerms<<" ";
	
    clock_gettime(CLOCK_REALTIME, &start_r); 
	
    switch (this->processMode){
        case WAND_MODE :
            tResults = wand->processQueryWAND(this->entireIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, this->topk, limiarType, this->indexType);
            break;
            
        case BMW_MODE:
            tResults = bmw->processQueryBMW(this->entireIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, this->topk, limiarType, this->indexType);
            break;
        
        case BMWCS_MODE :
            tResults = bmwcs->processQueryBMWCS(this->smallIndex, this->SecondSmallIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, plistsGlob, this->topk, limiarType, this->indexType);
            break;
        
        case BMWCSP_MODE :
            tResults = bmwcsp->processQueryBMWCSP(this->smallIndex, this->SecondSmallIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, plistsGlob, this->topk, limiarType, this->indexType);
            break;
            
        case WAVE_MODE : 
            tResults = waves->processQueryWave(this->smallIndex, this->SecondSmallIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, plistsGlob, this->topk, limiarType, this->indexType);
            break;
        
        case thirWAVES_MODE : 
            tResults = waves->processQueryTWave(this->smallIndex, this->SecondSmallIndex, this->entireIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, plistsGlob, this->topk, limiarType, this->indexType);
            break;
        
        case BMWCAMDAS_MODE : 
            tResults = bmwm->processQueryBMW_twoLayers(this->smallIndex, this->SecondSmallIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, this->topk, limiarType, this->indexType);
            break;
            
        case thirBMW_MODE : 
            tResults = bmwm->processQueryBMW_threeLayers(this->smallIndex, this->SecondSmallIndex, this->entireIndex, numberOfTerms, *terms, numberOfResults, idealDocScore, this->topk, limiarType, this->indexType);
            break;
	
        default:
            printf("------------MODO DE PROCESSAMENTO INEXISTENTE-----------------[%d]\n",this->processMode);
            exit(0);
    }
    
    clock_gettime(CLOCK_REALTIME, &stop_r);	
    time = ((stop_r.tv_sec - start_r.tv_sec) + ((double)(stop_r.tv_nsec - start_r.tv_nsec)/1000000000))*1000;									
    cout<<"TEMPO "<< time <<endl;
    totalTime += time;

    sort(tResults->begin(), tResults->end(), &resultComparator);

    if(tResults != NULL)
    delete tResults;
    delete terms;
}

int PreProcessor::getResultHeapSize() { 
    return this->topk; 
}

void PreProcessor::setResultHeapSize(int _newSize) { 
    this->topk = _newSize; 
}

void PreProcessor::setSecSIndex(Index *_SecSmallIndex){
    this->SecondSmallIndex = _SecSmallIndex;
}


vector<Term>* PreProcessor::preProcessQuery(const string &_query, int &_numberOfTerms) {
    int *termsIds;

    termsIds = simpleQueryParser.parseQuery(_query.c_str(), this->vocabulary, &_numberOfTerms);
    if(termsIds == NULL || _numberOfTerms==0) {
        if(termsIds)
            delete[] termsIds;
        _numberOfTerms = 0;
        return NULL;
    }

    vector<Term> *query_terms = new vector<Term>();
    query_terms->reserve(_numberOfTerms); 
    query_terms->resize(_numberOfTerms); 
    
    int tf = 1;
    int k = 0;

    for (int i = 0; i < _numberOfTerms; ++i) {
        tf = 1;
        if ((*query_terms)[i].tf != -1) { 
            for (int j = i+1; j < _numberOfTerms; ++j) {
                if (i != j) {
                    if (termsIds[i] == termsIds[j]) {
                        (*query_terms)[j].tf = -1;
                        tf ++;
                    }
                }
            }
            
            (*query_terms)[k].term_id = termsIds[i];
            (*query_terms)[k].tf = tf;
            k++;
        }
    }
    
    _numberOfTerms = k;
    delete[] termsIds;
    return query_terms;
}

inline void PreProcessor::saveResultsToFile(vector<result>* _results, FILE *_fpResults, int _idQuery) {
    int topK=0;
    fprintf(_fpResults,"\n %d\n ",_idQuery);
    for(vector<result>::iterator it = _results->begin(); it!= _results->end(); it++) {
        fprintf(_fpResults, "\t%d %d %d %f\n", topK, it->numberOfTerms, it->docId, it->score);
        topK++;
    }
}

inline void PreProcessor::showResults(vector<result>* _results){
    int topK=0;
    for(vector<result>::iterator it = _results->begin(); it!= _results->end(); it++) {
        printf("\t%d %d %d %f\n", topK, it->numberOfTerms, it->docId, it->score);
        topK++;
    }
    printf("total de %d resultados\n",topK);
}
