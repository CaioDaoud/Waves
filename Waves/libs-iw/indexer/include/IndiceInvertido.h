#ifndef INDICEINVERTIDO_H
#define INDICEINVERTIDO_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include "Heap.h"
#include "prefixinmem.h"

using namespace std;

typedef unsigned int _tipo_buffer;

#define NOME_TEMPORARIO "temp"
#define NOME_POSICIONAL "posicional"
#define NOME_FREQUENCIA "frequencia"
#define NOME_FREQ_SKIP "freq_skip"
#define NOME_POS_SKIP "pos_skip"

/*LIsta ordenada por frequencia*/
#define NOME_TERMO_FREQ "termo.frequencia"
#define NOME_TERMO_IDX "termo.idx"

#define NOME_IDX "idx"
#define NOME_IDF "idf"
#define NOME_NORMA "norma"

#define SIZE_BUFFER_LEITOR_LISTA 40000000
#define SIZE_FOLGA_LEITOR_LISTA 3
#define SIZE_MAX_SKIP_LIST 800000

//#define SALTO_SKIPLIST 256
#define SALTO_SKIPLIST 16
//#define SALTO_SKIPLIST 64

#define TAM_BUFFER 1000000000			/*Tamanho do vetor para armazenar as listas temporarias*/

#define TAM_INDICE_INVERTIDO 100000000	/*Tamanho do hash para armazenar os termos do indice invertido*/
#define TAM_BLOCO_DOCPOS 50				/*Tamanho do bloco para cada fragmento da lista parcial (Deve ser multiplo de 2)*/

#define ULT_DOC_LISTA 0 				/*Indica que a lista chegou ao final*/

#define TAM_BLOCO_LISTA 2

#define TAM_BUFFER_LISTA 50000000		/*Tamanho do buffer lido de cada parcial durante a compressao*/

#define TAM_BLOCO_LISTA_FINAL 100000000	/*Tamanho do buffer de compressao usado pelo Merge*/

#define TAM_BLOCO_INDICE 4000000000		/*Tamanho maximo de um arquivo de lista invertido (16GB)*/

#define FLAG_ESCREVE_BUFFER_FINAL 1
#define FLAG_FINALIZA_INDICE 2

#define VerificaBloco(a,b,i) if( a == b) LeListaParcial(i)
#define VerificaEspacoBufferFinal() if( outbuffer >= &buffer_compressao[TAM_BLOCO_LISTA_FINAL] ) EscreveBufferFinal();

#define FLAG_PARCIAL 0
#define FLAG_MERGE	 1

typedef struct no_lista {
	_tipo_buffer id;
	unsigned int tam_lista;
	unsigned int num_blocos;
	unsigned int plistadoc;
	unsigned int pultdoc;
	unsigned int pos_bloco_atual;
	struct no_lista *prox;

} NO_LISTA_INV;

typedef struct{
	_tipo_buffer id;
	unsigned int tam_lista;
	unsigned int plistadoc;
} TERMO;

typedef struct {
	_tipo_buffer doc;
	_tipo_buffer pos;
	int controle;
} GAP;

typedef struct {
	char id_arq;

	unsigned long int pos_inicio_termo;
	unsigned int pos_tam_lista;

	unsigned long int pos_inicio_skip;
	unsigned int pos_tam_skip;

	unsigned long int freq_inicio_termo;
	unsigned int freq_tam_lista;

	unsigned long int freq_inicio_skip;
	unsigned int freq_tam_skip;
} IDX;

typedef struct {
	char id_arq;
	unsigned long int freq_inicio_termo;
	unsigned int freq_tam_lista;
}IDX_TERMO;

class IndiceInvertido{

	public:

	int flag_operacao;

	_tipo_buffer *buffer;					/*Buffer para geracao dos temporarios em memoria*/
	_tipo_buffer *buffer_compressao;		/*Buffer para compressao das listas parciais durante o merge*/
	_tipo_buffer *buffer_parcial;			/*Buffer para leitura das listas parciais durante o merge*/

	NO_LISTA_INV **indice_invertido;
	TERMO *vet_termos;
	FILE **fparciais;
	FILE *fbfinal, *fidx;

	IDX tidx; //informacoes do termo para o .idx
	unsigned int controle_id;
	unsigned int flag_finaliza_parcial;
	unsigned int ocupacao_buffer;
	unsigned int ocupacao_buffer_compressao;
	unsigned int numero_termos;
	unsigned int cont_temporarios;
	unsigned long int cont_tam_indice;
	unsigned int cont_final;
	char *path_indice;
	char filename[4096];

	unsigned int num_doc_lista;

	/*Compressao*/
	unsigned char btat;
	unsigned int *outbuffer;

	IndiceInvertido(char *_path_indice, int _flag_operacao);
	~IndiceInvertido();

	void InicializaContadores();

	unsigned int get_ocupacao_buffer();
	unsigned int get_cont_temporarios();
	void set_cont_temporarios( unsigned int valor );

	_tipo_buffer funcao_hash( _tipo_buffer valor );

	NO_LISTA_INV* CriaTermo( _tipo_buffer id );
	void InsereNoBuffer( NO_LISTA_INV *termo, _tipo_buffer doc, _tipo_buffer pos );
	NO_LISTA_INV* BuscaTermo( unsigned int endereco, _tipo_buffer id );
	void Insere( _tipo_buffer doc, _tipo_buffer termo, _tipo_buffer pos );

	void OrdenaTermos();
	void CriaIndiceTemporario();

	void CarregaIndiceInvertido( unsigned int id_indice );
	void ImprimeVetTermos();

	void AbreParciais();
	void FechaParciais();
	void AbreArquivoFinal( unsigned int id );
	void AbreArquivoIDX();
	void FechaArquivoIDX();

	void ImprimeIDX();

	void Comprime(_tipo_buffer id);
	int TemTemporarios();
	void InicializaMerge();
	int LeTermo( unsigned int id, TipoHeap *flag );
	void EscreveIdTermo( TipoHeap termo_atual );
	void CopiaBufferFinal( TipoHeap termo_atual, GAP *gap );
	void EscreveIDX( unsigned int id );
	void EscreveBufferFinal();
	void FinalizaIndice();
	int  VerificaBufferFinal();
	void Merge();

	void dump_temporario();
	
	unsigned int full(){	return flag_finaliza_parcial;	}
};

#endif
