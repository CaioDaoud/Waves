#ifndef _PARSE_QUERY_H
#define  _PARSE_QUERY_H
///para usar o stemmer
//#ifndef _STEMMER
//#define  _STEMMER
//#endif
///
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vocabulario.h"
#include "valstringuniao_utf8.h"
#include "semaphore.h"
//#include "hash[cmd].h"
#ifdef _STEMMER
#include "PorterStemmer.h"
#endif
#define TAM_PALAVRA 30
#define TAM_TAB 10000

///----------caio--------- hash usado para gerar indice só com termos da consulta []	

typedef struct term_{
        char chave[50]; // A maior palavra do portuguÃªs tem 46 letras: "MoultramicroscopicossilicovulcanoconiÃ³tico"
        struct term_ *prox;
} TipoHash; 




	//int  tamanhoTabela = 10000;
	TipoHash *tabHash[TAM_TAB] ;

		
	int stringParaInt(char *string){
		int tamanho, primeira, segunda; //Inteiros que representam o tamanho,
										//o código da primeira letra da string
										//e o código da segunda letra.
		tamanho =  strlen(string);      //Mede o tamanho da string
		primeira = string[0];           //Obtém o código da primeira letra
		segunda = string[tamanho-1];            //Obtém o código da segunda letra
		for (int i = 1; i < tamanho ; i++)
		{
			segunda += string[i];
		}
		int resultado = (segunda * primeira) + tamanho; //Função de transformação
		return resultado;  //Retorna número que representa a string
    }
	
	int hash_(int valor) {
		return valor % TAM_TAB;  
    }
    
    
    void Inserir_Hash_cmd(TipoHash **Hash,  char *palavra)
	{
		TipoHash *novo, *aux=NULL;

		novo = (TipoHash*) malloc(sizeof(TipoHash));
		novo->prox = NULL;
		strcpy(novo->chave,palavra);

		aux = *Hash;

		if(*Hash == NULL) {
			*Hash = novo;  
		}
		  // O código, a partir desse else, é para tratar colisões.        
		else { 
			novo->prox = *Hash;
			*Hash = novo;			
		}       
	}
	
	
	bool Busca_Hash_cmd(TipoHash **Hash,  char *palavra)
	{
		
		TipoHash *aux=NULL;
		aux = *Hash;

		if(*Hash == NULL) {
		  return false;
		}	         
		else { // O código, a partir desse else, é para tratar colisões.
		 
			//Enquanto não acha uma palavra igual, ou não chega no fim do arquivo....
			while(aux != NULL && strcmp(aux->chave,palavra) != 0) {
			  
			   aux = aux->prox;
			}

			// Se estiver no fim do arquivo, INSERIR NO INICIO
			if(aux == NULL) {
				return false;
			} else{ //se encontrou a palavra
				//cout<< "==> " << aux->chave << " - "<< palavra<<endl;
				return true;
			}
		}       
	}
    
    
    void Imprimir_hash_cmd(TipoHash **Hash, int tamanho_hash) {
		 TipoHash *aux;
		 int i,conta_vazio = 0, conta_palavras = 0;
		 
		for(i=0; i<tamanho_hash; i++) {
			aux = Hash[i];
			
			if(Hash[i] == NULL) {  
			   //printf("Sem elementos");
			   conta_vazio++;
			}
			else {
			   while(aux!=NULL) {
					printf("%s - ",aux->chave);
					conta_palavras++;
					aux=aux->prox;
			   }
			   //cout<<endl;
			}   
		}
	 
		printf("\n\nPalavras inseridas no Hash: %d\nEspacos vazios no Hash: %d\n\n",conta_palavras, conta_vazio);
		printf("\n\n");    
	} 
    
	void Imprimir_hash_cmd(){
		Imprimir_hash_cmd(tabHash, TAM_TAB);
	
	}
///----------------------------------------------------------	




int parse_query(char *str_query, Vocabulario *voc, unsigned int *termos_ids)
{
#ifdef _STEMMER
    PorterStemmer pStemmer;
#endif
	unsigned int k, controle = 0;
	char pal[TAM_PALAVRA+1], charBase;
	unsigned char c;
	int ntermos_ids = 0;
	int id_termo;
	

	
	
	
	//imprimir_hash_cmd(tabHash, TAM_TAB);
	
	//exit(0);

	memset(pal,0,TAM_PALAVRA+1);
	k=0;
	pal[k] = '\0';

	while(*str_query)
	{
		if( *str_query == -61 )
		{
			controle = 1;
			str_query++;
		}

		if( controle )
		{
			controle = 0;
			c = ConverteBufferUTF8( *str_query );
			c = tolower(c);
		}
		else	c =  _ConvertSemAcentos[(unsigned char) *str_query];

		

		if(isalnum(c)) 
		{
			if(k < TAM_PALAVRA)	pal[k++] = (c);//acho que o _ConvertSemAcentos ja deixa minusculo. tem que testar
		}
		else if(k>0)
		{
			pal[k] = 0;
#ifdef _STEMMER
			pStemmer.stem(pal, k);
#endif

			//cout<<"@#$@#$@#$ == > "<< (char*)pal <<endl; 
			
			
			//if( Busca_Hash_cmd(&(tabHash[hash_(stringParaInt((char*)pal))]), (char*)pal)){	
				
				//cout<< " term OK"<< endl;		
				/** Tentem usar o insert para gerar o vocabulario em tempo de execucao*/
				//id_termo = voc->getIdNoCache((char*)pal, k, 0);
				id_termo = voc->insert((char*)pal, k, 0);

				/*verifica se encontrou a palavra no vocabulario*/
				if(id_termo>-1)	termos_ids[ntermos_ids++] = id_termo;
			//}
			
			k = 0;
		}
		str_query++;
	}
	
	if(k>0)
	{
		pal[k] = 0;
#ifdef _STEMMER
        pStemmer.stem(pal, k);
#endif



		//if( Busca_Hash_cmd(&(tabHash[hash_(stringParaInt((char*)pal))]), (char*)pal)){	
			//cout<< " term OK"<< endl;
			id_termo = voc->getIdNoCache((char*)pal, k, 0);

			/*verifica se encontrou a palavra no vocabulario*/
			if(id_termo>-1)	termos_ids[ntermos_ids++] = id_termo;
			
		//}	
	}

	return ntermos_ids;
}

#endif
