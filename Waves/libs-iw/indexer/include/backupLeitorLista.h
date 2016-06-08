#ifndef LEITOR_LISTA_H
#define LEITOR_LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IndiceInvertido.h"
#include "prefixinmem.h"

#define SIZE_BUFFER_LISTA 100000
#define SIZE_FOLGA 5

class LeitorLista
{
	public:
	FILE *fp;
	_tipo_buffer buffer[SIZE_BUFFER_LISTA+SIZE_FOLGA];
	unsigned char btat;
	_tipo_buffer *out;
	long tam_lista;
	char fileatual[1000];
	int doc_gap;

	LeitorLista(){	fp = NULL;	fileatual[0] = 0;}

	void Inicializa(char *arq, long _tam_lista, long inicio)
	{
		if(strcmp(fileatual, arq))
		{
			if(fp)	fclose(fp);
			strcpy(fileatual, arq);
			fp = fopen( arq, "rb");
		}

		fseek( fp, inicio, SEEK_SET);
		fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LISTA+SIZE_FOLGA, fp);

		out = buffer;
		btat = 0;
		tam_lista = _tam_lista;
		doc_gap = 0;
	}

inline	int getDoc(unsigned int *doc, unsigned int *info)
	{
		if(tam_lista == 0)	return 0;

		tam_lista --;

		/*Final do buffer. Deve-se ler mais*/
		if(out >= &buffer[SIZE_BUFFER_LISTA])
		{
			memcpy(buffer, &buffer[SIZE_BUFFER_LISTA], SIZE_FOLGA*sizeof(int));
			fread(buffer+SIZE_FOLGA, sizeof(_tipo_buffer), SIZE_BUFFER_LISTA, fp);
			out -= SIZE_BUFFER_LISTA;
		}

		*doc = doc_gap + RawDeltaDecode(out, btat) - 1;
		doc_gap = *doc;

		/*Final do buffer. Deve-se ler mais*/
		if(out >= &buffer[SIZE_BUFFER_LISTA])
		{
			memcpy(buffer, &buffer[SIZE_BUFFER_LISTA], SIZE_FOLGA*sizeof(int));
			fread(buffer+SIZE_FOLGA, sizeof(_tipo_buffer), SIZE_BUFFER_LISTA, fp);
			out -= SIZE_BUFFER_LISTA;
		}

		*info = RawDeltaDecode(out, btat) - 1;

		return 1;
	}
};

#endif
