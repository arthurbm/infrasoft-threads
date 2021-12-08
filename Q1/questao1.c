/*1. Você deverá criar um programa usando pthreads, no qual threads deverão incrementar um contador global de 0 até 1.000.000.
A thread que alcançar este valor deverá imprimir que o valor foi alcançado e todas as threads deverão finalizar a execução.*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define Threads_totais_utilizadas 100 //Aqui serve para escolher a quantidade de Threads que se deseja utilizar
#define limite 1000000

int contador_global = 0; //Contador para chegar ao valor de 1.000.000

pthread_mutex_t mutex_cont = PTHREAD_MUTEX_INITIALIZER; //Mutex para o contador_global

void *incrementar(void *threadid){ //Função que irá incrementar o contador_global até chegar em 1.000.000

    //"ID" da thread    
    int tid = *((int*)threadid); //tid = thread ID
    int flag = 0; //Indica se o contador_global chegou ao objetivo

    while(!flag){
        pthread_mutex_lock(&mutex_cont); //Travamos a região crítica
        
        if(contador_global < limite){ //Verifica se o contador_global ainda não atingiu a marca, se não aumenta o contador_global
            contador_global++;
            //Se ao incrementar, o contador_global ficar igual a 1, essa thread deve ser a "vencedora" e setamos a flag para 1
            if(contador_global == limite){
                printf("A Thread que chegou em 1.000.000 foi: %d\n", tid);
                flag = 1;
            }
        }
        //Se outra thread atingiu a marca de 1.000.000
        else
            flag = 1;

        pthread_mutex_unlock(&mutex_cont); //Liberação da região crítica de concorrência
    }
}

int main (){
    //Auxiliares para os "for" e na hora da criação das Threads
    int i, rc;  
    
    //Inicialização das threads 
    pthread_t threads[Threads_totais_utilizadas];
    int *ids[Threads_totais_utilizadas];

    //Processo da criação das Threads
    for(i = 0; i < Threads_totais_utilizadas; i++){
        ids[i] = (int*) malloc(sizeof(int)); 
        *ids[i] = i;
        rc = pthread_create(&threads[i], NULL, incrementar, (void *) ids[i]);
    }

    //União das threads
    for(i = 0; i < Threads_totais_utilizadas; i++)
        pthread_join(threads[i], NULL);    

    //Libera o vetor de ID's
    for(i = 0; i < Threads_totais_utilizadas; i++)
        free(ids[i]);

    printf("Valor final do contador_global: %d\n", contador_global); //Verifica o valor final do contador_global
    pthread_exit(NULL);
}