#include <stdio.h>
#include <stdlib.h>

char* get_message(char *string)
{
    int c;
    string = malloc(sizeof(char));
    if (string == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    string[0]='\0';

    for(int i=0; i<239 && (c=getchar())!='\n' && c != EOF ; i++)
    {
        string = realloc(string, (i+2)*sizeof(char));
        string[i] = (char) c; 
        string[i+1] = '\0'; 
    }

    return string;
}