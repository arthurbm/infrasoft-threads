#include <iostream>
#include <pthread.h>

// Questão 02 - verificar ordenação
using namespace std;

struct ThreadArg
{
  int *list;
  int start;
  int fim;
  int *is_ordered;
};

void *routine(void *arg)
{
  ThreadArg *t_arg = ((ThreadArg *)arg);
  int stop = 0;
  for (int i = t_arg->start; (i < t_arg->fim) && !stop; i++)
  {
    if (t_arg->list[i] > t_arg->list[i + 1])
    {
      *(t_arg->is_ordered) = 0;
      stop = 1;
    }
  }
  if (!stop) // se foi até o fim é pq tudo estava ordenado
  {
    *(t_arg->is_ordered) = 1;
  }
};

struct Thread
{
  pthread_t thread;
  int id;
  int *is_ordered = new int();
  ThreadArg *t_arg;
  void initThread(ThreadArg *arg)
  {
    t_arg = arg;
    t_arg->is_ordered = is_ordered; // ponteiro para a flag
    pthread_create(&thread, NULL, routine, arg);
  }
  void await()
  {
    pthread_join(thread, NULL);
  };
  void print_list()
  {
    cout << "=-=-=-= id:" << id << " =-==-=\n";
    for (int i = t_arg->start; i <= t_arg->fim; i++)
    {
      cout << t_arg->list[i] << " ";
    }
    cout << "\n";
  }
};

int main()
{
  int N = 0;
  int v_size;
  int array_is_ordered = 1;

  cout << "Digite N:\n";
  cin >> N;

  cout << "Digite o tamanho do vetor:\n";
  cin >> v_size;
  int vetor[v_size];

  cout << "Digite o valores do vetor: \n";
  for (int i = 0; i < v_size; i++)
  {
    cin >> vetor[i];
  }

  Thread threads[N];
  ThreadArg *thread_args[N];

  // inicializando as threads
  for (int i = 0; i < N; i++)
  {
    int step = v_size / N;
    ThreadArg *t_arg = new ThreadArg(); // criando os argumentos da rotina
    Thread t; // criando a thread
    t.id = i;
    t_arg->list = vetor;
    t_arg->start = step * i;
    if (i == N - 1)
    {
      t_arg->fim = v_size - 1;
    }
    else
    {
      t_arg->fim = (step * (i + 1)) - 1;
    }
    t.initThread(t_arg);
    threads[i] = t;
  }

  // verificar ordenação
  for (int i = 0; i < N; i++)
  {
    threads[i].await();
    if (!(*threads[i].is_ordered)) // caso essa parte esteja desordenada
    {
      array_is_ordered = 0;
      break;
    }
    else
    {
      // verifico se o fim desse é maior que o começo do próximo
      if ((i < N - 1) && vetor[threads[i].t_arg->fim] > vetor[threads[i + 1].t_arg->start])
      {
        array_is_ordered = 0;
        break;
      }
    };
    // threads[i].print_list();
  };

  if (array_is_ordered)
  {
    cout << "O array está ordenado!!\n";
  }
  else
  {
    cout << "O array não está ordenado!!\n";
  }
  return 1;
}