#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>

using namespace std;

#define TEXT "text"
#define LINK "linkcontenttext"
#define GERAL "geral"
#define READ 1
#define WRITE 2
#define TAMMAXARQUIVO 2000000000

class DescritorColecaoWBR {

public:
	char pathBase[200];
	char nomeIndiceText[200], nomeIndiceGeral[200], nomeIndiceLink[200];
	char nomeBaseText[200], nomeBaseGeral[200], nomeBaseLink[200];
	char nomePagerank[200];
	double *pagerank;
	int numArqText, numArqLink, numArqGeral;
	int qteArqText, qteArqLink, qteArqGeral;
	FILE *indiceText, *indiceGeral, *indiceLink;
	FILE *baseText, *baseLink, *baseGeral;
	FILE *arqPagerank;
		
	DescritorColecaoWBR(char *path, int mode);
	~DescritorColecaoWBR();
	void abrirArquivoText(int num, char* modo);
	void abrirArquivoLink(int num, char* modo);
	void abrirArquivoGeral(int num, char* modo);
	void fecharArquivoText();
	void fecharArquivoLink();
	void fecharArquivoGeral();

	void lerDoc(int id, char* doc, int &tam);
	void lerLinkContent(int id, char* doc, int &tam);
	void lerTitulo(int id, char* doc, int &tam);
	void lerURL(int id, char* doc, int &tam);

	void gravarDoc(char *doc);
	int tamanhoArquivo(char * nomeArquivo);
	int verificarExistenciaBase();
};
