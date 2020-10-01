// Trabalho de ATR
// 
//	Autor: Matheus Paiva 
//
//	Processo Principal 
//
//


#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <process.h>	
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>
using namespace std;
#include<string>
using std::string;

#define _CHECKERROR	1
#include "CheckForError.h"

#define	i       0x69    //Tecla para bloquear ou retomar a leitura do sistema de inspeção de defeitos
#define	d       0x64    //Tecla para bloquear ou retomar a captura de mensagens de defeitos de superficíe das tiras
#define	e       0x65    //Tecla para bloquear ou retomar a captura de mensagens de dados do processo de laminação
#define	a       0x61    //Tecla para bloquear ou retomar a exibição de defeitos de tiras
#define	l       0x6C    //Tecla para bloquear ou retomar a a exibição de dados do processo de laminação
#define	c       0x63    //Tecla para notificar à tarefa de exibição de dados do processo de laminação que esta deve limpar sua janela de console
#define	ESC 	0x1B	//Tecla para encerrar o programa


// Casting para terceiro e sexto parâmetros da função _beginthreadex
typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

// --------------- Declarações relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

//lista na memomiara ram
#define TAM_LIST 10
int indice=0;
string  LISTA[TAM_LIST];
int OCULPADO[TAM_LIST];

typedef struct TIPO11 {
	int nseq = 1;
	int tipo = 11;
	int cad = 0;
	int gravidade = 1;
	int classe = 1;
	//char arq[6] = "arq123";
}TIPO11; // definição do tipo 11

typedef struct TIPO22 {
	int nseq = 1;
	int tipo = 22;
	int cad = 1;
	string id = "AB";
	float temp = 1;
	float vel = 1;
}TIPO22; // definição do tipo 22

HANDLE hMutexNSEQ, hMutexOCUPADO,hMutexINDICE; // handle do mutex que protege NSEQ,OCUPADO
HANDLE hSemLISTAcheia, hSemLISTAvazia; //handle do semaforo que verifica se a lsita tah cheia ou vazia;
HANDLE hMutexPRODUTOR, hMutexCOSNSUMIDOR; // handle do mutex que bloqueia o produtor e o consumidor;

TIPO11  novaMensagem11();
TIPO22  novaMensagem22();
void EscreverLista(int tipo,int j, TIPO11 m1, TIPO22 m2);

int NSEQ = 1;

