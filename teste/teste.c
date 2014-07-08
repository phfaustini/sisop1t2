#include "../include/t2fs.h"
#include<stdio.h>
#include<stdlib.h>
/*Funciona com o .dat no mesmo diretório do executável*/

int main(int argc, char *argv[])
{
	char *nome;	
	nome = "/";
	
	//t2fs_delete (nome);
	//t2fs_create (nome);
	printf("%d\n", t2fs_close(t2fs_open(nome)));
	return 0;
}
