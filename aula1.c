#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {Q0, Q1, Q2, Qlixo} estado;

int ehLetra(char c) {
    if (isalpha(c))
        return 1;
    return 0;
}

int ehDigito(char c) {
    if (isdigit(c))
        return 1;
    return 0;
}

int DFA(char* cadeia){

    estado e = Q0;
    char c = cadeia[0];
    int i;

    if(!ehLetra(c)){
        e = Qlixo;
    } else {
        e = Q1;
    }

    for (i = 1; cadeia[i] != '\0'; i++)
    {
        c = cadeia[i];
        if (ehLetra(c) || ehDigito(c))
        {
            e = Q1;
        } else {
            e = Q2;
            break;
        }
    }

    if(e == Q1 && (cadeia[i] == '\0')){
        e = Q2;
    } else {
        e = Qlixo;
    }
        
    
}

int main() {
    char* String = "curso2024";
    int resultado = DFA(String);
    if (resultado == 1) {
        printf("A cadeia '%s' é válida.\n", String);
    } else {
        printf("A cadeia '%s' é inválida.\n", String);
    }
    return 0;
}