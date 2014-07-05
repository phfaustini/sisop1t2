#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/funcoesAuxiliares.h"
#include<stdio.h>
#include<stdlib.h>

WORD tamanho_bloco=0;
struct t2fs_superbloco* superBloco=NULL;
char* le_bloco(int numero_bloco)
{
	char* buffer = (char*) malloc(tamanho_bloco);
	int numero_setores = tamanho_bloco / TAM_SETOR, i, offset=0;

	if(tamanho_bloco==0)	//Caso esta variável global ainda não tenha sido atualizada pela função leSuperBloco
		return NULL;

	/*Sobre os índices do for: começa lendo em numero_bloco*numero_setores porque um bloco possui um ou mais setores. Por exemplo,no caso de um bloco possuir 4 setores, e quisermos ler o 2° bloco (bloco 1), precisamos começar a ler a partir do quinto setor (setor 4). Neste caso, teremos que ler ATÉ o oitavo setor (setor 7), que é dado por numero_bloco*numero_setores+numero_setores (neste caso, 1*4+4 = 8, então ele lê até o 7)*/
	for(i=numero_bloco*numero_setores;i<numero_bloco*numero_setores+numero_setores;i++)
	{
		if(read_sector(i,buffer+offset)!=0) // Armazena os bytes dos primeiros setores nas primeiras posições do buffer
			return NULL;
		offset+=TAM_SETOR;
	}
	return buffer;
}

BOOL escreve_bloco(char* bloco, int numero_bloco)
{
	int numero_setores = tamanho_bloco / TAM_SETOR, setores_serem_escritos, setor_lido, i;
	
	if(strlen(bloco) > tamanho_bloco || tamanho_bloco==0)
		return FALSE;

	setores_serem_escritos = (int) strlen(bloco) / TAM_SETOR;
	if(strlen(bloco) % TAM_SETOR ==0)
		--setores_serem_escritos;

	for(i=0; i <= setores_serem_escritos; i++){
		setor_lido = numero_bloco*numero_setores;		
		if(write_sector(setor_lido,bloco+i*TAM_SETOR)!=0){
				return FALSE;
		}
	}
	return TRUE;
}

struct t2fs_record* leRegistroArquivo(char* buffer) 
{
	if(superBloco==NULL)
		leSuperBloco();

	struct  t2fs_record* regFile = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
	regFile->TypeVal = buffer[0];
	memcpy(regFile->name,buffer+1,39);
	regFile->blocksFileSize=(BYTE) buffer[43]<<24 | (BYTE) buffer[42]<<16 | (BYTE) buffer[41]<<8 | (BYTE) buffer[40];
	regFile->bytesFileSize=(BYTE) buffer[47]<<24 | (BYTE) buffer[46]<<16 | (BYTE) buffer[45]<<8 | (BYTE) buffer[44];
	regFile->dataPtr[0] = (BYTE) buffer[51]<<24 | (BYTE) buffer[50]<<16 | (BYTE) buffer[49]<<8 | (BYTE) buffer[48];
	regFile->dataPtr[1] = (BYTE) buffer[55]<<24 | (BYTE) buffer[54]<<16 | (BYTE) buffer[53]<<8 | (BYTE) buffer[52];
	regFile->singleIndPtr = (BYTE) buffer[59]<<24 | (BYTE) buffer[58]<<16 | (BYTE) buffer[57]<<8 | (BYTE) buffer[56];
	regFile->doubleIndPtr = (BYTE) buffer[63]<<24 | (BYTE) buffer[62]<<16 | (BYTE) buffer[61]<<8 | (BYTE) buffer[60];
	return regFile;
}

