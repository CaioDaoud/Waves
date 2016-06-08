#include "../../libs-iw/indexer/include/HashDoc.h"

HashDoc::HashDoc()
{
	Inicializa();
	InicializaContadores();
}

void HashDoc::set_id_doc( _tipo_id id ){
	id_doc = id;
}

_tipo_id HashDoc::get_id_doc( ){
	return id_doc;
}

void HashDoc::Inicializa( )
{
	int i;
	for( i = 0; i < TAM_HASH_DOC; i++ ){
		hash_doc[i] = NULL;
	}
}

void HashDoc::InicializaContadores()
{
	numero_posicoes = 0;
	numero_termos = 0;
}

unsigned int HashDoc::RetornaOcupacao()
{
	return ( (3*numero_termos) + numero_posicoes);
}

unsigned int HashDoc::funcao_hash( _tipo_id valor )
{
	return valor;
}

NO_POSICAO* HashDoc::CriaPosicao( _tipo_posicional posicao )
{
	NO_POSICAO *aux = (NO_POSICAO*)malloc(sizeof(NO_POSICAO));
	aux->posicao = posicao;
	aux->prox = NULL;
	
	numero_posicoes++;
	
	return aux;
}

void HashDoc::InserePosicao( _tipo_posicional posicao, NO_TERMO *ptermo )
{
	if( ptermo->prim == NULL ) {
		ptermo->prim = CriaPosicao( posicao );
		ptermo->ult = ptermo->prim;
	}
	else{
		ptermo->ult->prox = CriaPosicao( posicao );
		ptermo->ult = ptermo->ult->prox;
	}
	ptermo->freq++;
}

NO_TERMO* HashDoc::CriaTermo( _tipo_id termo_id, _tipo_posicional posicao )
{
	NO_TERMO *aux = (NO_TERMO*)malloc(sizeof(NO_TERMO));
	aux->termo_id = termo_id;
	aux->freq = 0;
	aux->prox = NULL;
	aux->prim = NULL;
	aux->ult = NULL;
	
	InserePosicao( posicao, aux );
	
	numero_termos++;
	
	return aux;
}

NO_TERMO* HashDoc::BuscaTermo( NO_TERMO *inicio, _tipo_id termo_id )
{
	NO_TERMO *aux;	
	
	aux = inicio;
	
	while( aux != NULL ){
		if( aux->termo_id == termo_id ){
			return aux;
		}
		aux = aux->prox;
	}
	return NULL;
}

void HashDoc::Insere( _tipo_id id_termo, _tipo_posicional posicao )
{
	NO_TERMO *aux;
	
	int endereco = funcao_hash( id_termo ) % TAM_HASH_DOC;
	
	if( hash_doc[ endereco ] == NULL ) {
		hash_doc[ endereco ] = CriaTermo( id_termo, posicao );
	}
	else{
		aux = BuscaTermo( hash_doc[ endereco ], id_termo );
		if( aux == NULL ) {
			hash_doc[ endereco ] = CriaTermo( id_termo, posicao );
			hash_doc[ endereco ]->prox = NULL;
		}
		else{
			InserePosicao( posicao, aux );
		}
	}
}

void HashDoc::Imprime()
{
	int i;
	NO_TERMO *aux;
	NO_POSICAO *temp;
	
	for( i = 0; i < TAM_HASH_DOC; i++ ){
		aux = hash_doc[i];
		while( aux != NULL ) {
			printf("doc: (%d) freq(%d)\n\t", aux->termo_id, aux->freq );
			temp = aux->prim;
			while( temp != NULL ) {
				printf("(%d) -> ", temp->posicao );
				temp = temp->prox;
			}
			printf("\n\n");
			aux = aux->prox;
		}
	}
}

NO_TERMO* HashDoc::RetornaLista( unsigned int posicao )
{
	NO_TERMO *aux = hash_doc[ posicao ];
	hash_doc[ posicao ] = NULL;
	return aux;
}
