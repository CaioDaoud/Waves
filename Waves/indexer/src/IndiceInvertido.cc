#include "../../libs-iw/indexer/include/IndiceInvertido.h"

IndiceInvertido::IndiceInvertido(char *_path_indice, int _flag_operacao)
{
	indice_invertido = (NO_LISTA_INV**)malloc( sizeof(NO_LISTA_INV*)*TAM_INDICE_INVERTIDO );

	for(int i = 0; i < TAM_INDICE_INVERTIDO; i++ )
		indice_invertido[i] = (NO_LISTA_INV*)NULL;

	InicializaContadores();

	path_indice = strdup(_path_indice);

	flag_operacao = _flag_operacao;

	if(_flag_operacao == FLAG_MERGE)
		buffer_parcial = (_tipo_buffer*)malloc(sizeof(_tipo_buffer)*TAM_BUFFER_LISTA);
	else
		buffer = (_tipo_buffer*)malloc( sizeof(_tipo_buffer)*TAM_BUFFER );
}

IndiceInvertido::~IndiceInvertido()
{
	if(flag_operacao == FLAG_MERGE)
		free(buffer_parcial);
	else
		free(buffer);
}

void IndiceInvertido::set_cont_temporarios( unsigned int valor ){	cont_temporarios = valor;	}

void IndiceInvertido::InicializaContadores()
{
	ocupacao_buffer = 0;
	numero_termos = 0;
	flag_finaliza_parcial = 0;
}

_tipo_buffer IndiceInvertido::funcao_hash( _tipo_buffer valor ){	return valor;	}

void IndiceInvertido::FechaParciais()
{
	for(unsigned int i = 0; i < cont_temporarios; i++ ){
		if( fparciais[i] != NULL )	fclose( fparciais[i] );
	}
}

void IndiceInvertido::InicializaMerge()
{
	char nome_parcial[4000];

	/*Aloca os ponteiros de arquivo e abre os temporarios*/
	fparciais = (FILE**)malloc(sizeof(FILE*)*cont_temporarios);
	for(unsigned int i = 0; i < cont_temporarios; i++ ){
		sprintf( nome_parcial, "%s%s%d",path_indice, NOME_TEMPORARIO, i );
		if( (fparciais[i] = fopen( nome_parcial, "rb") ) == NULL ){
			printf("AbreParciais:: Impossivel abrir parcial (%s)\n", nome_parcial );
			exit(1);
		}
		printf("parcial (%d) aberto: %s\n", i, nome_parcial);
	}

	/*Aloca os buffers para as listas temporarias*/
	ocupacao_buffer = 0;

	/*Aloca o buffer de compressao para as listas invertidas finais*/
	buffer_compressao = (_tipo_buffer*)malloc( sizeof(_tipo_buffer)*(TAM_BLOCO_LISTA_FINAL+5));
	ocupacao_buffer_compressao = 0;
	cont_tam_indice = 0;

	/*Compressao*/
	btat = 0;
	outbuffer = buffer_compressao;
	buffer_compressao[0] = 0;
}

NO_LISTA_INV* IndiceInvertido::CriaTermo( _tipo_buffer id )
{
	NO_LISTA_INV *aux = (NO_LISTA_INV*)malloc( sizeof(NO_LISTA_INV) );
	aux->id = id;
	aux->num_blocos = 0;
	aux->plistadoc = ocupacao_buffer;
	aux->pultdoc = ocupacao_buffer;
	aux->pos_bloco_atual = 0;
	aux->tam_lista = 0;
	aux->prox = NULL;

	numero_termos++;// numero de termos no indice invertido

	return aux;
}

void IndiceInvertido::InsereNoBuffer( NO_LISTA_INV *termo, _tipo_buffer doc, _tipo_buffer pos )
{
	if( termo->pos_bloco_atual < TAM_BLOCO_DOCPOS ){
		buffer[ termo->pultdoc + termo->pos_bloco_atual ] = doc;
		buffer[ termo->pultdoc + termo->pos_bloco_atual + 1 ] = pos;
		if(termo->pultdoc + termo->pos_bloco_atual+1 > TAM_BUFFER){	printf("Zica!\n");	exit(1);}
		termo->pos_bloco_atual += 2; //doc e pos
		termo->tam_lista += 2;
	}
	else{
		if( (ocupacao_buffer + TAM_BLOCO_DOCPOS + 1) < TAM_BUFFER ) {
			buffer[ termo->pultdoc + termo->pos_bloco_atual ] = ocupacao_buffer;
			termo->pultdoc = ocupacao_buffer;
			termo->pos_bloco_atual = 0;
			buffer[ termo->pultdoc + termo->pos_bloco_atual ] = doc;
			buffer[ termo->pultdoc + termo->pos_bloco_atual + 1 ] = pos;
			termo->pos_bloco_atual += 2; //doc e pos
			termo->tam_lista += 2;
			buffer[ ocupacao_buffer + TAM_BLOCO_DOCPOS ] = ULT_DOC_LISTA; //inicializa o ponteiro com NULL
			ocupacao_buffer += TAM_BLOCO_DOCPOS + 1;// +1 do ponteiro
		}
		else{
			flag_finaliza_parcial = 1;
		}
	}
}