DWORD WINAPI LeituraTipo11(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 11
DWORD WINAPI LeituraTipo22(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 22 

// ----------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 2 e 3 Captura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI CapturaTipo11(LPVOID);	// declaração da thread  que  gerencia a captura do tipo 11
DWORD WINAPI CapturaTipo22(LPVOID);	// declaração da thread  que  gerencia a captura do tipo 22 

void LerLista(int tipo);








// ------------------------------------------------------------------------------------------------------- //


int main() {
	//variaveis e Handles
	HANDLE hTarefa1[2]; // handles da tarefa 1: produtoras
	HANDLE hTarefa23[2]; // handle das tarefas 2 e 3: consumidoras
	DWORD dwLeitura11ID, dwLeitura22ID,dwCaptura11ID,dwCaptura22ID;
	DWORD dwExitCode = 0;
	DWORD dwRet;
	int j,status;


	//Mutex e Semaforos
	hMutexNSEQ = CreateMutex(NULL, FALSE, L"ProtegeNSEQ");
	hMutexINDICE = CreateMutex(NULL, FALSE, L"ProtegeINDICE");
	hMutexOCUPADO = CreateMutex(NULL, FALSE, L"ProtegeOCUPADO");
	hMutexPRODUTOR = CreateMutex(NULL, FALSE, L"ProtegePRODUTOR");
	hMutexCOSNSUMIDOR = CreateMutex(NULL, FALSE, L"ProtegeCOSNSUMIDOR");
	hSemLISTAcheia = CreateSemaphore(NULL, 0, TAM_LIST,L"SemLISTAcheia");
	hSemLISTAvazia = CreateSemaphore(NULL, TAM_LIST, TAM_LIST, L"SemLISTAvazia");


	status = WaitForSingleObject(hMutexOCUPADO, INFINITE);
	for(j=0;j<TAM_LIST;j++){
		OCULPADO[j] = 0;
	}
	status = ReleaseMutex(hMutexOCUPADO);

	// Criando Threads
	//tarefa 1
	hTarefa1[0] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo11, &j, 0, (CAST_LPDWORD)&dwLeitura11ID);
	if (hTarefa1[0]) printf("Tarefa 1 criada com Id= %0x \n", dwLeitura11ID);
	hTarefa1[1] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo22, &j, 0, (CAST_LPDWORD)&dwLeitura22ID);
	if (hTarefa1[1]) printf("Tarefa 2 criada com Id= %0x \n", dwLeitura22ID);
	// tarefa 2 e 3 
	hTarefa23[0] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo11, &j, 0, (CAST_LPDWORD)&dwCaptura11ID);
	if (hTarefa23[0]) printf("Tarefa 1 criada com Id= %0x \n", dwCaptura11ID);
	hTarefa23[1] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo22, &j, 0, (CAST_LPDWORD)&dwCaptura22ID);
	if (hTarefa23[1]) printf("Tarefa 2 criada com Id= %0x \n", dwCaptura22ID);



	// Encerrando as threads 

	dwRet = WaitForMultipleObjects(2, hTarefa1, TRUE, INFINITE);
	//CheckForError((dwRet >= WAIT_OBJECT_0) && (dwRet < WAIT_OBJECT_0 + 2));

	for (j = 0; j < 2; j++) {
		GetExitCodeThread(hTarefa1[j], &dwExitCode);
		printf("thread %d terminou: codigo=%d\n", j, dwExitCode);
		CloseHandle(hTarefa1[j]);	// apaga referência ao objeto
	}  // for 

	dwRet = WaitForMultipleObjects(2, hTarefa23, TRUE, INFINITE);
	//CheckForError((dwRet >= WAIT_OBJECT_0) && (dwRet < WAIT_OBJECT_0 + 2));
	for (j = 0; j < 2; j++) {
		GetExitCodeThread(hTarefa23[j], &dwExitCode);
		printf("thread %d terminou: codigo=%d\n", j, dwExitCode);
		CloseHandle(hTarefa23[j]);	// apaga referência ao objeto
	}  // for 
	CloseHandle(hMutexNSEQ);
	CloseHandle(hMutexINDICE);
	CloseHandle(hMutexOCUPADO);
	CloseHandle(hSemLISTAcheia);
	CloseHandle(hSemLISTAvazia);
	CloseHandle(hMutexPRODUTOR);
	CloseHandle(hMutexCOSNSUMIDOR);

	return(0);
}


// --------------- Execução relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI LeituraTipo11(LPVOID index) {
	BOOL status;
	string aux = "erro";
	TIPO11 m1;

	while (1) {

		// -------------recebe a mensagem-------------//
		status = WaitForSingleObject(hMutexNSEQ, INFINITE); // mutex pra proteger a variavel NSEQ
		m1 = novaMensagem11();
		status = ReleaseMutex(hMutexNSEQ); 

		// -------------Produz Mensagem-------------//
		aux = to_string(m1.nseq) + " ";
		aux += to_string(m1.tipo) + " ";
		aux += to_string(m1.cad) + " ";
		aux += to_string(m1.gravidade) + " ";
		aux += to_string(m1.classe) + " ";

		//-------------Tenta guardar o dado produzido-------------//

		status = WaitForSingleObject(hMutexPRODUTOR, INFINITE);  //Garante um produtor por vez 
		status = WaitForSingleObject(hSemLISTAvazia, INFINITE); // Aguarda um espaço vazio

		status = WaitForSingleObject(hMutexINDICE, INFINITE);//atualiza o indice
		indice = (indice + 1) % TAM_LIST;
		status = ReleaseMutex(hMutexINDICE);

		status = WaitForSingleObject(hMutexOCUPADO, INFINITE); // atualiza o vetor ocupado
		OCULPADO[indice] = m1.tipo;
		status = ReleaseMutex(hMutexOCUPADO);

		LISTA[indice] = aux; // Armazena a mensagem na lista

		status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL); // Sonaliza que existe uma mensagem

		status = ReleaseMutex(hMutexPRODUTOR); // Libera Mutex

		Sleep(500); // dorme
	}
	_endthreadex((DWORD)index);
	return(0);

}

