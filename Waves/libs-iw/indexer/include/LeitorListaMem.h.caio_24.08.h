#ifndef LEITOR_LISTA_MEM_H
#define LEITOR_LISTA_MEM_H

//#define _CONTADORES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IndiceInvertido.h"
#include "prefixinmem.h"

class LeitorListaMem
{	
	public:
	char *plista; // ponteiro char com posicao de memoria onde a lista invertida comprimida comeca. Investigar se necessario durante a descompressao
    _tipo_buffer *out;   // posicao atual de memoria em um processo de descompressao
    unsigned char btat; // bit atual de um processo de descompressao
    unsigned int doc_gap;  // ID do ultimo doc descomprimido, somado ao proximo gap decodificado obtem-se o proximo doc_id decodificado
    unsigned int *buffer_skip_docid; // vetor com os limites de doc_id da skiplist. Da o docid do inicio da skiplist
    long *buffer_skip_offset; // vetor com os enderecos de memoria onde comeca uma skiplist
    unsigned char *buffer_skip_bitat;  // vetor com o bit onde comeca uma skiplist
    float *buffer_skip_max_score; // vetor com os scores maximos dentro de cada bloco da skiplist
    int pos_skip;  // posicao corrente dentro de um processamento na skiplist da lista invertida em questao
    int tam_skip;  //  tamanho da skiplist em numero de elementos
    long tam_lista; // tamanho da lista em numero de elementos
    int pos_lista;  // posicao em que estamos na lista (quantos descomprimidos)
    int salto_skip;  // 128

#ifdef _CONTADORES
	int total_descomprimido;
	int total_bloco_lido;
#endif
	LeitorListaMem()
	{	
		#ifdef _CONTADORES
		total_descomprimido = 0;
		total_bloco_lido = 0;
		#endif
	}

	~LeitorListaMem()
	{
		#ifdef _CONTADORES
		//cout << "TotalDescomprimido " << total_descomprimido << endl;
		//cout << "TotalLido " << total_bloco_lido << endl;
		#endif
	}

