#include "../../libs-iw/indexer/include/Heap.h"

void Heap::AlocaHeap( unsigned int n )
{
	tam_heap = n;
	heap = (TipoHeap*)malloc( (sizeof(TipoHeap))*(tam_heap+1) );
}

int Heap::GetTamHeap(){	return tam_heap;	}

void Heap::InsereNaPosicao( unsigned int pos, TipoHeap x){	heap[ pos ] = x;	}

void Heap::Refaz(TipoIndice Esq, TipoIndice Dir, TipoHeap *A)
{
	TipoIndice i = Esq;
	unsigned int j;
	TipoHeap x;
	j = i * 2;
	x = A[i];

	while (j <= Dir) 
	{
		if (j < Dir){
			if (A[j].Chave > A[j+1].Chave){
				j++;
			}else{
				if ( (A[j].Chave == A[j+1].Chave) && (A[j].id_parcial > A[j+1].id_parcial)){
					j++;
				}
			}
		}

		if (x.Chave < A[j].Chave)	break;

		if( (x.Chave == A[j].Chave) && (x.id_parcial < A[j].id_parcial) )	break;

		A[i] = A[j];
		i = j; j = i * 2;
	}
	A[i] = x;
} 

void Heap::Constroi( )
{
	TipoIndice Esq;

	Esq = tam_heap / 2 + 1;

	while (Esq > 1)
	{
		Esq--;
		Refaz(Esq, tam_heap, heap);
	}
} 

TipoHeap Heap::Retira( )
{
	TipoHeap Maximo;////ver aqui o que acontece quando o heap fica vazio
	if ( tam_heap < 1) 
  		printf("Erro: heap vazio\n");
	else {
		Maximo = heap[1];  heap[1] = heap[tam_heap];  tam_heap--;
		Refaz(1, tam_heap, heap);
	}
	return Maximo;
}

void Heap::AumentaChave(TipoIndice i, TipoChave ChaveNova, unsigned int id, TipoHeap *A)
{
	TipoHeap x;
	if (ChaveNova > A[i].Chave) {
		printf("Erro: ChaveNova menor que a chave atual\n");
		return;
	}
	A[i].Chave = ChaveNova;
	A[i].id_parcial = id;
	while ( (i > 1) && (A[i / 2].Chave >= A[i].Chave) ) {
		if( A[i / 2].Chave == A[i].Chave ){
			if( A[i / 2].id_parcial < A[i].id_parcial ){
				break;
			}
		}
		x = A[i / 2];  A[i / 2] = A[i];  A[i] = x;
		i /= 2;
	}
}

void Heap::Insere(TipoHeap x)
{
	tam_heap++;

	heap[tam_heap] = x;  heap[tam_heap].Chave = UINT_MAX;
	heap[tam_heap].id_parcial = UINT_MAX;
	AumentaChave( tam_heap, x.Chave, x.id_parcial, heap);
}

void Heap::Imprime( )
{
	for (unsigned int i = 1; i <= tam_heap; i++)
		printf("[%d,%d] ", heap[i].Chave, heap[i].id_parcial );
	putchar('\n');
}

void Heap::Testa()
{
	TipoHeap x;

	for(unsigned int i = 1; i <= tam_heap; i++){
    	scanf("%d", &heap[i].Chave);
		scanf("%d", &heap[i].id_parcial);
		printf("\n");
	}
    Imprime();
	
	printf("Constroi   : ");
  	Constroi();
  	Imprime();
	putchar('\n');
	
	
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
  
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
	
	x.Chave = 2;
	x.id_parcial = 0;
	printf("Insere [%d,%d]: ", x.Chave, x.id_parcial);
	Insere( x );
	Imprime();
	
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
	
	x.Chave = 2;
	x.id_parcial = 3;
	printf("Insere [%d,%d]: ", x.Chave, x.id_parcial);
	Insere(x);
	Imprime();
	
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
	x = Retira();
	printf("Retira [%d,%d]: ", x.Chave, x.id_parcial);
	Imprime();
	putchar('\n');
	
}
