#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int len;
    scanf("%d", &len);
    fgetc(stdin);

    char buffer[len];
    char *palavra = NULL;
    int tam_palavra = 0;

    fgets(buffer, len + 1, stdin);
    buffer[len] = '\0';

    char *delim = {" "};
    char *token = strtok(buffer, delim);
    char **palavras = NULL;
    int qntd_palavras = 0;
    while (token != NULL)
    {
        printf("%s\n", token);

        palavras = (char **)realloc(palavras, sizeof(char *) * (++qntd_palavras));
        palavras[qntd_palavras] = (char *)malloc(sizeof(char) * strlen(token));
        strcpy(palavras[qntd_palavras - 1], token);

        token = strtok(NULL, delim);
    }
}