#ifndef funcoesAuxiliares 
#define _funcoesAuxiliares_
#include <string.h>
#include "t2fs.h"

#define TAM_SETOR 256 
#define TAM_REG 64 /*Tam. do registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados*/

struct t2fs_superbloco* leSuperBloco(void); // Retorna o superbloco, ou NULL caso tenha acontecido alguma falha
char* le_bloco(int numero_bloco);
BOOL escreve_bloco(char* bloco, int numero_bloco); // Retorna TRUE se conseguiu escrever os dados no bloco, FALSE caso contrário
struct t2fs_record get_registro_bitmap();  // Retorna a estrutura de dados que descreve o arquivo que mantém o bitmap
struct t2fs_record get_registro_raiz(); // Análogo à anterior, mas para a raiz
void printBloco(int bloco);
void printSetor(char* buffer, int inicio, int fim);
BOOL procura_descritor_num_diretorio(int qtde_blocos, char* nome, DWORD bloco, DWORD* offset); /*Offset é o deslocamento dentro do
																							bloco para chegar no arquivo. Assume
																							que ponteiro já venha alocado*/
struct t2fs_record* le_t2fs_record(char* buffer); // Assume que recebe 64 bytes..
char* nome_final(char* final); // Recebe um caminho completo e devolve o final (e.g. /home/pedro/alvo retorna alvo)
int conta_niveis_caminho(char* caminho); // /home/pedro/algo tem 3 níves. Se for caminho inválido (e.g. #2crd/teste) retorna -1
int procura_descritores(DWORD bloco, int niveis, char* caminho, char* final);
int caminho_valido(char* caminho); /* Há três retornos:
									-1: caminho inválido (char* vazio, não comece com '/', nao tenha algum diretorio do meio)
									 0: caminho válido, mas o arquivo ao final do caminho não existe no diretório
									 1: arquivo existe (pode ser dado open e delete; se for create tem que deletar esse arquivo) */
struct t2fs_superbloco* superbloco;
WORD tamanho_bloco;

void init(void);
int achablocolivre(void);
int achabit(char*,int*);

#endif