DWORD WINAPI LeituraTipo22(LPVOID index) {
	BOOL status;
	string aux = "erro";
	TIPO22 m2;
	while (1) {

		// -------------recebe a mensagem-------------//
		status = WaitForSingleObject(hMutexNSEQ, INFINITE); // mutex pra proteger a variavel NSEQ
		m2 = novaMensagem22();
		status = ReleaseMutex(hMutexNSEQ);

		// -------------Produz Mensagem-------------//
		aux = to_string(m2.nseq) + " ";
		aux += to_string(m2.tipo) + " ";
		aux += to_string(m2.cad) + " ";
		aux += (to_string(m2.temp)).substr(0, 5) + " ";
		aux += to_string(m2.vel).substr(0, 5) + " ";
		aux += m2.id + " ";

		//-------------Tenta guardar o dado produzido-------------//

		status = WaitForSingleObject(hMutexPRODUTOR, INFINITE);  //Garante um produtor por vez 
		status = WaitForSingleObject(hSemLISTAvazia, INFINITE); // Aguarda um espaço vazio

		status = WaitForSingleObject(hMutexINDICE, INFINITE);//atualiza o indice
		indice = (indice + 1) % TAM_LIST;
		status = ReleaseMutex(hMutexINDICE);

		status = WaitForSingleObject(hMutexOCUPADO, INFINITE); // atualiza o vetor ocupado
		OCULPADO[indice] = m2.tipo;
		status = ReleaseMutex(hMutexOCUPADO);

		LISTA[indice] = aux; // Armazena a mensagem na lista

		status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL); // Sonaliza que existe uma mensagem

		status = ReleaseMutex(hMutexPRODUTOR); // Libera Mutex

		Sleep(600); // dorme
	}
	_endthreadex((DWORD)index);
	return(0);
}

TIPO11 novaMensagem11() {
	TIPO11 m1;
	m1.cad = rand() % 6 + 1;
	m1.classe = rand() % 9 + 1;
	m1.gravidade = rand() % 10 + 1;
	m1.nseq = NSEQ;
	NSEQ++;
	if (NSEQ == 99999) {
		NSEQ = 1;
	}
	return m1;
};

TIPO22  novaMensagem22() {
	TIPO22 m2;
	int aux = rand() % 9999;
	int aux2 = rand() % 999;
	m2.cad = rand() % 9 + 1;
	m2.id = (char)(rand() % 25 + 65);
	m2.id += (char)(rand() % 25 + 65);
	m2.id += to_string(rand() % 9999);
	m2.temp = (float)aux / 10;
	m2.vel = (float)aux2 / 10;
	m2.nseq = NSEQ;
	NSEQ++;
	if (NSEQ == 99999) {
		NSEQ = 1;
	}
	return m2;
};