struct t2fs_superbloco* leSuperBloco(void)
{
	struct t2fs_superbloco* superbloco = (struct t2fs_superbloco*)malloc(sizeof(struct t2fs_superbloco));
	char* buffer = (char*) malloc(TAM_SETOR);
	if(read_sector(0,buffer)==0)
	{
		//Armazena Id
		memcpy (superbloco->Id, buffer, 4);

		//Armazena versao: 2014/1
		superbloco->Version=(BYTE) buffer[5]<<8 | (BYTE) buffer[4]; // É little endian

		//Armazena quantos setores formam o superbloco
		superbloco->SuperBlockSize = (BYTE) buffer[7]<<8 | (BYTE) buffer[6];

		//Armazena tamanho em bytes da partição
		superbloco->DiskSize = (BYTE) buffer[11]<<24 | (BYTE) buffer[10]<<16 | (BYTE) buffer[9]<<8 | (BYTE) buffer[8];

		//Armazena nro de blocos na partição (há 1024)
		superbloco->NofBlocks = (BYTE) buffer[15]<<24 | (BYTE) buffer[14]<<16 | (BYTE) buffer[13]<<8 | (BYTE) buffer[12]; 

		//Armazena tam. de um bloco
		superbloco->BlockSize = (BYTE) buffer[19]<<24 | (BYTE) buffer[18]<<16 | (BYTE) buffer[17]<<8 | (BYTE) buffer[16]; 

		//Armazena registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados
		// Tipo da entrada. 0xFF: não associado a nenhum arquivos | 1: arquivo regular | 2: diretório
		superbloco->BitMapReg.TypeVal = buffer[128];

		//Armazena nome do arquivo que mantém o bitmap de blocos livres e ocupados
		memcpy(superbloco->BitMapReg.name,buffer+129,39);

		//Tamanho do arquivo em número de blocos de dados
		superbloco->BitMapReg.blocksFileSize = (BYTE) buffer[171]<<24 | (BYTE) buffer[170]<<16 | (BYTE) buffer[169]<<8 | (BYTE) buffer[168];

		//Armazena o tamanho do arquivo em bytes do bitmap de blocos livres e ocupados
		superbloco->BitMapReg.bytesFileSize = (BYTE) buffer[175]<<24 | (BYTE) buffer[174]<<16 | (BYTE) buffer[173]<<8 | (BYTE) buffer[172];

		//Armazena os dois ponteiros diretos para blocos de dados do arquivo
		superbloco->BitMapReg.dataPtr[0] = (BYTE) buffer[179]<<24 | (BYTE) buffer[178]<<16 | (BYTE) buffer[177]<<8 | (BYTE) buffer[176];
		superbloco->BitMapReg.dataPtr[1] = (BYTE) buffer[183]<<24 | (BYTE) buffer[182]<<16 | (BYTE) buffer[181]<<8 | (BYTE) buffer[180];

		//Armazena ponteiro de indireção simples, aponta prum bloco de índices onde estão ponteiros para blocos de dados do arquivo
		superbloco->BitMapReg.singleIndPtr = (BYTE) buffer[187]<<24 | (BYTE) buffer[186]<<16 | (BYTE) buffer[185]<<8 | (BYTE) buffer[184];

		//Armazena ptr de indireção dupla, apontandi prum bloco de índices onde estão outros ponteiros para blocos de índice. Esses últimos ptrs apontam para blocos de dados do arquivo.
		superbloco->BitMapReg.doubleIndPtr = (BYTE) buffer[191]<<24 | (BYTE) buffer[190]<<16 | (BYTE) buffer[189]<<8 | (BYTE) buffer[188];

		//Armazena registro que descreve o arquivo que mantém as entradas do diretório raiz
		
		superbloco->RootDirReg.TypeVal = buffer[192];
		memcpy(superbloco->RootDirReg.name,buffer+193,39);
		superbloco->RootDirReg.blocksFileSize = (BYTE) buffer[235]<<24 | (BYTE) buffer[234]<<16 | (BYTE) buffer[233]<<8 | (BYTE) buffer[232];
		superbloco->RootDirReg.bytesFileSize = (BYTE) buffer[239]<<24 | (BYTE) buffer[238]<<16 | (BYTE) buffer[237]<<8 | (BYTE) buffer[236];
		superbloco->RootDirReg.dataPtr[0] = (BYTE) buffer[243]<<24 | (BYTE) buffer[242]<<16 | (BYTE) buffer[241]<<8 | (BYTE) buffer[240];
		superbloco->RootDirReg.dataPtr[1] = (BYTE) buffer[247]<<24 | (BYTE) buffer[246]<<16 | (BYTE) buffer[245]<<8 | (BYTE) buffer[244];
		superbloco->RootDirReg.singleIndPtr = (BYTE) buffer[251]<<24 | (BYTE) buffer[250]<<16 | (BYTE) buffer[249]<<8 | (BYTE) buffer[248];
		superbloco->RootDirReg.doubleIndPtr = (BYTE) buffer[255]<<24 | (BYTE) buffer[254]<<16 | (BYTE) buffer[253]<<8 | (BYTE) buffer[252];
	
		tamanho_bloco = superbloco->BlockSize;
		superBloco = superbloco;
		return superbloco;
	}
	return NULL;
}

struct t2fs_record get_registro_bitmap()
{
	//struct t2fs_superbloco* superbloco = leSuperBloco();
	return superbloco->BitMapReg;
}

<<<<<<< HEAD
BOOL caminhoExiste(char* caminho)
{

	return FALSE;
}
=======

void init(void)
{

	superbloco = leSuperBloco();
}



>>>>>>> master
