#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <list>

#include "../../../libs-iw/indexer/include/IndiceInvertido.h"
#define MAX_SIM 1000000
#define DIST 100000

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


using namespace std;

typedef struct lista_invertida_doc { //Struct da Lista Invertida ordenada por Documento
	unsigned int doc;
	unsigned int freq;
	float score;
} lista_doc;

bool compareScore(const lista_doc& first, const lista_doc& second) { //Funcao usada no algoritmo Sort (para ordenar por frequencia a Lista Invertida)
	if (first.score == second.score)	return first.doc < second.doc;
	return first.score > second.score;
}

bool compareDocid(const lista_doc& first, const lista_doc& second) { //Funcao usada no algoritmo Sort (para ordenar por frequencia a Lista Invertida)
	return first.doc < second.doc;
}


int main(int argc, char **argv)
{

	float pre; // precalculo da similaridade do BM25
	unsigned int doc_gap;
	long *freq1 = new long[MAX_SIM];
	memset(freq1, 0, MAX_SIM*sizeof(long));
	int sim_merge=0;
	int t_=0;

	if(argc != 6)
	{
		printf("Execute:\n\t%s <path_indice> <firt_tier> <second_tier> <porcentagem> <1->UTI; 2->Vetorial; 3->BM25>\n", argv[0]);
		exit(1);
	}

	int tipo_score= atoi(argv[5]);
	float perc = atof(argv[4]);
	float minScore, maxScore;
	float Dmaior, Mmaior;
	float nMaior;
	time_t start = time(NULL);
	FILE *fidx = NULL;
	FILE *ftermo1 = NULL;
	FILE *ftermo2 = NULL;
	FILE *fidx_fracao1 = NULL;
	FILE *fidx_fracao2 = NULL;
	FILE *fmin_score = NULL;
	FILE *fDecimoMaior = NULL;
	FILE *fMilMaior = NULL;
	FILE *fidx_fracao1LastDoc = NULL;
	FILE *fidx_fracao2LastDoc = NULL;
	//	FILE *fidxLastDoc = NULL;
	IDX idx;
	IDX idx_fracao1;
	IDX idx_fracao2;
	//IDX idx_out;

	int term_id = 0;

	int id_arq_ant = -1;
	char arq[4096];
	LeitorLista leitor;
	_tipo_buffer docid=0;
	_tipo_buffer ultdocid=0;
	long offset=0;
	unsigned  bitoffset=0;
	
	if(tipo_score == 1) {
		printf("Gerando indice de impacto de UTI\n");
	}
	else if(tipo_score == 2) {
		printf("Gerando indice de impacto de VETORIAL\n");
	}else if(tipo_score == 3) {
		printf("Gerando indice de impacto de BM25\n");
	}

	_tipo_buffer *buffer_lista_freq = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA];
	_tipo_buffer *out_freq;
	unsigned char btat_freq;

	// Criando arquivo auxiliar para lastdocs


	sprintf(arq, "%s%s", argv[1], NOME_IDX);
	if((fidx = fopen(arq, "r")) == NULL)
	{
		printf("Nao foi possivel abrir o arquivo de idx %s\n", arq);
		exit(1);
	}
	
	sprintf(arq, "%s%s", argv[2], NOME_IDX);
	if((fidx_fracao1 = fopen(arq, "w+")) == NULL)
	{
		printf("Nao foi possivel criar o arquivo de idx do termo a termo %s\n", arq);
		exit(1);
	}
	sprintf(arq, "%s%s", argv[3], NOME_IDX);
	if((fidx_fracao2 = fopen(arq, "w+")) == NULL)
	{
		printf("Nao foi possivel criar o arquivo de idx do termo a termo %s\n", arq);
		exit(1);
	}

	sprintf(arq, "%s%s.lastdoc", argv[2], NOME_IDX);
	if((fidx_fracao1LastDoc = fopen(arq, "w+")) == NULL)
	{
		printf("Nao foi possivel criar o arquivo de idx do termo a termo lastdoc%s\n", arq);
		exit(1);
	}
	sprintf(arq, "%s%s.lastdoc", argv[3], NOME_IDX);
	if((fidx_fracao2LastDoc = fopen(arq, "w+")) == NULL)
	{
		printf("Nao foi possivel criar o arquivo de idx do termo a termo lastDoc, segunda camada %s\n", arq);
		exit(1);
	}

	sprintf(arq, "%smin_score", argv[2]);
	fmin_score = fopen(arq, "w");
	
	/**/
	sprintf(arq, "%s10Max_score", argv[1]);
	fDecimoMaior = fopen(arq, "w");
	
	sprintf(arq, "%s1000Max_score", argv[1]);
	fMilMaior = fopen(arq, "w");
	/**/
	size_t size;
	float *vetIdf;
	unsigned *ft;
	float *vetNorm;
	unsigned *vetFT;

	unsigned int totalNDocs;
	FILE *fft;
	//printf("tipo_score == %d - %s%s\n",tipo_score ,argv[1], NOME_IDF);
	if(tipo_score != 1) {
		//printf("tipo_score diferente de 1 %s%s\n",argv[1], NOME_IDF);
		sprintf(arq, "%s%s", argv[1], NOME_IDF);
		FILE *fidf, *fnorma;

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
			printf("Nao foi possivel abrir o arquivo de idf %s\n", arq);
			exit(1);
		}
		   
		fseek(fft, 0, SEEK_END);
		size = ftell(fft)/sizeof(unsigned);
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

	
	//int listId = 0;

	int cur_term =0;

	do{

		/**
		if (term[t] == -1){
			cout<<"novaconsulta "<<endl;
			t++;
		}else if(term[t] == -9){
			break;
		}
		/**/

		fread( &idx, sizeof(IDX), 1, fidx );
		if(feof(fidx))	break;


		idx_fracao1.id_arq = idx.id_arq;
		idx_fracao1.freq_inicio_termo = 0;
		idx_fracao1.freq_tam_lista = 0;

		idx_fracao2.id_arq = idx.id_arq;
		idx_fracao2.freq_inicio_termo = 0;
		idx_fracao2.freq_tam_lista = 0;


		if(idx.freq_tam_lista == 0  /**|| term_id != termos_[listId]/**/)
		{
			
			fwrite(&idx_fracao1, sizeof(IDX), 1, fidx_fracao1);
			fwrite(&idx_fracao2, sizeof(IDX), 1, fidx_fracao2);
			
			Mmaior = 0;
			Dmaior = 0;
			fwrite(&Dmaior, sizeof(float), 1, fDecimoMaior);
			fwrite(&Mmaior, sizeof(float), 1, fMilMaior);

			int temp = -1;
			fwrite(&temp, sizeof(int),1,fidx_fracao1LastDoc);
			fwrite(&temp, sizeof(int),1,fidx_fracao2LastDoc);

			term_id++;
			continue;
		}
		
		//listId++;


		t_++;
		//cout<<"term_id : "<< term_id;
		//cout<<"  termos_[listId] : "<< termos_[listId] <<endl;
		
		/**
		t=5;
		
		/**/
		///==============================================
		//if(term_id == 10 ) break;///Parada para teste
		///==============================================


		//cout<<"aqui-"<<endl;
		/*Abre o arquivo de listas de frequencia*/
		sprintf( arq, "%s%s%d", argv[1], NOME_FREQUENCIA, idx.id_arq );
		leitor.Inicializa(idx.freq_tam_lista, arq, idx.freq_inicio_termo);
		//cout<<"aqui"<<endl;
		//leitor.Inicializa(arq, idx.freq_tam_lista, idx.freq_inicio_termo);
		if(id_arq_ant != idx.id_arq){
			
			if(ftermo1)	fclose(ftermo1);
			/*Cria o arquivo de skiplist para a primeira camada*/
			sprintf( arq, "%s%s%d", argv[2], NOME_FREQUENCIA, idx.id_arq );
			ftermo1 = fopen( arq, "w+");

			if(ftermo2)     fclose(ftermo2);
			/*Cria o arquivo de skiplist para a segunda camada*/
			sprintf( arq, "%s%s%d", argv[3], NOME_FREQUENCIA, idx.id_arq );
			ftermo2 = fopen( arq, "w+");
			id_arq_ant = idx.id_arq;
		}



		pre = (float)3 * (float)(log( 1 + (float)(totalNDocs - vetFT[term_id] + 0.5)/(float)(vetFT[term_id] + 0.5))/log(2));

		vector <lista_doc> doc_a_doc;
		vector <lista_doc> doc_a_doc_second;
		lista_doc ld_aux;


		if(leitor.getDoc(&docid, &ld_aux.freq, &offset, &bitoffset)) {

			if(tipo_score == 1){
				/*UTI*/
				ld_aux.score = ld_aux.freq;
			}else if(tipo_score == 2){
				/*Vetorial*/
				ld_aux.score = ld_aux.freq * vetIdf[term_id] / vetNorm[docid];
			}else if(tipo_score == 3) {

				ld_aux.score = pre*(float)(ld_aux.freq) / ((float)(ld_aux.freq + 2 * ( 0.25 + 0.75 * (float)(vetNorm[docid])/(float)(130))));
				ld_aux.doc = docid;
				doc_a_doc.push_back(ld_aux);
			}

			maxScore = ld_aux.score;
			minScore = ld_aux.score;

		}else {
			printf("erro de leitura, primeiro decodificado vazio\n");
			exit(1);
		}


		while(leitor.getDoc(&ld_aux.doc, &ld_aux.freq, &offset, &bitoffset))
		//while(leitor.getDocSemSomaUm(&(ld_aux.doc), &(ld_aux.freq)))
		{
			
			
			if(ld_aux.doc != 0)
			{
				ultdocid = docid;
				docid += ld_aux.doc;

				if(tipo_score == 1)
				{
					/*UTI*/
					ld_aux.score = ld_aux.freq;
				}
				else if(tipo_score == 2)
				{
					/*Vetorial*/
					ld_aux.score = ld_aux.freq * vetIdf[term_id] / vetNorm[docid];
				}else if(tipo_score == 3)
				{
					/*BM25 com k1=2; b=0.75*/
					//score = (freq * vetIdf[term_id] * 3.0) / (freq + 2.0 * ( 0.25 + 0.75 * vetNorm[docid]) );
					ld_aux.score = pre*(float)(ld_aux.freq) / ((float)(ld_aux.freq + 2 * ( 0.25 + 0.75 * (float)(vetNorm[docid])/(float)(130))));
				}
				ld_aux.doc = docid;
				doc_a_doc.push_back(ld_aux);

				//printf("%d %.5f\n", docid , ld_aux.score);
			}
		}

		/* Ordena por score*/    
		sort(doc_a_doc.begin(), doc_a_doc.end(),compareScore); 
		//continue ;
		
		
		//Seleciona baseado em um limiar e docID
		idx_fracao1.freq_tam_lista = idx.freq_tam_lista;
		for(int i=1000; i<doc_a_doc.size(); i++) {

			if(doc_a_doc[i].score < perc) {
				idx_fracao1.freq_tam_lista = i;
				break;
			}
		}
		


		/**Monta o indice da segundacamada a partir do limiar encontrado*/
		for(int i=idx_fracao1.freq_tam_lista; i<doc_a_doc.size(); i++) {
			doc_a_doc_second.push_back(doc_a_doc[i]);
		}

		///Guarda o maior score da segunda camada para ser escrito no arq.  minScre da primeira
		if(doc_a_doc_second.size()>0){
			minScore = doc_a_doc[idx_fracao1.freq_tam_lista].score;
		}else{
			minScore = 0;
		}

		/**Remove da primeira camada os elementos que estram na segunda camada*/
		doc_a_doc.resize(idx_fracao1.freq_tam_lista);



		/**Ordena os indices das duas camadas por Documento*/
		sort(doc_a_doc.begin(), doc_a_doc.end(),compareDocid); //Ordenando a lista por docid
		sort(doc_a_doc_second.begin(), doc_a_doc_second.end(),compareDocid); //Ordenando a lista por docid

		/**Quarda o tamanho da indice da segunda camada*/
		idx_fracao2.freq_tam_lista = doc_a_doc_second.size();

		/** ? Escreve no arquivo correspondente o IDX*/
		idx_fracao1.freq_inicio_termo = ftell(ftermo1);//ftell() - retorna a posição corrente de leitura ou escrita no arquivo (em bytes)
		fwrite(&idx_fracao1, sizeof(IDX), 1, fidx_fracao1);

		idx_fracao2.freq_inicio_termo = ftell(ftermo2);
		fwrite(&idx_fracao2, sizeof(IDX), 1, fidx_fracao2);

		///Escreve minScore (maior score da segnda camda)
		fwrite(&minScore, 1, sizeof(float), fmin_score);
		//-----------

		doc_gap=0;

		buffer_lista_freq[0] = 0;
		out_freq = buffer_lista_freq;
		btat_freq = 0;

		for(int i=0; i<doc_a_doc.size(); i++)
		{
			RawDeltaEncode((doc_a_doc[i].doc) - doc_gap, out_freq, btat_freq);
			RawDeltaEncode(doc_a_doc[i].freq, out_freq, btat_freq);

			doc_gap = (doc_a_doc[i].doc);

			if(out_freq >= &buffer_lista_freq[SIZE_BUFFER_LEITOR_LISTA])
			{
				fwrite(buffer_lista_freq, sizeof(_tipo_buffer), out_freq-buffer_lista_freq, ftermo1);
				buffer_lista_freq[0] = *out_freq;
				out_freq = buffer_lista_freq;
			}
		}

		// escreve lastdoc da primeira camada
		//printf("ultimo doc da primeira camada %d\n",doc_a_doc[doc_a_doc.size()-1].doc);
		{  
			int tmp = doc_a_doc[doc_a_doc.size()-1].doc;
			fwrite(&tmp,sizeof(int),1,fidx_fracao1LastDoc);
		}
		if(out_freq != buffer_lista_freq)
			fwrite(buffer_lista_freq, sizeof(_tipo_buffer), out_freq-buffer_lista_freq, ftermo1);
		if(btat_freq)
			fwrite(out_freq, sizeof(_tipo_buffer), 1, ftermo1);

		//-----------
		buffer_lista_freq[0] = 0;
		out_freq 	= buffer_lista_freq;
		btat_freq 	= 0;

		doc_gap = 0;

		for(int i=0; i<doc_a_doc_second.size(); i++)
		{
			RawDeltaEncode((doc_a_doc_second[i].doc) - doc_gap, out_freq, btat_freq);
			RawDeltaEncode(doc_a_doc_second[i].freq, out_freq, btat_freq);

			doc_gap = (doc_a_doc_second[i].doc);

			if(out_freq >= &buffer_lista_freq[SIZE_BUFFER_LEITOR_LISTA])
			{
				fwrite(buffer_lista_freq, sizeof(_tipo_buffer), out_freq-buffer_lista_freq, ftermo2);
				buffer_lista_freq[0] = *out_freq;
				out_freq = buffer_lista_freq;
			}
		}
		if(doc_a_doc_second.size()) {
			// escreve lastdoc da segunda camada
			// printf("ultimo doc da segunda camada %d\n",doc_a_doc_second[doc_a_doc_second.size()-1].doc); 
			{
				int tmp =doc_a_doc_second[doc_a_doc_second.size()-1].doc;
				fwrite(&tmp,sizeof(int),1,fidx_fracao2LastDoc);
			}
		}
		else {
			int temp=-1;
			//    printf("ultimo doc da segunda camada %d\n",temp);
			fwrite(&temp, sizeof(int),1,fidx_fracao2LastDoc);
		}


		if(out_freq != buffer_lista_freq)
			fwrite(buffer_lista_freq, sizeof(_tipo_buffer), out_freq-buffer_lista_freq, ftermo2);
		if(btat_freq)
			fwrite(out_freq, sizeof(_tipo_buffer), 1, ftermo2);
		//---------
		term_id++;
	
	}while(term_id);


	printf("%d Termos\n", term_id);
	fclose(fidx);
