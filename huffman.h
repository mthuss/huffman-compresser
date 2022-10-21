
//Structs
//--------------------------------
typedef struct arvore
{
	char simbolo;
	int peso;
	struct arvore* esq;
	struct arvore* dir;
} Arvore;

typedef struct lista
{
	struct arvore* arv;
	struct lista* prox;
} Lista;

typedef struct codigo
{
	char simbolo;
	char* codigo;
	struct codigo* prox;
} Codigo;
//--------------------------------


int argumentos(int argc, char** argv);
int abrirArquivo(FILE** arq, char* nome, int* tam, char** frase);
char* nomeArquivo(char* nomeComExt);
int* calculaFrequencias(char* frase);
int calculaQuantidade(int* freq);
Lista* criarNo(char c, int freq);
void inserirLista(Lista** lista, Lista* no);
void criarListaArvores(Lista** lista, int* freq);
Arvore* arvoriza(Lista** listaArvores);
void inserirCodigo(Codigo** dicionario, char* codigo, char simbolo);
void montarDicionario(Arvore* no, char* codigo, int topo, Codigo** dicionario);
char* pegaCodigo(Codigo* dicionario, char c);
int tamCodificada(char* frase, Codigo* dicionario);
void codificar(char* frase, char* fraseCodificada, Codigo* dicionario, int tam);

//funcoes de conversao
char convert_byte ( char * vet , int n );
void convert ( char ch , char * Vet );

void imprimeHuf(FILE* out, char* fraseCodificada, int qtdChars, int tamCod, int* freq);
void imprimirCodigos(Codigo* dicio);

void imprimir(Lista* lista);
