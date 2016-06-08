#include "Vocabulario.h"

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

#if (__GNUC__ == 4 && 3 <= __GNUC_MINOR__) || 4 < __GNUC__
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

inline int comparaWordBusca(char *w1, int len, char *w2, int len2) {
    while(len && len2 && *w1 == *w2) {
        w1++;
        w2++;
        len --;
        len2--;
    }
	
    if(len==0 && len2)
        return -1;
    if(len && len2==0)
        return 1;
    if(len==0 && len2==0)
        return 0;
    return *w1 - *w2;	
}

inline int comparaWordQsort(const void *a, const void *b) {
    char *w1 = (*(TpWord**)a)->word;
    char *w2 = (*(TpWord**)b)->word;
    int len1 = (*(TpWord**)a)->len;
    int len2 = (*(TpWord**)b)->len;
    
    return comparaWordBusca(w1, len1, w2, len2);
}

inline bool comparaWord(char *word, int len, char *word2, int len2) {
    if(len != len2)
        return false;

    len --;
    while(len >= 0 && word[len] == word2[len]) 	
        len --;	

    if(len>-1)
        return false;

    return true;
}

inline void Vocabulario::word_to_buffer(TpWord *w, char buffer[]) {
    memset(buffer, 0, max_size_word);
    memcpy(buffer, &(w->id), sizeof(int));
    memcpy(buffer+sizeof(int), &(w->len), sizeof(short));
    memcpy(buffer+sizeof(int)+sizeof(short int), w->word, w->len);
}

inline void Vocabulario::buffer_to_word(TpWord *w, char buffer[]) {
    w->id = *(int*)buffer;
    w->len = *(short int*)&buffer[4];
    w->word = (char*)malloc(sizeof(char) * w->len);
    memcpy(w->word, buffer+6, w->len);
}

