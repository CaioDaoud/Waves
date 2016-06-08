#ifndef LEITOR_LISTA_H
#define LEITOR_LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IndiceInvertido.h"
#include "prefixinmem.h"

class LeitorLista
{
	public:
	FILE *fp;
	FILE *fpSkip;
	_tipo_buffer *buffer;
	_tipo_buffer *out;
	unsigned char btat;
	char fileatual[1000];
	char fileatualskip[1000];
	unsigned int doc_gap;
	unsigned int *buffer_skip_docid;
	long *buffer_skip_offset;
	unsigned char *buffer_skip_bitat;
	float *buffer_skip_max_score;
	int pos_skip;
	int tam_skip;
	long tam_lista;
	int pos_lista;
	int salto_skip;
	long pos_file;

	int total_descomprimido;
	int total_bloco_lido;

	int tf_max;

	int *dbuffer;
	int *fbuffer;
	int atual_buffer;
	int total_buffer;

	LeitorLista()
	{	
		fp = NULL;
		fpSkip = NULL;
		fileatual[0] = 0;
		fileatualskip[0] = 0;
		buffer = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA];
		buffer_skip_docid  = new unsigned int[SIZE_MAX_SKIP_LIST];
		buffer_skip_offset = new long[SIZE_MAX_SKIP_LIST];
		buffer_skip_bitat  = new unsigned char[SIZE_MAX_SKIP_LIST];
		buffer_skip_max_score = new float[SIZE_MAX_SKIP_LIST];

		dbuffer = new int[SIZE_BUFFER_LEITOR_LISTA * 10];
		fbuffer = new int[SIZE_BUFFER_LEITOR_LISTA * 10];

