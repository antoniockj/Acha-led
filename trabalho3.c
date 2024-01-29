/*      ICSF13 - 2023-2 - TRABALHO 3         /

/ ------------------------------------------/

/  Antonio Celso Krul Junior - 2449919      /
/  Nadsson Kauê Alves de Oliveira - 2609800 /
/  Luis Felipe Da Silva Campos - 2623820 /

/-------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "trabalho3.h"

int** alocaInicializaMatriz(int altura, int largura) {
    int i, j;
    int **matriz = (int **)malloc(altura * sizeof(int *));
    for (i = 0; i < altura; i++) {
        matriz[i] = (int *)malloc(largura * sizeof(int));
        for (j = 0; j < largura; j++) {
            matriz[i][j] = 0;
        }
    }
    return matriz;
}


void removeRuidos(Imagem1C* img) {
    int i, j;
    for (i = 0; i < img->altura; i++) {
        for (j = 0; j < img->largura; j++) {
            if (img->dados[i][j] < 150) {
                img->dados[i][j] = 0;
            } else {
                img->dados[i][j] = 255;
            }
        }
    }
}


void marcaPixelsBrancos(int** matriz, Imagem1C* img) {
    int i, j;
    for (i = 0; i < img->altura; i++) {
        for (j = 0; j < img->largura; j++) {
            if (img->dados[i][j] == 255) {
                matriz[i][j] = 1;
            }
        }
    }
}


void zeraRotulos(int** matrizTeste, int altura, int largura, int rotuloPrimeiraLED) {
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            if (matrizTeste[i][j] == rotuloPrimeiraLED) {
                matrizTeste[i][j] = 0;
            } else if (matrizTeste[i][j] != 0) {
                matrizTeste[i][j] = 1;
            }
        }
    }
}
int* rotulaVizinhosE_D(int** matrizTeste, int altura, int largura, int rotuloAtual) {
    int vizinho, i, j, k, l;
    int *rotulos = (int *)malloc(200 * sizeof(int));  // Aloca o vetor

    // Inicializ o vetor
    for (int i = 0; i < 200; i++) {
        rotulos[i] = 0;
    }

    // Rotula os vizinhos
    for (i = 1; i < altura - 1; i++) {
        for (j = 1; j < largura - 1; j++) {
            if (matrizTeste[i][j] == 1) {
                vizinho = 0;
                matrizTeste[i][j] = rotuloAtual;

                // Verificar vizinhos ao redor
                for (k = -1; k <= 1; k++) {
                    for (l = -1; l <= 1; l++) {
                        if (matrizTeste[i + k][j + l] == 1) {
                            matrizTeste[i + k][j + l] = rotuloAtual;
                            vizinho++;
                        }
                    }
                }

                // Se nao tiver vizinho muda o rotulo e se tiver adiciona quantos na posicao certa do vetor
                if (vizinho == 0) {
                    rotuloAtual++;
                } else {
                    rotulos[rotuloAtual] += vizinho;
                }
            }
        }
    }

    return rotulos;
}

int encontraMaiorValor(int* vetor, int tamanho) {
    int maior = 0;
    int indiceMaior = -1;
    // Encontra o indice que tem mais vizinhos que no caso e o led
    for (int i = 1; i < tamanho; i++) {
        if (vetor[i] > maior) {
            maior = vetor[i];
            indiceMaior = i;
        }
    }

    // Zera o indice do vetor para reutilizar a funcao para achar o outro led
    if (indiceMaior != -1) {
        vetor[indiceMaior] = 0;
    }

    return indiceMaior;
}


Coordenada encontraCentro(int** matriz, int altura, int largura, int rotulo, int* rotulos) {
    int i, j, primeira_linha, ultima_linha, primeira_coluna, ultima_coluna;
    int encontrouPrimeiro = 0;
    Coordenada centro;

    // Percorre a matriz
    for (i = 0; i < altura; i++) {
        for (j = 0; j < largura; j++) {
            if (matriz[i][j] == rotulo) {
                if (encontrouPrimeiro == 0) {
                    // Encontrou a primeira
                    primeira_linha = i;
                    primeira_coluna = j;
                    encontrouPrimeiro = 1;
                }
                // Encontrou a ultima
                ultima_linha = i;
                ultima_coluna = j;

                // Remove rotulo
                matriz[i][j] = 0;
                if (rotulos != NULL) {
                    rotulos[rotulo]--;
                }
            }
        }
    }
    centro.x = (ultima_coluna + primeira_coluna) / 2;
    centro.y = (ultima_linha + primeira_linha) / 2;

    return centro;
}


double calculaAnguloEntreCentros(Coordenada centro1, Coordenada centro2) {
    double angulo = 0, distancia_y, distancia_x, pi = 3.14159265;

    distancia_x = (centro1.x - centro2.x);
    distancia_y = (centro1.y - centro2.y);
    //Para a distancia nao fique negativa
    if (distancia_x < 0){
        distancia_x = (distancia_x*-1);
    }
    if (distancia_y < 0){
        distancia_y = (distancia_y*-1);
    }
    // Calcula o ângulo em radianos
    angulo = atan2(distancia_y, distancia_x);

    if (angulo < -(pi) / 2) {
        angulo += (pi);
    } else if (angulo > (pi) / 2) {
        angulo -= (pi);
    }
    return angulo;
}

double detectaSensorBar(Imagem1C* img, Coordenada* l, Coordenada* r) {
    int rotuloAtual_E = 2, rotuloAtual_D = 2, maior_D = 0, maior_E = 0;
    int **matrizTeste;
    int *rotulos_D, *rotulos_E;
    double angulo;
    Coordenada centro1, centro2;

    // Aloca e inicializa a matrizTeste
    matrizTeste = alocaInicializaMatriz(img->altura, img->largura);

    // Remove ruídos na imagem
    removeRuidos(img);

    // Marca os pixels brancos na matrizTeste
    marcaPixelsBrancos(matrizTeste, img);

    // Rotula os vizinhos para o primeiro LED
    rotulos_E = rotulaVizinhosE_D(matrizTeste, img->altura, img->largura, rotuloAtual_E);

    // Encontra o maior valor nos rótulos e o centro do primeiro LED
    maior_D = encontraMaiorValor(rotulos_E, 200);
    centro1 = encontraCentro(matrizTeste, img->altura, img->largura, maior_D, rotulos_E);

    // Zera os rótulos na matrizTeste, mantendo apenas o rótulo da primeira LED
    zeraRotulos(matrizTeste, img->altura, img->largura, maior_D);

    // Rotula os vizinhos para o segundo LED
    rotulos_D = rotulaVizinhosE_D(matrizTeste, img->altura, img->largura, rotuloAtual_D);

    // Encontra o maior valor nos rótulos e o centro do segundo LED
    maior_E = encontraMaiorValor(rotulos_D, 200);
    centro2 = encontraCentro(matrizTeste, img->altura, img->largura, maior_E, rotulos_D);

    angulo = calculaAnguloEntreCentros(centro1, centro2);

    // Adiciona os centros
    if (centro1.x <= centro2.x) {
        l->x = centro1.x;
        l->y = centro1.y;
        r->x = centro2.x;
        r->y = centro2.y;
    }
    else if (centro1.x > centro2.x) {
        l->x = centro2.x;
        l->y = centro2.y;
        r->x = centro1.x;
        r->y = centro1.y;
    }
    // Libera a memória alocada
    for (int i = 0; i < img->altura; i++) {
        free(matrizTeste[i]);
    }
    free(matrizTeste);

    return angulo;
}