inline unsigned int hashFunction(char *data, int len) {			
    unsigned int hash = len;
    int tmp, rem;

    if (len <= 0 || data == NULL)	return 0;

    rem = len & 3;
    len >>= 2;

    for (;len > 0; len--) {
        hash += get16bits (data);
        tmp = (get16bits (data+2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        data += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    switch (rem) {
        case 3:
            hash += get16bits (data);
            hash ^= hash << 16;
            hash ^= data[sizeof (uint16_t)] << 18;
            hash += hash >> 11;
            break;
    	case 2: 
            hash += get16bits (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1:
            hash += *data;
            hash ^= hash << 10;
            hash += hash >> 1;
    } 

    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

Vocabulario::Vocabulario(size_t _num_word_cache, size_t _size_secundario, char *_path_vocabulario, int _num_descritores) {
    unsigned long int i;
    size_cache  = _num_word_cache;
    size_principal  = _num_word_cache * 5;
    size_secundario = _size_secundario;
    num_descritores = _num_descritores;
    max_size_word = MAX_WORD;

    printf("numero de descritores %d\n",_num_descritores);
    sprintf(file_vocabulario, "%s%s", _path_vocabulario, VOCABULARIO_NAME);
    sprintf(file_offset, "%s%s", _path_vocabulario, VOCABULARIO_OFFSET);

    fpWord = (FILE**)malloc(sizeof(FILE*) * _num_descritores);
    fpOffset = (FILE**)malloc(sizeof(FILE*) * _num_descritores);

    hash_principal = (TpWord**)malloc(sizeof(TpWord*) * size_principal);
    hash_secundario = (TpWord**)malloc(sizeof(TpWord*) * size_secundario);
    arvore_cache = (TpWord*)malloc(sizeof(TpWord) * LEVEL_BUSCA);
    cache_word = (TpWord*)malloc(sizeof(TpWord) * size_cache);

    printf("size principal %lu size_secundario %lu\n",size_principal,size_secundario);
	
    countReader = 0;
    sem_init(&sem_reader, 0, 1);
    sem_init(&sem_writer, 0, 1);
    sem_init(&sem_cache, 0, 1);

    max_id = max_id_disco = 0;

    inicio_cache = -1;
    fim_cache = -1;
    ocupado_cache = 0;
	
    for(i = 0; i<size_principal; i++){
        hash_principal[i] = NULL;
    }
    
    for(i = 0; i<size_secundario; i++){
        hash_secundario[i] = NULL;
    }
    
    for(i = 0; i<size_cache; i++){
        cache_word[i].next = NULL;
    }
    
    for(i = 0; i < LEVEL_BUSCA; i++){
        arvore_cache[i].word = NULL;
    }
    
    for(int j = 0; j< num_descritores; j++) {
        fpWord[j] = fopen(file_vocabulario, "r");
        if(!fpWord[j]) {
            printf("Erro ao abrir arquivo %s\n",file_vocabulario);
            exit(1);
        }
        fpOffset[j] = fopen(file_offset, "r");
        if(!fpOffset[j]) {
            printf("Erro ao abrir arquivo %s\n",file_offset);
            exit(1);
        }
    }
    
    if(fpOffset[0] && fpWord[0])
        load();
    else {
        printf("criando vocabulário\n");
        num_descritores=1;
        fpWord[0] = fopen(file_vocabulario, "w");
        if(!fpWord[0]) {
            printf("Erro ao criar arquivo %s\n",file_vocabulario);
	    exit(1);
        }
        fpOffset[0] = fopen(file_offset, "w");
        if(!fpOffset[0]) {
	    printf("Erro ao criar arquivo %s\n",file_offset);
	    exit(1);
        }
        
        load();
    }
}

Vocabulario::~Vocabulario() {
    unsigned int i;
    TpWord *ptr, *ptr_aux;

    for(int j = 0; j < num_descritores; j++) {
        if(fpWord[j])
            fclose(fpWord[j]);
        if(fpWord[j])
            fclose(fpOffset[j]);
    }
    
    dump_disk();

    for(i = 0; i<size_secundario; i++) {
        ptr = hash_secundario[i];
        while(ptr) {
            ptr_aux = ptr;
            ptr = ptr->next;
            free(ptr_aux->word);
            free(ptr_aux);
        }
    }

    for(int j = 0; j<ocupado_cache; j++){
        free(cache_word[j].word);
    }

    for(int i = 0; i < LEVEL_BUSCA; i++){
        if(arvore_cache[i].word)
            free(arvore_cache[i].word);
    }
    
    free(fpWord);
    free(fpOffset);
    free(hash_secundario);
    free(hash_principal);
    free(cache_word);
    free(arvore_cache);
}

int Vocabulario::insertHash(char *word, int len, TpWord ** hash_word, size_t size_hash) {
    unsigned int pos = hashFunction(word, len) % size_hash;
    start_writer();
    TpWord *ptr = hash_secundario[pos];

    while(ptr && comparaWord(ptr->word, ptr->len, word, len) == false) {
        ptr = ptr->next;
    }

    if(ptr) {	
        stop_writer();
        return ptr->id;
    }

    TpWord * new_word = (TpWord*)malloc(sizeof(TpWord));

    new_word->word = (char*)malloc(sizeof(char)*len+1);
    memcpy(new_word->word, word, sizeof(char)*len);
    new_word->word[len] = 0;
    new_word->len = len;

    int id = max_id;
    new_word->id = max_id;
    max_id ++;
    new_word->next = hash_word[pos];
    hash_word[pos] = new_word;

    stop_writer();
    return id;
}

TpWord * Vocabulario::buscaHashSecundario(char *word, int len, int pos_hash) {
    start_reader();
    TpWord *ptr = hash_secundario[pos_hash];

    while(ptr && comparaWord(ptr->word, ptr->len, word, len) == false) {
        ptr = ptr->next;
    }

    stop_reader();
    return ptr;
}

inline TpWord * Vocabulario::buscaHashPrincipal(char *word, int len, int pos_hash) {
    TpWord *ptr = hash_principal[pos_hash];
    
    while(ptr && comparaWord(ptr->word, ptr->len, word, len) == false) {
        ptr = ptr->next;	
    }

    return ptr;
}

void Vocabulario::printHashPrincipal() {
    TpWord *ptr = hash_principal[0];
    
    while(ptr){
        printf("[ %s ] \n",ptr->word); 
        ptr = ptr->next;
    }
	
}

int Vocabulario::buscaWordDisco(char *word, int len, int sessao) {
    char buffer[max_size_word];
    int pos = 0, ret, meio, inicio = 0, fim = max_id_disco-1;

    if(sessao > num_descritores)
        sessao = 0;

    while(pos < LEVEL_BUSCA && pos < max_id_disco) {
        ret = comparaWordBusca(arvore_cache[pos].word, arvore_cache[pos].len, word, len);

        if(ret == 0)
            return arvore_cache[pos].id;

        if(ret < 0) {
            if(pos*2+2 >= LEVEL_BUSCA || pos*2+2 >= max_id_disco)	break;
            pos = pos*2+2;
        }	
        else {
            if(pos*2+1 >= LEVEL_BUSCA || pos*2+1 >= max_id_disco)	break;
            pos = pos*2+1;
        }
    }

    inicio = arvore_cache[pos].ant;
    fim = arvore_cache[pos].prox;
    meio = (inicio+fim)/2;

    (ret < 0) ? (inicio = meio+1) : (fim = meio-1);

    while(inicio <= fim) {
        meio = (inicio+fim)/2;

        fseek(fpWord[sessao], meio * max_size_word, SEEK_SET);
        fread(buffer, max_size_word, sizeof(char), fpWord[sessao]);
        ret = comparaWordBusca(buffer+sizeof(int)+sizeof(short), *(short*)&buffer[sizeof(int)], word, len);

        if(ret == 0) {
            insereWordCache(buffer);
            return *(int*)buffer;
        }

        (ret < 0) ? (inicio = meio + 1) : (fim = meio - 1);
    }
    
    return -1;
}

int Vocabulario::insert(char *word, int len, int sessao) {	
    unsigned int pos_hash = hashFunction(word, len);
    TpWord *ptr = buscaHashPrincipal(word, len, pos_hash % size_principal);

    if(ptr)
        atualizaFilaCache(ptr);

    if(ptr == NULL)
        ptr = buscaHashSecundario(word, len, pos_hash % size_secundario);

    if(ptr == NULL && max_id_disco) {
        int id = buscaWordDisco(word, len, sessao);
        if(id!=-1)
            return id;
    }

    if(ptr)
        return ptr->id; 

    int id = insertHash(word, len, hash_secundario, size_secundario);
    return id;
}

int Vocabulario::getId(char *word, int len, int sessao) {	
    unsigned int pos_hash = hashFunction(word, len);
    TpWord *ptr = buscaHashPrincipal(word, len, pos_hash % size_principal);

    if(ptr)
        atualizaFilaCache(ptr);

    if(ptr == NULL && max_id > max_id_disco)
        ptr = buscaHashSecundario(word, len, pos_hash % size_secundario);

    if(ptr == NULL && max_id_disco) {
        int id = buscaWordDisco(word, len, sessao);
        if(id!=-1)
            return id;
    }

    if(ptr)
        return ptr->id; 

    return -1;
}

int Vocabulario::getIdNoCache(char *word, int len, int sessao) {	
    unsigned int pos_hash = hashFunction(word, len);
    TpWord *ptr = buscaHashPrincipal(word, len, pos_hash % size_principal);

    if(ptr)
        return ptr->id; 

    return -1;
}

void Vocabulario::dump_disk() {
    if(max_id <= max_id_disco)
        return;

    unsigned int k=0, j, sum = 0;
    int i;
    int *inicio = (int*)malloc(sizeof(int)*2*max_id);
    char buffer[1000][max_size_word];
    char buffer_aux[max_size_word];
    FILE *fpWord = fopen(file_vocabulario, "r");
    FILE *fpOffset = fopen(file_offset, "w");
    FILE *fpNovo = fopen("auxiliar", "w");

    TpWord ** id_word = (TpWord**)malloc(sizeof(TpWord*) * max_id-max_id_disco);
    TpWord *ptr;

    for(j = 0; j<size_secundario; j++) {
        ptr = hash_secundario[j];

        while(ptr) {	
            id_word[k++] = ptr;
            ptr = ptr->next;
        }
    }

    qsort(id_word, k, sizeof(TpWord*), comparaWordQsort);

    if(max_id_disco == 0) {
        for(i = 0; i<max_id; i++) {
            inicio[2*id_word[i]->id] = sum;
            sum += max_size_word;
            inicio[2*id_word[i]->id + 1] = sum;
            word_to_buffer(id_word[i], buffer_aux);
            fwrite(buffer_aux, max_size_word, sizeof(char), fpNovo);
        }
    }
    else {
        unsigned int id, b, j;
        fseek(fpWord, 0, SEEK_SET);
        i= max_id_disco;

        while(i < max_id || !feof(fpWord)) {
            b = fread(buffer, sizeof(char)*max_size_word, 1000, fpWord);
            j = 0;
            
            while(j<b && i<max_id) {
                if(comparaWordBusca(buffer[j]+sizeof(int)+sizeof(short), *(short*)&buffer[j][sizeof(int)], id_word[i]->word, id_word[i]->len) > 0) {
                    id = id_word[i]->id;
                    word_to_buffer(id_word[i], buffer_aux);
                    fwrite(buffer_aux, sizeof(char)*max_size_word, 1, fpNovo);
                    i++;
                }
                else {
                    id = *(int*)buffer[j];
                    fwrite(buffer[j], sizeof(char)*max_size_word, 1, fpNovo);
                    j++;
                }
                inicio[2*id] = sum;
                sum += max_size_word;
                inicio[2*id + 1] = sum;
                
                if(j == b) {
                    b = fread(buffer, sizeof(char)*max_size_word, 1000, fpWord);
                    j = 0;
                }
            }

            while(j<b) {
                inicio[2*(*(int*)buffer[j])] = sum;
                sum += max_size_word;
                inicio[2*(*(int*)buffer[j]) + 1] = sum;
                fwrite(buffer[j], sizeof(char)*max_size_word, 1, fpNovo);
                j++;
            }
            
            while(i<max_id) {
                inicio[2*id_word[i]->id] = sum;
                sum += max_size_word;
                inicio[2*id_word[i]->id + 1] = sum;
                word_to_buffer(id_word[i], buffer_aux);

                fwrite(buffer_aux, sizeof(char)*max_size_word, 1, fpNovo);
                i++;
            }
        }
    }

    fwrite(inicio, 2*max_id, sizeof(int), fpOffset);
    fclose(fpNovo);
	
    if(fpWord)
        fclose(fpWord);
    
    fclose(fpOffset);
    char comando[10000];
    remove(file_vocabulario);
    sprintf(comando, "mv auxiliar %s", file_vocabulario);
    system(comando);
    free(id_word);
    free(inicio);
}

void Vocabulario::carrega_arvore_cache(int pos, int inicio, int fim) {
    if(inicio > fim || pos >= LEVEL_BUSCA || pos >= max_id_disco)
        return;

    char buffer[max_size_word];
    int meio = (inicio+fim)/2;

    fseek(fpWord[0], meio * max_size_word, SEEK_SET);
    fread(buffer, max_size_word, sizeof(char), fpWord[0]);

    buffer_to_word(&arvore_cache[pos], buffer);

    arvore_cache[pos].ant = inicio;
    arvore_cache[pos].prox = fim;

    carrega_arvore_cache(pos*2+1, inicio, meio-1);
    carrega_arvore_cache(pos*2+2, meio+1, fim);
}

void Vocabulario::load() {
    TpWord *new_word;
    int i;
    unsigned int pos;
    char buffer[max_size_word];

    fseek(fpOffset[0], 0, SEEK_END);
    max_id_disco = ftell(fpOffset[0]) / (sizeof(int)*2);

    printf("max_id_disco %d\n",max_id_disco);

    if(max_id_disco == 0)
        return;

    max_id = max_id_disco;

    fseek(fpWord[0], 0, SEEK_SET);

    fseek(fpWord[0], 0, SEEK_SET);

    for(i = 0; i<max_id && (unsigned)i<size_cache; i++){
        fread(buffer, max_size_word, sizeof(char), fpWord[0]);

        new_word = &cache_word[i];
        buffer_to_word(new_word, buffer);

        pos = hashFunction(new_word->word, new_word->len) % size_principal;

        new_word->next = hash_principal[pos];
        hash_principal[pos] = new_word;

        new_word->ant = i-1;
        new_word->prox = i+1;
    }

    carrega_arvore_cache(0, 0, max_id_disco-1);

    ocupado_cache = i;
    inicio_cache = 0;
    fim_cache = ocupado_cache-1;
}

void Vocabulario::start_reader() {
    sem_wait(&sem_reader);
    countReader ++;	

    if(countReader == 1)
        sem_wait(&sem_writer);
    
    sem_post(&sem_reader);
}

void Vocabulario::stop_reader() {
    sem_wait(&sem_reader);
    countReader --;

    if(countReader == 0)
        sem_post(&sem_writer);

    sem_post(&sem_reader);
}

void Vocabulario::start_writer() {
	sem_wait(&sem_writer);
}

void Vocabulario::stop_writer() {
	sem_post(&sem_writer);
}

void Vocabulario::removeWordHash(TpWord *ptr) {
    unsigned int pos = hashFunction(ptr->word, ptr->len) % size_principal;

    if(ptr == hash_principal[pos])
        hash_principal[pos] = ptr->next;
    else {
        TpWord *aux = hash_principal[pos];

        while(aux->next != ptr)
            aux = aux->next;
        
        aux->next = ptr->next;
    }
    
    ptr->next = NULL;
}

inline void Vocabulario::atualizaFilaCache(TpWord *ptr) {
    sem_wait(&sem_cache);

    if(ptr && ocupado_cache > 1 && ptr->prox < ocupado_cache) {
        int pos = cache_word[ptr->prox].ant;

        if(ptr->ant < 0)
            inicio_cache = ptr->prox;
        else
            cache_word[cache_word[pos].ant].prox = cache_word[pos].prox;

        cache_word[cache_word[pos].prox].ant = cache_word[pos].ant;
        cache_word[fim_cache].prox = pos;
        
        ptr->ant = fim_cache;
        ptr->prox = ocupado_cache;
        fim_cache = pos;
    }

    sem_post(&sem_cache);
}

void Vocabulario::insereWordCache(char *buffer) {
    sem_wait(&sem_cache);

    removeWordHash(&cache_word[inicio_cache]);

    free(cache_word[inicio_cache].word);

    buffer_to_word(&cache_word[inicio_cache], buffer);

    unsigned int pos = hashFunction(cache_word[inicio_cache].word, cache_word[inicio_cache].len) % size_principal;
    cache_word[inicio_cache].next = hash_principal[pos];
    hash_principal[pos] = &cache_word[inicio_cache];

    TpWord *ptr = &cache_word[inicio_cache];

    pos = cache_word[ptr->prox].ant;

    if(ptr->ant < 0)
        inicio_cache = ptr->prox;
    else
        cache_word[cache_word[pos].ant].prox = cache_word[pos].prox;

    cache_word[cache_word[pos].prox].ant = cache_word[pos].ant;
    cache_word[fim_cache].prox = pos;

    ptr->ant = fim_cache;
    ptr->prox = ocupado_cache;
    fim_cache = pos;

    sem_post(&sem_cache);
}
