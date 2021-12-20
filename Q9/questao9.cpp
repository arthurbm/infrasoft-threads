#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <bits/stdc++.h>

#define num_threads 5
#define array_size 30

using namespace std;

// vector<bool> bool_primes;
bool prime[array_size + 1];

bool is_visited[array_size + 1];

void *crivoDeErastoteles(void *arg)
{
    for (int p = 2; p * p <= array_size; p++)
    {
        // Se prime[p] não mudou, então ele pode ser primo
        if (prime[p] == true)
        {
            // Atualizar multiplos de p 
            for (int i = p * p; i <= array_size; i += p)
                prime[i] = false;
        }
    }

    // Print all prime numbers
    for (int p = 2; p <= array_size; p++)
        if (prime[p])
            cout << p << " ";
}

// A intenção é fazer o crivo, e pegar os dois vetores, um booleano para dizer se
// o número é primo ou não, e outro para ver se a posição já foi checada por alguma thread.
// Ao achar um primo, ele será jogado numa fila, e as threads vão tirando eles da fila e os processando

// Não consegui implementar a tempo usando threads
int main()
{
  pthread_t threads[num_threads];
  int *threadIDs[num_threads];

  memset(prime, true, sizeof(prime));

  for (int i = 0; i < num_threads; i++)
  {
    threadIDs[i] = (int*) malloc(sizeof(int));
    *threadIDs[i] = i;
    if (pthread_create(&threads[i], NULL, &crivoDeErastoteles, (void*)threadIDs[i]) != 0)
      printf("Falha na criação da thread");
  }

  return 0;
}
