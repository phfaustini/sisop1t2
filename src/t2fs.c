#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/funcoesAuxiliares.h"
#include<stdio.h>
#include<stdlib.h>

char *t2fs_identify (void)
{
	return "Bruno Pinto Silva - 217446 \nPedro Henrique Arruda Faustini - 217432\nWilliam Weber Berruti - 205693\n\0";
}

t2fs_file t2fs_create (char *nome)
{ 
	//struct t2fs_record bloco = get_registro_bitmap();
	/*
	char* bitmap;
	printf("\nTamanho de um bloco: %d bytes\n", tamanho_bloco);
	printf("\nNome: %s\n", bloco.name);
	printf("Ponteiro direto para bloco de dados do bitmap: %lX\n", bloco.dataPtr[0]);
	printf("Tipo arquivo:: %d\n", bloco.TypeVal);
	printf("Numero de blocos: %d\n", bloco.blocksFileSize);
	printf("Tamnho em bytes: %d\n", bloco.bytesFileSize);
	bitmap = le_bloco(4);
	printf("Tamanho do bitmap: %d\n", sizeOf);*/
	//struct t2fs_superbloco* b = leSuperBloco(); 
	//printf("%d\n",b->bytesFileSize); //Raiz tem 256 bytes e fica no bloco 1. BitMap tem 128b. Root tem 256

	char* buffer = (char*) malloc(TAM_SETOR);
	if(read_sector(1,buffer)==0) // armazena em buffer o primeiro setor depois do superbloco
	{
			printf("\nO que diz o primeiro byte do primeiro setor apos o superbloco: %d\n", buffer[0]); // Primeiro setor depois do superbloco ta invalido
			struct t2fs_record* registro = leRegistroArquivo(buffer); 
			printf("Li este setor e mapeei para um t2fs_record\n");
			printf("O que diz o primeiro byte deste mapeamento: %d\n", registro->TypeVal;
	}
return 1;
}