#include <stdio.h>

#include <stdlib.h>

#define NUM 100000

#define BLOCO_TAMANHO 1000
#define NUMERO 50 //Maximo numero de letras em uma palavra
#define MAX_COLUNAS 10  // Numero maximo de letra em uma palavra que iremos adotar obs:(pode ser menor que qntd de letras de uma palavra)

unsigned int tabHashAleatorio[256][MAX_COLUNAS]; //Tabela de cada letra em cada posição da palavra

int tabela_letras[256];

typedef struct no{
        unsigned char chave[50]; // A maior palavra do portuguÃªs tem 46 letras: "MoultramicroscopicossilicovulcanoconiÃ³tico"
        struct no *prox;
} TipoNo;   
  
  
void GeraTabela(){
     int j,k;
     unsigned int temp;
     
     srand(time(NULL)+543532);
     
     for(k=0;k<256;k++) {
          for(j=0;j<MAX_COLUNAS;j++){
               tabHashAleatorio[k][j] = 1 + rand();
          }
     }
}

void Inserir_Hash(TipoNo **Hash, unsigned char *palavra)
{
	
     TipoNo *novo, *aux=NULL;
     
     novo = (TipoNo*) malloc(sizeof(TipoNo));
     novo->prox = NULL;
     strcpy(novo->chave,palavra);
     
     aux = *Hash;
     
     if(*Hash == NULL) {
          *Hash = novo;  
          //printf("palavra: %s inserida no inicio (Hash era NULL)\n", palavra);
     }
     
      // O código, a partir desse else, é para tratar colisões.        
     else { 
     
          //Enquanto não acha uma palavra igual, ou não chega no fim do arquivo....
          while(aux != NULL && strcmp(aux->chave,palavra) != 0) {
               aux = aux->prox;
          }
     
          // Se estiver no fim do arquivo, INSERIR NO INICIO
          if(aux == NULL) {
               novo->prox = *Hash;
               *Hash = novo;
          //   printf("palavra: %s (conflito, mas inserido)\n", palavra);
          } 
     }       
}

unsigned int Pos_Hash(unsigned char *palavra, int tamanho_hash) {
     int tam = strlen((const char*)palavra);
     int i;
     unsigned int pos;
     if (tam < MAX_COLUNAS) {
          for(i=0;i<tam;i++) {
               pos += tabHashAleatorio[palavra[i]][i];
          }
     }
     else {
          for(i=0;i<tam;i++) {
               pos += tabHashAleatorio[palavra[i]][i % MAX_COLUNAS];
          }
     }
    
     return pos % tamanho_hash;
}

void imprimir_hash(TipoNo **Hash, int tamanho_hash, FILE *arquivo) {
     TipoNo *aux;
     char saida[5000];
     char saida2[5000];
     char number[33];
     int i,conta_vazio = 0, conta_palavras = 0;
     
     for(i=0; i<tamanho_hash; i++) {
          aux = Hash[i];
          printf("\nTipoHash[%d]-> ",i);
          if(Hash[i] == NULL) {  
               printf("Sem elementos");
               conta_vazio++;
          }
          else {
               while(aux!=NULL) {
				printf("%s  ",aux->chave);
                    strcat(saida,aux->chave);
                    strcat(saida,"\n");
                    conta_palavras++;
                    aux=aux->prox;
               }
          }   
     }
 
 	sprintf(number,"%d\n", conta_palavras);
 	strcat(saida2, number);
 	strcat(saida2, saida);  
     fprintf(arquivo, saida2);
     printf("\n\nPalavras inseridas no Hash: %d\nEspacos vazios no Hash: %d\n\n",conta_palavras, conta_vazio);
	printf("\n\n");    
}     

