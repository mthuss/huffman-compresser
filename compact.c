#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "huffman.h"

/*			ATENÇÃO ATENÇÃO
  O PROGRAMA PARECE FUNCIONAR BEM PARA ARQUIVOS PEQUENOS,
  MAS ESTÁ DANDO PROBLEMAS NA FUNÇÃO DE PEGAR O TAMANHO DA FRASE CODIFICADA
  PARA ARQUIVOS DE TEXTO MAIORES*/
//PELO VISTO NÃO MAIS???? NÃO MUDEI NADA

int main(int argc,char** argv)
{
	FILE* arq;
	FILE* output;
	int opt, i;
	int tam, qtdChars;
	char* frase;
	int* freq;

	opt = argumentos(argc,argv);

	if(!abrirInput(&arq, argv[2], &tam))
		return 1;

	if(opt == 1) //Compressão
	{
		frase = malloc(tam + 1);
//		fgets(frase, tam + 1, arq);
		fread(frase, 1, tam + 1, arq);
		printf("informações do arquivo:\n- tamanho em bytes: %d\n- conteúdo: %s\n\n",tam,frase);
//		printf("tam: %d\nstrlen: %d\n",tam,strlen(frase));

		//Primeiro passo:
		//Criar uma lista de árvores ordenada por peso
		freq = calculaFrequencias(frase); //calcula a frequencia de caracteres
		qtdChars = calculaQuantidade(freq);
		if(qtdChars == 1)
		{
			printf("Este programa não funciona para arquivos compostos por um único caractere!!\nPor favor tente com outro arquivo.\n");
			return 1;
		}
		Lista* listaArvores = NULL;
		criarListaArvores(&listaArvores,freq); //cria lista de arvores ordenada por frequencia

		//Segundo passo: 
		//Construir uma árvore de codificação
		Arvore* arvoreHuffman = arvoriza(&listaArvores);

		//Terceiro passo:
		//montar o dicionário de codificação
		Codigo* dicionario = NULL;
		char codigo[9];
		montarDicionario(arvoreHuffman, codigo, 0, &dicionario);

		//Quarto passo:
		//converter a frase do arquivo para o formato codificado
		int tamCod = tamCodificada(frase,dicionario);
		char* fraseCodificada = malloc(tamCod + (8 - tamCod % 8) + 1); //aloca memoria pra string de tamanho multiplo de 8
		codificar(frase, fraseCodificada, dicionario, tamCod);
	//	printf("frase codificada: %s\ntamanho: %d\nem bytes: %d\n",fraseCodificada,strlen(fraseCodificada),strlen(fraseCodificada)/8);
		printf("%s\n",fraseCodificada);

		//Quinto passo:
		//abre o arquivo para escrita, 
		//converte 8 'bits' da frase codificada em um byte
		//e imprime no arquivo
		output = fopen(nomeOutput(argv[2],1),"w+");
		if(!output)
		{
			printf("Ocorreu um erro ao criar o arquivo de saída!!\n");
			return 1;
		}
		imprimeHuf(output,fraseCodificada,qtdChars,tamCod,freq);
//		imprimirCodigos(dicionario);	


		fclose(arq);
		fclose(output);
	}
	if(opt == 2) //descompressão
	{
		char c;
		//Lê o arquivo e reconstroi a árvore de codificação
		fscanf(arq, "HUFFMAN 1.0\n%d",&qtdChars);
		fgetc(arq); //lê o \n
		Lista* listaArvores = NULL;
		lerTabelaFreq(arq,&listaArvores,qtdChars);

		Arvore* arvoreHuffman = arvoriza(&listaArvores);

		int numBits;
		fscanf(arq,"%d",&numBits);
		fgetc(arq); //lê o \n
		printf("numBits: %d\n");

		int pos = 0;
		char codigo[9];
		char* fraseCodificada = malloc(numBits + (8 - numBits % 8) + 1);
		int size = ceil(numBits / 8.0);
		printf("size: %d\n",size);
		while(!feof(arq))
		{
			convert(fgetc(arq),codigo);	
			if(feof(arq)) 
				break;
			strcat(fraseCodificada,codigo);
		}
//		printf("%s\n%d\n",fraseCodificada,strlen(fraseCodificada));
		
		int len = strlen(nomeArquivo(argv[2]));
		
		output = fopen(nomeOutput(argv[2],2),"w+");
		while(pos < numBits)
			fputc(decodificar(arvoreHuffman,fraseCodificada,&pos,pos),output);

	}


	return 0;
}
