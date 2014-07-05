#ifndef funcoesAuxiliares 
#define _funcoesAuxiliares_
#include <string.h>
#include "t2fs.h"

#define TAM_SETOR 256
#define TAM_BITMAPREG 64 /*Tam. do registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados*/

struct t2fs_superbloco* leSuperBloco(void); // Retorna o superbloco, ou NULL caso tenha acontecido alguma falha
char* le_bloco(int numero_bloco);
BOOL escreve_bloco(char* bloco, int numero_bloco); // Retorna TRUE se conseguiu escrever os dados no bloco, FALSE caso contrário
struct t2fs_record get_registro_bitmap();  // Retorna a estrutura de dados que descreve o arquivo que mantém o bitmap
struct t2fs_record get_registro_raiz();
BOOL caminhoExiste(char* caminho); // Verifica se um arquivo existe
struct t2fs_record* leRegistroArquivo(char* buffer);
BOOL inicializarInode(char* nome, DWORD bloco);
void init(void);
//extern struct t2fs_superbloco* superbloco;
extern WORD tamanho_bloco;
extern struct t2fs_superbloco* superBloco;


#endif
