#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "huffman.h"
//Checagem de argumentos:
//-------------------------------------------------------------------------
int argumentos(int argc, char** argv)
{

	if(argc <= 2)
	{
		printf("Argumentos insuficientes!!\n\n");
		printf("Uso do programa:\n./programa -c arquivo.txt\n\t  ou\n./programa -d arquivo.txt\n");
		return 1;
	}
	else if(argc > 3)
	{
		printf("Argumentos demais!!\n");
		return 1;
	}

	if(strcmp(argv[1],"--comprimir") == 0 || strcmp(argv[1],"-c") == 0)
		return 1;
	else if(strcmp(argv[1],"--descomprimir") == 0 || strcmp(argv[1],"-d") == 0)
		return 2;
	else 
		return 0;

}

//Retorna o nome do arquivo sem a extensão
unsigned char* nomeArquivo(unsigned char* nomeComExt)
{
	if(!strrchr(nomeComExt,'.')) //se o nome não tem pontos
		return strcpy(malloc(strlen(nomeComExt)),nomeComExt);

	int cont = strlen(nomeComExt) - 1;
	unsigned char* nomeSemExt;
	while(nomeComExt[cont] != '.')
		cont--;

	nomeSemExt = malloc(cont+1); 
	for(int i = 0; i < cont; i++)
		nomeSemExt[i] = nomeComExt[i];

	return nomeSemExt;
}

//Retorna a extensão do arquivo
unsigned char* extArquivo(unsigned char* nomeComExt)
{
	return strrchr(nomeComExt,'.');
}

//Retorna o nome que deve ser usado para o arquivo de output
unsigned char* nomeOutput(unsigned char* nome, int tipo)
{
	unsigned char* nomeSemExt = nomeArquivo(nome);
	unsigned char* output;

	if(tipo == 1) //output da compressão
	{
		output = malloc(strlen(nomeSemExt) + 5); //tamanho + 4 bytes de ".huf"/".txt" + '\0'
		output = strcat(nomeSemExt,".huf");
		return output;
	}
	if(tipo == 2) // output da descompressão
	{
		output = malloc(strlen(nomeSemExt) + 18);
		output = strcat(nomeSemExt,"_decompressed.txt");
		return output;
	}
}

//Abre o arquivo de input e faz as checagens necessárias
int abrirInput(FILE** arq, unsigned char* nome, int* tam, int opt)
{
	*arq = fopen(nome,"r");
	if(!(*arq))
	{
		printf("O arquivo especificado não existe!!\n\n");
		return 0;
	}
	if(opt == 2 && strcmp(".huf",extArquivo(nome)) != 0)
	{
		printf("Este arquivo não é do tipo .huf e não pode ser descomprimido.\n");
		return 0;
	}

	//pega o tamanho do arquivo e cria uma string desse tamanho + 1 ('\0')
	fseek(*arq, 0L, SEEK_END);
	*tam = ftell(*arq);
	rewind(*arq);

	if(*tam == 0)
	{
		printf("O arquivo especificado está vazio!!\n");
		return 0;
	}
	return 1;
}

//Calcula e retorna a frequencia de todos os caracteres que aparecem no arquivo
int* calculaFrequencias(unsigned char* frase)
{
	//Inicializa a matriz
	int* freq = malloc(256*sizeof(int));
	memset(freq,0,256*sizeof(int));

	for(int i = 0; i < strlen(frase); i++)
		freq[frase[i]]++;

	return freq;
}

//Calcula e retorna a quantidade de caracteres diferentes presentes no arquivo
int calculaQuantidade(int* freq)
{
	int cont = 0;
	for(int i = 0; i < 256; i++)
		if(freq[i] > 0)
			cont++;
			
	return cont;
}

//Cria um nó de lista contendo uma nova árvore com simbolo c e peso freq
Lista* criarNo(unsigned char c, int freq)
{
	Arvore* arv = malloc(sizeof(Arvore));
	Lista* no = malloc(sizeof(Lista));
	arv->simbolo = c;
	arv->peso = freq;
	arv->esq = NULL;
	arv->dir = NULL;

	no->arv = arv;
	no->prox = NULL;

	return no;

}

//Insere um item na lista de forma ordenada
void inserirLista(Lista** lista, Lista* novo)
{
	if(*lista == NULL) //é o primeiro elemento
	{
		*lista = novo;
		return;
	}
	
	Lista* aux = *lista;
	Lista* prev = NULL;

	//insere o nó por ordem de peso
	while(aux && aux->arv->peso <= novo->arv->peso)
	{
		prev = aux;

		//se o nó sendo inserido for um nó de caracter, ele ordena também pela ordem "alfabetica"
		if(novo->arv->simbolo != -1 && aux->arv->peso == novo->arv->peso && aux->arv->simbolo > novo->arv->simbolo)
			break;
		aux = aux->prox;
	}
	novo->prox = aux;
	if(prev)
		prev->prox = novo;
	else
		*lista = novo;
}

//Cria todas as árvores de nó unico a partir da lista de frequencia e as insere na lista de árvores
void criarListaArvores(Lista** lista, int* freq)
{
	for(int i = 0; i < 256; i++)
		if(freq[i] > 0)
			inserirLista(lista,criarNo(i,freq[i]));
}

