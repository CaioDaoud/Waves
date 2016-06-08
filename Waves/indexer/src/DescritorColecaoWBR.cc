#include "../../libs-iw/indexer/include/DescritorColecaoWBR.h"

//por enquanto manipular soh o texto; naum eh preciso mexer no resto

DescritorColecaoWBR::DescritorColecaoWBR(char *path, int mode){
	numArqText = numArqLink = numArqGeral = -1;
	strcpy(pathBase,path);
	indiceText = NULL;
	indiceLink = NULL;
	indiceGeral = NULL;	
	baseText = NULL;
	baseLink = NULL;
	baseGeral = NULL;
	arqPagerank = NULL;
	
	cout << "pathBase"<<pathBase << endl;
	sprintf(nomeIndiceText,"%s%s.idx",pathBase,TEXT);
	sprintf(nomeIndiceGeral,"%s%s.idx",pathBase,GERAL);
	sprintf(nomeIndiceLink,"%s%s.idx",pathBase,LINK);
	sprintf(nomePagerank,"%s%s",pathBase,"autglobal");
	switch(mode){
		case WRITE:
			if(!verificarExistenciaBase()){
				
				indiceText = fopen(nomeIndiceText,"w");
			}
			else {
				cout << "Colecao de saida jah existe!! abortando execucao" << endl;
				exit(1);
			}
			indiceLink = fopen(nomeIndiceLink,"a+");
			indiceGeral = fopen(nomeIndiceGeral,"a+");
			indiceText = fopen(nomeIndiceText,"a+");
			break;
		case READ:
			indiceText = fopen(nomeIndiceText,"r");
			indiceLink = fopen(nomeIndiceLink,"r");
			indiceGeral = fopen(nomeIndiceGeral,"r");			
	}
}

DescritorColecaoWBR::~DescritorColecaoWBR(){
	fclose(indiceText);
	fclose(indiceLink);
	fclose(indiceGeral);	
}

void DescritorColecaoWBR::abrirArquivoText(int num, char* modo){
	numArqText = num;
	sprintf(nomeBaseText,"%s%s%d",pathBase,TEXT,numArqText);
	baseText = fopen(nomeBaseText,modo);
}

void DescritorColecaoWBR::abrirArquivoLink(int num, char* modo){
	numArqLink = num;
	sprintf(nomeBaseLink,"%s%s%d",pathBase,LINK,numArqLink);
	baseLink = fopen(nomeBaseLink,modo);
}

void DescritorColecaoWBR::abrirArquivoGeral(int num, char* modo){
	numArqGeral = num;
	sprintf(nomeBaseGeral,"%s%s%d",pathBase,GERAL,numArqGeral);
	baseGeral = fopen(nomeBaseGeral,modo);
}

void DescritorColecaoWBR::fecharArquivoText(){
	if(baseText)	fclose(baseText);	
}
void DescritorColecaoWBR::fecharArquivoLink(){
	if(baseLink)	fclose(baseLink);
}
void DescritorColecaoWBR::fecharArquivoGeral(){
	if(baseGeral)	fclose(baseGeral);	
}

int DescritorColecaoWBR::tamanhoArquivo(char * nomeArquivo) {
	struct stat info;
	stat(nomeArquivo,&info);
	return info.st_size;
}

void DescritorColecaoWBR::lerDoc(int id, char* doc, int &tam){	
	int posAtual, proxPos;
	short arqAtual, proxArq;
	int offset = 0;
	
	fseek(indiceText,id*6,SEEK_SET);
	fread(&arqAtual,sizeof(short),1,indiceText);
	fread(&posAtual,sizeof(int),1,indiceText);
	
	if(arqAtual != numArqText){ //arquivo diferente do aberto
		fecharArquivoText();
		cout <<"teste"<< arqAtual << "  - " << numArqText<<endl;
		abrirArquivoText(arqAtual,"r");
	}			
	
	//se chegar no final do indice
	if(ftell(indiceText) == tamanhoArquivo(nomeIndiceText)){
		offset = tamanhoArquivo(nomeBaseText) - posAtual;	
	}	
	
	else {
		fread(&proxArq,sizeof(short),1,indiceText);
		if(arqAtual != proxArq){ //se for o final do arquivo de listas
			offset = tamanhoArquivo(nomeBaseText) - posAtual;		
		}
		else {
			fread(&proxPos,sizeof(int),1,indiceText);
			offset = proxPos - posAtual;
		}
	}
	//ler o documento
	fseek(baseText,posAtual,SEEK_SET);
	tam = fread(doc,sizeof(char),offset,baseText);
	doc[offset] = '\0';
}


void DescritorColecaoWBR::gravarDoc(char *doc){
	int tamDoc = strlen(doc);
	int posBaseText = ftell(baseText);
	
	if(posBaseText+tamDoc > TAMMAXARQUIVO){
		fecharArquivoText();
		numArqText++;
		abrirArquivoText(numArqText,"a+");
	}
	fwrite(&numArqText,sizeof(short),1,indiceText);
	fwrite(&posBaseText,sizeof(int),1,indiceText);
	fwrite(doc,sizeof(char),tamDoc,baseText);
	fputc('\0',baseText);
}

int DescritorColecaoWBR::verificarExistenciaBase(){
	int retorno;
	abrirArquivoText(0,"r");
	if(baseText){
		retorno = 1;
		fecharArquivoText();
	}
	else retorno = 0;
	return retorno;
}
