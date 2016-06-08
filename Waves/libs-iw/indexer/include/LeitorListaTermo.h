#ifndef LEITOR_LISTA_TERMO_H
#define LEITOR_LISTA_TERMO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IndiceInvertido.h"
#include "prefixinmem.h"

class LeitorListaTermo
{
	public:
	FILE *fp;
	_tipo_buffer *buffer;
	_tipo_buffer *out;
	unsigned char btat;
	char fileatual[1000];
	int doc_gap;
	long tam_lista;
	int pos_lista;
	long cont_doc;
	int freq_atual;

	int tf_max;

	LeitorListaTermo() // Iniciar Leitor de Termos
	{	
		fp = NULL;
		fileatual[0] = 0;
		buffer = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA];

		cont_doc = 0;
		tf_max = 0;
	}

	~LeitorListaTermo() //Deve ser pra encerrar o Leitor Lista Termo
	{
		delete[] buffer;
		if(fp)	fclose(fp);
	}

//	inline unsigned int RawDeltaDecode(out, btat) //Função responsável por Descomprimir
//	{
//		return RawDeltaDecode(out, btat);
//	}

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

		out = buffer;
		btat = 0;
		tam_lista = _tam_lista;
		doc_gap = 0;
		pos_lista = 0;
	}

	void load_buffer()
	{
		memcpy(buffer, &buffer[SIZE_BUFFER_LEITOR_LISTA], SIZE_FOLGA_LEITOR_LISTA*sizeof(_tipo_buffer));
		fread(buffer+SIZE_FOLGA_LEITOR_LISTA, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA, fp);
		out -= SIZE_BUFFER_LEITOR_LISTA;
	}

	int getDoc(unsigned int *doc, unsigned int *info)
	{
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		pos_lista ++;

		/*Checa o final do buffer. Deve-se ler mais*/
		if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
		{
			load_buffer();
		}

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

			/*Checa o final do buffer. Deve-se ler mais*/
			if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
			{
				load_buffer();
			}

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
