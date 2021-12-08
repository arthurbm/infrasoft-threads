#include <mutex>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

mutex mux;
#define nPessoas 20

#define VAZIO 0
#define CHEIO 1
#define ENTRANDO 2
#define SAINDO 3

class mrussa {
  private:
  int tamCarro;
  int modo;
  int cont;
  condition_variable cheio,vazio,entrando,saindo,voltando; // nos modos eu defini
                                                          // 0 = vazio
                                                          //1 = cheio
                                                          //2 = entrando
                                                          //3 = saindo

  public:
  mrussa(int tam){
    tamCarro = tam;
    modo = 0;
    cont = 0;
  }

  void espera_encher(){
    unique_lock<mutex> lck (mux);
    while (modo != VAZIO)
      vazio.wait(lck);
    cout << "Entrando...\n ";
    entrando.notify_all();
    modo = ENTRANDO;
  }

  void espera_esvaziar(){
    unique_lock<mutex> lck (mux);
    while (modo != CHEIO)
      cheio.wait(lck);
    cout << "Saindo...\n";
    saindo.notify_all();
    modo = SAINDO;

  }

  void entra_no_carro(){
    unique_lock<mutex> lck (mux);
    while(modo != ENTRANDO)
      entrando.wait(lck);
    cont ++;
    cout << "Entrou\n";
    if (cont == tamCarro) {
      cheio.notify_all();
      modo = CHEIO;
    }
  }
  void sai_do_carro(){
    unique_lock<mutex> lck (mux);

    while (modo != 3)
      saindo.wait(lck);

    cont--;
    cout << "saiu\n";

    if(cont == 0){
      vazio.notify_all();
      modo = 0;
    }
  }
};
mrussa montanhar(5);

void f1(int n){
  for (int i=0;i<n;i++){
    montanhar.espera_encher();
    usleep(100); // da a volta
    montanhar.espera_esvaziar();
  }
}

void f2(){
    montanhar.entra_no_carro();
    montanhar.sai_do_carro();
    usleep(100); //passeia
}

int main(){
  int n;
  vector<thread> pessoas;
  cout << "inserir numero de voltas: " << endl;
  cin >> n;
  thread carro(f1,n);
  for (int i=0;i<nPessoas;i++){
    pessoas.push_back(thread(f2));
  }
  carro.join();
  for (thread &t: pessoas)
    t.join();
  return 0;
}