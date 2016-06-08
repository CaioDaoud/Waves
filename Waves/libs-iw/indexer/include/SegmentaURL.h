#ifndef _SEGMENTA_URL_
#define _SEGMENTA_URL_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "valstringuniao_utf8.h"

class SegmentaURL{
	public:
	
	char path_base[1024];
	
	//SegmentaURL();
	
	void ProximoTermo( char *linha, char *termo_atual, int *ini, int tam_termo );
	void removePath( char *url );
	void geraSegmentos( char *url, char *source, char *segmentos);
};



#endif
