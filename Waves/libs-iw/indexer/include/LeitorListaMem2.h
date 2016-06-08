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
	unsigned char lastBlock;
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
	//d	cout << "TotalDescomprimido " << total_descomprimido << endl;
	//d	cout << "TotalLido " << total_bloco_lido << endl;
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
		if(pos_skip == tam_skip) { 
		  lastBlock = 1;
		}
		else {
		  lastBlock = 0;
		}
		salto_skip = SALTO_SKIPLIST; // tamanho da skiplist. Nao precisa estar definido aqui! Eu acho. normalmente 128 (entradas ou bytes ?)
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
	  printf("skipdoc2 acionado\n");
		if(!this->lastBlock) {

			unsigned int *mbuffer_skip_docid= this->buffer_skip_docid;
			//int mpos_skip = pos_skip;
			/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
			while(/**pos_skip < this->tam_skip &&*/ mbuffer_skip_docid[pos_skip] < doc_skip)   pos_skip++; // Se for igual cheggou no bloco certo se for maior verificar o bloco
			
			if(pos_skip == this->tam_skip)  {
				lastBlock = 1;
			}

			if(pos_skip > 0) { // se skip for a zero, documento procurado esta antes do 1o documento da lista e deve descomprimir o 1o para retornar um doc maior que ele
				//pos_skip --;
				/*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
				if(this->doc_gap < mbuffer_skip_docid[pos_skip]) {

					/*Salta com o cursor sobre a lista*/
					out = (unsigned int*) (this->plista + this->buffer_skip_offset[pos_skip]);

					btat = this->buffer_skip_bitat[pos_skip];

					/*Decodifica o codigo do docId da skiplist*/
					RawDeltaDecode(this->out, this->btat);  // decodifica o 1o numero do bloco, mas joga fora a informacao. Poderia economizar essa chamada se apontasse para a frequencia abaixo diretamente
					// NA skiplist. Assim seria 1 chamada de rawdecode a menos
					/*Decodifica o TF*/
					*info = RawDeltaDecode(this->out, this->btat);  // decodifica a frequencia aqui

					/*Redefine o doc_gap com o docId da skip*/
					this->doc_gap = mbuffer_skip_docid[pos_skip]; // Salto no documento decodificado para a posicao inicial desse bloco

					*doc = this->doc_gap;

					if(*doc >= doc_skip) {
						this->pos_skip = pos_skip;
						return 1;
					}
				}
			}
			if(lastBlock) {
				/*Calcula qual a nova posicao na lista*/
				this->pos_lista = (pos_skip+1) * (this->salto_skip) + 1;  // AQUI TENHO DUVIDA DO +1 ANTIGO E  TIREI.A POSICAO DEVERIA SER 129 e NAO 257 para mpos = 1 ?
				this->pos_skip = pos_skip; // ISSO AQUI SO PRA DEBUG. DEPOIS CORTA
			}
			else {
				/// AQUI WHILE PARA TERMINAR SEM LAST BLOCK
				int x;

				/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
				while(1)
				{    
					this->doc_gap += RawDeltaDecode(this->out, this->btat) -1; // porque -1 ? Nao seria +1 ? TEM Que ver na compressao
					x = RawDeltaDecode(this->out, this->btat);
					#ifdef _CONTADORES	
					total_descomprimido++;
					#endif
					if(this->doc_gap >= doc_skip) {
						// this->pos_skip = (pos_lista-1)/salto_skip; // Tem mesmo esse -1 na conta ? Se estou na 128 minha posicao na skip seria a 0 ou 1. NAO DEVE SER NECESSARIA!
						this->pos_skip = pos_skip;
						*info = x;
						*doc = this->doc_gap;
						return 1;
					}
				}
				printf("[ERRO GRAVE]  passou aqui mas nao podia \n");
				  // return 0;
			}
		}

		if(this->pos_skip != (pos_lista)/salto_skip) { // ISSO AQUI SO PRA DEBUG. DEPOIS CORTA
			printf("antes - ERRO NAS CONTAS ?: %d %d %d\n",this->pos_skip, pos_lista, salto_skip);  // Tem mesmo esse -1 na conta ? Se estou na 1$
		}


		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(this->pos_lista < this->tam_lista)
		{ 
			int x;
			this->pos_lista ++;
			this->doc_gap += RawDeltaDecode(this->out, this->btat) -1; // porque -1 ? Nao seria +1 ? TEM Que ver na compressao
			x = RawDeltaDecode(this->out, this->btat);
			#ifdef _CONTADORES	
			total_descomprimido++;
			#endif

			if(this->doc_gap >= doc_skip) {
				if(this->pos_skip != (int)(pos_lista)/salto_skip) { // ISSO AQUI SO PRA DEBUG. DEPOIS CORTA
				  printf("ERRO NAS CONTAS ?: %d %d %d %d\n",this->pos_skip, pos_lista, salto_skip, (pos_lista)/salto_skip);  // Tem mesmo esse -1 na conta ? Se estou na 128 minha posicao na skip seria a 0 ou 1
				}
				*info = x;
				*doc = this->doc_gap;
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

