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
#ifdef _STEMMER
#include "PorterStemmer.h"
#endif
#define TAM_PALAVRA 30



	




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
		
			/** Tentem usar o insert para gerar o vocabulario em tempo de execucao*/
			//id_termo = voc->getIdNoCache((char*)pal, k, 0);
			id_termo = voc->insert((char*)pal, k, 0);

			/*verifica se encontrou a palavra no vocabulario*/
			if(id_termo>-1)	termos_ids[ntermos_ids++] = id_termo;
			
			
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
		
		id_termo = voc->getIdNoCache((char*)pal, k, 0);

		/*verifica se encontrou a palavra no vocabulario*/
		if(id_termo>-1)	termos_ids[ntermos_ids++] = id_termo;
			
	
	}

	return ntermos_ids;
}

#endif