NO_LISTA_INV* IndiceInvertido::BuscaTermo( unsigned int endereco, _tipo_buffer id )
{
	NO_LISTA_INV *aux;
	aux = indice_invertido[ endereco ];
	while( aux ){
		if( aux->id == id )	return aux;

		aux = aux->prox;
	}
	return NULL;
}

void IndiceInvertido::Insere( _tipo_buffer doc, _tipo_buffer termo, _tipo_buffer pos )
{
	unsigned int endereco = funcao_hash( termo ) % TAM_INDICE_INVERTIDO;
	NO_LISTA_INV *aux = BuscaTermo( endereco, termo );

	if( aux ){
		InsereNoBuffer( aux, doc, pos );
	}
	else{			
		if( (ocupacao_buffer + TAM_BLOCO_DOCPOS + 1) < TAM_BUFFER ){
			aux = indice_invertido[ endereco ];
			indice_invertido[endereco] = CriaTermo( termo );
			buffer[ ocupacao_buffer + TAM_BLOCO_DOCPOS ] = ULT_DOC_LISTA; //inicializa o ponteiro com NULL
			indice_invertido[endereco]->prox = aux;

			InsereNoBuffer( indice_invertido[endereco], doc, pos );
			ocupacao_buffer += TAM_BLOCO_DOCPOS + 1;// +1 do ponteiro
		}
		else{
			flag_finaliza_parcial = 1;		/*Memoria cheia*/
		}
	}
}

int ComparaPorID(const void *a, const void *b)
{
	TERMO *reg1 = (TERMO *)a;
	TERMO *reg2 = (TERMO *)b;
	
//	if (reg1->id > reg2->id) return 1;
//	//if (reg1->id < reg2->id) return -1;
//	return 0;
	return reg1->id - reg2->id;
}

void IndiceInvertido::OrdenaTermos()
{
	vet_termos = (TERMO*)malloc( sizeof(TERMO)*numero_termos );
	NO_LISTA_INV *aux, *aux2;
	unsigned int i, cont_termos;

	cont_termos = 0;
	for( i = 0; i < TAM_INDICE_INVERTIDO; i++ ){
		aux = indice_invertido[i];
		while( aux ){

			vet_termos[cont_termos].id = aux->id;
			vet_termos[cont_termos].plistadoc = aux->plistadoc;
			vet_termos[cont_termos].tam_lista = aux->tam_lista;

			aux2 = aux;
			aux = aux->prox;
			free( aux2 );
			cont_termos++;
		}
		indice_invertido[i] = NULL;
	}

	if( cont_termos != numero_termos ){
		printf("ERROO!!! OrdenaTermos() -> cont_termos != numero_termos!!!\n" );
		exit(1);
	}

	qsort( vet_termos, numero_termos, sizeof(TERMO), ComparaPorID );
}

void IndiceInvertido::CriaIndiceTemporario()
{
	FILE *f;
	char nome_indice[ 100 ];
	unsigned int i, atual, num_blocos = 0, restante_ult_bloco, cont_blocos;

	sprintf( nome_indice, "%s%s%d", path_indice, NOME_TEMPORARIO, cont_temporarios );

	if( (f = fopen( nome_indice, "wb")) == NULL ){
		printf("CriaIndiceTemporario:: Impossivel criar (%s)\n", nome_indice );
		exit(1);
	}

	/*Define o tamanho do buffer do arquivo*/
	setvbuf ( f , NULL , _IOFBF , 10000000 );

	for( i = 0; i < numero_termos; i++ ){
		fwrite( &(vet_termos[i].id), sizeof(_tipo_buffer), 1, f );
		fwrite( &(vet_termos[i].tam_lista), sizeof(unsigned int), 1, f );

		atual = vet_termos[i].plistadoc;
		num_blocos = vet_termos[i].tam_lista / TAM_BLOCO_DOCPOS;
		restante_ult_bloco = vet_termos[i].tam_lista % TAM_BLOCO_DOCPOS;
		cont_blocos = 0;

		do{
			if( cont_blocos < num_blocos ){
				fwrite( &(buffer[atual]), sizeof(_tipo_buffer), TAM_BLOCO_DOCPOS, f );
			}
			else{
				fwrite( &(buffer[atual]), sizeof(_tipo_buffer), restante_ult_bloco, f );
			}
			atual = buffer[ atual + TAM_BLOCO_DOCPOS ];
			cont_blocos++;

		} while( atual != ULT_DOC_LISTA );
	}
	fclose( f );
	free( vet_termos );
	cont_temporarios++;
}