/*void EscreverLista(int tipo, int j, TIPO11 m1, TIPO22 m2) {
	int  index=0;
	int status;
	string aux="erro";

	status = WaitForSingleObject(hMutexOCUPADO, INFINITE);
	while (OCULPADO[j]!=0)
	{
		j++;
		if (j >= TAM_LIST) {
			j = 0;
		}
	}
	index = j;
	status = ReleaseMutex(hMutexOCUPADO);
	if (tipo == 11 && OCULPADO[j] == 0) {
		aux = to_string(m1.nseq) +" ";
		aux += to_string(m1.tipo) + " ";
		aux += to_string(m1.cad) + " ";
		aux += to_string(m1.gravidade) + " ";
		aux += to_string(m1.classe) + " ";

		status = WaitForSingleObject(hMutexOCUPADO, INFINITE);
		OCULPADO[index] = m1.tipo;
		status = ReleaseMutex(hMutexOCUPADO);
	} else if(tipo == 22 && OCULPADO[j] == 0){
		aux= to_string(m2.nseq) + " ";
		aux += to_string(m2.tipo) + " ";
		aux += to_string(m2.cad) + " ";
		aux += (to_string(m2.temp)).substr(0,5) + " ";
		aux += to_string(m2.vel).substr(0, 5) + " ";
		aux += m2.id + " ";

		status = WaitForSingleObject(hMutexOCUPADO, INFINITE);
		OCULPADO[index] = m2.tipo;
		status = ReleaseMutex(hMutexOCUPADO);
	}else {

	}

	LISTA[index] = aux;
	//cout << LISTA[index]<<"\n";

}*/;

// ----------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 2 e 3 Captura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI CapturaTipo11(LPVOID index) {
	BOOL status;
	int j,aux;
	while (true)
	{	

		//-------------Tenta Acessar o dado na lista-------------//

		status = WaitForSingleObject(hMutexCOSNSUMIDOR, INFINITE);  //Garante um consumidor por vez 
		status = WaitForSingleObject(hSemLISTAcheia, INFINITE); // Aguarda um espaço preenchido;

		status = WaitForSingleObject(hMutexOCUPADO, INFINITE); // busca no vetor Ocupado uma mensagem do tipo 11
		for (j = 0; j < TAM_LIST; j++) {
			if (OCULPADO[j] == 11) {
				aux = j;
				if (j == TAM_LIST) {
					j = 0;
				}
				break;
			}
		}
		status = ReleaseMutex(hMutexOCUPADO);

		cout << "\n" << LISTA[aux] << "\n" << "LIDO" << "\n"; // Captura a mensagem da lista

		status = ReleaseSemaphore(hSemLISTAvazia, 1, NULL); // Sinaliza que uma mensagem foi lida 

		status = ReleaseMutex(hMutexCOSNSUMIDOR); // Libera Mutex

		Sleep(500); // dorme

	}
	_endthreadex((DWORD)index);

	return(0);
};

DWORD WINAPI CapturaTipo22(LPVOID index) {
	BOOL status;
	int j, aux;
	while (true)
	{

		//-------------Tenta Acessar o dado na lista-------------//

		status = WaitForSingleObject(hMutexCOSNSUMIDOR, INFINITE);  //Garante um consumidor por vez 
		status = WaitForSingleObject(hSemLISTAcheia, INFINITE); // Aguarda um espaço preenchido;

		status = WaitForSingleObject(hMutexOCUPADO, INFINITE); // busca no vetor Ocupado uma mensagem do tipo 22
		for (j = 0; j < TAM_LIST; j++) {
			if (OCULPADO[j] == 22) {
				aux = j;
				if (j == TAM_LIST) {
					j = 0;
				}
				break;
			}
		}
		status = ReleaseMutex(hMutexOCUPADO);

		cout << "\n" << LISTA[aux] << "\n" << "LIDO" << "\n"; // Captura a mensagem da lista

		status = ReleaseSemaphore(hSemLISTAvazia, 1, NULL); // Sinaliza que uma mensagem foi lida 

		status = ReleaseMutex(hMutexCOSNSUMIDOR); // Libera Mutex

		Sleep(500); // dorme
	}
	_endthreadex((DWORD)index);
	return (0);
};

void LerLista(int tipo) {
	int j;
	int status;
	string aux = "       ";

	status = WaitForSingleObject(hMutexOCUPADO, INFINITE);
	for (j = 0; j < TAM_LIST; j++) {
		if (OCULPADO[j] == tipo) {
			break;
		}
	}
	status = ReleaseMutex(hMutexOCUPADO);

	cout << "\n" << LISTA[j] << "\n" << "LIDO" << "\n";
	LISTA[j] = aux;
	OCULPADO[j] = 0;

};








// ------------------------------------------------------------------------------------------------------- //