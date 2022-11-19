
//Structs
//--------------------------------
typedef struct arvore
{
	unsigned char simbolo;
	int peso;
	struct arvore* esq;
	struct arvore* dir;
} Arvore;

typedef struct lista
{
	struct arvore* arv;
	struct lista* prox;
} Lista;
//--------------------------------


int argumentos(int argc, char** argv);
int abrirInput(FILE** arq, unsigned char* nome, int* tam, int opt);
unsigned char* nomeArquivo(unsigned char* nomeComExt);
unsigned char* nomeOutput(unsigned char* nome, int tipo);
unsigned char* extArquivo(unsigned char* nomeComExt);

int* calculaFrequencias(unsigned char* frase);
int calculaQuantidade(int* freq);

Lista* criarNo(unsigned char c, int freq);
void inserirLista(Lista** lista, Lista* no);
void criarListaArvores(Lista** lista, int* freq);
Arvore* arvoriza(Lista** listaArvores);
void montarDicionario(Arvore* no, unsigned char* codigo, int topo, char** dicionario);
int tamCodificada(unsigned char* frase, char** dicionario);
void codificar(unsigned char* frase, unsigned char* fraseCodificada, char** dicionario, int tam);

void lerTabelaFreq(FILE* arq, Lista** listaArvores, int qtdChars);
int pegaTam(FILE* arq);
unsigned char decodificar(Arvore* no, unsigned char* frase, int* abspos, int pos);

unsigned char convert_byte ( unsigned char * vet , int n );
void convert ( unsigned char ch , unsigned char * Vet );

void imprimeHuf(FILE* out, unsigned char* fraseCodificada, int qtdChars, int tamCod, int* freq);