//Constroi a árvore de codificação a partir da lista com arvores de nós unicos ordenados por peso
Arvore* arvoriza(Lista** listaArvores)
{
	Arvore *T1, *T2, *Tr;
	Lista* novo;
	while((*listaArvores)->prox)
	{
		T1 = (*listaArvores)->arv;
		*listaArvores = (*listaArvores)->prox;
		T2 = (*listaArvores)->arv;
		*listaArvores = (*listaArvores)->prox;

		Tr = malloc(sizeof(Arvore));
		Tr->peso = T1->peso + T2->peso;
		Tr->simbolo = -1;
		Tr->esq = T1;
		Tr->dir = T2;

		novo = malloc(sizeof(Lista));
		novo->arv = Tr;
		novo->prox = NULL;

		inserirLista(listaArvores,novo);
	}
	return (*listaArvores)->arv;
}

//Insere um código no dicionario de códigos
void inserirCodigo(Codigo** dicionario, unsigned char* codigo, unsigned char simbolo)
{
	Codigo* novo = malloc(sizeof(Codigo));
	novo->simbolo = simbolo;
	novo->codigo = codigo;
	novo->prox = NULL;

	if(*dicionario == NULL)
	{
		*dicionario = novo;
		return;
	}

	Codigo* aux = *dicionario;

	while(aux->prox)
		aux = aux->prox;
	aux->prox = novo;



}

//Atribui códigos a cada um dos caracteres presentes na árvore e os insere no dicionário
void montarDicionario(Arvore* no, unsigned char* codigo, int topo, char** dicionario)
{
	if(no->esq)
	{
		codigo[topo] = '0';
		montarDicionario(no->esq, codigo, topo + 1, dicionario);
	}
	if(no->dir)
	{
		codigo[topo] = '1';
		montarDicionario(no->dir, codigo, topo + 1, dicionario);
	}
	if(no->esq == NULL && no->dir == NULL) //é folha
	{
		codigo[topo] = '\0';
		dicionario[no->simbolo] = strcpy(malloc(topo),codigo);
	}

}

//retorna o tamanho total a ser ocupada pela frase em forma codificada
int tamCodificada(unsigned char* frase, char** dicionario)
{
	int tam = 0;
	for(int i = 0; i < strlen(frase); i++)
		tam += strlen(dicionario[frase[i]]);
	return tam;
}

//Traduz a frase lida do arquivo para sua forma codificada, salvando-a na string fraseCodificada
void codificar(unsigned char* frase, unsigned char* fraseCodificada, char** dicionario, int tam)
{
	int i;
	for(i = 0; i < strlen(frase); i++)
		strcat(fraseCodificada,dicionario[frase[i]]);

	//Completa a string com 0s para que seja de tamanho multiplo de 8
	int max = (tam + (8 - tam % 8));
	for(i = tam; i < max; i++)
		fraseCodificada[i] = '0';
}

//Converte um vetor de bits em um único byte
unsigned char convert_byte ( unsigned char * vet , int n )
{
	unsigned char rtno = 0 ,
	     i = 0;
	while ( i < n )
	{
		rtno += pow(2,i) * (vet [(n - 1) - i] - 48) ;

		i++;
	}
	return rtno ;
}

//Converte um byte em um vetor de bits
void convert ( unsigned char ch , unsigned char * Vet )
{
	int i = 7;
	// indice auxiliar
	while ( i >=0 )
	{
		if ( ch & 1)
			Vet [i] = '1';
		else
			Vet[i] = '0';
		i--;
		ch >>= 1;
	}
}

//Imprime as informação no arquivo de saída codificado
void imprimeHuf(FILE* out, unsigned char* fraseCodificada, int qtdChars, int tamCod, int* freq)
{
	int i = 0, j = 0;
	unsigned char binario[9];
	fprintf(out,"HUFFMAN 1.0\n%d\n",qtdChars);

	for(i = 0 ; i < 256; i++)
		if(freq[i] > 0)
			fprintf(out,"%c %d\n",i,freq[i]);

	fprintf(out,"%d\n",tamCod);

//	for(i = 0; i <= strlen(fraseCodificada); i++)
//	{
//
//		if(i != 0 && i % 8 == 0)
//		{
//			fputc(convert_byte(binario,8),out);
//		}
//		binario[i % 8] = fraseCodificada[i];
//	}
	for(i = 0; i <= strlen(fraseCodificada); i+=8)
		fputc(convert_byte(&fraseCodificada[i],8),out);
		
}

//Lê a tabela de frequencia do arquivo compactado e insere os itens na lista de nós únicos de árvore
void lerTabelaFreq(FILE* arq, Lista** listaArvores, int qtdChars)
{
	int freq;
	unsigned char c;
	for(int i = 0; i < qtdChars; i++)
	{
		c = fgetc(arq);
		fgetc(arq); //lê o espaço em branco
		fscanf(arq,"%d",&freq);
		fgetc(arq); //lê o \n
		inserirLista(listaArvores,criarNo(c,freq));
	}

}

//Lê a frase codificada lida do arquivo huf e converte de volta à sua forma original
unsigned char decodificar(Arvore* no, unsigned char* frase, int* abspos, int pos)
{
	if(!(no->esq || no->dir)) //é folha
	{
		*abspos = pos; //atualiza a posição absoluta em relação à string frase
		return no->simbolo;
	}

	if(frase[pos] == '0')
	{
		return decodificar(no->esq, frase, abspos,pos+1);
	}
	if(frase[pos] == '1')
	{
		return decodificar(no->dir, frase, abspos, pos+1);
	}
}
void imprimirCodigos(Codigo* aux)
{
	printf("\n\nLista de Codigos:\n");
	while(aux)
	{
		printf("Simbolo: %c\tCodigo: %s\n\n",aux->simbolo,aux->codigo);
		aux = aux->prox;
	}

}
