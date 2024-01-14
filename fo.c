/********************************* includes ***********************************/


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>

#include <fileutils.h>
#include <dirutils.h>
#include <strutils.h>


/********************************* defs & globais *****************************/


#define PROGNAME "fo"

/* basedirname vai ter cópia do nome do
 * diretório alvo para puder montar caminho
 * de nome para o diretório base para os arquivos
 * que estiverem em subdiretórios. Confuso?? */
static char *basedirname;


/******************************** funções *************************************/


void usage()
{
    printf("Uso: %s <Diretório-Alvo>\n", PROGNAME);
}


/* Cuida de varrer o diretório alvo, distinguindo de arquivo
 * para subdiretório. Se for subdiretório, chama sweep_dir() (recursivo)
 * depois, verifica se arquivo tem extensão, e cria pasta em basedirname
 * com o nome da extensão, então vove o arquivo para a pasta */
void sweep_dir(char *dirname)
{
    DIR *dhandle = opendir(dirname);
    if (dhandle == NULL)
    {
        fprintf(stderr, "Erro ao abrir diretório %s\n", dirname);
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    struct stat item;

    while ((entry = readdir(dhandle)) != NULL)
    {
        /* função stat() espera receber caminho absoluto ou relativo */
        char *pathname;
        if ((pathname = create_pathname(dirname, entry->d_name)) == NULL)
        {
            fprintf(stderr, "Erro ao criar caminho para item %s\n",
                entry->d_name);
            closedir(dhandle);
            exit(EXIT_FAILURE);
        }

        if (stat(pathname, &item) == -1)
        {
            fprintf(stderr, "Erro ao obter informações sobre item %s\n",
                pathname);
            free(pathname);
            closedir(dhandle);
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(item.st_mode))
        {
            /* Aqui, descartamos os diretórios . e .., ex:
             * /home/usuario/teste/. e /teste/.. */
            if (rev_strstr(pathname, "/.", 2) == NULL &&
                rev_strstr(pathname, "/..", 3) == NULL)
            {
                sweep_dir(pathname);
            }
        }
        else if (S_ISREG(item.st_mode))
        {
            /* Só agimos sob arquivos com extensão */
            char *ext;
            if ((ext = has_extension(pathname)) != NULL)
            {
                /* Converte a primeira letra da extensão para maiúscula
                 * para criar o diretório Pdf ao invés de pdf. Opcional ..
                 * só comentar ou deletar se não interessa */
                title_case_word(ext);
                char *new_dir;
                if ((new_dir = create_pathname(basedirname, ext)) == NULL)
                {
                    fprintf(stderr, "Erro ao criar caminho para novo diretório\n");
                    free(pathname);
                    closedir(dhandle);
                    exit(EXIT_FAILURE);
                }

                /* Faz par com title_case_word(ext). Se não converter de volta
                 * depois de ter criado o caminho do diretório com maiuscula,
                 * quando mais a frente for renomear, vai assumir  aextensão com
                 * maiuscula e vai dar erro */
                *ext = tolower(*ext);
                if (is_valid_directory(new_dir) == false)
                {
                    if (is_directory_created(new_dir) == false)
                    {
                        fprintf(stderr, "Erro ao criar novo diretório %s\n",
                            new_dir);
                        free(pathname);
                        free(new_dir);
                        closedir(dhandle);
                        exit(EXIT_FAILURE);
                    }
                }

                char *new_entry_name;
                if ((new_entry_name = create_pathname(new_dir, entry->d_name)) == NULL)
                {
                    fprintf(stderr, "Erro ao criar caminho para novo arquivo\n");
                    free(pathname);
                    free(new_dir);
                    closedir(dhandle);
                    exit(EXIT_FAILURE);
                }

                if (rename(pathname, new_entry_name) == -1)
                {
                    fprintf(stderr, "Erro ao renomear de %s para %s\n",
                        pathname, new_entry_name);
                    free(pathname);
                    free(new_dir);
                    free(new_entry_name);
                    closedir(dhandle);
                    exit(EXIT_FAILURE);
                }

                free(new_dir);
                free(new_entry_name);
            }
        }

        free(pathname);
    }

    closedir(dhandle);
}


/********************************* entrada ************************************/


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage();
        return(1);
    }

    if (is_valid_directory(*(argv + 1)) == false)
    {
        fprintf(stderr, "Diretório especificado é inválido\n");
        return(1);
    }

    char *dirname = *(argv + 1);

    /* Cuida de remover a barra /home/usuario/teste/,
     * porque quando montamos os caminhos, incluimos a barra */
    remove_last_char(dirname, '/');
    basedirname = duplicate(dirname);
    sweep_dir(dirname);
    return(0);
}


/******************************************************************************/
