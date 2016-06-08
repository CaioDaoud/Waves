#include "../../libs-iw/indexer/include/DescritorInfoweb.h"
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

DescritorInfoweb::DescritorInfoweb( char *_path_base ) {
    char nome_idx[1024];
    strcpy( path_base, _path_base );
    sprintf( nome_idx, "%sidx", path_base );
    
    if( (fidx = fopen( nome_idx, "rb")) == NULL ){
        printf( "DescritorInfoweb::DescritorInfoweb -> Nao eh possivel abrir o arquivo de idx (%s)", nome_idx );
        exit(1);
    }

    furl = ftext = ftitle = fanchor = foutlink = NULL;
    id_text_atual = id_outlink_atual = id_title_atual = id_anchor_atual = id_url_atual = -1;
    fseek( fidx, 0, SEEK_END );
    num_docs = ftell(fidx)/sizeof(IDXbase);	
}

DescritorInfoweb::~DescritorInfoweb() {
    if(fidx)
        fclose(fidx);
    if(furl)
        fclose(furl);
    if(ftext)
        fclose(ftext);
    if(ftitle)
        fclose(ftitle);
    if(fanchor)
        fclose(fanchor);
    if(foutlink)
        fclose(foutlink);
}

unsigned int DescritorInfoweb::get_num_docs() {
    return num_docs;
}

int DescritorInfoweb::get_text( unsigned int doc, char *text ){
    if(doc > num_docs)
        return -1;

    IDXbase idx;
    char collection_name[4096];
    long offset = doc*sizeof(IDXbase);
    fseek( fidx, offset, SEEK_SET );
    fread( &idx, sizeof(IDXbase), 1, fidx );

    if(id_text_atual != idx.id_arq) {
        if(ftext)
            fclose( ftext );
        id_text_atual = idx.id_arq;
        sprintf( collection_name, "%st.%d", path_base, idx.id_arq );
        if( (ftext = fopen( collection_name, "r")) == NULL ){
            printf( "DescritorInfoweb::get_text -> Nao eh possivel abrir o arquivo de texto (%s) *** ", collection_name );
            exit(1);
            }
    }

    fseek(ftext, idx.offset_text, SEEK_SET);
    fread(text, sizeof(char), idx.size_text, ftext);
    text[idx.size_text] = 0;

    return idx.size_text;
}

int DescritorInfoweb::get_title( unsigned int doc, char *text ) {
    if(doc > num_docs)
        return -1;

    IDXbase idx;
    char collection_name[4096];
    long offset = doc*sizeof(IDXbase);
    fseek(fidx, offset, SEEK_SET);
    fread(&idx, sizeof(IDXbase), 1, fidx);

    if(id_title_atual != idx.id_arq) {
        if(ftitle)
            fclose(ftitle);
        id_title_atual = idx.id_arq;
	
        sprintf( collection_name, "%stitle%hu", path_base, idx.id_arq );
        if( (ftitle = fopen( collection_name, "r")) == NULL ){
            printf( "DescritorInfoweb::get_title -> Nao eh possivel abrir o arquivo de title (%s)", collection_name );
            exit(1);
        }
    }
    
    fseek(ftitle, idx.offset_title, SEEK_SET);
    fread(text, sizeof(char), idx.size_title, ftitle);
    text[idx.size_title] = 0;

    return idx.size_title;
}

int DescritorInfoweb::get_anchor( unsigned int doc, char *text ) {
    if(doc > num_docs)
        return -1;

    IDXbase idx;
    char collection_name[4096];
    long offset = doc*sizeof(IDXbase);
    fseek(fidx, offset, SEEK_SET);
    fread(&idx, sizeof(IDXbase), 1, fidx);
    idx.id_arq = 0;

    if(id_anchor_atual != idx.id_arq) {
        if(fanchor)
            fclose( fanchor );
        id_anchor_atual = idx.id_arq;
        
        sprintf( collection_name, "%sanchor%d", path_base, idx.id_arq );	
        if( (fanchor = fopen( collection_name, "r")) == NULL ){
            printf( "DescritorInfoweb::get_anchor -> Nao eh possivel abrir o arquivo de anchor (%s)", collection_name );
            exit(1);
        }
    }

    if(idx.size_anchor > 0) {
        fseek(fanchor, idx.offset_anchor, SEEK_SET);
        fread(text, sizeof(char), idx.size_anchor, fanchor);
    }
    text[idx.size_anchor] = 0;

    return idx.size_anchor;
}

int DescritorInfoweb::get_outlink( unsigned int doc, char *text ) {
    if(doc > num_docs)
        return -1;

    IDXbase idx;
    char collection_name[4096];
    long offset = doc*sizeof(IDXbase);
    fseek(fidx, offset, SEEK_SET);
    fread(&idx, sizeof(IDXbase), 1, fidx);

    if(id_anchor_atual != idx.id_arq) {
        if(foutlink)
            fclose( foutlink );
        id_outlink_atual = idx.id_arq;
        sprintf( collection_name, "%soutlink%d", path_base, idx.id_arq );	
        if( (foutlink = fopen( collection_name, "r")) == NULL ){
            printf( "DescritorInfoweb::get_anchor -> Nao eh possivel abrir o arquivo de anchor (%s)", collection_name );
            exit(1);
            }
    }

    fseek(foutlink, idx.offset_outlink, SEEK_SET);
    fread(text, sizeof(char),idx.size_outlink, foutlink);
    text[ idx.size_outlink ] = 0;

    return idx.size_outlink;
}

int DescritorInfoweb::get_url( unsigned int doc, char *text ) {
    if(doc > num_docs)
        return -1;

    IDXbase idx;
    char collection_name[1024];
    unsigned long offset = doc*sizeof(IDXbase);
    fseek( fidx, offset, SEEK_SET );
    fread( &idx, sizeof(IDXbase), 1, fidx );

    if(id_url_atual != idx.id_arq) {
        if(furl)	
            fclose( furl );
        id_url_atual = idx.id_arq;
        sprintf( collection_name, "%surl%d", path_base, idx.id_arq );
        
        if( (furl = fopen( collection_name, "r")) == NULL ){
            printf( "DescritorInfoweb::get_url -> Nao eh possivel abrir o arquivo de url (%s)", collection_name );
            exit(1);
        }
    }

    fseek( furl, idx.offset_url, SEEK_SET );
    fread( text, sizeof(char),idx.size_url, furl );
    text[ idx.size_url ] = 0;

    return idx.size_url;
}
