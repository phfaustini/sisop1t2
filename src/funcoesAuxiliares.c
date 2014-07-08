#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/funcoesAuxiliares.h"
#include<stdio.h>
#include<stdlib.h>

void init(void)
{
    tamanho_bloco = 0;
	superbloco = leSuperBloco();
    inicializavetorabertos();
	
}



//se retorna número do bloco se não achou retorna -1
int achabit(char *buffer,int *counter)
{

    unsigned char aux;

    unsigned char comparador;

    int i, j;

    for(i = 0; i < tamanho_bloco && *counter < superbloco->NofBlocks; i++)
    {

        aux = buffer[i];
        comparador = (char)(1);
        for(j = 0 ; j < 8 && *counter < superbloco->NofBlocks; j++)
        {

            if(((unsigned char)aux & (unsigned char)comparador) == 0)
            {

                buffer[i] = (unsigned char)aux | (unsigned char)comparador;
                escreve_bloco(buffer, superbloco->BitMapReg.dataPtr[0]);
                return 1;
            }
            else
            {
                comparador=comparador<<(unsigned char)1;
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

    //mesmo que esse sistema tenha no máximo 1024 
    //os outros bitmaps estão sendo feitos para deixar genético
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
        for(i = 0; i<tamanho_bloco; i+=4)
        {
            end = buffer[i+3]<<24 | buffer[i+2]<<16 | buffer[i+1]<<8| buffer[i];
            buffer = le_bloco(end);
            ret = achabit(buffer, &counter);

            if(ret== 1)
                return counter;

        }

    }

    if(superbloco->NofBlocks >= (8*superbloco->BlockSize*2 + 8*superbloco->BlockSize*superbloco->BlockSize/4))	//indireção dupla
    {
        buffer2 = le_bloco(superbloco->BitMapReg.doubleIndPtr);
        for(j = 0; j<tamanho_bloco; j+=4)

            end = buffer2[j+3]<<24 | buffer2[j+2]<<16 | buffer2[j+1]<<8| buffer2[j];
        buffer = le_bloco(end);
        for(i = 0; i<(tamanho_bloco/4); i+=4)
        {
            end = buffer[i+3]<<24 | buffer[i+2]<<16 | buffer[i+1]<<8| buffer[i];
            buffer = le_bloco(end);
            ret = achabit(buffer, &counter);

            if(ret == 1)
                return counter;

        }
    }


    return -1;
}

DWORD* listablocosarquivo(struct t2fs_record *file, DWORD *lista)
{
	int i = 0, j = 0, k = 0;

	DWORD *aux, *aux2;

	char *buffer, *buffer2;
	if(file->dataPtr[0] != INVALIDO)
	{		
		lista[i++] = file->dataPtr[0];
	}else
		{
			lista[i] = -1;
			return lista;
		}
	if(file->dataPtr[1] != INVALIDO)
	{
		
		lista[i++] = file->dataPtr[1];
	}else
		{
			lista[i] = -1;
			return lista;
		}
	
	char auxbuffer[4];

	if(file->singleIndPtr != INVALIDO)
	{
		buffer = le_bloco(file->singleIndPtr);
		for(j = 0 ; j < tamanho_bloco;j+=4)
		{
            auxbuffer[0] = buffer[j];
            auxbuffer[1] = buffer[j+1];
            auxbuffer[2] = buffer[j+2];
            auxbuffer[3] = buffer[j+3];
            aux = (DWORD*)auxbuffer;
			
			if(*aux != INVALIDO)
			{
				lista[j++] = *aux;
			}
		}
	}else
		{
			lista[i] = -1;
			return lista;
		}
	
	

	if(file->doubleIndPtr != INVALIDO)
	{
		buffer = le_bloco(file->doubleIndPtr);
		for(j = 0 ; j < tamanho_bloco;j+=4)
		{
			auxbuffer[0] = buffer[j];
            auxbuffer[1] = buffer[j+1];
            auxbuffer[2] = buffer[j+2];
            auxbuffer[3] = buffer[j+3];
            aux = (DWORD*)auxbuffer;

			buffer2 = le_bloco(*aux);
			for(k = 0 ; k < tamanho_bloco;k++)
            {
                auxbuffer[0] = buffer2[k];
                auxbuffer[1] = buffer2[k+1];
                auxbuffer[2] = buffer2[k+2];
                auxbuffer[3] = buffer2[k+3];
                aux2 = (DWORD*)auxbuffer; 
                if(*aux2 != INVALIDO)
                {
                    lista[i++] = *aux2;
                }

            }
				
		}
	}else
		{
			lista[i] = -1;
			return lista;
		}

	return lista;
}

void excluiarquivobitmap(DWORD *lista)
{

	int b, os, i = 0;
	char* buffer = (char*) malloc(tamanho_bloco);
	buffer = le_bloco(superbloco->BitMapReg.dataPtr[0]);

	while(lista[i] != INVALIDO && i< superbloco->NofBlocks)
	{

		b = lista[i]/8;
		os = lista[i]%8;
		buffer[b] = buffer[b] & ~((1<<7)>>os);
		i++;
	}

	escreve_bloco(buffer,superbloco->BitMapReg.dataPtr[0]);
}

struct t2fs_record* carregaarquivo(DWORD bloco)
{

	char* buffer = (char*) malloc(tamanho_bloco);
	struct t2fs_record* record = (struct t2fs_record*) malloc(tamanho_bloco);
	int i;
	buffer = le_bloco(bloco);
    

	record->TypeVal = buffer[0];
	
	for(i = 1; i <= 39;i++)
	{
		record->name[i-1] = buffer[i];		
	}

	record->blocksFileSize = (buffer[43]<<24)|(buffer[42]<<16)|(buffer[41]<<8) | buffer[40];

	record->bytesFileSize = (buffer[47]<<24)|(buffer[46]<<16)|(buffer[45]<<8) | buffer[44];

	record->dataPtr[0] = (buffer[51]<<24)|(buffer[50]<<16)|(buffer[49]<<8) | buffer[48];
	
	record->dataPtr[1] = (buffer[55]<<24)|(buffer[54]<<16)|(buffer[53]<<8) | buffer[52];

	record->singleIndPtr = (buffer[59]<<24)|(buffer[58]<<16)|(buffer[57]<<8) | buffer[56];

	record->doubleIndPtr = (buffer[63]<<24)|(buffer[62]<<16)|(buffer[61]<<8) | buffer[60];

	return record;
}

struct t2fs_record* inicializainode(char* nome)
{

	struct t2fs_record* record = (struct t2fs_record*) malloc(tamanho_bloco);
	int i;
	
	record->TypeVal = 1;
	
	for(i = 0; i < 39;i++)
	{
		record->name[i] = nome[i];		
	}

	record->blocksFileSize = 0;

	record->bytesFileSize = 0;

	record->dataPtr[0] = INVALIDO;
	
	record->dataPtr[1] = INVALIDO;

	record->singleIndPtr = INVALIDO;

	record->doubleIndPtr = INVALIDO;

	return record;
}

char* inodeparachar(struct t2fs_record* record)
{
	char* buffer = (char*) malloc(tamanho_bloco);

	buffer[0] = record->TypeVal;
	int i;	
	for(i = 1; i <= 39;i++)
	{
		buffer[i]  = record->name[i-1];		
	}

	buffer[43] = record->blocksFileSize >> 24;
	buffer[42] = record->blocksFileSize >> 16;
	buffer[41] = record->blocksFileSize >> 8;
	buffer[40] = record->blocksFileSize;
	
	buffer[47] = record->bytesFileSize >> 24;
	buffer[46] = record->bytesFileSize >> 16;
	buffer[45] = record->bytesFileSize >> 8;
	buffer[44] = record->bytesFileSize;

	buffer[51] = record->dataPtr[0] >> 24;
	buffer[50] = record->dataPtr[0] >> 16;
	buffer[49] = record->dataPtr[0] >> 8;
	buffer[48] = record->dataPtr[0];

	buffer[55] = record->dataPtr[1] >> 24;
	buffer[54] = record->dataPtr[1] >> 16;
	buffer[53] = record->dataPtr[1] >> 8;
	buffer[52] = record->dataPtr[1];

	buffer[59] = record->singleIndPtr >> 24;
	buffer[58] = record->singleIndPtr >> 16;
	buffer[57] = record->singleIndPtr >> 8;
	buffer[56] = record->singleIndPtr;

	buffer[63] = record->doubleIndPtr >> 24;
	buffer[62] = record->doubleIndPtr >> 16;
	buffer[61] = record->doubleIndPtr >> 8;
	buffer[60] = record->doubleIndPtr;

	return buffer;
}


void inicializavetorabertos()
{
    int i;
    for(i = 0 ; i < 20;i++)
        arquivosabertos[i].handle = INVALIDO;

}

int marcarabertos(DWORD handle, DWORD currentpointer)
{
    int i;
    for(i = 0 ; i < 20 ; i++)
    {
        if(arquivosabertos[i].handle == INVALIDO)
        {
            arquivosabertos[i].handle = handle;
            arquivosabertos[i].currentpointer = currentpointer;
            return 1;
        }
    }
    return 0;

}

int alteracurentpoiter(DWORD handle, DWORD currentpointer)
{
    int i;
    for(i = 0 ; i < 20 ; i++)
    {
        if(arquivosabertos[i].handle == handle)
        {
            arquivosabertos[i].currentpointer = currentpointer;
            return 1;
        }
    }

    printf("ERRO: ARQUIVO NAO ESTA ABERTO");
    return 0;
}


DWORD* criablocoenderecos(DWORD *blocolivre)
{
    char* buffer = (char*) malloc(tamanho_bloco);

    DWORD aux = INVALIDO;

    int i = 0;

    buffer[0] = *blocolivre;
    buffer[1] = *blocolivre>>8;
    buffer[2] = *blocolivre>>16;
    buffer[3] = *blocolivre>>24;
    for(i = 4 ; i < tamanho_bloco; i+=4)
    {
        buffer[i] = aux;
        buffer[i+1] = aux>>8;
        buffer[i+2] = aux>>16;
        buffer[i+3] = aux>>24;
    }
    escreve_bloco(buffer, *blocolivre);

    return blocolivre;
}




char* nome_final(char* caminho)
{
    // /home/pedro/algo
    char* final;
    int i=1, niveis = conta_niveis_caminho(caminho);

    if(niveis!=-1)
    {
        while(niveis >= 1 && caminho[i]!='\0')
        {
            if(caminho[i-1]=='/')
            {
                niveis--;
            }
            i++;
        }
        i--;
        final = (char*)malloc((strlen(caminho+i)));
        strcpy(final,caminho+i);
        return final;
    }
    return NULL;
}

char* subcaminho(char* caminho, int j)
{
    int i=1, k=0;
    char* buffer = (char*)malloc(39);

    while(k<j)
    {
        if(caminho[i]=='/')
            k++;;
        i++;
    }
    k=0;
    while(caminho[i]!='/')
    {
        buffer[k]=caminho[i];
        i++;
        k++;
    }
    buffer[k]='\0';
    return buffer;
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

char* get_caminho_do_pai(char* caminho)
{
    char* buffer = (char*)malloc(tamanho_bloco);
    int niveis = conta_niveis_caminho(caminho), i=0,k=0;
    if(niveis>1)
    {
        while(i!=niveis)
        {
            buffer[k]=caminho[k];
            if(buffer[k]=='/')
                i++;
            k++;
        }
        buffer[k-1]='\0';

        return buffer;
    }
    else if(niveis==1)
        return "/";
    else return caminho;
}



struct t2fs_record* le_t2fs_record(char* buffer)
{
        struct t2fs_record* registro = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
        registro->TypeVal = buffer[0];
        memcpy(registro->name,buffer+1,39);
        registro->blocksFileSize = (BYTE) buffer[43]<<24 | (BYTE) buffer[42]<<16 | (BYTE) buffer[41]<<8 | (BYTE) buffer[40];
        registro->bytesFileSize = (BYTE) buffer[47]<<24 | (BYTE) buffer[46]<<16 | (BYTE) buffer[45]<<8 | (BYTE) buffer[44];
        registro->dataPtr[0] = (BYTE) buffer[51]<<24 | (BYTE) buffer[50]<<16 | (BYTE) buffer[49]<<8 | (BYTE) buffer[48];
        registro->dataPtr[1] = (BYTE) buffer[55]<<24 | (BYTE) buffer[54]<<16 | (BYTE) buffer[53]<<8 | (BYTE) buffer[52];
        registro->singleIndPtr = (BYTE) buffer[59]<<24 | (BYTE) buffer[58]<<16 | (BYTE) buffer[57]<<8 | (BYTE) buffer[56];
        registro->doubleIndPtr = (BYTE) buffer[63]<<24 | (BYTE) buffer[62]<<16 | (BYTE) buffer[61]<<8 | (BYTE) buffer[60];
        return registro;
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

char* le_bloco(int numero_bloco)
{
    char* buffer = (char*) malloc(tamanho_bloco);
    int numero_setores = tamanho_bloco / TAM_SETOR, i, offset=0;

    if(tamanho_bloco==0)    //Caso esta variável global ainda não tenha sido atualizada pela função leSuperBloco
        return NULL;

    /*Sobre os índices do for: começa lendo em numero_bloco*numero_setores porque um bloco possui um ou mais setores.
    Por exemplo,no caso de um bloco possuir 4 setores, e quisermos ler o 2° bloco (bloco 1), precisamos começar a ler a
    partir do quinto setor (setor 4). Neste caso, teremos que ler ATÉ o oitavo setor (setor 7), que é dado por
    numero_bloco*numero_setores+numero_setores (neste caso, 1*4+4 = 8, então ele lê até o 7)
    Por que o +1: o superbloco não conta*/
    for(i=numero_bloco*numero_setores+1; i<numero_bloco*numero_setores+numero_setores+1; i++)
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

    for(i=0; i <= setores_serem_escritos; i++)
    {
        setor_alvo = numero_bloco*numero_setores+1;
        if(write_sector(setor_alvo, bloco+i*TAM_SETOR)!=0)
        {
            return FALSE;
        }
    }
    return TRUE;
}

struct t2fs_record get_registro_bitmap()
{
//struct t2fs_superbloco* superbloco = leSuperBloco();
    return superbloco->BitMapReg;
}

struct t2fs_record get_registro_raiz()
{
    return superbloco->RootDirReg;
}

struct registro_bloco* procura_descritor_num_bloco_diretorio(char* nome, DWORD bloco)
{
    int i, arquivos_por_bloco = tamanho_bloco / TAM_REG;
    char* buffer;
    struct registro_bloco* descritor = (struct registro_bloco*)malloc(sizeof(struct registro_bloco));

    if(nome!=NULL)
    {
        buffer = le_bloco(bloco);
        for(i=0; i<arquivos_por_bloco; i++)
        {
            descritor->registro = le_t2fs_record(buffer + i*TAM_REG);
            if(strcmp(descritor->registro->name,nome)==0)
            {
                descritor->bloco=bloco;
                return descritor;
            }
        }
        return NULL;
    }
    return NULL;
}




//Precisa de TESTES com arquivos que ocupem mais de um bloco
struct registro_bloco* procura_descritor_num_diretorio(char* nome, DWORD* offset, struct registro_bloco* descritor)
{
    int i=0;
    struct registro_bloco* descr = (struct registro_bloco*)malloc(sizeof(struct registro_bloco));
    char* buffer;
    long int end;

    if(nome!=NULL)
    {
        if(descritor->registro->dataPtr[0]!=INVALIDO){
            descr=procura_descritor_num_bloco_diretorio(nome, descritor->registro->dataPtr[0]);
            if(descr!=NULL)
            {
                descr->bloco=descritor->registro->dataPtr[0];
                return descr;
            }
        }
        if(descritor->registro->dataPtr[1]!=INVALIDO)
        {
            descr=procura_descritor_num_bloco_diretorio(nome, descritor->registro->dataPtr[1]);
            if(descr!=NULL)
            {
                descr->bloco=descritor->registro->dataPtr[1];
                return descr;
            }
        }
        if(descritor->registro->singleIndPtr!=INVALIDO)// A partir daqui procura na indireção simples
        {
            buffer = le_bloco(descritor->registro->singleIndPtr);
            for(i = 0; i<(tamanho_bloco); i+=4) //cada 4 posições formam um endereço de bloco
            {
                end = buffer[i+3]<<24 | buffer[i+2]<<16 | buffer[i+1]<<8| buffer[i];
                descr=procura_descritor_num_bloco_diretorio(nome, end);
                if(descr!=NULL)
                {
                    descr->bloco = descritor->registro->singleIndPtr;
                    return descr;
                }
            }      
        }
        if(descritor->registro->doubleIndPtr!=INVALIDO)
        {                                
            buffer = le_bloco(descritor->registro->doubleIndPtr);
            for(i = 0; i< tamanho_bloco ; i+=4)
                end = buffer[i+3]<<24 | buffer[i+2]<<16 | buffer[i+1]<<8| buffer[i];
                        
            buffer = le_bloco(end);
            for(i = 0; i<(tamanho_bloco/4); i+=4)
            {
                end = buffer[i+3]<<24 | buffer[i+2]<<16 | buffer[i+1]<<8| buffer[i];
                descr=procura_descritor_num_bloco_diretorio(nome, end);
                if(descr!=NULL)
                {
                    descr->bloco = descritor->registro->doubleIndPtr;
                    return descr;
                }
            }                 
        }
        return NULL;
    }
    return NULL;
}


//Acho que ta ok
DWORD procura_descritores(int niveis, char* caminho, char* final, struct registro_bloco* descritor)//Assume que recebe níveis válido. TO NESSA
{
    int i;
    DWORD* ptr = (DWORD*)malloc(sizeof(DWORD)); // Sugestão no Nicolas, ainda não usei

    /*Vai ser uma busca para cada nível no caminho (e.g. /home/pedro/dir tem 3 níveis, logo serão 3 iterações no for)
    Até a penúltima busca, se houver algum erro é porque o caminho passado é inválido, então retorna -1;
    Quando chegar na última busca (quando cai no else) o resultado não pode mais ser caminho inválido: ou
    o arquivo está lá ou não está. Se não estiver, retorna 0 (significa que há 0 arquivos com esse nome nesse caminho).
    Senão, o for termina e o retorno é 1 (significando que há 1 arquivo com esse nome nesse caminho)*/
    for(i=0; i<niveis; i++)
    {
        if(i<niveis-1)
        {
    //e.g. se o caminho for /home/pedro/dir, e i=1, ele procurará a pasta 'pedro'
    //Dúvida: dataPtr[1] é usado quando??
            descritor=procura_descritor_num_diretorio(subcaminho(caminho,i), ptr, descritor);
            if(descritor==NULL)
            {
                return -1;
            }
        }
        else
        {
            descritor=procura_descritor_num_diretorio(final, ptr, descritor);
            if(descritor==NULL)
            {
                return 0;
            }
        }
    }
    return descritor->bloco;
}

DWORD caminho_valido(char* caminho)
{
    int niveis;
    struct t2fs_record raiz = get_registro_raiz();
    struct registro_bloco* descritor = (struct registro_bloco*)malloc(sizeof(struct registro_bloco));
    descritor->registro=&raiz;

    niveis = conta_niveis_caminho(caminho);
    if(niveis==-1) // Testa se o caminho está mal formatado
        return -1;
    descritor->bloco=procura_descritores(niveis,caminho,nome_final(caminho),descritor);
    return descritor->bloco;
}

struct t2fs_record* get_descritor_arquivo(char* caminho)
{
    int niveis;
    struct t2fs_record raiz = get_registro_raiz();
    struct registro_bloco* descritor = (struct registro_bloco*)malloc(sizeof(struct registro_bloco));
    struct t2fs_record* arquivo = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
    descritor->registro=&raiz;

    if(caminho_valido(caminho)>0)
    {
        niveis = conta_niveis_caminho(caminho);
        if(niveis==-1) // Testa se o caminho está mal formatado
            return NULL;
        arquivo = procura_descritores2(niveis,caminho,nome_final(caminho),descritor);
        return arquivo;
    }
    else return NULL;
}

struct t2fs_record* procura_descritores2(int niveis, char* caminho, char* final, struct registro_bloco* descritor)//Assume que recebe níveis válido.
{
    int i;
    DWORD* ptr = (DWORD*)malloc(sizeof(DWORD)); // Sugestão no Nicolas, ainda não usei

    for(i=0; i<niveis; i++)
    {
        if(i<niveis-1)
        {
            descritor=procura_descritor_num_diretorio(subcaminho(caminho,i), ptr, descritor);
            if(descritor==NULL)
                return NULL;
        }
        else
        {
            descritor=procura_descritor_num_diretorio(final, ptr, descritor);
            if(descritor==NULL)
                return NULL;
        }
    }
    return descritor->registro;
}