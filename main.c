#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct {
    char titulo[50];
    char genero[30];
    int ano;
    float nota;
    char status; // A = ativo | E = excluído
} Filme;

// PROTÓTIPOS
void limpaBuffer();
void lerString(char*, int);
int tamanho(FILE*);
void cadastrar(FILE*);
void consultar(FILE*);
void listar(FILE*);
void excluir(FILE*);

// Relatórios
void gerarRelatorio(FILE*);
void gerarRelatorioCompleto(FILE*);

// ----------------------------------------------------------

int main() {
    setlocale(LC_ALL, "portuguese");

    FILE *arq = fopen("filmes.bin", "r+b");
    if (!arq) {
        arq = fopen("filmes.bin", "w+b");
        if (!arq) {
            printf("Erro ao abrir o arquivo!\n");
            return 1;
        }
    }

    int opcao;
    do {
        printf("\n===== GERENCIADOR DE FILMES =====\n");
        printf("1 - Cadastrar filme\n");
        printf("2 - Consultar por índice\n");
        printf("3 - Listar todos os filmes\n");
        printf("4 - Excluir filme\n");
        printf("5 - Quantidade de registros\n");
        printf("6 - Gerar relatório (Ativos)\n");
        printf("7 - RELATÓRIO COMPLETO\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {
            case 1: cadastrar(arq); break;
            case 2: consultar(arq); break;
            case 3: listar(arq); break;
            case 4: excluir(arq); break;
            case 5: printf("Total de registros: %d\n", tamanho(arq)); break;
            case 6: gerarRelatorio(arq); break;
            case 7: gerarRelatorioCompleto(arq); break;
            case 0: printf("Encerrando...\n"); break;
            default: printf("Opção inválida!\n");
        }

    } while (opcao != 0);

    fclose(arq);
    return 0;
}

// ==========================================================
// FUNÇÕES BÁSICAS
// ==========================================================

void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerString(char *str, int tam) {
    fgets(str, tam, stdin);
    str[strcspn(str, "\n")] = '\0';
}

int tamanho(FILE *arq) {
    fseek(arq, 0, SEEK_END);
    return ftell(arq) / sizeof(Filme);
}

// ----------------------------------------------------------

void cadastrar(FILE *arq) {
    Filme f;

    printf("\nTítulo: ");
    lerString(f.titulo, 50);

    printf("Gênero: ");
    lerString(f.genero, 30);

    printf("Ano: ");
    scanf("%d", &f.ano);
    limpaBuffer();

    printf("Nota (0 a 10): ");
    scanf("%f", &f.nota);
    limpaBuffer();

    f.status = 'A';

    fseek(arq, 0, SEEK_END);
    fwrite(&f, sizeof(Filme), 1, arq);

    printf("\nFilme cadastrado!\n");
}

// ----------------------------------------------------------

void consultar(FILE *arq) {
    int index, realIndex;
    Filme f;

    printf("\nNúmero do filme (começa em 1): ");
    scanf("%d", &index);
    limpaBuffer();

    realIndex = index - 1;

    if (realIndex < 0 || realIndex >= tamanho(arq)) {
        printf("Índice inválido!\n");
        return;
    }

    fseek(arq, realIndex * sizeof(Filme), SEEK_SET);
    fread(&f, sizeof(Filme), 1, arq);

    if (f.status == 'E') {
        printf("Este filme foi excluído.\n");
        return;
    }

    printf("\n--- FILME %d ---\n", index);
    printf("Título: %s\n", f.titulo);
    printf("Gênero: %s\n", f.genero);
    printf("Ano: %d\n", f.ano);
    printf("Nota: %.1f\n", f.nota);
    printf("Status: %c\n", f.status);
}

// ----------------------------------------------------------

void listar(FILE *arq) {
    Filme f;
    int total = tamanho(arq);

    printf("\n===== LISTA DE FILMES =====\n");

    for (int i = 0; i < total; i++) {
        fseek(arq, i * sizeof(Filme), SEEK_SET);
        fread(&f, sizeof(Filme), 1, arq);

        if (f.status == 'A') {
            printf("\nNúmero: %d\n", i + 1);
            printf("Título: %s\n", f.titulo);
            printf("Gênero: %s\n", f.genero);
            printf("Ano: %d\n", f.ano);
            printf("Nota: %.1f\n", f.nota);
        }
    }
}