	void InicializaSkip(char *_plista, long _tam_lista, long _inicio_lista, char *pskip, long _tam_skip, long _inicio_skip)
	{
		Inicializa(_plista, _tam_lista, _inicio_lista);

		/*Le o vetor de docid, offset e bitat*/
		buffer_skip_docid = (unsigned int*) (pskip + _inicio_skip);
		buffer_skip_offset = (long*) (pskip + _inicio_skip + sizeof(unsigned int)*_tam_skip);
		buffer_skip_bitat = (unsigned char*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long))*_tam_skip);
		buffer_skip_max_score = (float*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long) + sizeof(unsigned char))*_tam_skip);

		pos_skip = 0;
		tam_skip = _tam_skip;
		salto_skip = SALTO_SKIPLIST;
	}

	void Inicializa(char *_plista, long _tam_lista, long _inicio_lista)
	{
		plista = _plista;
		out = (unsigned int*) (plista + _inicio_lista);
		btat = 0;
		tam_lista = _tam_lista;
		doc_gap = 0;
		pos_lista = 0;
	}

	/*
		Le uma lista invertida
	*/
	int getDoc(unsigned int *doc, unsigned int *info)
	{
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		pos_lista ++;
		pos_skip = (pos_lista-1)/salto_skip;

		doc_gap +=  RawDeltaDecode(out, btat) - 1;
		*info =  RawDeltaDecode(out, btat);

		*doc = doc_gap;

		return 1;
	}

	/*
		Le a lista invertida. Buscando o primeiro docid >= doc_skip
	*/
	inline int getDoc(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{
		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(pos_lista < tam_lista)
		{
			pos_lista ++;

			doc_gap += RawDeltaDecode(out, btat) -1;
			*info = RawDeltaDecode(out, btat);

			*doc = doc_gap;
           #ifdef _CONTADORES
			total_descomprimido++;
			#endif

			if(*doc >= doc_skip) {
				pos_skip = (pos_lista-1)/salto_skip;
				return 1;
			}
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
//		(pos_lista > 0) ? (pos_skip = (pos_lista-1)/salto_skip) : (pos_skip = 0);

		/*Checa se o doc_skip eh menor que o proximo salto da skip*/
		if(tam_skip == 0/* || doc_skip < buffer_skip_docid[pos_skip]*/)	return getDoc(doc, info, doc_skip);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
		pos_skip --;

		/*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
		if(pos_skip < 0 || doc_gap >= buffer_skip_docid[pos_skip])	return getDoc(doc, info, doc_skip);

		/*Salta no proprio bloco que esta em memoria*/
		out = (unsigned int*) (plista + buffer_skip_offset[pos_skip]);

		btat = buffer_skip_bitat[pos_skip];

		/*Calcula qual a nova posicao na lista*/
		pos_lista = (pos_skip+1) * (salto_skip) + 1;

		/*Decodifica o codigo do docId da skiplist*/
		RawDeltaDecode(out, btat) - 1;
		/*Decodifica o TF*/
		*info = RawDeltaDecode(out, btat);

		/*Redefine o doc_gap com o docId da skip*/
		doc_gap = buffer_skip_docid[pos_skip];

		*doc = doc_gap;

		if(*doc >= doc_skip)	return 1;

		/*Procura pelo primeiro docId >= doc_skip*/
		return getDoc(doc, info, doc_skip);
	}


	/*
		Le a lista invertida aplicando saltos com a skiplist. Busca o primeiro docid >= doc_skip
	*/
	inline int getSkipDoc2(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{	
	
		/*Verifica se a lista terminou*/
		if(this->pos_lista == this->tam_lista){	 printf("[getSkipDoc2] fim da lista | lista %d  skip %d\n", tam_lista, tam_skip); return 0;}
		
		int mpos_skip = this->pos_skip; 
		/*Checa se o doc_skip eh menor que o proximo salto da skip*/
		if(this->tam_skip > 0) {
		  unsigned int *mbuffer_skip_docid= this->buffer_skip_docid;
		  
			/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
			while(mpos_skip < this->tam_skip && mbuffer_skip_docid[mpos_skip] <= doc_skip)   mpos_skip++;
			mpos_skip --;

			/*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
			if(mpos_skip >= 0 && this->doc_gap < mbuffer_skip_docid[mpos_skip]) {

				/*Salta com o cursor sobre a lista*/
				out = (unsigned int*) (this->plista + this->buffer_skip_offset[mpos_skip]);

				this->btat = this->buffer_skip_bitat[mpos_skip];

				/*Calcula qual a nova posicao na lista*/
				this->pos_lista = (mpos_skip+1) * (this->salto_skip) + 1;

				/*Decodifica o codigo do docId da skiplist*/
				RawDeltaDecode(this->out, btat);
				/*Decodifica o TF*/
				*info = RawDeltaDecode(this->out, btat);

				/*Redefine o doc_gap com o docId da skip*/
				this->doc_gap = mbuffer_skip_docid[mpos_skip];

				*doc = this->doc_gap;
				
				this->pos_skip = mpos_skip;
				if(*doc >= doc_skip)	return 1; // tirei o this->doc_skip
				
			}
		}

		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(this->pos_lista < this->tam_lista)
		{
			this->pos_lista ++;

			this->doc_gap += RawDeltaDecode(this->out, btat) -1;
			*info = RawDeltaDecode(this->out, btat);

			//*doc = this->doc_gap;
            #ifdef _CONTADORES	
			total_descomprimido++;
			#endif

			if(this->doc_gap >= doc_skip) {
				*doc = this->doc_gap;
				if((mpos_skip!=(pos_lista-1)/salto_skip) && (mpos_skip+2 < (pos_lista-1)/salto_skip)) { printf("123ERRO! : mpos_skip %d,  conta %d\n",mpos_skip, (pos_lista-1)/salto_skip);  }
				this->pos_skip = (pos_lista-1)/salto_skip;
				return 1;
			}
		}

		return 0;
	}

	/*
		Move o ponteiro da skiplist para o bloco com o docid>=doc_skip sem descomprimir nada
	*/
	inline void nextShallow(unsigned int doc_skip)
	{	
		/*Calcula entre qual posicao da skiplist a lista invertida esta*/
//		(pos_lista > 0) ? (pos_skip = (pos_lista-1)/salto_skip) : (pos_skip = 0);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
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
		if(tam_skip == 0 || pos_skip == tam_skip)	return 0;

		return buffer_skip_max_score[pos_skip];
	}
};

#endif
