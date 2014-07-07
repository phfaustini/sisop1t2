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

	//printBloco(8); // Recebe como parâmetro o bloco a ser printado.
	//printf("%d\n", caminho_valido("/teste1"));
	//printf("Bloco livre: %i\n", achablocolivre());
	
	struct t2fs_record* d = (struct t2fs_record*)malloc(sizeof(struct t2fs_record)); //procura_descritor_num_diretorio("teste1",(DWORD*)malloc(sizeof(DWORD)), superbloco->BitMapReg);
	printf("%d \n",caminho_valido("/dir1/teste4-dir1"));
	d = get_descritor_arquivo("/dir1/teste4-dir1");
	if(d!=NULL)
		printf(" %s\n", d->name);
	//printBloco(1);

	//printf("Bloco livre: %i\n", achablocolivre());


return 1;
}