/**/
	///copia arquivo de norma para os novos indices
	sprintf( arq, "cp %snorma %snorma", argv[1], argv[2] );
	system(arq); 
	sprintf( arq, "cp %snorma %snorma", argv[1], argv[3] );
	system(arq);
	///copia arquivo de IDF para os novos indices
	sprintf( arq, "cp %sidf %sidf", argv[1], argv[2] );
	system(arq);
	sprintf( arq, "cp %sidf %sidf", argv[1], argv[3] );
	system(arq);
	
	///copia arquivo de FT para os novos indices
	sprintf( arq, "cp %sFT %sFT", argv[1], argv[2] );
	system(arq);
	sprintf( arq, "cp %sFT %sFT", argv[1], argv[3] );
	system(arq);
/**/



	//fclose(fidx);
	
	fclose(fMilMaior);
	fclose(fDecimoMaior);
	
	fclose(fidx_fracao1);
	fclose(fidx_fracao2);
	
	fclose(fidx_fracao1LastDoc);
	fclose(fidx_fracao2LastDoc);

	fclose(ftermo1);
	fclose(ftermo2);

	
	printf("Indice termo a termo gerado em %.2lf\n", difftime(time(NULL), start)/60);
	printf("# de termos : %d\n", t_);
	

	return 0;
}
