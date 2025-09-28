// analisador_lexico.c: Este arquivo contém a função 'main' para o analisador léxico simples em C.
// O código converte o original C++ para C padrão, usando 'int' em vez de 'bool'.

#include <stdio.h> // Para printf, NULL
#include <string.h> // Para strlen, strcat, strcpy, strchr

// Estados do AFD
typedef enum { Q0, Q1, Q2, QLixo } Estado;

// Estrutura para representar um lexema (token)
typedef struct Lexema {
    char tipo[50];
    char valor[50];
} Lexema;

// Função para verificar se um dado caracter c é 
// um caracter alfabético: [a-zA-Z]
// Retorna 1 (verdadeiro) ou 0 (falso)
int eh_letra(char c) {
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
        return 1;
    }
    return 0;
}

// Função para verificar se um dado caracter c é 
// um caracter numérico: [0-9]
// Retorna 1 (verdadeiro) ou 0 (falso)
int eh_digito(char c) {
    if ((c >= '0') && (c <= '9')) {
        return 1;
    }
    return 0;
}

// Programação do autômato AFD_ID: [a-zA-Z]([a-zA-Z0-9]*)
// Retorna o índice do próximo caractere a ser lido (estratégia lookahead)
int AFD_ID(char* cadeia, int verbose, int i, Lexema *lexema, int *k) {

    char traco[1000] = ""; // Rastreamento dos estados (para o modo verbose)

    Estado q = Q0; // Estado inicial
    if (verbose) {
        strcat(traco, "Q0");
    }

    char c = cadeia[i];
    int start_i = i; // Guarda o índice inicial

    if (!eh_letra(c)) {
        q = QLixo;
        if (verbose) {
            strcat(traco, "->QLixo");
        }
    }
    else {
        // Encontrou o primeiro caractere válido (uma letra)
        strcpy(lexema->tipo, "ID");

        int i_lexema = 0;

        lexema->valor[i_lexema] = c;
        i_lexema = i_lexema + 1;

        q = Q1; // Transição para Q1
        if (verbose) {
            strcat(traco, "->Q1");
        }
        i = i + 1; // Move para o próximo caractere
        
        // Loop para ler os caracteres subsequentes ([a-zA-Z0-9]*)
        // Continua enquanto o caractere atual for letra ou dígito e não for o fim da string
        while (cadeia[i] != '\0' && (eh_letra(cadeia[i]) || eh_digito(cadeia[i]))) {
            c = cadeia[i]; 

            q = Q1; // Permanece em Q1 (estado de aceitação temporário)
            if (verbose) {
                strcat(traco, "->Q1");
            }
            lexema->valor[i_lexema] = c;
            i_lexema = i_lexema + 1;
            i = i + 1;
        }

        // Se o loop terminou, o caractere atual não é uma letra nem um dígito,
        // ou chegamos ao fim da cadeia.
        
        // Se o estado anterior não foi QLixo, o lexema foi reconhecido.
        if (q != QLixo) {
             q = Q2; // Transição final (estado de aceitação)
             if (verbose) {
                 strcat(traco, "->Q2");
             }
             // i já está posicionado no caractere lookahead (o primeiro que não pertence ao lexema).
             lexema->valor[i_lexema] = '\0';
             *k = *k + 1; // Incrementa o contador de lexemas
        } else {
             // Se o primeiro caractere não era letra, retornamos o índice inicial
             i = start_i; 
        }
    }

    if (verbose) {
        printf("\nTraco: %s\n", traco);
        printf("\nLexema: <%s,%s>\n", lexema->tipo, lexema->valor);
    }
    
    // Retorna o índice do próximo caractere a ser processado
    return i; 
}

// Função auxiliar para verificar se o caractere é um símbolo simples.
// Retorna 1 (verdadeiro) ou 0 (falso)
int eh_simbolo(char c) {
    // c in "+-*/;=>"
    if (strchr("+-*/;=>", c) != NULL) { 
        return 1;
    }
    return 0;
}

