#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

FILE *Image;
FILE *NewImage;
int **grayMatrix;
int n, m;

typedef struct{
    int red;
    int blue;
    int green;
    int line;
} Pixel;

void openImageOriginal(){
    Image = fopen("ImagemOriginal.txt", "r");
    if (Image == NULL){
        printf("Falha na abertura do arquivo\n");
        exit(-1);
    }
}

void openImageCinza(){
    NewImage = fopen("ImagemCinza.txt", "w");
    if (NewImage == NULL){
        printf("Falha na abertura do arquivo\n");
        exit(-1);
    }
}

void escreverImagemCinza(){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            fprintf(NewImage, "%d ", grayMatrix[i][j]);
        }
        fprintf(NewImage, "\n");
    }
}

void fecharArquivos(){
    fclose(Image);
    fclose(NewImage);
}

void *CombLinear(void *p){
    Pixel pixel = *(Pixel *)p;
    int grey = pixel.red * 0.30 + pixel.green * 0.59 + pixel.blue * 0.11;
    grayMatrix[pixel.line][0] = grey;
    grayMatrix[pixel.line][1] = grey;
    grayMatrix[pixel.line][2] = grey;
    pthread_exit(NULL);
}

void threadsGenerator(int** matrix){
    pthread_t *threads = malloc(sizeof(pthread_t) * n);

    for (int i = 0; i < n; i++){
        Pixel *pixel = malloc(sizeof(Pixel));
        (*pixel).red = matrix[i][0];
        pixel->green = matrix[i][1];
        pixel->blue = matrix[i][2];
        pixel->line = i;

        pthread_create(&threads[i], NULL, &CombLinear, pixel);
    }

    for (int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }
}

int main(){

    openImageOriginal();
    openImageCinza();

    fseek(Image, sizeof(char) * 3, SEEK_CUR);
    fscanf(Image, "%d %d", &m, &n);

    n = n * m;

    int **matrix = malloc(sizeof(int *) * n);
    grayMatrix = malloc(sizeof(int *) * n);

    for (int i = 0; i < n; i++){
        matrix[i] = malloc(sizeof(int) * m);
        grayMatrix[i] = malloc(sizeof(int *) * m);
    }

    fscanf(Image, "\n");
    fseek(Image, sizeof(int) + sizeof(char), SEEK_CUR);

    
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            fscanf(Image, "%d", &matrix[i][j]);
        }
        fscanf(Image, "\n");
    }

    threadsGenerator(matrix);
    escreverImagemCinza();
    
    free(matrix);
    free(grayMatrix);

    fecharArquivos();

    return 0;
}