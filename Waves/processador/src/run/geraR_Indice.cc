#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include "../../../libs-iw/indexer/include/IndiceInvertido.h"

class LeitorLista
{
	public:
		FILE *fp;
_tipo_buffer * buffer;
unsigned char btat;
_tipo_buffer *out;
int tam_lista;
long offset;
char fileatual[1000];

LeitorLista(){	fp = NULL;
buffer = new  _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+5];	
fileatual[0] = 0;}


~LeitorLista() {
	delete buffer;
}
void Inicializa(long _tam_lista, char *arq, long inicio)
{
	if(strcmp(fileatual, arq))
	{
		if(fp)	fclose(fp);
		strcpy(fileatual, arq);
		fp = fopen( arq, "rb");
	}

	fseek( fp, inicio, SEEK_SET);

	if(_tam_lista*2 > SIZE_BUFFER_LEITOR_LISTA+5)
		fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA+5, fp);
	else
		fread(buffer, sizeof(_tipo_buffer), _tam_lista*2, fp);

	out = buffer;
	btat = 0;
	tam_lista = _tam_lista;
	offset = 0;
	//	offset = (inicio/sizeof(unsigned int));
}

int getDoc(unsigned int *doc, unsigned int *info, long *_offset, unsigned int *_bitoffset)
{
	if(tam_lista == 0)	return 0;

	tam_lista --;

	/*Final do buffer. Deve-se ler mais*/
	if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
	{

		memcpy(buffer, &buffer[SIZE_BUFFER_LEITOR_LISTA], 5*sizeof(int));
		fread(buffer+5, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA, fp);
		out -= SIZE_BUFFER_LEITOR_LISTA;
		offset += SIZE_BUFFER_LEITOR_LISTA * sizeof(_tipo_buffer);
	}


	*doc = RawDeltaDecode(out, btat);
	*info = RawDeltaDecode(out, btat);
	*_offset = offset + (out-buffer);
	//	*_offset = offset + (out-buffer) * sizeof(_tipo_buffer);
	//		printf("offset %d\n",*_offset);
	*_bitoffset = btat;
	return 1;
}
};


typedef struct post { //Struct da Lista Invertida ordenada por Documento
	unsigned int doc;
	unsigned int freq;
} tPost;



int main(int argc, char **argv)
{
	long offset;
	//unsigned int num_docs_colecao = (unsigned int) atoi( argv[2] );
	int nl=0,nw=0;
	tPost doc_a_doc[1024];
	int nDocsBloco;
	tPost currPost;
	FILE *ftermo_out=NULL;
	unsigned int doc_gap;
	_tipo_buffer *out_freq;
	unsigned char btat_freq;

	_tipo_buffer *buffer_lista_freq = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA];

	//float *normas = (float*)malloc(sizeof(float)*num_docs_colecao);
	//memset( normas, 0, num_docs_colecao*sizeof(float) );



	if(argc!=2)
	{
		printf("Execute\n\t%s <path_indice> \n", argv[0]);
		exit(1);
	}

	//int tipo_norma = atoi(argv[3]);
	time_t start = time(NULL);


	FILE *fidx = NULL;
	FILE *fidf = NULL;
	FILE *fdocs = NULL;
	FILE *ffreqs = NULL;
	float pre;
	int totalNDocs;
	IDX idx;
	int term_id = 0;

	int id_arq_ant = -1;
	char arq[4096];
	int salto, total, num_doc;
	_tipo_buffer docid=0;
	_tipo_buffer ultdocid=0;

	int k;
	LeitorLista leitor;
	unsigned int doc, freq=0, bitoffset=0;


	sprintf(arq, "%s%s", argv[1], "docs.dat");
	if((fdocs = fopen( arq, "w" )) == NULL )
	{
		printf("gera_IndicesR::Impossivel criar o arquivo de docs.dat: (%s)\n", arq );
		exit(1);
	}
	
	sprintf(arq, "%s%s", argv[1], "freqs.dat");
	if((ffreqs = fopen( arq, "w" )) == NULL )
	{
		printf("gera_IndicesR::Impossivel criar o arquivo de docs.dat: (%s)\n", arq );
		exit(1);
	}


	sprintf(arq, "%s%s", argv[1],NOME_IDX);
	if((fidx = fopen(arq, "r+")) == NULL)
	{
		printf("Nao foi possivel abrir o arquivo de idx %s\n", arq);
		exit(1);
	}
	
	/*
	SimpleWBRQueryParser simple_query_parser;

	string vocabulario ( "./1M/govTxt." );
    Vocabulario *voc = new Vocabulario(10000, 5000, (char*)vocabulario.c_str(), 2);
	
	cout<<"\n\n IMPRIME VOCABULARIO-------\n\n"<<endl;
	printHashPrincipal();
	cout<<"\n\n IMPRIME VOCABULARIO-------\n\n"<<endl;
	*/
	
	size_t size;
	float *vetIdf;

	unsigned *ft;
	FILE *fft;

	
	int nTerms =0;
	vector<int> *documentos = new vector<int>();
	vector<int> *frequencias = new vector<int >();
	int nDocs =0;

	do{

		doc_gap= 0; 
		nDocsBloco = 0;
		fread( &idx, sizeof(IDX), 1, fidx );
		if(feof(fidx))	break;

		if(idx.freq_tam_lista == 0)
		{
			term_id++;
			continue;
		}

		nTerms++;
		/*Abre o arquivo de listas de frequencia*/
		sprintf( arq, "%s%s%d", argv[1], NOME_FREQUENCIA, idx.id_arq );
		leitor.Inicializa(idx.freq_tam_lista, arq, idx.freq_inicio_termo);

		docid =0;
		documentos = new vector<int>();
		while(leitor.getDoc(&doc, &freq, &offset, &bitoffset)){
			
			
			if(doc != 0)
			{
				docid += doc;
				
				documentos->push_back(freq);
				frequencias->push_back(freq);
			}
		}
		
	/***
		cout << documentos->size(); 
		for(vector<int>::iterator it = documentos->begin(); it!= documentos->end(); it++) {
			cout<< " "<< *it;
		}
		delete(documentos);	
		cout<<endl;

		
	/***/	
		
			   
		term_id++;

	}while(1);
	
	cout<<" N- term : "<<term_id<<endl;


	return 0;
}


