#ifndef funcoesAuxiliares
#define _funcoesAuxiliares_
#include <string.h>
#include "t2fs.h"

#define TAM_SETOR 256
#define TAM_REG 64 /*Tam. do registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados*/



void init(void);
struct t2fs_superbloco* leSuperBloco(void); // Retorna o superbloco, ou NULL caso tenha acontecido alguma falha
char* le_bloco(int numero_bloco);
BOOL escreve_bloco(char* bloco, int numero_bloco); // Retorna TRUE se conseguiu escrever os dados no bloco, FALSE caso contrário
struct t2fs_record get_registro_bitmap(); // Retorna a estrutura de dados que descreve o arquivo que mantém o bitmap
struct t2fs_record get_registro_raiz(); // Análogo à anterior, mas para a raiz
void printBloco(int bloco);
void printSetor(char* buffer, int inicio, int fim);


struct t2fs_record* le_t2fs_record(char* buffer); // Assume que recebe 64 bytes..
void imprime2(DWORD comp);
char* nome_final(char* final); // Recebe um caminho completo e devolve o final (e.g. /home/pedro/alvo retorna alvo)
char* subcaminho(char* caminho, int j);
int conta_niveis_caminho(char* caminho); // /home/pedro/algo tem 3 níves. Se for caminho inválido (e.g. #2crd/teste) retorna -1


struct t2fs_record* procura_descritores2(int niveis, char* caminho, char* final, struct t2fs_record* descritor);
struct t2fs_record* get_descritor_arquivo(char* caminho);


struct t2fs_record* procura_descritor_num_bloco_diretorio(char* nome, DWORD bloco);
struct registro_bloco* procura_descritor_num_diretorio(char* nome, DWORD* offset, struct registro_bloco* descritor);

DWORD procura_descritores(int niveis, char* caminho, char* final, struct t2fs_record* descritor);
DWORD caminho_valido(char* caminho); /* Há três retornos:
-1: caminho inválido (char* vazio, não comece com '/', nao tenha algum diretorio do meio)
0: caminho válido, mas o arquivo ao final do caminho não existe no diretório
outro: bloco onde está o arquivo*/

struct t2fs_superbloco* superbloco;
WORD tamanho_bloco;

typedef struct aberto
{
	DWORD handle;
	int currentpointer;

}ARQUIVOABERTO;

ARQUIVOABERTO arquivosabertos[20];
void inicializavetorabertos();
int marcarabertos(DWORD, DWORD);
int alteracurentpoiter(DWORD handle, DWORD currentpointer);

int insereblocorecord(char *nome, DWORD *bloco);
char* get_caminho_do_pai(char* caminho);
DWORD* criablocoenderecos(DWORD *blocolivre);
static DWORD INVALIDO = -1; 
void init(void);
char* inodeparachar(struct t2fs_record*);
struct t2fs_record* inicializainode(char*);
struct t2fs_record* carregaarquivo(DWORD);
void excluiarquivobitmap(DWORD*);
DWORD* listablocosarquivo(struct t2fs_record*, DWORD*);
int achablocolivre(void);
int achabit(char*,int*);
struct registro_bloco { struct t2fs_record* registro; DWORD bloco; };
#endif
