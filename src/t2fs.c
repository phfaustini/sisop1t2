#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/funcoesAuxiliares.h"
#include<stdio.h>
#include<stdlib.h>

char *t2fs_identify (void)
{
	return "Bruno Pinto Silva - 217446 \nPedro Henrique Arruda Faustini - 217432\n\0";
}

/*t2fs_file t2fs_create (char *nome)
{ // checar caminho valido,super bloco valido,bitmap valido

	init();
	int validadecaminho;
	DWORD blocolivre;
	struct t2fs_record* d = (struct t2fs_record*)malloc(sizeof(struct t2fs_record)); //procura_descritor_num_diretorio("teste1",(DWORD*)malloc(sizeof(DWORD)), superbloco->BitMapReg);
	struct t2fs_record* record = (struct t2fs_record*) malloc(tamanho_bloco);
	char* buffer = (char*) malloc(tamanho_bloco);

	validadecaminho = caminho_valido(nome);

	
	
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
}*/

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


int t2fs_close(t2fs_file handle)
{
	int i;

	init();

	for(i=0;i<20;i++)
	{
		if(arquivosabertos[i].handle == handle)
		{
			arquivosabertos[i].handle=INVALIDO;
			return 0;
		}
	}
	return INVALIDO;
}

t2fs_file t2fs_open(char* nome)
{
	init();

	t2fs_file handle = caminho_valido(nome);
	switch(handle)
	{
		case -2: 
				return INVALIDO;
		case -1: 
				return INVALIDO;
		default:
				if(marcarabertos(handle,0)==1){
					return handle;
				}
				else return INVALIDO;
	}
}

int t2fs_seek (t2fs_file handle, unsigned int offset)
{
	int i;
	for(i=0;i<20;i++)
	{
		if (arquivosabertos[i].handle == handle)
		{
			struct t2fs_record* record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
			record = carregaarquivo(handle);
			if(offset > record->bytesFileSize || offset < -1)
				return INVALIDO;
			if(offset >= 0)
			{
				alteracurentpoiter(handle,offset);
				return 0;
			}
			if(offset == -1)
			{
				return INVALIDO; //sim, isso ta errado, tenho que mudar
			}

		}
	}
	return INVALIDO;

}