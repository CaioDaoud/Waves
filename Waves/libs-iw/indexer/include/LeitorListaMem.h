#ifndef LEITOR_LISTA_MEM_H
#define LEITOR_LISTA_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IndiceInvertido.h"
#include "prefixinmem.h"

class LeitorListaMem
{
	public:
	char *plista;
	_tipo_buffer *out, *ini;
	unsigned char btat;
	int doc_gap;
	unsigned int *buffer_skip_docid;
	long *buffer_skip_offset;
	unsigned char *buffer_skip_bitat;
	float *buffer_skip_max_score;
	int pos_skip;
	int tam_skip;
	long tam_lista;
	int lastDoc;
	int pos_lista;
	int salto_skip;

	int total_descomprimido;
	int total_bloco_lido;

	LeitorListaMem()
	{	
		total_descomprimido = 0;
		total_bloco_lido = 0;
	}

	~LeitorListaMem()
	{
		
		//cout << "CAIO_TotalDescomprimido " << total_descomprimido << endl;
		//cout << "TotalLido " << total_bloco_lido << endl;
	}

	void InicializaSkip(char *_plista, long _tam_lista, int _lastDoc, long _inicio_lista, char *pskip, long _tam_skip, long _inicio_skip)
	{
	  Inicializa(_plista, _tam_lista, _lastDoc, _inicio_lista);

		/*Le o vetor de docid, offset e bitat*/
		buffer_skip_docid = (unsigned int*) (pskip + _inicio_skip);
		buffer_skip_offset = (long*) (pskip + _inicio_skip + sizeof(unsigned int)*_tam_skip);
		buffer_skip_bitat = (unsigned char*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long))*_tam_skip);
		buffer_skip_max_score = (float*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long) + sizeof(unsigned char))*_tam_skip);

		pos_skip = 0;
		tam_skip = _tam_skip;
		salto_skip = SALTO_SKIPLIST;
	}
	
	
	void InicializaSkipTemp()
	{
		pos_skip = 0;
		out = ini;
		btat = 0;
		doc_gap = 0;
		pos_lista = 0;
		
	}
	  

	

	void Inicializa(char *_plista, long _tam_lista, int _lastDoc, long _inicio_lista)
	{
		plista = _plista;
		out = (unsigned int*) (plista + _inicio_lista);
		ini = out;
		btat = 0;
		tam_lista = _tam_lista;
		lastDoc = _lastDoc;
		doc_gap = 0;
		pos_lista = 0;
	}

	/*
		Le uma lista invertida
	*/
	int getDocXX(unsigned int *doc, unsigned int *info)
	{
	  // printf("getdoc listamen\n");
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		pos_lista ++;
		pos_skip = (pos_lista-1)/salto_skip;

		doc_gap +=  RawDeltaDecode(out, btat) - 1;
		*info =  RawDeltaDecode(out, btat);

		*doc = doc_gap;

		return 1;
	}




	/*
		Le uma lista invertida
	*/
	int getDoc3(unsigned int *doc, unsigned int *info)
	{
	  // printf("getdoc listamen\n");
		/*Verifica se a lista terminou*/
		if(doc_gap >= lastDoc)	return 0;

		//		pos_lista ++;
		//pos_skip = (pos_lista-1)/salto_skip;

		// doc_gap +=  RawDeltaDecode(out, btat) - 1;
		// indice com correcao de compressao sem -1
		doc_gap +=  RawDeltaDecode(out, btat);
		*info =  RawDeltaDecode(out, btat);

		*doc = doc_gap;

		return 1;
	}


	/*
		Le uma lista invertida
	*/
	int getDoc(unsigned int *doc, unsigned int *info)
	{
	  // printf("getdoc listamen\n");
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		pos_lista ++;
		pos_skip = (pos_lista-1)/salto_skip;

		doc_gap +=  RawDeltaDecode(out, btat) - 1;
		*info =  RawDeltaDecode(out, btat);

		*doc = doc_gap;

		return 1;
	}




	/*
		Le a lista invertida. Buscando o primeiro docid >= doc_skip
	*/
	inline int getDocXX(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{
	  // printf("get doc 2 listamen\n");
		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(pos_lista < tam_lista)
		{
			pos_lista ++;

			doc_gap += RawDeltaDecode(out, btat) -1;
			*info = RawDeltaDecode(out, btat);

			*doc = doc_gap;

			total_descomprimido++;

			if(*doc >= doc_skip) {
				pos_skip = (pos_lista-1)/salto_skip;
				return 1;
			}
		}

		return 0;
	}




	/*
	  Le a lista invertida. Buscando o primeiro docid >= doc_skip
	*/
	inline int getDoc3(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{
	  // printf("get doc 2 listamen\n");
	  /*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
	  while(doc_gap < lastDoc)
	    {
	      //	      pos_lista ++;
	      // correcao da compressao sem -1
	      //doc_gap += RawDeltaDecode(out, btat) -1;
	      doc_gap +=  RawDeltaDecode(out, btat) ;
	      *info = RawDeltaDecode(out, btat);
	      
	      *doc = doc_gap;
	      
	      total_descomprimido++;
	      
	      if(*doc >= doc_skip) {
		//pos_skip = (pos_lista-1)/salto_skip;
		return 1;
	      }
	    }
	  
	  return 0;
	}










	/*
		Le a lista invertida aplicando saltos com a skiplist. Busca o primeiro docid >= doc_skip
	*/
	inline int getSkipDoc(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{	
	  // printf("skipdoc lista men\n");
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		/*Calcula entre qual posicao da skiplist a lista invertida esta*/
//		(pos_lista > 0) ? (pos_skip = (pos_lista-1)/salto_skip) : (pos_skip = 0);

		/*Checa se o doc_skip eh menor que o proximo salto da skip*/
		if(tam_skip == 0/* || doc_skip < buffer_skip_docid[pos_skip]*/)	return getDocXX(doc, info, doc_skip);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
		pos_skip --;

		/*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
		if(pos_skip < 0 || doc_gap >= buffer_skip_docid[pos_skip])	return getDocXX(doc, info, doc_skip);

		/*Salta no proprio bloco que esta em memoria*/
		out = (unsigned int*) (plista + buffer_skip_offset[pos_skip]);

		btat = buffer_skip_bitat[pos_skip];

		/*Calcula qual a nova posicao na lista*/
		pos_lista = (pos_skip+1) * (salto_skip) + 1;

		/*Decodifica o codigo do docId da skiplist*/
		RawDeltaDecode(out, btat) - 1;
		/*Decodifica o TF*/
		*info = RawDeltaDecode(out, btat);

		/*Redefine o doc_gap com o docId da skip*/
		doc_gap = buffer_skip_docid[pos_skip];

		*doc = doc_gap;

		if(*doc >= doc_skip)	return 1;

		/*Procura pelo primeiro docId >= doc_skip*/
		return getDocXX(doc, info, doc_skip);
	}


	/*
		Le a lista invertida aplicando saltos com a skiplist. Busca o primeiro docid >= doc_skip
	*/
	inline int getSkipDoc2(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{	

	  // printf("skip doc 2 lista men\n");
		/*Verifica se a lista terminou*/
		if(pos_lista == tam_lista)	return 0;

		/*Checa se o doc_skip eh menor que o proximo salto da skip*/
		if(tam_skip > 0) {

			/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
			while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
			pos_skip --;

			// -- printf("Primeiro documento do bloco %d \n",buffer_skip_docid[pos_skip]);
			/*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
			if(pos_skip >= 0 && doc_gap < buffer_skip_docid[pos_skip]) {

				/*Salta com o cursor sobre a lista*/
				out = (unsigned int*) (plista + buffer_skip_offset[pos_skip]);

				btat = buffer_skip_bitat[pos_skip];

				/*Calcula qual a nova posicao na lista*/
				pos_lista = (pos_skip+1) * (salto_skip) + 1;

				/*Decodifica o codigo do docId da skiplist*/
				/**doc_gap = */RawDeltaDecode(out, btat) - 1;
				
				/*Decodifica o TF*/
				*info = RawDeltaDecode(out, btat);

				/*Redefine o doc_gap com o docId da skip*/
				doc_gap /**+*/= buffer_skip_docid[pos_skip];

				*doc = doc_gap;

				if(*doc >= doc_skip)	return 1;
			}
		}

		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		while(pos_lista < tam_lista)
		{
			pos_lista ++;

			doc_gap += RawDeltaDecode(out, btat) -1;
			*info = RawDeltaDecode(out, btat);

			*doc = doc_gap;

			total_descomprimido++;

			if(*doc >= doc_skip) {
				pos_skip = (pos_lista-1)/salto_skip;
				return 1;
			}
		}

		return 0;
	}
	
	

	/*****************************
		Le a lista invertida aplicando saltos com a skiplist. Busca o primeiro docid >= doc_skip
	*******************************/
	inline int getSkipDoc3(unsigned int *doc, unsigned int *info, unsigned int doc_skip)
	{	
	  unsigned int linfo;
	  
	  
	  /*Verifica se a lista terminou*/
	  if(doc_gap >= lastDoc) {
	    // printf("FALSO: skipdoc: %d %d %d (skip para %d)\n",doc_gap, lastDoc,tam_lista,doc_skip); 
	    return 0;
	  }	  
	  /*Checa se o doc_skip eh menor que o proximo salto da skip*/
	  // if(tam_skip > 0) {
	  
	  /*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
	  while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
	  pos_skip--;
	  // -- printf("Primeiro documento do bloco %d \n",buffer_skip_docid[pos_skip]);
	  /*Checa se o doc_gap atual eh maior que o docid do salto (Evita redescomprimir)*/
	  if((pos_skip >=0) && (doc_gap < buffer_skip_docid[pos_skip])) {
	    
	    /*Salta com o cursor sobre a lista*/
	    out = (unsigned int*) (plista + buffer_skip_offset[pos_skip]);
	    
	    btat = buffer_skip_bitat[pos_skip];
	    
	    /*Calcula qual a nova posicao na lista*/
	    //pos_lista = (pos_skip+1) * (salto_skip) + 1;
	    
	    /*Decodifica o codigo do docId da skiplist*/
	    /**doc_gap = */RawDeltaDecode(out, btat);
	    
	    /*Decodifica o TF*/
	    linfo = RawDeltaDecode(out, btat);
	    
	    /*Redefine o doc_gap com o docId da skip*/
	      doc_gap /**+*/= buffer_skip_docid[pos_skip];
	      
	      
	      if(doc_gap >= doc_skip)	{
		*info = linfo;
		*doc = doc_gap;
		//	printf("VERD: skipdoc: %d %d %d - pos_skip %d\n",doc_gap, lastDoc,tam_lista,pos_skip);	
		return 1;
	      }
	  } else {
	    if(pos_skip <0) pos_skip=0;
	  }
	  // }
	  
	  /*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
	  while(doc_gap < lastDoc)
	    {
	      //pos_lista ++;
	      
	      // correcao da compressao -1 abaixo. 
	      doc_gap += RawDeltaDecode(out, btat);  // -1;
	      linfo = RawDeltaDecode(out, btat);
	      
	      // *doc = doc_gap;
	      
	      total_descomprimido++;
	      
	      if(doc_gap >= doc_skip) {
		*info = linfo;
		*doc = doc_gap;
		//pos_skip = (pos_lista-1)/salto_skip;
		//	printf("VERD: skipdoc: %d %d %d - pos_skip %d\n",doc_gap, lastDoc,tam_lista,pos_skip);	
		return 1;
	      }
	    }
	  // printf("FALSO: skipdoc: %d %d %d (skip para %d) pos_skip %d\n",doc_gap, lastDoc,tam_lista,doc_skip,pos_skip);
	  *info = linfo;
	  *doc = doc_gap; 
	  return 0;
         
	}
	
	




	/*
		Move o ponteiro da skiplist para o bloco com o docid>=doc_skip sem descomprimir nada
	*/
	inline void nextShallow(unsigned int doc_skip)
	{	
		/*Calcula entre qual posicao da skiplist a lista invertida esta*/
//		(pos_lista > 0) ? (pos_skip = (pos_lista-1)/salto_skip) : (pos_skip = 0);

		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while(pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)   pos_skip++;
	}
	
	///Para em cada posição da skipList	[caio]
	void nextShallow_Todos()
	{
		while(pos_skip < tam_skip) {  pos_skip++;}
		
	}

	/*
		Pega o MaxDocId do bloco
	*/
	unsigned int getDocBlockBoundary()
	{
	  if(tam_skip == 0)	return doc_gap+1;
	  
	  if(pos_skip >= tam_skip && buffer_skip_docid[tam_skip-1] < doc_gap)	return doc_gap+1;

	  if(pos_skip >= tam_skip)		return buffer_skip_docid[tam_skip-1];
	  
	  return buffer_skip_docid[pos_skip];		
	}
	
	/*
		Pega o MaxScore do bloco
	*/
	float getBlockMaxScore()
	{
	  if(tam_skip == 0 || pos_skip == tam_skip)	return 0;
	  
	  return buffer_skip_max_score[pos_skip];
	}
	
	float next_todos()
	{
	  while(pos_skip < tam_skip) {  pos_skip++; break;}
//cout<<"tam_skip : " << tam_skip << "pos_skip : "<< pos_skip <<endl;
		return 0.0;
	}
};

#endif
