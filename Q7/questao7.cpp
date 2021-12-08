#include <bits/stdc++.h>
#include <pthread.h>

#define N 2
#define bufferSize 100
#define reqNum 200    // Número de requisicoes

using namespace std;

int numThreads = 0;

pthread_mutex_t mutexs = PTHREAD_MUTEX_INITIALIZER;      // mutex de uso real
pthread_mutex_t mutexThreadCreation = PTHREAD_MUTEX_INITIALIZER; // Mutex para a criacao de threads

pthread_cond_t Threads = PTHREAD_COND_INITIALIZER;       // condicional para produtor consumidor para criacao de threads
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;         // condicional para produtor consumidor de requisicoes do buffer vazio
pthread_cond_t fillo = PTHREAD_COND_INITIALIZER;         // condicional para produtor consumidor de requisicoes do buffer cheio

pthread_mutex_t BufferMutex[reqNum];                     // serve para sabermos se podemos acessar o buffer de respostas pela funcao pegarResultadoExecucao
pthread_cond_t BufferCond[reqNum];                       // serve como condicao para pegar resultado

pthread_mutex_t mutexPrint = PTHREAD_MUTEX_INITIALIZER; // um mutex que serve apenas para os prints naos e sobreporem

long long ID = 0;              // id usado em execução das funcoes
int contador = 0;              // um contador para dar os argumentos das funcoes sem se atrapalharem
int threadDisponivel[N] = {1}; // diz as threads estao disponiveis para serem criadas(inicialmente todas)

class bufferResp
{ // classe para criar buffer de saida
public:
  int retorno;    // o valor de retorno da funcao
  int disponivel; // valor que diz se a função está disponível

  bufferResp()
  {
    this->disponivel = 0; 
  }
};

bufferResp bufferResposta[reqNum]; // buffer de saida

class generatorFunc
{ // cria uma estrutura que recebe uma funcao e um argumento, (o valor A eh o valor numerico do argumento e o arg e um ponteiro para ele)
public:
  void *(*funcao)(void *);
  void *arg;
  int a;
  int id;
};

queue<generatorFunc> buffer; //  fila como buffer de entrada

int generateId(int id) { // funcao generica para gerar IDS
  return (id + 1)*2;
}

void *customPrint(void *num)
{ // funcao funexec
  int c = *((int *)num);
  pthread_mutex_lock(&mutexPrint);
  cout << "Print: " << c << endl;
  pthread_mutex_unlock(&mutexPrint);
  
}


void agendarExecucao(void *structure)
{ // geradora de ids para o buffer (funciona como um produtor)

  generatorFunc tudo = *((generatorFunc *)structure); // recebe uma estrutura q contem a funcao e o argumento da funcao
  tudo.id = ID;                     
  pthread_mutex_lock(&mutexs);
  while (buffer.size() == bufferSize)
  {
    pthread_cond_wait(&empty, &mutexs);
  } // se buffer tiver cheio dorme.

  buffer.push(tudo); // se nao estiver cheio, coloca a estrutura no buffer
  if (buffer.size() == 1) pthread_cond_broadcast(&fillo); // acorda os consumidores
  pthread_mutex_unlock(&mutexs);

  ID = generateId(ID);
}

void *executeThread(void *arg)
{ // funcao que recebe a estrutura do buffer e salva o retorno no buffer de resultado
  generatorFunc b = *((generatorFunc *)arg);
  pthread_mutex_lock(&BufferMutex[b.id]); // travar o acesso a uma posicao do buffer de resultado

  b.funcao(&b.a);                   // executa a funcao com seu argumento
  pthread_mutex_lock(&mutexThreadCreation); // trava mutex de criacao de threads
  numThreads--;                     // diminui o numero de threads sendo usadas

  if (numThreads == N - 1)
  {
    pthread_cond_signal(&Threads);
  }                                       // destrava o criador de threads;(serve como consumidor de threads)
  pthread_mutex_unlock(&mutexThreadCreation);     // destrava o multex de criacao de threads
  bufferResposta[b.id].retorno = b.a;     // enche o buffer de resposta com a respectiva resposta
  bufferResposta[b.id].disponivel = 1;    // diz q a resposta esta disponivel
  pthread_cond_signal(&BufferCond[b.id]); // Acorda a funcao de pegarResultadoExecucao caso ela tenha tentando pegar um resultado que ainda nao esta pronto

  pthread_mutex_unlock(&BufferMutex[b.id]);
}

