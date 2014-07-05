#ifndef funcoesAuxiliares 
#define _funcoesAuxiliares_
#include <string.h>
#include "t2fs.h"

#define TAM_SETOR 256
#define TAM_BITMAPREG 64 /*Tam. do registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados*/

struct t2fs_superbloco* leSuperBloco(void); // Retorna o superbloco, ou NULL caso tenha acontecido alguma falha
char* le_bloco(int numero_bloco);
BOOL escreve_bloco(char* bloco, int numero_bloco); // Retorna TRUE se conseguiu escrever os dados no bloco, FALSE caso contrário
struct t2fs_record get_registro_bitmap();  // Retorna a estrutura de dados que descreve o arquivo que mantém o bitmap e 

struct t2fs_superbloco* superbloco;
WORD tamanho_bloco;

void init(void);
int achablocolivre(void);
int achabit(char*,int*);

#endif
