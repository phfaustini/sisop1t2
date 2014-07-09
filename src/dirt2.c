/*
    Lista o conteúdo do diretório fornecido no caminho fornecido (absoluto). 
    Se por um acaso não existir diretórios que aparecem no meio do string de caminho, 
        o programa dirt2 retorna um mensagem de erro. 
    A saída desse programa é a listagem do conteúdo dos registros do arquivo de diretório 
        formatado para apresentar, na ordem:
            nome do arquivo
            indicação se é um arquivo regular (r) ou diretório (d)
            quantidade de blocos que utiliza
            tamanho do arquivo em bytes.
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

    // Valida entrada
    if(2 != argc)
    {
        printf("\n Erro! Correto: dirt2 <caminho_completo> \n");
        return 1;
    }


    record = get_descritor_arquivo(argv[1]);

    if(record==NULL || record->TypeVal!=2)
    {
        printf("Caminho informado nao eh de diretorio valido!\n");
        return 0;
    }
    ptr1=record->dataPtr[1];

    if(record->dataPtr[0]!= -1)
    {
        buffer=le_bloco(record->dataPtr[0]);
        for(i=0; i< tamanho_bloco  ; i+=64)
        {
            diretorio = le_t2fs_record(buffer + i);
            if(diretorio->TypeVal == 2)
            {
                printf("%s   d   %d   %d bytes\n",diretorio->name, diretorio->blocksFileSize, diretorio->bytesFileSize );
            }
            else if(diretorio->TypeVal == 1)
            {
                printf("%s   r   %d   %d bytes\n",diretorio->name, diretorio->blocksFileSize, diretorio->bytesFileSize );
            }
        }
    }
    
    record->dataPtr[1] = ptr1;
    if(record->dataPtr[1]!= -1)
    {
        buffer=le_bloco(record->dataPtr[1]);
        for(i=0; i< tamanho_bloco  ; i+=64)
        {
            diretorio = le_t2fs_record(buffer + i);
            if(diretorio->TypeVal == 2)
            {
                printf("%s   d   %d   %d bytes\n",diretorio->name, diretorio->blocksFileSize, diretorio->bytesFileSize );
            }
            else if(diretorio->TypeVal == 1)
            {
                printf("%s   r   %d   %d bytes\n",diretorio->name, diretorio->blocksFileSize, diretorio->bytesFileSize );
            }
        }
    }

    return 0;
}