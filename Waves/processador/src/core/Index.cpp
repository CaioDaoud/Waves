/*
 * Index.cpp
 *
 *  Created on: 21/09/2010
 *      Author: felipe
 */

#include "Index.h"
#include <string>

FILE *openFile(char *_fileName){
    FILE *fp = fopen(_fileName, "r");
    if(fp == NULL){
        printf("Problemas na abertura do arquivo [%s]\n", _fileName);
        exit(0);
    }

    return fp;
}

Index::Index(const char *_indexBasePath, IndexType _type, int _topk ){
    Index(_indexBasePath, _type, _topk, NULL);
}

Index::Index(const char *_indexBasePath, IndexType _type, int _topk, const char *_basePath ){
    FILE *fp;
    long int size;
    this->type = _type;
    this->indexBasePath = _indexBasePath;
    this->basePath = _basePath;
    char fileName[4096];

    sprintf( fileName, "%sidx", this->indexBasePath );
    this->idxFile = openFile(fileName);
    fseek(this->idxFile, 0 , SEEK_END);
    size = ftell(this->idxFile)/sizeof(IDX);
    idxArray = new IDX[size];
    cout << "Tamanho do indice: " << size << endl;
    fseek(this->idxFile, 0 , SEEK_SET);
    fread(idxArray, size, sizeof(IDX), this->idxFile);
    this->numberOfTerms = size;

    sprintf( fileName, "%sidf", this->indexBasePath );
    fp = openFile(fileName);
    fseek(fp, 0, SEEK_END);
    size = ftell(fp)/sizeof(float);
    ftArray = new unsigned int[size];
    idfArray = new float[size];
    maxScoreArray = new float[size];
    minScoreArray = new float[size];
    thousandScoreArray = new float[size];
    memset(idfArray, 0, sizeof(float) * size);
    memset(minScoreArray, 0, sizeof(float) * size);
    memset(maxScoreArray, 0, sizeof(float) * size);
    memset(thousandScoreArray, 0, sizeof(float) * size);
    fseek(fp, 0, SEEK_SET);
    fread(idfArray, size, sizeof(float), fp);
    fclose(fp);
    
    sprintf( fileName, "%sFT", this->indexBasePath );
    fp = openFile(fileName);
    fread(ftArray, size, sizeof(unsigned int), fp);
    fclose(fp);
    
    sprintf( fileName, "%smax_score", this->indexBasePath );
    fp = openFile(fileName);
    fread(maxScoreArray, size, sizeof(float), fp);
    fclose(fp);

    sprintf( fileName, "%smin_score", this->indexBasePath);
    fp = openFile(fileName);
    fread(minScoreArray, size, sizeof(float), fp);
    fclose(fp);

    sprintf( fileName, "%sdist_max_min_score", this->indexBasePath );
    FILE *fm=fopen(fileName, "w");
    for(int i=0; i<size; i++) {
        fprintf(fm, "%.4f %.4f\n", maxScoreArray[i], minScoreArray[i]);
    }
    fclose(fm);

    sprintf( fileName, "%s%dMax_score",this->basePath, _topk);
    cout<<fileName<<endl;
    fp = openFile(fileName);
    fread(thousandScoreArray, size, sizeof(float), fp);
    printf("Abrindo arquivo [%s]\n", fileName); 
    fclose(fp);
    
    sprintf( fileName, "%snorma", this->indexBasePath );
    fp = openFile(fileName);
    fseek(fp, 0, SEEK_END);
    size = ftell(fp)/sizeof(float);
    this->numberOfDocs = size;
    normArray = new float[size];
    fseek(fp, 0, SEEK_SET);
    fread(normArray, size, sizeof(float), fp);
    fclose(fp);

    loadToMainMemory();
}	

Index::~Index(){
    fclose(this->idxFile);
    delete[] normArray;
    delete[] idfArray;
    delete[] idxArray;
    delete[] maxScoreArray;
    delete[] minScoreArray;
    delete[] thousandScoreArray;
}

float Index::getTermIdf(int _termId) {	
    return idfArray[_termId];
}

float Index::getDocNorm(int _docId) {	
    return normArray[_docId];
}

float Index::getTermMinScore(int _termId) { 
    return minScoreArray[_termId]; 
}
float Index::getTermMaxScore(int _termId) { 
    return maxScoreArray[_termId]; 
}

float Index::getTermMilScore(int _termId) { 
    return thousandScoreArray[_termId]; 
}

unsigned int Index::getNumberOfDocs() { 
    return this->numberOfDocs; 
}

void Index::loadToMainMemory(){
    long total = 0;
    int numberOfFiles = idxArray[this->numberOfTerms-1].id_arq+1;

    indexPointer = (unsigned char**)malloc(sizeof(unsigned char*) * numberOfFiles);
    skipPointer = (unsigned char**)malloc(sizeof(unsigned char*) * numberOfFiles);

    for(int i = 0; i < numberOfFiles; i++){
        char fileName[1000], skilFileName[1000];
        sprintf( fileName, "%sfrequencia%d.newFormat", this->indexBasePath, i );
        sprintf( skilFileName, "%sfreq_skip%d", this->indexBasePath, i );

        FILE *fpIndex = openFile(fileName);
        fseek(fpIndex, 0, SEEK_END);
        long indexSize = ftell(fpIndex);
        indexPointer[i] = (unsigned char*)malloc(sizeof(char) * indexSize);
        fseek(fpIndex, 0, SEEK_SET);
        fread(indexPointer[i], sizeof(char), indexSize, fpIndex);
        fclose(fpIndex);

        FILE *fpSkip = openFile(skilFileName);
        fseek(fpSkip, 0, SEEK_END);
        long skipSize = ftell(fpSkip);
        skipPointer[i] = (unsigned char*)malloc(sizeof(char) * skipSize);
        fseek(fpSkip, 0, SEEK_SET);
        fread(skipPointer[i], sizeof(char), skipSize, fpSkip);
        fclose(fpSkip);

        total += skipSize + indexSize;
    }

    cout << "Carregados " << total << " dados de indice" << endl;
}

PostingList* Index::getPostingList(int _termId){
    if (_termId > this->numberOfTerms) return new PostingList();
    IDX idx = idxArray[_termId];
    if (idx.freq_tam_lista == 0)	return new PostingList();
    
    PostingList *y = new PostingList();
    y->skipInit((unsigned int*)(indexPointer[(unsigned)idx.id_arq]), idx.freq_tam_lista, idx.freq_inicio_termo, (char*)(skipPointer[(unsigned)idx.id_arq]), idx.freq_tam_skip, idx.freq_inicio_skip,_termId);
    y->ft = ftArray[_termId];
    y->listMaxScore = maxScoreArray[_termId];
    y->listMinScore = minScoreArray[_termId];
    y->listThousandScore = thousandScoreArray[_termId];
    return y;

}

