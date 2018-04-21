#include <stdio.h>
#include <stdlib.h>

#include "pdi.h"

#define KERNEL 7

//Novas funções
void mascara(Imagem *original, Imagem *mascara, Imagem *saida);
int cmpfunc(const void * a, const void * b);

int main() {

    //Localização das imagens a serem segmentadas.
    char *imagens[5] = {
                        "../imagens/60.bmp" ,
                        "../imagens/82.bmp" , 
                        "../imagens/114.bmp", 
                        "../imagens/150.bmp", 
                        "../imagens/205.bmp" 
                       };
    char name[40] = "";
    Imagem *original, *entrada, *saida, *buffer;
    Imagem *kernel = criaKernelCircular(KERNEL);
    ComponenteConexo *componente;

    //Procedimento para cada imagens.
    for(int i = 0; i < 5; i += 1) {

        //Carregando imagem em escala de cinza.
        original = abreImagem(imagens[i], 1);
        sprintf(name, "../resultados/%d1 - Cinza.bmp", i + 1);
        salvaImagem(original, name); 

        //Criando imagens auxiliares, com o mesmo tamanho da imagem carregada.
        entrada = criaImagem(original->largura, original->altura, original->n_canais);
        copiaConteudo(original, entrada);
        saida = criaImagem(original->largura, original->altura, original->n_canais);
        buffer = criaImagem(original->largura, original->altura, original->n_canais);

        filtroGaussiano(entrada, saida, 5, 5, buffer);
        sprintf(name, "../resultados/%d2 - borrada.bmp", i + 1);
        salvaImagem(saida, name);
        copiaConteudo(saida, entrada);

        normalizaSemExtremos8bpp(entrada, saida, 0, 1, 0.01f);
        sprintf(name, "../resultados/%d3 - normalizada.bmp", i + 1);
        salvaImagem(saida, name);
        copiaConteudo(saida, entrada);

        binarizaAdapt(entrada, saida, 101, 0.2, buffer);
        sprintf(name, "../resultados/%d4 - binAdapt.bmp", i + 1);
        salvaImagem(saida, name);
        copiaConteudo(saida, entrada);

        dilata(entrada, kernel, criaCoordenada((KERNEL/2), (KERNEL/2)), saida);
        sprintf(name, "../resultados/%d5 - dilata.bmp", i + 1);
        salvaImagem(saida, name);
        copiaConteudo(saida, entrada);

        mascara(original, entrada, saida);
        sprintf(name, "../resultados/%d6 - mascara.bmp", i + 1);
        salvaImagem(saida, name);
        copiaConteudo(saida, entrada);

        binariza(entrada, saida, 0.7f);
        sprintf(name, "../resultados/%d7 - binarizada.bmp", i + 1);
        salvaImagem(saida, name);
        copiaConteudo(saida, entrada);

        Imagem *k       = criaKernelCircular(5);
        Coordenada c    = criaCoordenada(2, 2);

        erode(entrada, k, c, saida);
        copiaConteudo(saida, entrada);
        dilata(entrada, k, c, saida);
        copiaConteudo(saida, entrada);
        erode(entrada, k, c, saida);
        sprintf(name, "../resultados/%d8 - tapaBuraco.bmp", i + 1);
        salvaImagem(saida, name);
        copiaConteudo(saida, entrada);

        int qArroz = rotulaFloodFill(entrada, &componente, 2, 2, 5);
        int nPixels = 0;

        qsort(componente, qArroz, sizeof(ComponenteConexo), cmpfunc);

        int mediana = componente[(qArroz/2)-1].n_pixels;

        for(int cont = 0; cont < qArroz; cont += 1)
            nPixels += componente[cont].n_pixels;

        printf("Imagem %d\n", i + 1);
        printf("FloodFill: \t\t%d\n", qArroz);
        printf("Calculo com mediana: \t%d\n\n", nPixels/mediana);        

        //Desalocando memória previamente alocada.
        destroiImagem(original);
        destroiImagem(entrada);
        destroiImagem(saida);
        destroiImagem(buffer);
    }

    return 0;
}

//Função para subtrair a imagem original da mascara, colocando o resultado na saida.
void mascara(Imagem *original, Imagem *mascara, Imagem *saida) {
    for(int y = 0; y < original->altura; y += 1) {
        for(int x = 0; x < original->largura; x += 1) {
            if(mascara->dados[0][y][x] == 1.0f)
                saida->dados[0][y][x] = original->dados[0][y][x];
            else
                saida->dados[0][y][x] = 0.0f;
        }
    }
}

int cmpfunc(const void * a, const void * b) {
	return (*(ComponenteConexo*)a).n_pixels - (*(ComponenteConexo*)b).n_pixels;
}
