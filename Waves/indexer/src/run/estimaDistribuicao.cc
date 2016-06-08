#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <list>
#include "../../../libs-iw/indexer/include/IndiceInvertido.h"
#define MAX_SIM 1000000
#define DIST 1000


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
	
	
	
	int nl=0,nw=0;
	tPost doc_a_doc[1024];
	int nDocsBloco;
	tPost currPost;
	FILE *ftermo_out=NULL;
	unsigned int doc_gap;
	_tipo_buffer *out_freq;
	unsigned char btat_freq;

	_tipo_buffer *buffer_lista_freq = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA];


	if(argc != 3)
	{
		printf("Execute:\n\t%s <path_indice> <1->UTI; 2->Vetorial; 3->BM25>\n", argv[0]);
		exit(1);
	}


	long *freq1 = new long[MAX_SIM];
	memset(freq1, 0, MAX_SIM*sizeof(long));
	int sim_merge;
	
	for (int i = 0; i < 80000; i++)
	{
		freq1[i]=0;
	}
	
	
	
	
	time_t start = time(NULL);
	FILE *ffreq_skip = NULL;
	FILE *fidx = NULL;
	FILE *fmax_score = NULL;
	FILE *fmin_score = NULL;
	FILE *fidf = NULL;
	FILE *fft;
	FILE *fnorma = NULL;
	float pre;
	float nTerms = 0;
	float norma = 0;
	int totalNDocs;
	int tipo_score= atoi(argv[2]);
	IDX idx;
	int term_id = 0;

	_tipo_buffer *buffer_skip_freq_docid;
	float *buffer_skip_freq_max_score;
	long *buffer_skip_freq_offset, offset=0;
	unsigned char *buffer_skip_freq_bitat;

	float maxScore, maxScoreBlock, score, maxTodos = 0;
	float minScore, minScoreBlock, minTodos = 0;

	int id_arq_ant = -1;
	char arq[4096];
	int salto, num_doc;
	_tipo_buffer docid=0;
	_tipo_buffer ultdocid=0;

	int k;
	LeitorLista leitor;
	unsigned int doc, freq=0, bitoffset=0;


	buffer_skip_freq_docid = (_tipo_buffer*)malloc(sizeof(_tipo_buffer)*SIZE_MAX_SKIP_LIST);
	buffer_skip_freq_offset = (long*)malloc(sizeof(long)*SIZE_MAX_SKIP_LIST);
	buffer_skip_freq_bitat = (unsigned char*)malloc(sizeof(unsigned char)*SIZE_MAX_SKIP_LIST);
	buffer_skip_freq_max_score = (float*)malloc(sizeof(float)*SIZE_MAX_SKIP_LIST);
	cout << "aqui 1" << endl;
	sprintf(arq, "%s%s", argv[1],NOME_IDX);
	if((fidx = fopen(arq, "r+")) == NULL)
	{
		printf("Nao foi possivel abrir o arquivo de idx %s\n", arq);
		exit(1);
	}

	size_t size;
	float *vetIdf;
	float *vetNorm;
	unsigned *vetFT;
	if(true && tipo_score != 1) {
		sprintf(arq, "%s%s", argv[1], NOME_IDF);
		if((fidf = fopen(arq, "r+")) == NULL)
		{
			printf("Nao foi possivel abrir o arquivo de idf %s\n", arq);
			exit(1);
		}
		fseek(fidf, 0, SEEK_END);
		size = ftell(fidf)/sizeof(float);
		vetIdf = new float[size];
		fseek(fidf, 0, SEEK_SET);
		fread(vetIdf, size, sizeof(float), fidf);
		fclose(fidf);

		sprintf(arq, "%sFT", argv[1]);
		if((fft = fopen(arq, "r+")) == NULL)
		{
			printf("Nao foi possivel abrir o arquivo de FT %s\n", arq);
			exit(1);
		}
		   
		fseek(fft, 0, SEEK_END);
		size = ftell(fft)/sizeof(unsigned);
		printf("tamanho do FT: %d\n",size);
		vetFT = new unsigned[size];
		fseek(fft, 0, SEEK_SET);
		fread(vetFT, sizeof(unsigned),size, fft);
		fclose(fft);

		
		sprintf(arq, "%s%s", argv[1], NOME_NORMA);
		if((fnorma = fopen(arq, "r+")) == NULL)
		{
			printf("Nao foi possivel abrir o arquivo de norma %s\n", arq);
			exit(1);
		}
		
		fseek(fnorma, 0, SEEK_END);
		size = ftell(fnorma)/sizeof(float);
		vetNorm = new float[size];
		totalNDocs = size;
		fseek(fnorma, 0, SEEK_SET);
		fread(vetNorm, size, sizeof(float), fnorma);
		fclose(fnorma);
	 }
	


	float contDoc =0.0f;
	
	do{
		doc_gap= 0; 
		buffer_lista_freq[0] = 0;
		out_freq = buffer_lista_freq;
		btat_freq = 0;
		maxScore = minScore = 0;
		nDocsBloco = 0;

		fread( &idx, sizeof(IDX), 1, fidx );
		if(feof(fidx))	break;


		if(idx.freq_tam_lista == 0)
		{
			term_id++;

			//fwrite(&maxScore, 1, sizeof(float), fmax_score);
			//fwrite(&minScore, 1, sizeof(float), fmin_score);

			continue;
		}
		nTerms++;
		//term_id++;
		//continue;
		
		/*Abre o arquivo de listas de frequencia*/
		sprintf( arq, "%s%s%d", argv[1], NOME_FREQUENCIA, idx.id_arq );
		//cout<<"---> "<<arq<<endl;
		leitor.Inicializa(idx.freq_tam_lista, arq, idx.freq_inicio_termo);

		pre = (float)3 * (float)(log( 1 + (float)(totalNDocs - vetFT[term_id] + 0.5)/(float)(vetFT[term_id] + 0.5))/log(2));

		
		salto = 64;
		num_doc = 0;
		k = 0;
		offset = 0;
		nDocsBloco = 0;
		if(leitor.getDoc(&docid, &freq, &offset, &bitoffset)) {
			
			doc_a_doc[nDocsBloco].doc = docid;
			doc_a_doc[nDocsBloco].freq = freq;
			nl++;
			nDocsBloco++;


			num_doc++;
		}
		else {
			printf("erro de leitura, primeiro decodificado vazio\n");
			exit(1);
		}
		
		if(tipo_score == 1)
		{
			/*UTI*/
			score = freq;
		}
		else if(tipo_score == 2)
		{
			/*Vetorial*/
			score = freq * vetIdf[term_id] / vetNorm[docid];
		}else if(tipo_score == 3)
		{
		
			/*BM25 com k1=2; b=0.75*/
			//	score = (freq * vetIdf[term_id] * 3.0) / (freq + 2.0 * ( 0.25 + 0.75 * vetNorm[docid]) );
			score = pre*(float)(freq) / ((float)(freq + 2 * ( 0.25 + 0.75 * (float)(vetNorm[docid])/(float)(130))));
			
			sim_merge = (int)(score*DIST);
			if(sim_merge > MAX_SIM-1)	sim_merge = MAX_SIM-1;
			freq1[sim_merge]++;
			
			contDoc ++;
		}


		while(leitor.getDoc(&doc, &freq, &offset, &bitoffset))
		{


			if(doc != 0)
			{
				nl++;
				ultdocid = docid;
				docid += doc;
				doc_a_doc[nDocsBloco].doc = doc;
				doc_a_doc[nDocsBloco].freq = freq;

				nDocsBloco++;
		
				if(tipo_score == 1)
				{
					/*UTI*/
					score = freq;
				}
				else if(tipo_score == 2)
				{
					/*Vetorial*/
					norma = vetNorm[docid]!=0.00000?vetNorm[docid]:1;
					score = freq * vetIdf[term_id] / norma;
					
					cout<< "score = "<< freq <<" * "<< vetIdf[term_id]<<"/"<<norma<< endl;
					
				}else if(tipo_score == 3)
				{
					/*BM25 com k1=2; b=0.75*/
					//score = (freq * vetIdf[term_id] * 3.0) / (freq + 2.0 * ( 0.25 + 0.75 * vetNorm[docid]) );
					score = pre*(float)(freq) / ((float)(freq + 2 * ( 0.25 + 0.75 * (float)(vetNorm[docid])/(float)(130))));
				}

				//cout<<"Score : "<<score<<endl;
				sim_merge = (int)(score*DIST);
				if(sim_merge > MAX_SIM-1)	sim_merge = MAX_SIM-1;
				freq1[sim_merge]++;
				
				contDoc++;
				
			}
		}
		term_id++;

	}while(term_id);

	fclose(fidx);

	free(buffer_skip_freq_docid);
	free(buffer_skip_freq_offset);
	free(buffer_skip_freq_bitat);
	free(buffer_skip_freq_max_score);
//cout <<"# Terms : " <<nTerms<<endl;
printf("#Termos %.1f\n", nTerms);

	if(true){
		printf("\n**********************Distribuicoes**********************\n\n");
		float total=0, parcial=0;
		
		for(int i=MAX_SIM;i>=0; i--)
		{
			total=total+freq1[i];
			//printf("Total de documentos %d\t %d\n", total, freq1[i]);
		}
		printf("---------->  Total de documentos %.1f\t %.1f\n", total, contDoc);
		float p;
		for(int i=MAX_SIM;i>=0; i--)
		{
			parcial= parcial+freq1[i];
			if(parcial>0){
				p=((float)parcial/(float)total)*100;
				printf("id : %d \t Freq : %d \t score : %f \t  [%.6f\%] \t Parcial %.1f \t total : %.1f\n", (i),(freq1[i]), (((float)i)/DIST), p, parcial, total);

			}
		}
	
		printf("Total de documentos %.1f\t", total);	
		
	}
	
	return 0;
}
