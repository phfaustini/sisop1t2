#ifndef funcoesAuxiliares
#define _funcoesAuxiliares_
#include <string.h>
#include "t2fs.h"





typedef struct aberto
{
	DWORD handle;
	int currentpointer;

}ARQUIVOABERTO;
ARQUIVOABERTO arquivosabertos[20];
void init(void);//inicializações

void inicializavetorabertos();//invalida todos os handles do vetor de abertos
int marcarabertos(DWORD, DWORD);//procura um handle invalido no vetor de abertos e o preenche com dados válidos
int alteracurentpoiter(DWORD handle, DWORD currentpointer);//varre a lista de arquivos abertos e altera current pointer

int achablocolivre(void); //acha um bloco livre no bitmap
int achabit(char*,int*); //acha um bitlivre no bitmap e o marca como ocupado. Retorna o bloco correpondente a esse bit

DWORD* criablocoenderecos(DWORD *blocolivre);//cria um bloco e o preenche com endereços invalidos, retorna o endereço do bloco
char* inodeparachar(struct t2fs_record*);//recebe um record e retorna um vetor de char equivalente a um bloco
struct t2fs_record* inicializainode(char*);//retorna um record "zerado" com o nome passado como parametro
struct t2fs_record* carregaarquivo(DWORD);//dado um bloco retorna um record
void excluiarquivobitmap(DWORD*);//usa listablocosarquivo para liberar blocos no bitmap
DWORD* listablocosarquivo(struct t2fs_record*, DWORD*);//lista todos os blocos válidos de um arquivo
static DWORD INVALIDO = -1; 

struct registro_bloco
{
	struct t2fs_record* registro;
	DWORD bloco;
};

WORD tamanho_bloco;
#define TAM_SETOR 256
#define TAM_REG 64
struct t2fs_superbloco* superbloco;
char* nome_final(char* final); // Extrai o último token em um caminho. Exemplo: "/home/pedro/arquivo" -> "arquivo"
char* subcaminho(char* caminho, int j); // Devolve o token j. Exemplo: subcaminho(" /home/pedro/arquivo", 1) -> "pedro"
int conta_niveis_caminho(char* caminho); // /home/pedro/algo tem 3 níves. Se for caminho inválido (e.g. #2crd/teste) retorna -1
char* get_caminho_do_pai(char* caminho); // home/pedro/arquivo -> "/home/pedro"


struct t2fs_superbloco* leSuperBloco(void); // Retorna o superbloco, ou NULL caso tenha acontecido alguma falha
struct t2fs_record get_registro_bitmap(); // Retorna a estrutura de dados que descreve o arquivo que mantém o bitmap
struct t2fs_record get_registro_raiz(); // Análogo à anterior, mas para a raiz
struct t2fs_record* le_t2fs_record(char* buffer); // Assume que recebe 64 bytes vindos direto do disco. Mapeia para um descritor
char* le_bloco(int numero_bloco); // Dado um bloco, retorna uma cadeira de caracteres com os bytes que compõem o bloco
BOOL escreve_bloco(char* bloco, int numero_bloco); // Retorna TRUE se conseguiu escrever os dados no bloco, FALSE caso contrário
struct registro_bloco* procura_descritor_num_bloco_diretorio(char* nome, DWORD bloco); //Retorna tupla com registro e bloco do registro
struct registro_bloco* procura_descritor_num_diretorio(char* nome, DWORD* offset, struct registro_bloco* descritor); /*Offset é o deslocamento dentro do
																													bloco em bytes para chegar no arquivo.
																													Assume que ponteiro já venha alocado*/
DWORD procura_descritores(int niveis, char* caminho, char* final, struct registro_bloco* descritor);//Retorna bloco do descritor
DWORD caminho_valido(char* caminho); /* Há três retornos:
									-1: caminho inválido (char* vazio, não comece com '/', nao tenha algum diretorio do meio)
									 0: caminho válido, mas o arquivo ao final do caminho não existe no diretório
									 outro: bloco onde está o arquivo*/


struct t2fs_record* procura_descritores2(int niveis, char* caminho, char* final, struct registro_bloco* descritor);//Retorna descritor. Função auxilair para get_descritor_arquivo
struct t2fs_record* get_descritor_arquivo(char* caminho); // Dado um caminho, retorna o descritor daquele arquivo.
#endif
