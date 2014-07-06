#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/funcoesAuxiliares.h"
#include<stdio.h>
#include<stdlib.h>

WORD tamanho_bloco=0;

void printBloco(int bloco)
{
	printf("\nLENDO BLOCO %d:\n", bloco);
	le_bloco(bloco);
}

void printSetor(char* buffer, int inicio, int fim) //Estou chamando essa função na le_bloco, para printar o que é lido
{
	int i;
	for(i=inicio;i<fim;i++){
		printf("%c ", buffer[i]); 
	}
	printf("\nFim setor\n");
}

char* le_bloco(int numero_bloco)
{
	char* buffer = (char*) malloc(tamanho_bloco);
	int numero_setores = tamanho_bloco / TAM_SETOR, i, offset=0;

	if(tamanho_bloco==0)	//Caso esta variável global ainda não tenha sido atualizada pela função leSuperBloco
		return NULL;

	/*Sobre os índices do for: começa lendo em numero_bloco*numero_setores porque um bloco possui um ou mais setores. 
	Por exemplo,no caso de um bloco possuir 4 setores, e quisermos ler o 2° bloco (bloco 1), precisamos começar a ler a 
	partir do quinto setor (setor 4). Neste caso, teremos que ler ATÉ o oitavo setor (setor 7), que é dado por 
	numero_bloco*numero_setores+numero_setores (neste caso, 1*4+4 = 8, então ele lê até o 7)
	Por que o +1: o superbloco não conta*/
	for(i=numero_bloco*numero_setores+1;i<numero_bloco*numero_setores+numero_setores+1;i++)
	{
		if(read_sector(i,buffer+offset)!=0) // Armazena os bytes dos primeiros setores nas primeiras posições do buffer
			return NULL;
		//printSetor(buffer,offset,offset+TAM_SETOR);
		offset+=TAM_SETOR;
	}
	return buffer;
}

BOOL escreve_bloco(char* bloco, int numero_bloco)
{
	int numero_setores = tamanho_bloco / TAM_SETOR, setores_serem_escritos, setor_alvo, i;
	
	//Verifica integridades de variáveis e dados a serem escritos
	if(strlen(bloco) > tamanho_bloco || tamanho_bloco==0)
		return FALSE;

	/*Calcula quantos setores precisarão ser escritos. Sobre o if: ele subtrai 1 caso o número de setores 
	seja divisível pelo tamanho de um setor em bytes para evitar casos do tipo: 
	256 bytes a serem escritos vai em um byte. 256/256 tem resto 0. Logo, resultado será 1,e no for ele iria escrever
	em dois setores*/
	setores_serem_escritos = (int) strlen(bloco) / TAM_SETOR;
	if(strlen(bloco) % TAM_SETOR ==0)
		--setores_serem_escritos;

	for(i=0; i <= setores_serem_escritos; i++){
		setor_alvo = numero_bloco*numero_setores+1;		
		if(write_sector(setor_alvo, bloco+i*TAM_SETOR)!=0){
				return FALSE;
		}
	}
	return TRUE;
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
		/*printf("Nome do arquivo raiz: %s\n", superbloco->RootDirReg.name);
		printf("Endereco dele: %d\n", superbloco->RootDirReg.dataPtr[0]);
		printf("Nro blocos dele: %d\n", superbloco->RootDirReg.blocksFileSize);
		printf("Nro bytes dele: %d\n", superbloco->RootDirReg.bytesFileSize);*/
		tamanho_bloco = superbloco->BlockSize;
		return superbloco;
	}
	return NULL;
}

struct t2fs_record get_registro_bitmap()
{
	//struct t2fs_superbloco* superbloco = leSuperBloco();
	return superbloco->BitMapReg;
}
void imprime(unsigned char comp)//auxiliar para auxiliares- apagar depois
{
	int i  = 0;
	int v[8];

	for(;i<8;i++)
	{
		v[7-i] = (int)comp%2;
		comp = (int)comp/2;	
	}
	for(i = 0;i<8;i++)
	{
		printf("%i", v[i]);
	}
}

void init(void)
{

	superbloco = leSuperBloco();
	
}



//se retorna número do bloco se não achou retorna -1
int achabit(char *buffer,int *counter)
{

unsigned char aux;

unsigned char comparador = (char)(1<<7);

int i, j;

	for(i = 0; i < tamanho_bloco ; i++)
	{


		aux = buffer[i];
		comparador = (char)(1<<7);
		for(j = 0 ; j < 8 ; j++)
		{
		
			if(((unsigned char)aux & (unsigned char)comparador) == 0)
			{

				buffer[i] = (unsigned char)aux | (unsigned char)comparador;
				escreve_bloco(buffer, superbloco->BitMapReg.dataPtr[0]);
				return 1;
			}else
				{
				comparador=comparador>>(unsigned char)1;
				(*counter)++;
				}
		}
	}
return 0;
}


int achablocolivre()
{
	int counter = 0, ret = 0, i, j;
	char *buffer, *buffer2;
	//direto 1
	 buffer = le_bloco(superbloco->BitMapReg.dataPtr[0]);
	
	ret = achabit(buffer, &counter);	
	
	if(ret == 1 && counter < superbloco->DiskSize)
		return counter;

//mesmo que esse sistema tenha no máximo 1024 os outros bitmaps estão sendo feitos para deixar genético
if(superbloco->NofBlocks >= (8*superbloco->BlockSize))	//direto 2
{
	buffer = le_bloco(superbloco->BitMapReg.dataPtr[1]);
	ret = achabit(buffer, &counter);	

	if(ret == 1)
		return counter;
}

long int end;
if(superbloco->NofBlocks >= (8*superbloco->BlockSize*2))	//indireção simples
{
	
	buffer = le_bloco(superbloco->BitMapReg.singleIndPtr);
		//cada 4 posições formam um endereço de bloco
		for(i = 0; i<(tamanho_bloco/4);i+=4)
		{
			end = buffer[i]<<24 | buffer[i+1]<<16 | buffer[i+2]<<8| buffer[i+3];
			buffer = le_bloco(end);
			ret = achabit(buffer, &counter);	

			if(ret== 1)
				return counter;

		}

}
if(superbloco->NofBlocks >= (8*superbloco->BlockSize*2 + 8*superbloco->BlockSize*superbloco->BlockSize/4))	//indireção dupla
{
	buffer2 = le_bloco(superbloco->BitMapReg.doubleIndPtr);
	for(j = 0; j<(tamanho_bloco/4);j+=4)
		
		end = buffer2[j]<<24 | buffer2[j+1]<<16 | buffer2[j+2]<<8| buffer2[j+3];
		buffer = le_bloco(end);
		for(i = 0; i<(tamanho_bloco/4);i+=4)
		{
			end = buffer[i]<<24 | buffer[i+1]<<16 | buffer[i+2]<<8| buffer[i+3];
			buffer = le_bloco(end);
			ret = achabit(buffer, &counter);	

			if(ret == 1)
				return counter;

		}
}


return -1;
}


int conta_niveis_caminho(char* caminho)
{
	int i=1, niveis=0;
	if (caminho!=NULL && caminho[0]=='/')
	{
		while(caminho[i]!='\0')
		{
			if(caminho[i-1]=='/')
				niveis++;
			i++;
		}
		return niveis;
	}
	return -1;
}

int caminho_valido(char* caminho)
{
	int niveis;

	niveis = conta_niveis_caminho(caminho);
	
	if(niveis==-1) // Testa se o caminho está mal formatado
		return -1;

	return 1; //NÃOA CABEI ESSA FUNCAO
}