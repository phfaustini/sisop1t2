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
	printBloco(2); // Recebe como parâmetro o bloco a ser printado.
	//printf("%d\n", caminho_valido("/teste1"));
	//printf("Bloco livre: %i\n", achablocolivre());



return 1;
}