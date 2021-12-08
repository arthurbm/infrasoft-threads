/* As threads leitoras podem simultaneamente acessar a região crítica (array). Ou seja, uma thread leitora não bloqueia outra thread leitora;*/
/* Threads escritoras precisam ter acesso exclusivo à região crítica. Ou seja, a manipulação deve ser feita usando exclusão mútua.*/
/* Ao entrar na região crítica, uma thread escritora deverá bloquear todas as outras threads escritoras e threads leitoras que desejarem acessar o recurso compartilhado.*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define threads_leitoras 10 // Coloque aqui a quantidade de Threads Leitoras desajada
#define threads_escritoras 10 // Coloque aqui a quantidade de Threads Escritoras desejada

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Único mutex que vai garantir a exclusão mútua na região crítica, que será o banco de informacoes
int *informacoes = NULL; //'informacoes' é um array dinâmico para armazenar os dados
int flag = 1; //variável de checagem para saber se as informações estão sendo acessadas por uma thread escritora ou não
int tamanho = 0; //tamanho do array 'informacoes'


void *leitura(void *arg){
    int threadID = *((int*)arg);  //'threadID' é o ID da thread escritora
    int pos; //variável que indicará qual a posição do banco de informações para acesso e exibição
    while(1){ //Assim como as threads escritoras, todas threads leitoras funcionaram dentro de um while para que assim elas possam ler o array 'informacoes' continuamente
        if(flag == 0){ //Se a flag = 0, nenhuma thread escritora está alterando o banco de informacoes, logo podemos liberar o acesso p/ que uma thread leitora acesse as informações
            pos = rand()%(tamanho+1); // posição aleatoria para que a thread leitora acesse no vetor 'informacoes'
            printf("A Thread leitora de ID [%d] leu a posição [%d] do Banco de Informações: %d\n", threadID, pos, informacoes[pos]);
        }
    }
}
void *escrita(void *arg){
    int threadID = *((int*)arg); //Salvamos o ID da thread escritora em 'threadID'
    int i = 0;
    while(1){ //Todas threads escritoras funcionaram dentro de um while para que assim elas possam escrever no array 'informacoes' continuamente
        pthread_mutex_lock(&mutex); //Travamos o mutex para garantir que uma única thread escritora está escrevendo no banco de Dados
        printf("A Thread escritora de ID [%d] está escrevendo no Banco de Informações\n", threadID);
        flag = 1; //Colocamos a flag = 1 para sinalizar que o banco de informacoes está sendo acessado por uma thread escritora
        if(i <= (-1e5))
            i = 0;
        else
            i--;

        informacoes = (int*) realloc(informacoes, sizeof(int)*(tamanho+1)); //Criamos uma nova posição no banco informacoes
        informacoes[tamanho] = i; //Armazenamos o valor que geramos na nova posição do banco de informacoes
        tamanho++; //Aumentamos a variável que indica qual o tamanho do 'informacoes'
        flag = 0; //Colocamos a flag = 0 para sinalizar que o banco de informacoes está livre, ou seja, não está sendo alterado por nenhuma thread escritora
        pthread_mutex_unlock(&mutex); //Liberamos o mutex para que outra thread escritora possa acrescentar algo no banco de informacoes
    }
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
    return 0;
}