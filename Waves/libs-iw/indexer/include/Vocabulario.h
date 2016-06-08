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

typedef struct tpWord {
    struct tpWord * next;
    char *word;	
    int id;
    short len;
    int ant;
    int prox;
}TpWord;

class Vocabulario {
public:
    
    int max_size_word;
    int max_id_disco;
    int max_id;
    int num_descritores;
    FILE **fpWord;
    FILE **fpOffset;

    char file_vocabulario[4096];
    char file_offset[4096];

    size_t size_principal;
    size_t size_secundario;
    size_t size_cache;
    int ocupado_cache;
    int inicio_cache;
    int fim_cache;

    TpWord ** hash_principal;
    TpWord ** hash_secundario;
    TpWord *  cache_word;
    TpWord *arvore_cache;

    int countReader;
    sem_t sem_reader;
    sem_t sem_writer;
    sem_t sem_cache;

    Vocabulario(size_t _num_word_cache, size_t _size_secundario, char *_path_vocabulario, int _num_descritores);
    ~Vocabulario();

    int insert(char *word, int len, int sessao);
    int getId(char *word, int len, int sessao);
    int getIdNoCache(char *word, int len, int sessao);
    int buscaWordDisco(char *word, int len, int sessao);
    TpWord * buscaHashPrincipal(char *word, int len, int pos_hash);
    TpWord * buscaHashSecundario(char *word, int len, int pos_hash);
    void insertWord(char *word, int len);
    int insertHash(char *word, int len, TpWord ** hash_word, size_t size_hash);
    void start_reader();
    void stop_reader();
    void start_writer();
    void stop_writer();
    void dump_disk();
    void load();
    inline void atualizaFilaCache(TpWord *ptr);
    void printHashPrincipal();
    void removeWordHash(TpWord *ptr);
    void insereWordCache(char *buffer);
    void carrega_arvore_cache(int pos, int inicio, int fim);
    void buffer_to_word(TpWord *w, char buffer[]);
    void word_to_buffer(TpWord *w, char buffer[]);
};

#endif
