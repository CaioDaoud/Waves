#ifndef VOCABULARIO_H
#define VOCABULARIO_H

#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define LEVEL_BUSCA 32768
#define MAX_ID_WORD 50000000
#define VOCABULARIO_NAME "word"
#define VOCABULARIO_OFFSET "offset"

#define MAX_WORD 30

typedef struct tpWord
{
	struct tpWord * next;				/*Ponteiro para a proxima word da lista*/
	char *word;							/*Word*/
	int id;								/*Id da word*/
	short len;							/*Tamanho da word*/
	int ant;							/*Anterior da fila do cache*/
	int prox;							/*Proximo da fila do cache*/
}TpWord;

class Vocabulario
{
	public:

	int max_size_word;
	int max_id_disco;
	int max_id;
	int num_descritores;				/*Numero de descritores dos arquivos*/
	FILE **fpWord;						/*Descritores para o arquivo de words*/
	FILE **fpOffset;					/*Descritores para o arquivo de offset*/

	char file_vocabulario[4096];
	char file_offset[4096];

	size_t size_principal;				/*Tamanho do hash principal*/
	size_t size_secundario;				/*Tamanho do hash secundario*/
	size_t size_cache;					/*Tamanho do cache*/
	int ocupado_cache;					/*Numero de elementos no cache*/
	int inicio_cache;					/*Indice do primeiro elemento do cache*/
	int fim_cache;						/*Indice do ultimo elemento do cache*/

	TpWord ** hash_principal;			/*Hash principal*/
	TpWord ** hash_secundario;			/*Hash secundario. Utilizado para inserir novas words*/
	TpWord *  cache_word;				/*Cache de words para pesquisa binaria em disco*/

	TpWord *arvore_cache;				/*cache para acelerar a busca binaria em disco*/

	int countReader;					/*Numero de leitores*/
	sem_t sem_reader;					/*Semaforo para leitura do hash secundario*/
	sem_t sem_writer;					/*Semaforo para escrita do hash secundario*/
	sem_t sem_cache;					/*Semaforo para modificacao do cache*/

	Vocabulario(size_t _num_word_cache, size_t _size_secundario, char *_path_vocabulario, int _num_descritores);
	~Vocabulario();

	/*
		Metodos para manipulacao do Vocabulario
	*/

	/*Retorna o Id de uma word. Caso nao exista insere. Usado para Geracao do vocabulario*/
	int insert(char *word, int len, int sessao);

	/*Retorna o Id de uma word e atualiza o cache. Caso nao exista retorna -1. Usado pelo Processador*/
	int getId(char *word, int len, int sessao);

	/*Retorna o Id de uma word . Caso nao exista retorna -1. Usado durante a indexacao (Busca somente no hash Principal)*/
	int getIdNoCache(char *word, int len, int sessao);

	/*
		Metodos internos da Classe
	*/

	/*Busca a word no disco*/
	int buscaWordDisco(char *word, int len, int sessao);

	/*Busca no hash principal (cache) */
	TpWord * buscaHashPrincipal(char *word, int len, int pos_hash);

	/*Busca uma word no hash Secundario (de novas words)*/
	TpWord * buscaHashSecundario(char *word, int len, int pos_hash);

	/*Insere uma word no vocabulario secundario*/
	void insertWord(char *word, int len);

	/*Insere uma word no hash*/
	int insertHash(char *word, int len, TpWord ** hash_word, size_t size_hash);

	/*Marca a entrade de um leitor*/
	void start_reader();

	/*Marca a saida de um leitor*/
	void stop_reader();

	/*Marca a entrada de um escritor, travando os leitores*/
	void start_writer();

	/*Marca a saida de um escritor, liberando os leitores*/
	void stop_writer();

	/*Salva em disco*/
	void dump_disk();

	/*Carrega do disco*/
	void load();

	/*Atualiza a fila do cache LRU*/
	inline void atualizaFilaCache(TpWord *ptr);
	
	/*Imprime VOcabulario Caio Teste*/
	void printHashPrincipal();
	
	/*Remove a palavra do hash*/
	void removeWordHash(TpWord *ptr);

	/*Insere uma palavra no cache*/
	void insereWordCache(char *buffer);

	/*Carrega as primeiras posicoes em um cache para acelerar a pesquisa binaria*/
	void carrega_arvore_cache(int pos, int inicio, int fim);

	void buffer_to_word(TpWord *w, char buffer[]);

	void word_to_buffer(TpWord *w, char buffer[]);
};

#endif
