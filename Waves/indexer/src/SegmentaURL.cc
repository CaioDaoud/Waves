#include "../../libs-iw/indexer/include/SegmentaURL.h"

//SegmentaURL::SegmentaURL()
//{
//	StartConvertSemAcentos();
//}

void SegmentaURL::ProximoTermo( char *linha, char *termo_atual, int *ini, int tam_termo )
{
	termo_atual[0] = 0;
	int cont_char = 0;
	int i = (*ini), j = 0;
	while( !(isalnum(linha[i])) ){
		i++;
	}
	//i++;
	j=0;
	while( (isalnum(linha[i])) && (cont_char < tam_termo) ){
		termo_atual[j] = linha[i];
		j++;
		i++;
		cont_char++;
	}
	termo_atual[j] = 0;
	
	if( cont_char == tam_termo ){
		while( isalnum(linha[i]) ){
			i++;
		}
	}
	
	(*ini) = i;
}

// -> /mnt/hd0/ufam/parser/indexer/src/run
// -> /mnt/hd0/ufam/parser/indexer/build/src
// roda -> ./geraParcial /mnt/hd_host10/ufam/nova_base/infoweb. /mnt/hd0/ufam/novo_indice/url/infoweb. 3

void SegmentaURL::removePath( char *url )
{
	int i = 0; //pra pular o http://

	if(!strncmp(url, "http://", 7))	i = 7;
	
	if( strlen(url) < 8 ) { return; }
	
	while( url[i] && url[i] != '/' ){
		i++;
	}
	url[i] = 0;
}

void SegmentaURL::geraSegmentos( char *url, char *source, char *segmentos)
{
	//char parsed_url[1024];
	//char parsed_source[1024];
	//char url[1024];
	char termo[33];
	int len = 0, pos = 0;
	
	//ConverteBufferSemAcentos( url, &url[strlen(url)], parsed_url );
	//ConverteBufferSemAcentos( source, &source[strlen(source)], parsed_source );
	segmentos[0] = 0;
	
	//strcpy( url, url_in );
//	removePath( url );
	
	len = strlen( source );
	pos = 0;
	while( pos < len ){
		
		ProximoTermo( source, termo, &pos, 32 ); // 32 é o tamanho maximo do termo
		
		if( strcmp(termo,"www") && strcmp(termo, "http") && strcmp(termo,"com") && strcmp(termo,"br") ) {
			
			if( strstr( url, termo ) ) {
				strcat( segmentos, termo );
				strcat( segmentos, " " );
			}
			
		}
	}	
	
}
