/* As threads leitoras podem simultaneamente acessar a região crítica (array). Ou seja, uma thread leitora não bloqueia outra thread leitora;*/
/* Threads escritoras precisam ter acesso exclusivo à região crítica. Ou seja, a manipulação deve ser feita usando exclusão mútua.*/
/* Ao entrar na região crítica, uma thread escritora deverá bloquear todas as outras threads escritoras e threads leitoras que desejarem acessar o recurso compartilhado.*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define threads_leitoras 5 // Coloque aqui a quantidade de Threads Leitoras desajada
#define threads_escritoras 5 // Coloque aqui a quantidade de Threads Escritoras desejada

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Único mutex que vai garantir a exclusão mútua na região crítica, que será o banco de informacoes
int *informacoes = NULL; //'informacoes' é um array dinâmico para armazenar os dados
int tamanho = 0; //tamanho do array 'informacoes'

void *leitura(void *arg){
    int threadID = *((int*)arg);  //'threadID' é o ID da thread escritora
    int pos; //variável que indicará qual a posição do banco de informações para acesso e exibição
    int i = 0;
    do { //Assim como as threads escritoras, todas threads leitoras funcionaram dentro de um while para que assim elas possam ler o array 'informacoes' continuamente
        pthread_mutex_lock(&mutex); //Travamos o mutex para garantir que uma única thread escritora está escrevendo no banco de Dados

        pos = rand()%(tamanho); // posição aleatoria para que a thread leitora acesse no vetor 'informacoes'
        printf("A Thread leitora de ID [%d] leu a posição [%d] do Banco de Informações: %d\n", threadID, pos, informacoes[pos]);
        // Na prática, cada thread dará 6 iterações
        if(i >= (5))
            i = -1;
        else
            i++;
        pthread_mutex_unlock(&mutex); //Liberamos o mutex para que outra thread escritora possa acrescentar algo no banco de informacoes
    // Para checar não sendo um um loop infinito, substitua 1 por i != 0
    } while(i != 0);
}
void *escrita(void *arg){
    int threadID = *((int*)arg); //Salvamos o ID da thread escritora em 'threadID'
    int i = 0;
    // Coloquei um número alto e diferente, para visualizar o último elemento escrito por uma thread
    do {
       pthread_mutex_lock(&mutex); //Travamos o mutex para garantir que uma única thread escritora está escrevendo no banco de Dados
        printf("A Thread escritora de ID [%d] está escrevendo no Banco de Informações\n", threadID);
        // Na prática, cada thread dará 6 iterações
        if(i >= (5))
            i = 0;
        else
            i++;

        informacoes = (int*) realloc(informacoes, sizeof(int)*(tamanho+1)); //Criamos uma nova posição no banco informacoes
        informacoes[tamanho] = i; //Armazenamos o valor que geramos na nova posição do banco de informacoes
        tamanho++; //Aumentamos a variável que indica qual o tamanho do 'informacoes'
        pthread_mutex_unlock(&mutex); //Liberamos o mutex para que outra thread escritora possa acrescentar algo no banco de informacoes
    // Para checar não sendo um um loop infinito, substitua 1 por i != 0
    } while(i != 0); //Todas threads escritoras funcionaram dentro de um while para que assim elas possam escrever no array 'informacoes' continuamente
}

int main(){

    pthread_t escritoras[threads_escritoras]; //Threads escritoras 
    pthread_t leitoras[threads_leitoras]; //Threads leitoras
    int *idsE[threads_escritoras]; // idsE[M] = ID das threads escritoras
    int *idsL[threads_leitoras]; // idsL[N] = ID das trheads leitoras

    for(int i = 0; i < threads_escritoras; i++){
        idsE[i] = (int*) malloc(sizeof(int));
        *idsE[i] = i;
        if(pthread_create(&escritoras[i], NULL, &escrita, (void*)idsE[i]) != 0)
            printf("Falha na criação da thread");
    } 
    for(int i = 0; i < threads_leitoras; i++){
        idsL[i] = (int*) malloc(sizeof(int));
        *idsL[i] = i;
        if(pthread_create(&leitoras[i], NULL, &leitura, (void*)idsL[i]) != 0)
            printf("Falha na criação da thread");
    }

    pthread_exit(NULL);
    free(informacoes);
    return 0;
}