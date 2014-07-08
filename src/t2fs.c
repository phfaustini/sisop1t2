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

	init();
	int validadecaminho;
	DWORD blocolivre;
	struct t2fs_record* d = (struct t2fs_record*)malloc(sizeof(struct t2fs_record)); //procura_descritor_num_diretorio("teste1",(DWORD*)malloc(sizeof(DWORD)), superbloco->BitMapReg);
	struct t2fs_record* record = (struct t2fs_record*) malloc(tamanho_bloco);
	char* buffer = (char*) malloc(tamanho_bloco);

	//validadecaminho = caminho_valido(nome);


	switch(validadecaminho)
	{
		case -1:
			printf("CAMINHO FORNECIDO INVALIDO.\n");
			return INVALIDO;
			break;
		case 0:
			blocolivre = achablocolivre();
			if(!marcarabertos(blocolivre, 0))
			{
				printf("ATINGIDO O LIMITE MAXIMO DE ARQUIVOS ABERTOS.\n");
				return INVALIDO;

			}

			//colocar bloco livre no diretório pai
			record = inicializainode(nome);
			buffer = inodeparachar(record);
			escreve_bloco(buffer, blocolivre);
			break;
		default:
			d = get_descritor_arquivo(nome);
			t2fs_delete(d->name);
			blocolivre = achablocolivre();
			if(!marcarabertos(blocolivre, 0))
			{
				printf("ATINGIDO O LIMITE MAXIMO DE ARQUIVOS ABERTOS.\n");
				return INVALIDO;

			}
			//colocar bloco livre no diretório pai
			record = inicializainode(nome);
			buffer = inodeparachar(record);
			escreve_bloco(buffer, blocolivre);

			break;
	}

	return 1;
}

int t2fs_delete (char *nome)
{


	init();
	DWORD *listablocos = (DWORD*) malloc(superbloco->NofBlocks);
	struct t2fs_record* record = (struct t2fs_record*)malloc(tamanho_bloco);
	
	//record = carregaarquivo(caminho_valido(nome));
	listablocos = listablocosarquivo(record, listablocos);
	excluiarquivobitmap(listablocos);

		

return 0;//arrumar retorno

}