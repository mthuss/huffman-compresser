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

	if(!abrirArquivo(&arq, argv[2], &tam, &frase))
		return 1;

	if(opt == 1) //Compressão
	{
		fgets(frase, tam + 1, arq);
//		printf("informações do arquivo:\n- tamanho em bytes: %d\n- conteúdo: %s\n\n",tam,frase);

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
//		printf("frase codificada: %s\ntamanho: %d\n",fraseCodificada,strlen(fraseCodificada));

		//Quinto passo:
		//abre o arquivo para escrita, 
		//converte 8 'bits' da frase codificada em um byte
		//e imprime no arquivo
		output = fopen(strcat(nomeArquivo(argv[2]),".huf"),"w+");
		imprimeHuf(output,fraseCodificada,qtdChars,tamCod,freq);
//		imprimirCodigos(dicionario);	


		fclose(arq);
		fclose(output);
	}


	return 0;
}