void parse_Hash(FILE *arquivo, FILE *arq) {

     unsigned char texto[BLOCO_TAMANHO],palavra[NUMERO];
     long tamanho_arquivo;
     int i,j,tamanho_texto,busca_char_nulo=0;
     int tamanho_hash, tabela_letras[256];
	int cont = 0;


     //Calculando o numero de palavras existentes no arquivo
     //Obs: No. de Palavras = Tamanho da tabela HASH
     fseek(arquivo,0,SEEK_END);
     tamanho_arquivo = ftell(arquivo);
     tamanho_hash = (int) pow((float)tamanho_arquivo,0.7);
     tamanho_hash = tamanho_hash * 4;
     
     //Criando a Tabela Hash
     TipoNo *hash[tamanho_hash];
     for (i=0;i<tamanho_hash;i++) {
          hash[i] = NULL;
     }
     
     //Gerando a Tabela de valores de cada letra em cada posição da palavra
     GeraTabela();

     
     //Buscando palavras do texto para inserir no Hash
     fseek(arquivo,0,SEEK_SET);
     
     //Criando uma tabela de Letras
     for (i=0;i<256;i++) {
         tabela_letras[i] = 0;
     }
     for (i=97;i<123;i++) {
         tabela_letras[i] = 1;
     }

     //Buscando palavras do texto para inserir no Hash
     fseek(arquivo,0,SEEK_SET);
	
     
     //Lendo palavras de um arquivo... O algoritmo lê em memória parte por parte do arquivo, utilizando blocos...
	while (!feof(arquivo)) {
		memset(texto,'\0', BLOCO_TAMANHO);
		tamanho_texto = fread(texto,sizeof(char),BLOCO_TAMANHO,arquivo);
		busca_char_nulo = 0;	

          //Quando, ao ler o BLOCO_TAMANHO, chegar no fim do arquivo, não precisa utilizar todo o vetor... então, segue:
          if (feof(arquivo)) {
               break;
          }

		//Colocar as palavras em letras minusculas!
		for (i=0;i<strlen((const char*)texto);i++){
			texto[i]=tolower(texto[i]);
		}

		//Para não pegar pedaços de palavras e/ou palavra incompletas...	
		while (( tabela_letras [ texto [ tamanho_texto - busca_char_nulo -1]])) {
			busca_char_nulo++;
		}

		texto[tamanho_texto - busca_char_nulo] = '\0';
		fseek(arquivo,-(busca_char_nulo * sizeof(char)), SEEK_CUR);

		tamanho_texto = tamanho_texto - busca_char_nulo;
		i=0;

		while (i< (tamanho_texto)) {
			memset(palavra,'\0',NUMERO);
              	j=0;
              	while ((tabela_letras[texto[i]]) && (i<tamanho_texto)) {
				palavra[j] = texto[i];
                   	j++;
                   	i++;     
              	}
			i++;
      		if (strlen((const char*)palavra)>1) { //Palavra com um caracter não entra no hash
      			cont++;
				Inserir_Hash(&(hash[Pos_Hash(palavra, tamanho_hash)]), palavra);
			}
    	     }  
	}     
	

     //Lendo a última parte do texto
     texto[tamanho_texto] = '\0';
     
	
	for (i=0;i<strlen((const char*)texto);i++){
		texto[i]=tolower(texto[i]);
	}
	
	i=0;
	while (i< (tamanho_texto)) {
		memset(palavra,'\0',NUMERO);
         	j=0;
         	while ((tabela_letras[texto[i]]) && (i<tamanho_texto)) {
			palavra[j] = texto[i];
              	j++;
              	i++;     
         	}
		i++;
		if (strlen((const char*)palavra)>1) {
			cont++;
			Inserir_Hash(&(hash[Pos_Hash(palavra, tamanho_hash)]), palavra);
		}
	}     
	
	imprimir_hash(hash, tamanho_hash, arq);
	
	printf("palavras: %d\n", cont);
}



void editaTitle(FILE *arquivo_leitura, FILE *arquivo_escrita) {

	if (!arquivo_leitura) {
		printf("Erro na abertura do arquivo!");
		exit(1);
	}
	if (!arquivo_escrita) {
		printf("Erro na abertura do arquivo!");
		exit(1);
	}	
	
	parse_Hash(arquivo_leitura, arquivo_escrita);


	printf("Consultas editadas com Sucesso!\n\n");
	
}


int main(int argc, char **argv) {
	FILE *topicos;
	FILE *title;
	FILE *saida;
	
	if (argc != 3) {
		printf("\n\n\tExecute %s  <Arquivo Entrada (Topicos)>  <Arquivo Saida>\n\n", argv[0]);
		exit(1);
	}
	
	int i=0,j=0,y=0;
	int cont = 0;
	
	char c[NUM];
	char escrita[NUM];
	

	topicos = fopen(argv[1], "r+");
	title = fopen("title", "w+");
	saida = fopen(argv[2], "w+");

	if (!topicos) {
		printf("Erro na abertura do arquivo\n\n");
		exit(1);
	}

	if (!title) {
		printf("Erro na abertura do arquivo\n\n");
		exit(1);
	}	
	
	fread(c, sizeof(char), NUM, topicos);
	
	for (i=0;i<NUM;i++) {
		escrita[i] = '\0';
	}
	
	i = 0;
	
	
	while (i<NUM) {
	
		
	
		if (feof(topicos)) {
//			printf("Fim do arquivo!\n");
//			break;
		}
		
/*		if (i > (NUM - 200)) {
			fseek(topicos, (NUM - i) * sizeof(char), SEEK_CUR);
			fread(c, sizeof(char), NUM, topicos);
			i = 0;
		}
*/		
		
		j = 0;		
		if ((c[i] == '<') && (c[i+1] == 't') && (c[i+2] == 'i')&& (c[i+3] == 't')&& (c[i+4] == 'l')&& (c[i+5] == 'e')&& (c[i+6] == '>')) {
			for (y=0;y<NUM;y++) {
				escrita[y] = '\0';
			}
			
			i = i+8;
			cont++;
			while (escrita[j] != '<') {
				escrita[j] = c[i];
//				printf("%c", escrita[j]);
//				getchar();
				
				if (escrita[j] == ' ') {
					escrita[j] = '\n';
					//if (escrita[j+1] == '\n') {
					//	escrita[j+1] = '\0';
					//}
				}
				if (escrita[j] == '?') {
					escrita[j] = '\n';
					if (escrita[j+1] == '\n') {
						escrita[j+1] = '\0';
					} else escrita[j+1] = '\n';
				}
				if (escrita[j] == '?') {
					escrita[j] = '\0';
				}

				if (escrita[j] == '<') break;

				j++;
				i++;
				
			}
			if (escrita[j] != '\n') {
				escrita[j] = '\n';
				escrita[j+1] = '\0';
			} else escrita[j] = '\0';
			fprintf(title, "%s", escrita);
		}
 
		
		i++;
	}

	fclose(topicos);
	fclose(title);
	title = fopen("title", "r+");
	editaTitle(title, saida);
	fclose(saida);	
	fclose(title);
	
	return 0;
}
