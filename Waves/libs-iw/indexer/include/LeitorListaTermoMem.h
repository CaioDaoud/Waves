#ifndef LEITOR_LISTA_TERMO_MEM_H
#define LEITOR_LISTA_TERMO_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IndiceInvertido.h"
#include "prefixinmem.h"

class LeitorListaTermoMem
{
	public:
	char *plista;
	_tipo_buffer *out;
	unsigned char btat;
	int doc_gap;
	long tam_lista;
	int pos_lista;
	long cont_doc;
	int freq_atual;

	int tf_max;

	LeitorListaTermoMem() // Iniciar Leitor de Termos
	{	
		cont_doc = 0;
		tf_max = 0;
	}

	~LeitorListaTermoMem() //Deve ser pra encerrar o Leitor Lista Termo
	{
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

	int getDoc(unsigned int *doc, unsigned int *info)
	{
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		pos_lista ++;

		/*Descomprime freq e o numero de docs da freq*/
		if(cont_doc == 0)
		{
			freq_atual = RawDeltaDecode(out, btat);
			cont_doc = RawDeltaDecode(out, btat);
			doc_gap = 0;
		}

		/*Descomprime o docId e calcula o doc_gap*/
		doc_gap +=  RawDeltaDecode(out, btat) - 1;
		*doc = doc_gap;
		*info = freq_atual;

		cont_doc --;

		return 1;
	}

	int getDoc(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{
		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(pos_lista < tam_lista)
		{
			pos_lista ++;

			/*Descomprime freq e o numero de docs da freq*/
			if(cont_doc == 0)
			{
				freq_atual = RawDeltaDecode(out, btat);
				cont_doc = RawDeltaDecode(out, btat);
				doc_gap = 0;
			}

			/*Descomprime o docId e calcula o doc_gap*/
			doc_gap +=  RawDeltaDecode(out, btat) - 1;
			*doc = doc_gap;
			*info = freq_atual;

			cont_doc --;

			if(*doc >= doc_skip)	return 1;
		}

		return 0;
	}
};

#endif
