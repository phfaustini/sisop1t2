/*
    Remove um diretório, passado por caminho absoluto.
    Para que o diretório seja removido, ele deve estar vazio. 
        Se houver, erro indicativo..
    Todos os diretórios que compõem o caminho abosluto devem existir. 
        Se não, erro indicativo.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/t2fs.h"
#include "../include/funcoesAuxiliares.h"

int main(int argc, char *argv[])
{

    char* buffer = (char*) malloc(tamanho_bloco);
    struct t2fs_record* diretorio = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
    struct t2fs_record* record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
    int i, ptr1;
    DWORD* bloco;
    DWORD blocol;
    // Valida entrada
    if(2 != argc)
    {
        printf("\n Entrada invalida! Uso: rmdirt2 <diretorio logico> \n");
        return 1;
    }

    record = get_descritor_arquivo(argv[1]);


    if(record==NULL || record->TypeVal!=2)
    {
        printf("Caminho informado nao eh de diretorio valido!\n");
        return 0;
    }

    blocol=caminho_valido(argv[1]);
    bloco=&blocol;

    if(record->dataPtr[0]==-1 && record->dataPtr[1]==-1 && record->singleIndPtr==-1 && record->doubleIndPtr==-1)
    {
        t2fs_close(argv[1]);
        excluiarquivobitmap(bloco);
    }
    else printf("Diretorio nao vazio!\n");

    return 0;
}