int IndiceInvertido::LeTermo( unsigned int id, TipoHeap *temp )
{
	_tipo_buffer vet[TAM_BLOCO_LISTA];

	if(fread(vet, sizeof(int), TAM_BLOCO_LISTA, fparciais[id]))
	{
		temp->Chave = vet[0];
		temp->tam_lista = vet[1]; 
		temp->id_parcial = id;
		return 1;
	}
	return 0;
}

inline void IndiceInvertido::Comprime(_tipo_buffer id)
{
	VerificaEspacoBufferFinal();
	RawDeltaEncode( id+1, outbuffer, btat);
}

/*Le as listas parciais e concatena no indice final comprimido*/
void IndiceInvertido::CopiaBufferFinal( TipoHeap termo_atual, GAP *gap )
{
	unsigned int j, tam_lista, cont;

	tam_lista = termo_atual.tam_lista;

	while( tam_lista ){

		if( TAM_BUFFER_LISTA < tam_lista )
			cont = fread( buffer_parcial, sizeof(_tipo_buffer), TAM_BUFFER_LISTA, fparciais[ termo_atual.id_parcial ] );
		else
			cont = fread( buffer_parcial, sizeof(_tipo_buffer), tam_lista, fparciais[ termo_atual.id_parcial ] );

		tam_lista -= cont;

		num_doc_lista += cont/2;		/*Numero de documentos na lista invertida*/			

		/*Checa se eh o primeiro documento da lista*/
		if( gap->controle ){
			Comprime(buffer_parcial[0]);		/*Comprime o docid*/
			Comprime(buffer_parcial[1]);		/*Comprime a posicao*/

			/*Inicializa os gaps de docid e posicao para a nova lista*/
			(*gap).doc = buffer_parcial[ 0 ];
			(*gap).pos = buffer_parcial[ 1 ];
		}

		/*Comprime o restante da lista*/
		for( j = gap->controle; j < cont; j += 2){

			/*Checa se eh um novo docid e inicializa o gap de posicao*/
			if( buffer_parcial[j] != gap->doc )	gap->pos = 0;

			Comprime(buffer_parcial[j]-gap->doc);		/*Comprime o docid*/
			Comprime(buffer_parcial[j+1]-gap->pos);	/*Comprime a posicao*/

			/*Incrementa os gaps*/
			(*gap).doc = buffer_parcial[j];
			(*gap).pos = buffer_parcial[j+1];
		}
		gap->controle = 0;
	}	
}

void IndiceInvertido::AbreArquivoFinal( unsigned int id)
{
	char nome_arquivo[128];
	sprintf( nome_arquivo, "%s%s%d", path_indice,NOME_POSICIONAL, id );
	if( (fbfinal = fopen( nome_arquivo,"wb" )) == NULL ){
		printf("IndiceInvertido::AbreArquivoFinal-> arquivo (%s) nao pode ser criado\n", NOME_POSICIONAL );
		exit(1);
	}
}

void IndiceInvertido::FechaArquivoIDX(){	fclose( fidx );	fidx = NULL;	}

void IndiceInvertido::AbreArquivoIDX()
{
	sprintf(filename, "%s%s", path_indice, NOME_IDX);
	if( (fidx = fopen( filename ,"wb")) == NULL ){
		printf("IndiceInvertido::AbreArquivoIDX-> arquivo (%s) nao pode ser criado\n", NOME_IDX );
		exit(1);
	}
}