		total_descomprimido = 0;
		total_bloco_lido = 0;
		tf_max = 0;
	}

	~LeitorLista()
	{
		delete[] buffer_skip_docid;
		delete[] buffer_skip_offset;
		delete[] buffer_skip_bitat;
		delete[] buffer_skip_max_score;
		delete[] buffer;
		if(fp)	fclose(fp);
		if(fpSkip)	fclose(fpSkip);
		
		//dcout << "TotalDescomprimido " << total_descomprimido << endl;
		//dcout << "TotalLido " << total_bloco_lido << endl;
	}

	void InicializaSkip(char *arq_lista, long _tam_lista, long inicio_lista, char *arq_skip, long _tam_skip, long _inicio_skip)
	{
		Inicializa(arq_lista, _tam_lista, inicio_lista);

		/*Checa se mudou o arquivo de origem da skiplist*/
		if(strcmp(fileatualskip, arq_skip))
		{
			if(fpSkip)	fclose(fpSkip);
			strcpy(fileatualskip, arq_skip);
			fpSkip = fopen( arq_skip, "rb");
			if(fpSkip == NULL)
			{
				printf("Problemas na abertura do arquivo de skiplist (%s)\n", arq_skip);
				exit(0);
			}
		}

		if(_tam_skip > SIZE_MAX_SKIP_LIST)
		{
			printf("Tamanho da skiplist excedido: %d\n", _tam_skip);
			exit(1);
		}

		/*Le o vetor de docid, offset e bitat*/
		fseek( fpSkip, _inicio_skip, SEEK_SET);
		fread(buffer_skip_docid, sizeof(_tipo_buffer), _tam_skip, fpSkip);
		fread(buffer_skip_offset, sizeof(long), _tam_skip, fpSkip);
		fread(buffer_skip_bitat, sizeof(unsigned char), _tam_skip, fpSkip);
		fread(buffer_skip_max_score, sizeof(float), _tam_skip, fpSkip);

		pos_skip = 0;
		tam_skip = _tam_skip;
		salto_skip = SALTO_SKIPLIST;
	}

	void Inicializa(char *arq, long _tam_lista, long inicio)
	{
		/*Chega se mudou o arquivo de origem da lista invertida atual*/
		if(strcmp(fileatual, arq))
		{
			if(fp)	fclose(fp);
			strcpy(fileatual, arq);
			fp = fopen( arq, "rb");
			if(fp == NULL)
			{
				printf("Problemas na abertura do arquivo de frequencia (%s)\n", arq);
				exit(0);
			}
		}

		fseek( fp, inicio, SEEK_SET);
		fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA, fp);

		total_bloco_lido ++;

		out = buffer;
		btat = 0;
		tam_lista = _tam_lista;
		doc_gap = 0;
		pos_lista = 0;
		pos_file = inicio;

		decode_buffer();
	}

	void decode_buffer()
	{
		atual_buffer = 0;
		total_buffer = 0;
		while(out < &buffer[SIZE_BUFFER_LEITOR_LISTA] && pos_lista+total_buffer <= tam_lista)
		{			
			doc_gap +=  RawDeltaDecode(out, btat) - 1;
			dbuffer[total_buffer] = doc_gap;
			fbuffer[total_buffer] =  RawDeltaDecode(out, btat);
			total_buffer ++;
			
			if(total_buffer > SIZE_BUFFER_LEITOR_LISTA*10){
				cout << "Zica!!!"<< endl;
				exit(1);
			}
		}
	}

	/*
		Carrega um pedaco da lista invertida para um buffer de descompressao
	*/
	void load_buffer()
	{
		pos_file += SIZE_BUFFER_LEITOR_LISTA*sizeof(_tipo_buffer);
		memcpy(buffer, &buffer[SIZE_BUFFER_LEITOR_LISTA], SIZE_FOLGA_LEITOR_LISTA*sizeof(_tipo_buffer));
		fread(buffer+SIZE_FOLGA_LEITOR_LISTA, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA, fp);
		out -= SIZE_BUFFER_LEITOR_LISTA;

		decode_buffer();

		total_bloco_lido ++;
	}

	/*
		Le uma lista invertida
	*/
	int getDoc(unsigned int *doc, unsigned int *info)
	{
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		if(atual_buffer == total_buffer)
		{
			load_buffer();
		}

		pos_lista ++;

		/*Checa o final do buffer. Deve-se ler mais*/
/*		if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
		{
			load_buffer();
		}

		doc_gap +=  RawDeltaDecode(out, btat) - 1;
		*info =  RawDeltaDecode(out, btat);

		*doc = doc_gap;
*/

		*doc = dbuffer[atual_buffer];
		*info = fbuffer[atual_buffer];
		atual_buffer ++;

		return 1;
	}

	/*
		Le a lista invertida. Buscando o primeiro docid >= doc_skip
	*/
	int getDoc(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{
		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(pos_lista < tam_lista)
		{
			pos_lista ++;

			/*Checa o final do buffer. Deve-se ler mais*/
/*			if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
			{
				load_buffer();
			}

			doc_gap += RawDeltaDecode(out, btat) -1;
			*info = RawDeltaDecode(out, btat);

			*doc = doc_gap;
*/
			if(atual_buffer == total_buffer)
			{
				load_buffer();
			}

			*doc = dbuffer[atual_buffer];
			*info = fbuffer[atual_buffer];
			atual_buffer ++;

			total_descomprimido++;

			if(*doc >= doc_skip)	return 1;
		}
		
		return 0;
	}

	/*
		Le a lista invertida aplicando saltos com a skiplist. Busca o primeiro docid >= doc_skip
	*/
	inline int getSkipDoc(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{	
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		/*Calcula entre qual posicao da skiplist a lista invertida esta*/
		(pos_lista > 0) ? (pos_skip = (pos_lista-1)/salto_skip) : (pos_skip = 0);

		/*Checa se o doc_skip eh menor que o proximo salto da skip*/
		if(tam_skip == 0 || doc_skip < buffer_skip_docid[pos_skip])	return getDoc(doc, info, doc_skip);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
		pos_skip --;

		/*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
		if(pos_skip < 0 || doc_gap >= buffer_skip_docid[pos_skip])	return getDoc(doc, info, doc_skip);

		/*Verifica se o bloco a ser descomprimido esta no disco*/
		if((pos_file+sizeof(int)*SIZE_BUFFER_LEITOR_LISTA) < buffer_skip_offset[pos_skip])
		{
			fseek(fp, buffer_skip_offset[pos_skip], SEEK_SET);
			pos_file = ftello(fp);
			fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA, fp);
			out = buffer;

			total_bloco_lido ++;
		}
		else {
			/*Salta no proprio bloco que esta em memoria*/
			out = &buffer[(buffer_skip_offset[pos_skip]-pos_file)/4];
		}
		btat = buffer_skip_bitat[pos_skip];

		/*Calcula qual a nova posicao na lista*/
		pos_lista = (pos_skip+1) * (salto_skip) + 1;

		/*Decodifica o codigo do docId da skiplist*/
		RawDeltaDecode(out, btat) - 1;
		/*Decodifica o TF*/
		*info = RawDeltaDecode(out, btat);

		/*Redefine o doc_gap com o docId da skip*/
		doc_gap = buffer_skip_docid[pos_skip];

		decode_buffer();

		*doc = doc_gap;

		if(*doc >= doc_skip)	return 1;

		/*Procura pelo primeiro docId >= doc_skip*/
		return getDoc(doc, info, doc_skip);
	}

	/*
		Move o ponteiro da skiplist para o bloco com o docid>=doc_skip sem descomprimir nada
	*/
	inline int nextShallow(unsigned int doc_skip)
	{	
		/*Calcula entre qual posicao da skiplist a lista invertida esta*/
		(pos_lista > 0) ? (pos_skip = (pos_lista-1)/salto_skip) : (pos_skip = 0);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;

		return 1;
	}

	/*
		Pega o MaxDocId do bloco
	*/
	unsigned int getDocBlockBoundary()
	{
		if(tam_skip == 0)	return doc_gap+1;

		if(pos_skip >= tam_skip && buffer_skip_docid[tam_skip-1] < doc_gap)	return doc_gap+1;

		if(pos_skip >= tam_skip)		return buffer_skip_docid[tam_skip-1];

		return buffer_skip_docid[pos_skip];		
	}

	/*
		Pega o MaxScore do bloco
	*/
	float getBlockMaxScore()
	{
		if(tam_skip == 0 || pos_skip >= tam_skip)	return 0;

		return buffer_skip_max_score[pos_skip];
	}

	/*
		Utilizado para ler a lista invertida em forma de vetores de features
	*/
	int getDoc(unsigned int *doc, unsigned int *tfUrl, unsigned int *tfTitle, unsigned int *tfAnchor, unsigned int *tfText)
	{
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		pos_lista ++;

		/*Checa o final do buffer. Deve-se ler mais*/
		if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
		{
			load_buffer();
		}

		/*Descomprime o docId e calcula o doc_gap*/
		doc_gap +=  RawDeltaDecode(out, btat) - 1;
		/*Descomprime o TF*/
		*tfUrl = RawDeltaDecode(out, btat) - 1;
		*tfTitle = RawDeltaDecode(out, btat) - 1;
		*tfAnchor = RawDeltaDecode(out, btat) - 1;
		*tfText = RawDeltaDecode(out, btat) - 1;

		*doc = doc_gap;

		return 1;
	}
	
	/*
		Utilizado para ler a lista invertida em forma de vetores de features. Busca o primeido docid >= doc_skip
	*/
	int getDoc(unsigned int *doc, unsigned int *tfUrl, unsigned int *tfTitle, unsigned int *tfAnchor, unsigned int *tfText, unsigned int doc_skip)
	{
		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(pos_lista < tam_lista)
		{
			pos_lista ++;

			/*Checa o final do buffer. Deve-se ler mais*/
			if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
			{
				load_buffer();
			}

			doc_gap += RawDeltaDecode(out, btat) - 1;

			*tfUrl = RawDeltaDecode(out, btat) - 1;
			*tfTitle = RawDeltaDecode(out, btat) - 1;
			*tfAnchor = RawDeltaDecode(out, btat) - 1;
			*tfText = RawDeltaDecode(out, btat) - 1;

			*doc = doc_gap;

			if(*doc >= doc_skip)	return 1;
		}

		return 0;
	}

	/*
		Utilizado para ler a lista invertida em forma de vetores de features. Aplicando salto nas skiplists
	*/
	int getSkipDoc(unsigned int *doc, unsigned int *tfUrl, unsigned int *tfTitle, unsigned int *tfAnchor, unsigned int *tfText, unsigned int doc_skip)
	{	
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		/*Calcula entre qual posicao da skiplist a lista invertida esta*/
		pos_skip = pos_lista/salto_skip;

		/*Checa se o doc_skip eh menor que o proximo salto da skip*/
		if(doc_skip < buffer_skip_docid[pos_skip])	return getDoc(doc, tfUrl, tfTitle, tfAnchor, tfText, doc_skip);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
		pos_skip --;

		/*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
		if(doc_gap >= buffer_skip_docid[pos_skip])	return getDoc(doc, tfUrl, tfTitle, tfAnchor, tfText, doc_skip);

		/*Verifica se o bloco a ser descomprimido esta no disco*/
		if((pos_file+sizeof(int)*SIZE_BUFFER_LEITOR_LISTA) < buffer_skip_offset[pos_skip])
		{
			fseek(fp, buffer_skip_offset[pos_skip], SEEK_SET);
			pos_file = ftello(fp);
			fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA, fp);
			out = buffer;
		}
		else
		{
			/*Salta no proprio bloco que esta em memoria*/
			out = &buffer[(buffer_skip_offset[pos_skip]-pos_file)/4];
		}
		btat = buffer_skip_bitat[pos_skip];

		/*Calcula qual a nova posicao na lista*/
		pos_lista = (pos_skip+1) * (salto_skip) + 1;

		/*Decodifica o codigo do docId da skiplist*/
		RawDeltaDecode(out, btat);
		/*Decodifica o TF*/
		*tfUrl = RawDeltaDecode(out, btat) - 1;
		*tfTitle = RawDeltaDecode(out, btat) - 1;
		*tfAnchor = RawDeltaDecode(out, btat) - 1;
		*tfText = RawDeltaDecode(out, btat) - 1;

		/*Redefine o doc_gap com o docId da skip*/
		doc_gap = buffer_skip_docid[pos_skip];

		*doc = doc_gap;

		if(*doc >= doc_skip)	return 1;

		/*Procura pelo primeiro docId >= doc_skip*/
		return getDoc(doc, tfUrl, tfTitle, tfAnchor, tfText, doc_skip);
	}
};

#endif
