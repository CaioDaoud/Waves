#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../libs-iw/indexer/include/IndiceInvertido.h"

int main( int argc, char **argv )
{
	if(argc != 4)
	{
		printf("Execute\n\t%s <path_indice> <num_doc_colecao> <1-bm25, 2-vetorial>\n", argv[0]);
		exit(0);
	}

	FILE *fidx, *fidf;
	IDX termo;
	float idf;

	float num_docs_colecao = (float) atof( argv[2] );
	int ptipo = (int)atof(argv[3]);
	
	char filename[4096];

	sprintf(filename, "%s%s", argv[1], NOME_IDX);
	fidx = fopen( filename, "rb" );

	if(fidx == NULL)
	{
		printf("Nao foi possivel abrir o arquivo de IDX %s\n", filename);
		exit(0);
	}

	sprintf(filename, "%s%s", argv[1], NOME_IDF);

	if((fidf = fopen( filename, "wb" )) == NULL)
	{
		printf("Nao foi possivel criar o arquivo de idf %s\n", filename);
		exit(0);
	}

	fread( &termo, sizeof(IDX), 1, fidx );
	int term_id=0;
	do{
		idf = 0.0;
		if( termo.freq_tam_lista > 0 ){
//			printf("num_docs_colecao :%f - ((float)termo.freq_tam_lista): %f\n", num_docs_colecao, idf);
			if(ptipo == 2)
				idf = log( (num_docs_colecao/((float)termo.freq_tam_lista)));
			else
				idf = log(1.0 + (num_docs_colecao-(float)termo.freq_tam_lista + 0.5)/((float)termo.freq_tam_lista + 0.5))/log(2.0);
		}

		fwrite( &idf, sizeof(float), 1, fidf );
		fread( &termo, sizeof(IDX), 1, fidx );
	//	printf("%d %f\n", term_id, idf);
		term_id++;

	} while( !(feof(fidx)) );
	printf("%d Termos\n", term_id);
	fclose(fidx);
	fclose(fidf);

	return 0;
}
