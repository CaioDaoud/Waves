#ifndef LEITOR_LISTA_BLOCK_MEM_H
#define LEITOR_LISTA_BLOCK_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IndiceInvertido.h"
#include "prefixinmem.h"

class LeitorListaBlockMem
{
	public:
	char *plista;
	_tipo_buffer *buffer;
	_tipo_buffer *out_docid;
	_tipo_buffer *out_freq;
	unsigned char btat_freq;
	unsigned char btat_docid;
	unsigned int doc_gap;
 long *buffer_skip_offset;
	unsigned int *buffer_skip_docid;
	unsigned int *buffer_skip_pos_freq;
	unsigned int *buffer_skip_max_score;
	int pos_skip;
	int tam_skip;
	long tam_lista;
	int pos_lista;
	int salto_skip;
	int cont_block;
	int pos_freq_block;

	LeitorListaBlockMem()
	{	
	}

	~LeitorListaBlockMem()
	{
	}

	void InicializaSkip(char *_plista, long _tam_lista, long _inicio_lista, char *pskip, long _tam_skip, long _inicio_skip)
	{
		plista = _plista;

		btat_docid = 0;
		btat_freq = 0;
		tam_lista = _tam_lista;
		doc_gap = 0;
		pos_lista = 0;

		/*Le o vetor de docid, offset e bitat*/
		buffer_skip_docid = (unsigned int*) (pskip + _inicio_skip);
		buffer_skip_offset = (long*) (pskip + _inicio_skip + sizeof(unsigned int)*_tam_skip);
		buffer_skip_pos_freq = (unsigned int*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long))*_tam_skip);
		buffer_skip_max_score = (unsigned int*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long) + sizeof(unsigned int))*_tam_skip);

		out_docid = (unsigned int*) (plista + _inicio_lista);
		out_freq = (unsigned int*) (plista + _inicio_lista + buffer_skip_pos_freq[0]);

		pos_skip = 0;
		tam_skip = _tam_skip;
		salto_skip = SALTO_SKIPLIST;
		cont_block = 0;
		pos_freq_block = 0;
	}

	/*
		Le uma lista invertida
	*/
	int getDoc(unsigned int *doc, unsigned int *info)
	{
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		pos_lista ++;

		if(cont_block == SALTO_SKIPLIST)
		{
			pos_skip = pos_lista/SALTO_SKIPLIST;
			/*Salta no proprio bloco que esta em memoria*/
			out_docid = (unsigned int*) (plista + buffer_skip_offset[pos_skip]);
			out_freq  = (unsigned int*) (plista + buffer_skip_offset[pos_skip] + buffer_skip_pos_freq[pos_skip]);
			btat_freq = btat_docid = cont_block =	pos_freq_block = 0;
		}

		cont_block ++;

		doc_gap +=  RawDeltaDecode(out_docid, btat_docid) - 1;

		*doc = doc_gap;

		return 1;
	}

	/*Descomprime a frequencia desejada*/
	inline int getFreq(unsigned int *info)
	{
		while(pos_freq_block < cont_block)
		{
			*info = RawDeltaDecode(out_freq, btat_freq);
			pos_freq_block ++;
		}
		return 1;
	}

	/*
		Le a lista invertida. Buscando o primeiro docid >= doc_skip
	*/
	inline int getDoc(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{
		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
	  printf("usando getdoc blockmen\n");
		while(pos_lista < tam_lista)
		{
			pos_lista ++;

			if(cont_block == SALTO_SKIPLIST)
			{
				pos_skip = pos_lista/SALTO_SKIPLIST;
				/*Salta no proprio bloco que esta em memoria*/
				out_docid = (unsigned int*) (plista + buffer_skip_offset[pos_skip]);
				out_freq  = (unsigned int*) (plista + buffer_skip_offset[pos_skip] + buffer_skip_pos_freq[pos_skip]);
				btat_freq = btat_docid = cont_block =	pos_freq_block = 0;
			}
			cont_block ++;

			/*Descomprime o docid*/
			doc_gap += RawDeltaDecode(out_docid, btat_docid) -1;

			*doc = doc_gap;

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
		if(doc_skip <= buffer_skip_docid[pos_skip])	return getDoc(doc, info, doc_skip);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] < doc_skip)   pos_skip++;

		/*Não achou docid>=doc_skip*/
		if(pos_skip == tam_skip)
		{
			pos_lista = tam_lista;
			return 0;
		}

		/*Salta no proprio bloco que esta em memoria*/
		out_docid = (unsigned int*) (plista + buffer_skip_offset[pos_skip]);
		out_freq  = (unsigned int*) (plista + buffer_skip_offset[pos_skip] + buffer_skip_pos_freq[pos_skip]);
		btat_freq = btat_docid = cont_block =	pos_freq_block = 0;

		/*Calcula qual a nova posicao na lista*/
		pos_lista = (pos_skip) * (salto_skip);

		/*Redefine o doc_gap com o docId da skip anterior*/
		doc_gap = buffer_skip_docid[pos_skip-1];

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
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] < doc_skip)   pos_skip++;

		return 1;
	}

	/*
		Pega o MaxDocId do bloco
	*/
	unsigned int getDocBlockBoundary()
	{
		if(pos_skip >= tam_skip)		return buffer_skip_docid[tam_skip-1];

		return buffer_skip_docid[pos_skip];		
	}

	/*
		Pega o MaxScore do bloco
	*/
	unsigned int getBlockMaxScore()
	{
		if(pos_skip >= tam_skip)	return buffer_skip_max_score[tam_skip-1];

		return buffer_skip_max_score[pos_skip];
	}
};

#endif