// Programação do autômato AFD_simbolos: [+ | - | * | / | ; | = | >]
// Retorna o índice do próximo caractere a ser lido
int AFD_simbolos(char* cadeia, int verbose, int i, Lexema *lexema, int *k) {

    char traco[1000] = "";

    Estado q = Q0;
    if (verbose) {
        strcat(traco, "Q0");
    }

    char c = cadeia[i];
    int start_i = i; // Guarda o índice inicial

    if (eh_simbolo(c)) {
        // Reconheceu o símbolo
        lexema->tipo[0] = c; 
        lexema->tipo[1] = '\0';

        lexema->valor[0] = c;
        lexema->valor[1] = '\0';
        
        i = i + 1; // Estratégia lookahead: avança para o próximo caractere
        *k = *k + 1; // Lexema reconhecido
        
        q = Q1; // Estado de aceitação
        if (verbose) {
            strcat(traco, "->Q1");
        }
    }
    else {
        q = QLixo;
        if (verbose) {
            strcat(traco, "->QLixo");
        }
    }

    if (verbose) {
        printf("\nTraco: %s\n", traco);
        printf("\nLexema: <%s,%s>\n", lexema->tipo, lexema->valor);
    }
    
    // Se não reconheceu o símbolo, retorna o índice inicial
    if (q == QLixo) {
        return start_i;
    }

    return i;
}

// Função principal de varredura (scanner)
void varredura(char* cadeia) {

    // Array para armazenar os lexemas reconhecidos (máximo de 100)
    Lexema lexema[100];
    int k = 0; // Contador de lexemas

    for (int i = 0; cadeia[i] != '\0'; ) {

        int i_before = i; // Guarda a posição antes da tentativa de reconhecimento

        // 1. Ignorar espaços em branco
        if (cadeia[i] == ' ') {
            i = i + 1;
            continue;
        }

        // 2. Tentar reconhecer um ID
        i = AFD_ID(cadeia, 0, i, &lexema[k], &k);
        // Se AFD_ID avançou o índice (i > i_before), ele reconheceu um token ID, 
        // e o contador k já foi incrementado dentro da função.
        if (i > i_before) {
            continue;
        }
        
        // 3. Tentar reconhecer um Símbolo (se AFD_ID falhou)
        i = AFD_simbolos(cadeia, 0, i, &lexema[k], &k);
        // Se AFD_simbolos avançou o índice (i > i_before), ele reconheceu um Símbolo.
        if (i > i_before) {
            continue;
        }
        
        // Se i não avançou após todas as tentativas, é um erro léxico ou caractere não reconhecido
        if (i == i_before) {
            printf("\nERRO LEXICO: Caractere nao reconhecido na posicao %d: '%c'\n", i, cadeia[i]);
            // Avança para tentar recuperar do erro (opcional, dependendo da estratégia de erro)
            i++; 
        }

        /*
        * NOTA: Os exercícios 1 a 6 abaixo requerem a adição de novos AFDs
        * ou a modificação da lógica de varredura (varredura).
        * Para que o analisador funcione, a lógica de varredura deve 
        * aplicar todos os AFDs em ordem de prioridade.
        */

        /* Exercício 1: Projetar e implementar um AFD que reconheça
        * as palavras da linguagem {if, repeat, until, end, write, read}.
        */

        /* Exercício 2: Confeccionar um AFD que reconheça comentários dados
        * entre {} desde que sejam simples (não-aninhados).
        */

        /* Exercício 3: Confeccionar um AFD que reconheça números naturais
        * não sinalizados.
        */

        /* Exercício 4: Confeccionar um AFD que reconheça "atribuições 
        * pascalinas/pascal-like" (:=).
        */

        /* Exercício 5: Conferir as restrições/definições da linguagem TINY
        * (pascalZIM) do livro do K. Louden e implementar um analisador
        * léxico para a mesma e avaliá-lo sobre o código de cálculo de
        * fatorial.
        */

        /* Exercício 6: Converter o analisador léxico para C. (FEITO)
        */
    }

    // Imprime a lista final de lexemas
    printf("\n--- Lexemas Encontrados (%d) ---\n", k);
    for(int j = 0; j < k; j++) {
        printf("Lexema %d: <%s, %s>\n", j + 1, lexema[j].tipo, lexema[j].valor);
    }
    printf("--------------------------------\n");

}

int main() {
    // A string de teste original continha um símbolo '=' que era tratado como ID no AFD_ID
    // e depois como símbolo. Ajustei a varredura para priorizar a maior leitura possível.
    // E ajustei a string de teste para testar IDs e Símbolos
    char teste[100] = "var1 = repeat + end; >";

    varredura(teste);

    return 0;
}