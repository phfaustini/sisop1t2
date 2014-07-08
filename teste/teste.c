#include "../include/t2fs.h"
#include<stdio.h>
#include<stdlib.h>
/*Funciona com o .dat no mesmo diretório do executável*/

int main(int argc, char *argv[])
{
	char *nome;	
	nome = "/";
	
	//t2fs_delete (nome);
	t2fs_create (nome);
	return 0;
}
