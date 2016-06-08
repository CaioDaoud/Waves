#ifndef HASH_DOC_H
#define HASH_DOC_H

#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#define TAM_HASH_DOC 1000000

#define NUMBYTES_HASH_DOC ( TAM_HASH_DOC*sizeof(NO_TERMO) )
#define NUMBYTES_NO_TERMO (sizeof(NO_TERMO))
#define NUMBYTES_NO_POSICAO (sizeof(NO_POSICAO))

using namespace std;

typedef unsigned int _tipo_posicional;
typedef unsigned int _tipo_id;
typedef unsigned int _tipo_frequencia;

typedef struct no_posicao {
	_tipo_posicional posicao;
	struct no_posicao *prox;
} NO_POSICAO;

typedef struct no_termo {
	_tipo_id termo_id;
	_tipo_frequencia freq;
	struct no_termo *prox;
	NO_POSICAO *prim;
	NO_POSICAO *ult;
} NO_TERMO;

class HashDoc{
	
	public:
	
	_tipo_id id_doc;
	NO_TERMO *hash_doc[ TAM_HASH_DOC ];
	
	unsigned int numero_termos;
	unsigned int numero_posicoes;
	
	
	HashDoc();
	
	void set_id_doc( _tipo_id id );
	_tipo_id get_id_doc();
	
	void Inicializa( );
	void InicializaContadores();
	unsigned int RetornaOcupacao();
	unsigned int funcao_hash( _tipo_id valor );
	
	NO_POSICAO* CriaPosicao( _tipo_posicional posicao );
	void InserePosicao( _tipo_posicional posicao, NO_TERMO *ptermo );
	NO_TERMO* CriaTermo( _tipo_id termo_id, _tipo_posicional posicao );
	NO_TERMO* BuscaTermo( NO_TERMO *inicio, _tipo_id termo_id );
	void Insere( _tipo_id id_termo, _tipo_posicional posicao );
	void Imprime();
	NO_TERMO* RetornaLista( unsigned int posicao_hash );
};


#endif