void despachante(void *argument)
{ // essa funcao serve como consumidor do buffer e como produtor de threads
  pthread_t *threadsUsed;
  threadsUsed = new pthread_t[N];
  int i = 0;
  generatorFunc *arg = (generatorFunc *)malloc(reqNum * sizeof(generatorFunc));
  pthread_mutex_lock(&mutexs);

  while (buffer.size() == 0)
  {                                     // consumidor do buffer de requisicoes
    pthread_cond_wait(&fillo, &mutexs); // dorme caso buffer esteja vazio
  }
  pthread_mutex_lock(&mutexThreadCreation);
  while (numThreads == N)
  {                                            // Produtor de threads
    pthread_cond_wait(&Threads, &mutexThreadCreation); // dorme caso nao possa criar threads
  }
  do
  {
    if (threadDisponivel[i] == 1)
    {                                                                         // procura qual thread ta disponivel para ser criada
      arg[contador] = buffer.front();                                   // pega o primeiro item da lista buffer de requisicoes
      buffer.pop();                                                           // diminui o tamanho do buffer em um;
      pthread_create(&threadsUsed[i], NULL, executeThread, &arg[contador]); // cria a thread que executa a funcao recebida
      threadDisponivel[i] == 0;                                               // sinaliza se a thread está disponível para ser criada
      numThreads++;                                                           
      contador++;                                                             
      pthread_mutex_unlock(&mutexThreadCreation);
    }

    i++;
  } while (threadDisponivel[i - 1] == 0); // procura uma thread disponivel no momento

  if (buffer.size() == bufferSize - 1)
  {
    pthread_cond_signal(&empty);
  } // caso tenha espaco para produtor colocar no buffer, acorda o produtor
  pthread_mutex_unlock(&mutexs);
}

void *dispatcher(void *argument)
{ // serve apenas para determinar quantas requisicoes o despachador vai tirar do buffer
  for (int i = 0; i < reqNum; i++)
    despachante(NULL);
}

int pegarResultadoExecucao(int id)
{// pegar as requisicoes do buffer de resultados de saida e checar os valores de retorno
  pthread_mutex_lock(&BufferMutex[id]); // ve se esta disponivel para ser acessado
  while (bufferResposta[id].disponivel == 0)
  { // se nao estiver diponivel, dorme
    pthread_cond_wait(&BufferCond[id], &BufferMutex[id]);
  }
  pthread_mutex_lock(&mutexPrint);
  cout << "Recebi:" << bufferResposta[id].retorno << ", da execucao do id: " << id << endl; // mostra o valor que recebeu
  bufferResposta[id].disponivel == 0;                                                       // abre espaco no buffer de saida
  pthread_mutex_unlock(&mutexPrint);
  pthread_mutex_unlock(&BufferMutex[id]);
}

int main()
{

  pthread_t *threads;
  threads = new pthread_t[N];
  long *taskids = new long[N];
  generatorFunc *functionTest = (generatorFunc *)malloc(reqNum * sizeof(generatorFunc));
  for (int i = 0; i < reqNum; i++)
  {
    BufferMutex[i] = PTHREAD_MUTEX_INITIALIZER; // serve para a funcao pegarResultadoExecucao saber se ja pode ou nao acessar o buffer de respostas
    BufferCond[i] = PTHREAD_COND_INITIALIZER;
  }

  int rc;
  pthread_create(&threads[0], NULL, dispatcher, NULL); // cria um despachador que vai rodar simultaneamente no codigo

  for (int t = 0; t < reqNum; t++)
  {
    functionTest[t].funcao = customPrint;         // recebe a funcao generica(funexec)
    functionTest[t].a = t * 562 / 5;         // cria um argumento pseudo aleatorio(mas dependendo da aplicacao esse valor pode ser mudado, mas por simplicidade dcolocamos desse jeito)
    functionTest[t].arg = &functionTest[t].a; // pega um pontei
    agendarExecucao(&functionTest[t]);       // chama afuncao apr acolocar as requisicoes no buffer
  }

  for (int t = 0; t < reqNum; t++)
  {
    pegarResultadoExecucao(rand() % reqNum); // serve para um acesso aleatorio de Ids;
  }

  pthread_join(threads[0], NULL);

  return 0;
}