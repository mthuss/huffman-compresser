#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "huffman.h"

int main(int argc, char** argv)
{
	FILE* arq;
	FILE* output;
	int opt, i;
	int tam, qtdChars;
	unsigned char* frase;
	int* freq;

	opt = argumentos(argc,argv);

	if(opt == 0)
	{
		printf("Uso do programa:\n./programa -c arquivo.txt\n\t  ou\n./programa -d arquivo.huf\n");
		return 1;
	}

	if(!abrirInput(&arq, argv[2], &tam, opt))
		return 1;


	if(opt == 1) //Compressão
	{
		frase = malloc(tam + 1);
		fread(frase, 1, tam + 1, arq);

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
		char* dicionario[256];
		unsigned char codigo[100];
		int topo = 0;
		montarDicionario(arvoreHuffman, codigo, topo, dicionario);

		//Quarto passo:
		//converter a frase do arquivo para o formato codificado
		int tamCod = tamCodificada(frase,dicionario);
		unsigned char* fraseCodificada = malloc(tamCod + (8 - tamCod % 8) + 1); //aloca memoria pra string de tamanho multiplo de 8
		codificar(frase, fraseCodificada, dicionario, tamCod);
		free(frase);

		//Quinto passo:
		//abre o arquivo para escrita, 
		//converte 8 'bits' da frase codificada em um byte
		//e imprime no arquivo
		char* outputFilename = nomeOutput(argv[2],1);
		output = fopen(outputFilename,"w+");
		if(!output)
		{
			printf("Ocorreu um erro ao criar o arquivo de saída!!\n");
			return 1;
		}
		imprimeHuf(output,fraseCodificada,qtdChars,tamCod,freq);
		printf("%s > %s\n",argv[2],outputFilename);
		if(tam - ftell(output) < 0)
			printf("\nDevido ao tamanho pequeno da frase original e da existência do cabeçalho do arquivo, o arquivo de saída acabou %d bytes maior do que o de entrada!!\n",ftell(output) - tam);
		else printf("%d bytes economizados\n",tam - ftell(output));


		fclose(arq);
		fclose(output);
	}
	if(opt == 2) //descompressão
	{
		unsigned char c;
		//Lê o arquivo e reconstroi a árvore de codificação
		fscanf(arq, "HUFFMAN 1.0\n%d",&qtdChars);
		fgetc(arq); //lê o \n
		Lista* listaArvores = NULL;
		lerTabelaFreq(arq,&listaArvores,qtdChars);

		Arvore* arvoreHuffman = arvoriza(&listaArvores);

		int numBits;
		fscanf(arq,"%d",&numBits);
		fgetc(arq); //lê o \n

		//pega os caracteres do arquivo, transforma-os em blocos de 8 bits
		//e junta todos numa string fraseCodificada
		unsigned char* fraseCodificada = malloc(numBits + (8 - numBits % 8) + 1);
		int pos = 0;
		unsigned char codigo[9];
		while(!feof(arq))
		{
			convert(fgetc(arq),codigo);	
			if(feof(arq)) 
				break;
			strcat(fraseCodificada,codigo);
		}
		
		//abre o arquivo de saída e faz a descodificação da string, imprimindo-a no arquivo
		char* outputFilename = nomeOutput(argv[2],2);
		output = fopen(outputFilename,"w+");
		if(!output)
		{
			printf("Ocorreu um erro ao criar o arquivo de saída!!\n");
			return 1;
		}

		//"corta" fora os bits de padding da frase codificada, caso esta os possua
		fraseCodificada[numBits] = '\0';

		while(pos < strlen(fraseCodificada))
			fputc(decodificar(arvoreHuffman,fraseCodificada,&pos,pos),output);

		printf("%s > %s\n",argv[2],outputFilename);

		fclose(arq);
		fclose(output);

	}


	return 0;
}
