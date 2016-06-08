#include <stdio.h>      
#include <time.h>      
#include <math.h>    
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sys/time.h>
#include <ctime>
#include "core/Index.h"
#include "core/PreProcessor.h"
#include "utils/ConfigFile.h"

float totalTime;
struct timespec start_r, stop_r;
struct timespec star, sto;
                


int main(int argc, char **argv) {
    startConvertWithoutAccent();
    
    if(argc != 2) {
        printf ("\t <Processador> Execute %s <arq.config> \n", argv[0]);
        exit(0);
    }
    
    string configPath = argv[1];
    
    ConfigFile config(configPath);
   
    cout << "Starting Query Processor ...  " << endl;

    string metodo(config.read<string>("metodo"));
    
    int process_mode;
    if(!metodo.compare("WAND") || !metodo.compare("wand")) {
        printf("Processing in mode WAND!\n");
        process_mode = 1;
    } 
    else if(!metodo.compare("BMW") || !metodo.compare("bmw")) {
        printf("Processing in mode BMW!\n");
        process_mode = 2;
    }
    else if(!metodo.compare("CSP") || !metodo.compare("csp")) {
        printf("Processing in mode CSP \n");
        process_mode = 3;
    }
    else if(!metodo.compare("BMWLAYER") || !metodo.compare("bmwlayer")) {
        printf("Processing in mode  BMW-Camdas de 2011 \n");
        process_mode = 4;
    }
    else if(!metodo.compare("WAVE") || !metodo.compare("wave")) {
        printf("Processing in mode  WAVE \n");
        process_mode = 5;
    }
    else if(!metodo.compare("TWAVE") || !metodo.compare("twave")) {
        printf("Processing in mode  TWAVE\n");
        process_mode = 6;
    }
    else if(!metodo.compare("TBMWLAYER") || !metodo.compare("tbmwlayer")) {
        printf("Processing in mode  TBMW\n");
        process_mode = 7;
    }
    else if(!metodo.compare("CS") || !metodo.compare("cs")) {
        printf("Processing in mode  CS\n");
        process_mode = 8;
    }
    else {
        printf("modo não existe !\n");
        process_mode = 0;
    }

    string scoreType(config.read<string>("score"));
    
    int index_type;
    if(!scoreType.compare("VET") || !scoreType.compare("vet")) {
        printf("Using Vetorial score!\n");
        index_type = 3;
    }
    else if(!scoreType.compare("UTI") || !scoreType.compare("uti")) {
        printf("Using UTI score!\n");
        index_type = 1;
    } 
    else if(!scoreType.compare("BM25") || !scoreType.compare("bm25")) {
        printf("Using BM25 score!\n");
        index_type = 2;
    }
    else {
        cout << "Modo de score inexistente " << scoreType << endl;
        exit(0);
    }

    int resultHeapSize = config.read<int>("topk");

    Index *smallIndex=NULL;
    Index *SecondSmallIndex=NULL;
    Index *entireIndex=NULL;
    
    string camadaUnica(config.read<string>("camadaUnica"));
    
    if(process_mode == 3 || process_mode == 4 || process_mode == 5 || process_mode == 8) {
        string camada1(config.read<string>("camada1"));
        string camada2(config.read<string>("camada2"));
        
        cout << "Carregando arquivo com primeira camada " << camada1 << endl;
        smallIndex = new Index(camada1.c_str(), Index::UTI, resultHeapSize, camadaUnica.c_str()  );

        cout << "Carregando arquivo com segunda camada" << camada2 << endl;
        SecondSmallIndex = new Index(camada2.c_str(), Index::UTI,resultHeapSize,camadaUnica.c_str()  );

    }
    else if(process_mode == 6 || process_mode == 7){
        string camada1(config.read<string>("camada1"));
        string camada2(config.read<string>("camada2"));
        string camada3(config.read<string>("camada3"));
        
        cout << "Carregando arquivo com primeira camada " << camada1 << endl;
        smallIndex = new Index(camada1.c_str(), Index::UTI, resultHeapSize,camadaUnica.c_str() ) ;

        cout << "Carregando arquivo com segunda camada " << camada2 << endl;
        SecondSmallIndex = new Index(camada2.c_str(), Index::UTI, resultHeapSize,camadaUnica.c_str()  );

        cout << "Carregando arquivo com terceira camada " << camada3 << endl;
        entireIndex = new Index(camada3.c_str(), Index::UTI, resultHeapSize,camadaUnica.c_str()  );

    }
    else{
        cout << "Carregando arquivo com camada unica " << camadaUnica << endl;
        entireIndex = new Index(camadaUnica.c_str(), Index::UTI, resultHeapSize,camadaUnica.c_str()  );
    }
   
    Index *wholedocIndex = NULL;
    Index *textIndex = NULL;
    Index *anchorIndex = NULL;
    Index *titleIndex = NULL;
    Index *urlIndex = NULL;

    string vocabulario = camadaUnica;
    Vocabulario *voc = new Vocabulario(10000, 5000, (char*)vocabulario.c_str(), 2);
    
    string dataset = camadaUnica;

    int limiar = config.read<int>("limiar");
    string resultFile = config.read<string>("resultFile");
    
    PreProcessor processor(limiar, entireIndex, smallIndex, wholedocIndex, textIndex, titleIndex, anchorIndex, urlIndex, voc, resultFile.c_str(), process_mode, index_type);

    processor.setSecSIndex(SecondSmallIndex);
    processor.setResultHeapSize(resultHeapSize);
    printf("Heap size: %d\n", processor.getResultHeapSize());

    int cont_linha=0;
    
    string interface = config.read<string>("mode");
    cout << "Carregando arquivo de query " << argv[1] << endl;
    
    if (!interface.compare("queryFile")) {
        string queryFile = config.read<string>("query");
        string query;                
        
        ifstream in(queryFile.c_str(), ifstream::in);
        
        if(!in.is_open()){
            cout << "Erro ao abrir arquivo de query " << queryFile << endl;
            exit(0);
        }
        
        getline(in, query);
        while(in.good()) {
            printf("\nQID: %d ", cont_linha);
            processor.processQuery(query);
            cont_linha++;
            getline(in, query);
        }
        in.close();
    }
    else{
        char q[1024];
        while(true) {
            cout << "Enter query: " << endl;
            cin.getline(q, 1024);
            processor.processQuery(string(q));
        }
    }

   printf("\n\n-----\nTotal Time is: %f ms \nAVG_Time is  : %f ms \n-----\n",totalTime,totalTime/cont_linha);

   delete voc;
   return 0;
}

