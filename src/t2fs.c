#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/funcoesAuxiliares.h"
#include<stdio.h>
#include<stdlib.h>

char *t2fs_identify (void)
{
	return "Bruno Pinto Silva - 217446 \nPedro Henrique Arruda Faustini - 217432\n\0";
}

t2fs_file t2fs_create (char *nome)
{ // checar caminho valido,super bloco valido,bitmap valido
	//struct t2fs_record bloco = get_registro_bitmap();
	//printf("Ponteiro direto para bloco de dados do bitmap: %lX\n", bloco.dataPtr[0]);

	init();
	DWORD blocolivre = achablocolivre();
	printf("Bloco livre: %i\n", blocolivre);
	struct t2fs_record* record = (struct t2fs_record*) malloc(tamanho_bloco);
	record = inicializainode(nome);
	char* buffer = (char*) malloc(tamanho_bloco);
	buffer = inodeparachar(record);
	escreve_bloco(buffer, blocolivre);
	

return 1;
}

int t2fs_delete (char *nome)
{


	init();
	//recebo o nome do arquivo e através dele preciso achar o bloco do seu i-node
	
	DWORD *listablocos = (DWORD*) malloc(superbloco->NofBlocks);
	struct t2fs_record* record = (struct t2fs_record*)malloc(tamanho_bloco);
	record = carregaarquivo(superbloco->RootDirReg.dataPtr[0]);
	

	listablocos = listablocosarquivo(record, listablocos);
	excluiarquivobitmap(listablocos);

		

return 0;//arrumar retorno

}