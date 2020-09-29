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
	int id = 1;
	float temp = 1;
	float vel = 1;
}TIPO22; // definição do tipo 22

HANDLE hMutexNSEQ,hMutexOcupado; // handle do mutex que protege NSEQ,OCUPADO
HANDLE hSemLISTA; //handle do semaforo que verifica se u

TIPO11  novaMensagem11();
TIPO22  novaMensagem22();
void EscreverLista(int tipo, TIPO11 m1, TIPO22 m2);

int NSEQ = 1;

DWORD WINAPI LeituraTipo11(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 11
DWORD WINAPI LeituraTipo22(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 22 

// ----------------------------------------------------------------------------------------------------- //


int main() {
	//variaveis e Handles
	HANDLE hTarefa1[2]; // handles da tarefa 1
	DWORD dwLeitura11ID, dwLeitura22ID;
	DWORD dwExitCode = 0;
	DWORD dwRet;
	int j,status;

	//Mutex e Semaforos
	hMutexNSEQ = CreateMutex(NULL, FALSE, L"ProtegeNSEQ");
	hMutexOcupado = CreateMutex(NULL, FALSE, L"ProtegeOCUPADO");
	//hSemLISTA =


	status = WaitForSingleObject(hMutexOcupado, INFINITE);
	for(j=0;j<TAM_LIST;j++){
		OCULPADO[j] = 0;
	}
	status = ReleaseMutex(hMutexOcupado);

	// Criando Threads
	hTarefa1[0] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo11, &j, 0, (CAST_LPDWORD)&dwLeitura11ID);
	if (hTarefa1[0]) printf("Tarefa 1 criada com Id= %0x \n", dwLeitura11ID);

	hTarefa1[1] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo22, &j, 0, (CAST_LPDWORD)&dwLeitura22ID);
	if (hTarefa1[1]) printf("Tarefa 2 criada com Id= %0x \n", dwLeitura22ID);


	// Encerrando as threads 

	dwRet = WaitForMultipleObjects(2, hTarefa1, TRUE, INFINITE);
	//CheckForError((dwRet >= WAIT_OBJECT_0) && (dwRet < WAIT_OBJECT_0 + 2));

	for (j = 0; j < 2; j++) {
		GetExitCodeThread(hTarefa1[j], &dwExitCode);
		printf("thread %d terminou: codigo=%d\n", j, dwExitCode);
		CloseHandle(hTarefa1[j]);	// apaga referência ao objeto
	}  // for 
	CloseHandle(hMutexNSEQ);

	return(0);
}



// --------------- Execução relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //
DWORD WINAPI LeituraTipo11(LPVOID) {
	int status;
	TIPO11 m1;

	status = WaitForSingleObject(hMutexNSEQ, INFINITE); // mutex pra proteger a variavel NSEQ
	m1 = novaMensagem11();
	status = ReleaseMutex(hMutexNSEQ);

	printf("\n %i \t %i \t %i \n", m1.nseq, m1.tipo, m1.cad);




	return(0);

}

DWORD WINAPI LeituraTipo22(LPVOID) {
	int status;
	TIPO22 m1;


	
		status = WaitForSingleObject(hMutexNSEQ, INFINITE); // mutex pra proteger a variavel NSEQ
		m1 = novaMensagem22();
		status = ReleaseMutex(hMutexNSEQ);

		printf("\n %i \t %i \t %i \n", m1.nseq, m1.tipo, m1.cad); \


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
	m2.id = rand() % 9999999 + 1;
	m2.temp = (float)aux / 10;
	m2.vel = (float)aux2 / 10;
	m2.nseq = NSEQ;
	NSEQ++;
	if (NSEQ == 99999) {
		NSEQ = 1;
	}
	return m2;
};

void EscreverLista(int tipo,TIPO11 m1, TIPO22 m2) {
	int j, index;
	int status;

	status = WaitForSingleObject(hMutexOcupado, INFINITE);
	for (j = 0; j < TAM_LIST; j++) {
		if (OCULPADO[j] == 0) {
			index = j;
			break;
		}
	}
	status = ReleaseMutex(hMutexOcupado);
	if (tipo == 11) {
		LISTA[index] = std::to_string(m1.nseq);
		LISTA[index] += std::to_string(m1.tipo);
		LISTA[index] += std::to_string(m1.cad);
		LISTA[index] += std::to_string(m1.gravidade);
		LISTA[index] += std::to_string(m1.classe);
		status = WaitForSingleObject(hMutexOcupado, INFINITE);
		OCULPADO[index] = m1.tipo;
		status = ReleaseMutex(hMutexOcupado);
	} 
	else if(tipo == 22){
		LISTA[index] = std::to_string(m2.nseq);
		LISTA[index] += std::to_string(m2.tipo);
		LISTA[index] += std::to_string(m2.cad);
		LISTA[index] += std::to_string(m2.temp);
		LISTA[index] += std::to_string(m2.vel);
		LISTA[index] += std::to_string(m2.id);

		status = WaitForSingleObject(hMutexOcupado, INFINITE);
		OCULPADO[index] = m2.tipo;
		status = ReleaseMutex(hMutexOcupado);
	}else {
		printf("\ntipo nao existe\n");
	}

};

// ----------------------------------------------------------------------------------------------------- //