void IndiceInvertido::EscreveIDX( unsigned int id )
{
	unsigned int i;
	IDX idx_lixo;

	memset(&idx_lixo, 0, sizeof(IDX));

	if( id == 0 ){
		fwrite( &tidx, sizeof(IDX), 1, fidx );
		controle_id = id;
		return;
	}

	if( (id - 1) == controle_id ){
		fwrite( &tidx, sizeof(IDX), 1, fidx );
		controle_id = id;
		return;
	}

	if( controle_id == UINT_MAX ){
		controle_id = 0;
		fwrite( &idx_lixo, sizeof(IDX), 1, fidx );
	}

	for( i = controle_id+1; i < id; i++ ){
		fwrite( &idx_lixo, sizeof(IDX), 1, fidx );//pra garantir, eh bom escrever um idx padrao
	}
	fwrite( &tidx, sizeof(IDX), 1, fidx );
	controle_id = id;
}

void IndiceInvertido::EscreveBufferFinal()
{
	fwrite( buffer_compressao, sizeof(_tipo_buffer), outbuffer - buffer_compressao, fbfinal );

	ocupacao_buffer_compressao = 0;

	cont_tam_indice += outbuffer-buffer_compressao; /*Tamanho do buffer escrito */

	/*compressao*/
	buffer_compressao[0] = *outbuffer;
	outbuffer = buffer_compressao;
}

void IndiceInvertido::FinalizaIndice()
{
	fclose( fbfinal );
	cont_final++;
	cont_tam_indice = 0;
	ocupacao_buffer_compressao = 0;
	ocupacao_buffer = 0;
}

int IndiceInvertido::VerificaBufferFinal()
{
	if( cont_tam_indice >= TAM_BLOCO_INDICE ){//ATENCAO: precisa garantir que vai ter uma lista completa aqui
		FinalizaIndice();
		AbreArquivoFinal( cont_final );
		return FLAG_FINALIZA_INDICE;
	}
	
	return 0;
}

void IndiceInvertido::ImprimeIDX()
{
	printf("arquivo (%d)\t", tidx.id_arq );
	printf("inicio_termo (%lu)\t", tidx.pos_inicio_termo );
	printf("tam_lista (%d)\n", tidx.pos_tam_lista );
}

void IndiceInvertido::Merge()
{
	unsigned int i;
	GAP gap;
	int controle = 0;
	
	Heap heap;
	TipoHeap atual, anterior;
	
	cont_final = 0; //contador de arquivos finais
	cont_tam_indice = 0; //numero de elementos em cada indice(bytes = num_elem*sizeof(_tipo_buffer))
	AbreArquivoFinal( cont_final );
	AbreArquivoIDX();
	controle_id = UINT_MAX;//termo impossivel de aparecer
	
	heap.AlocaHeap(cont_temporarios);
	
	for( i = 0; i < cont_temporarios; i++ ){
		if(LeTermo(i, &atual))	heap.InsereNaPosicao( i+1, atual );//i+1 porque o heap começa em 1
	}

	heap.Constroi();

	num_doc_lista = 0;	/*Numero de documentos na lista invertida*/
	anterior.Chave = UINT_MAX;// seria uma chave impossivel de aparecer (para um termo).

	while( heap.GetTamHeap() > 0 )
	{
		atual = heap.Retira();

		/*Novo termo*/
		if( atual.Chave != anterior.Chave )
		{
			EscreveBufferFinal();
			/*Caso tenha informacao no ultimo int salva*/
			if(btat)	fwrite( outbuffer, sizeof(_tipo_buffer), 1, fbfinal );

			/*compressao*/
			buffer_compressao[0] = 0;
			outbuffer = buffer_compressao;
			btat = 0;

			if( controle )
			{
				tidx.pos_tam_lista = num_doc_lista;
				EscreveIDX( anterior.Chave );
				controle = 0;
			}
			VerificaBufferFinal();

			tidx.id_arq = (char)cont_final;
			tidx.pos_inicio_termo = ftell(fbfinal);

			gap.doc = 0;
			gap.controle = 2;
			controle++;
			num_doc_lista = 0;
		}
		CopiaBufferFinal( atual, &gap );
		anterior = atual;

		/*tenta ler e inserir um novo item do heap*/
		if(LeTermo(anterior.id_parcial, &atual))	heap.Insere( atual ); 
	}

	tidx.pos_tam_lista = num_doc_lista;
	EscreveIDX( anterior.Chave );

	EscreveBufferFinal();
	/*Caso tenha informacao no ultimo int salva*/
	if(btat)	fwrite( outbuffer, sizeof(_tipo_buffer), 1, fbfinal );

	FinalizaIndice();
	FechaArquivoIDX();
	FechaParciais();

	free( buffer_compressao );
	free( fparciais );
}

void IndiceInvertido::dump_temporario()
{
	OrdenaTermos();
	CriaIndiceTemporario();
	InicializaContadores();
}