// ----------------------------------------------------------

void excluir(FILE *arq) {
    int index, realIndex;
    Filme f;

    printf("\nNúmero do filme para excluir (começa em 1): ");
    scanf("%d", &index);
    limpaBuffer();

    realIndex = index - 1;

    if (realIndex < 0 || realIndex >= tamanho(arq)) {
        printf("Índice inválido!\n");
        return;
    }

    fseek(arq, realIndex * sizeof(Filme), SEEK_SET);
    fread(&f, sizeof(Filme), 1, arq);

    if (f.status == 'E') {
        printf("Este filme já está excluído!\n");
        return;
    }

    f.status = 'E';

    fseek(arq, realIndex * sizeof(Filme), SEEK_SET);
    fwrite(&f, sizeof(Filme), 1, arq);

    printf("Filme excluído com sucesso!\n");
}

// ==========================================================
// RELATÓRIOS
// ==========================================================

void gerarRelatorio(FILE *arq) {
    FILE *rel = fopen("relatorio_ativos.txt", "w");

    if (!rel) {
        printf("Erro ao criar arquivo!\n");
        return;
    }

    Filme f;
    int total = tamanho(arq);

    fprintf(rel, "===== RELATÓRIO DE FILMES ATIVOS =====\n\n");

    for (int i = 0; i < total; i++) {
        fseek(arq, i * sizeof(Filme), SEEK_SET);
        fread(&f, sizeof(Filme), 1, arq);

        if (f.status == 'A') {
            fprintf(rel, "Número: %d\n", i + 1);
            fprintf(rel, "Título: %s\n", f.titulo);
            fprintf(rel, "Gênero: %s\n", f.genero);
            fprintf(rel, "Ano: %d\n", f.ano);
            fprintf(rel, "Nota: %.1f\n\n", f.nota);
        }
    }

    fclose(rel);
    printf("\nRelatório gerado: relatorio_ativos.txt\n");
}

// ----------------------------------------------------------

void gerarRelatorioCompleto(FILE *arq) {
    FILE *rel = fopen("relatorio_completo.txt", "w");
    if (!rel) {
        printf("Erro ao criar relatório completo!\n");
        return;
    }

    int total = tamanho(arq);
    if (total == 0) {
        fprintf(rel, "Nenhum filme cadastrado.\n");
        fclose(rel);
        printf("Relatório gerado: relatorio_completo.txt (vazio)\n");
        return;
    }

    Filme *lista = (Filme*) malloc(total * sizeof(Filme));
    if (!lista) {
        printf("Erro de memória!\n");
        fclose(rel);
        return;
    }

    for (int i = 0; i < total; i++) {
        fseek(arq, i * sizeof(Filme), SEEK_SET);
        fread(&lista[i], sizeof(Filme), 1, arq);
    }

    fprintf(rel, "========== RELATÓRIO COMPLETO ==========\n\n");

    int ativos = 0, excluidos = 0;

    for (int i = 0; i < total; i++) {
        fprintf(rel, "Título: %s\n", lista[i].titulo);
        fprintf(rel, "Gênero: %s\n", lista[i].genero);
        fprintf(rel, "Ano: %d\n", lista[i].ano);
        fprintf(rel, "Nota: %.1f\n", lista[i].nota);
        fprintf(rel, "Status: %c\n", lista[i].status);
        fprintf(rel, "-----------------------------------\n");

        if (lista[i].status == 'A') ativos++;
        else excluidos++;
    }

    fprintf(rel, "\n===== RESUMO =====\n");
    fprintf(rel, "Total de filmes: %d\n", total);
    fprintf(rel, "Ativos: %d\n", ativos);
    fprintf(rel, "Excluídos: %d\n", excluidos);

    free(lista);
    fclose(rel);

    printf("\nRelatório COMPLETO gerado: relatorio_completo.txt\n");
}
