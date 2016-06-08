#ifndef _DESCRITORINFOWEB_
#define _DESCRITORINFOWEB_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id_arq;

    long  offset_text;
    int   size_text;

    long  offset_title;
    int   size_title;

    long  offset_anchor;
    int   size_anchor;

    long  offset_url;
    int   size_url;

    long offset_outlink;
    int size_outlink;
}IDXbase;

class DescritorInfoweb {
public:
    FILE *fidx;
    FILE *furl;
    FILE *ftext;
    FILE *ftitle;
    FILE *fanchor;
    FILE *foutlink;

    char path_base[4096];
    char id_text_atual;
    char id_title_atual;
    char id_anchor_atual;
    char id_url_atual;
    char id_outlink_atual;
    unsigned int num_docs;

    DescritorInfoweb( char *_path_base );
    ~DescritorInfoweb();

    int get_url( unsigned int doc, char *text );
    int get_text( unsigned int doc, char *text );
    int get_title( unsigned int doc, char *text );
    int get_anchor( unsigned int doc, char *text );
    int get_outlink( unsigned int doc, char *text );
    unsigned int get_num_docs();
    
};

#endif
