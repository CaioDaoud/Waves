#ifndef HEAP_H
#define HEAP_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int TipoChave;
typedef struct TipoHeap {
	TipoChave Chave;
	unsigned int id_parcial;
	unsigned int tam_lista;
} TipoHeap;
typedef unsigned int TipoIndice;

class Heap {
	public:
	
	TipoHeap *heap;
	unsigned int tam_heap;

	void AlocaHeap( unsigned int n );

	int GetTamHeap();
	void Imprime();
	void Refaz(TipoIndice Esq, TipoIndice Dir, TipoHeap *A);
	void Constroi();
	TipoHeap Retira();
	void InsereNaPosicao(unsigned int pos, TipoHeap x);
	void Insere( TipoHeap x );
	void AumentaChave(TipoIndice i, TipoChave ChaveNova, unsigned int id, TipoHeap *A);
	void Testa();
};

#endif
