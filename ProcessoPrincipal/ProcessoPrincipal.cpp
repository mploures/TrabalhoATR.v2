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

// Casting para terceiro e sexto parâmetros da função _beginthreadex
typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

// --------------- Declarações relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

//lista na memomiara ram
#define TAM_LIST 20
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
	string id = "";
	float temp = 1;
	float vel = 1;
}TIPO22; // definição do tipo 22

HANDLE hMutexNSEQ, hMutexOCUPADO,hMutexINDICE; // handle do mutex que protege NSEQ,OCUPADO
HANDLE hSemLISTAcheia11, hSemLISTAcheia22, hSemLISTAvazia; //handle do semaforo que verifica se a lsita tah cheia ou vazia;
HANDLE hMutexPRODUTOR, hMutexCOSNSUMIDOR; // handle do mutex que bloqueia o produtor e o consumidor;

TIPO11  novaMensagem11();
TIPO22  novaMensagem22();
//void EscreverLista(int tipo,int j, TIPO11 m1, TIPO22 m2);

int NSEQ = 1;

DWORD WINAPI LeituraTipo11(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 11
DWORD WINAPI LeituraTipo22(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 22 

// ----------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 2 e 3 Captura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI CapturaTipo11(LPVOID);	// declaração da thread  que  gerencia a captura do tipo 11
DWORD WINAPI CapturaTipo22(LPVOID);	// declaração da thread  que  gerencia a captura do tipo 22 

//void LerLista(int tipo);

// ------------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 6: Eventos de Bloqueio e Encerramento --------------- //

#define	i       0x69    //Tecla para bloquear ou retomar a leitura do sistema de inspeção de defeitos---------------------- hEventoI
#define	d       0x64    //Tecla para bloquear ou retomar a captura de mensagens de defeitos de superficíe das tiras-------- hEventoD
#define	e       0x65    //Tecla para bloquear ou retomar a captura de mensagens de dados do processo de laminação---------- hEventoE
#define	a       0x61    //Tecla para bloquear ou retomar a exibição de defeitos de tiras----------------------------------- hEventoA
#define	l       0x6C    //Tecla para bloquear ou retomar a a exibição de dados do processo de laminação-------------------- hEventoL
#define	c       0x63    //Tecla para notificar à tarefa de exibição de dados do processo para limpar sua janela de console- hEventoC
#define	ESC 	0x1B	//Tecla para encerrar o programa

HANDLE hEventoI, hEventoD, hEventoE, hEventoA, hEventoL, hEventoC; // eventos i,d,e,a,l,c
HANDLE hEventoESC; // evento de encerramento geral
HANDLE hEventoNADA; // evento que dispara quando nada acontece

// ------------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 4 e 5 : thread opemProcess --------------- //

DWORD WINAPI AbreTarefa4(LPVOID);	// declaração da thread  que  gerencia da tarefa 4
DWORD WINAPI AbreTarefa5(LPVOID);	// declaração da thread  que  gerencia da tarefa 5
// -------------------------------------------------------------------------------------------- //


int main() {
	//variaveis e Handles
	HANDLE hTarefa1[2]; // handles da tarefa 1: produtoras
	HANDLE hTarefa23[2]; // handle das tarefas 2 e 3: consumidoras
	HANDLE hTarefa45[2]; // handle das tarefas 4 e 5: Exibe dados
	DWORD dwLeitura11ID, dwLeitura22ID, dwCaptura11ID, dwCaptura22ID, dwExibe11ID, dwExibe22ID;
	DWORD dwExitCode = 0;
	DWORD dwRet;
	int j,status,Tecla=0;


	//Mutex e Semaforos
	hMutexNSEQ = CreateMutex(NULL, FALSE, "ProtegeNSEQ");
	hMutexINDICE = CreateMutex(NULL, FALSE, "ProtegeINDICE");
	hMutexOCUPADO = CreateMutex(NULL, FALSE, "ProtegeOCUPADO");
	hMutexPRODUTOR = CreateMutex(NULL, FALSE, "ProtegePRODUTOR");
	hMutexCOSNSUMIDOR = CreateMutex(NULL, FALSE, "ProtegeCOSNSUMIDOR");
	hSemLISTAcheia11 = CreateSemaphore(NULL, 0, TAM_LIST,"SemLISTAcheia11");
	hSemLISTAcheia22 = CreateSemaphore(NULL, 0, TAM_LIST, "SemLISTAcheia22");
	hSemLISTAvazia = CreateSemaphore(NULL, TAM_LIST, TAM_LIST, "SemLISTAvazia");

	// Eventos
	hEventoI = CreateEvent(NULL, FALSE, FALSE, "EventoI");
	hEventoD = CreateEvent(NULL, FALSE, FALSE, "EventoD");
	hEventoE = CreateEvent(NULL, FALSE, FALSE, "EventoE");
	hEventoA = CreateEvent(NULL, FALSE, FALSE, "EventoA");
	hEventoL = CreateEvent(NULL, FALSE, FALSE, "EventoL");
	hEventoC = CreateEvent(NULL, FALSE, FALSE, "EventoC");
	hEventoESC = CreateEvent(NULL, FALSE, FALSE, "EventoESC");
	hEventoNADA= CreateEvent(NULL, FALSE, FALSE, "EventoNADA");

	status = WaitForSingleObject(hMutexOCUPADO, INFINITE);
	for(j=0;j<TAM_LIST;j++){
		OCULPADO[j] = 0;
	}
	status = ReleaseMutex(hMutexOCUPADO);

	// Criando Threads

	//tarefa 1
	hTarefa1[0] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo11, NULL, 0, (CAST_LPDWORD)&dwLeitura11ID);
	if (hTarefa1[0]) 	cout <<"Tarefa 1 leitura 11 criada com Id="<< dwLeitura11ID<< "\n";
	hTarefa1[1] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo22, NULL, 0, (CAST_LPDWORD)&dwLeitura22ID);
	if (hTarefa1[1]) 	cout << "Tarefa 1 leitura 2 criada com Id=" << dwLeitura22ID << "\n";
	// tarefa 2 e 3 
	hTarefa23[0] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)CapturaTipo11, NULL, 0, (CAST_LPDWORD)&dwCaptura11ID);
	if (hTarefa23[0]) 	cout << "Tarefa 2 leitura 11 criada com Id=" << dwCaptura11ID << "\n";
	hTarefa23[1] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)CapturaTipo22, NULL, 0, (CAST_LPDWORD)&dwCaptura22ID);
	if (hTarefa23[1]) 	cout << "Tarefa 3 leitura 22 criada com Id=" << dwCaptura22ID << "\n";
	//tarefa 4 e 5
	hTarefa45[0] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)AbreTarefa4, NULL, 0, (CAST_LPDWORD)&dwExibe11ID);
	if (hTarefa45[0]) 	cout << "Tarefa 4 leitura 11 criada com Id=" << dwExibe11ID << "\n";
	hTarefa45[1] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)AbreTarefa5, NULL, 0, (CAST_LPDWORD)&dwExibe22ID);
    if (hTarefa45[1]) 	cout << "Tarefa 5 leitura 22 criada com Id=" << dwExibe22ID << "\n";



	// Execução continua da tarefa 6 : Disparo dos Eventos 

	do {
		cout << "\n Tecle <i> para gerar evento que bloqueia ou retoma a leitura do sistema de inspecao de defeitos,\n Tecle <d> para gerar evento que bloqueoa ou retoma a captura de mensagens de defeitos de superficie das tiras,\n Tecle <e> para gerar evento que bloqueia ou retoma a captura de mensagens de dados do processo de laminacao,\n Tecle <a> para gerar evento que bloqueia ou retoma a exibicao de defeitos de tiras,\n Tecle <l> para gerar evento que  bloqueia ou retoma a a exibicao de dados do processo de laminacao,\n Tecle <c> para gerar evento que notifica a tarefa de exibicao de dados do processo para limpar sua janela de console,\n ou <Esc> para terminar\n";
		Tecla = _getch();

		if (Tecla == i) {
			status = SetEvent(hEventoI);

			cout << "\n Evento I ocorreu \n";
			Tecla = 0;
		}
		else if (Tecla == d) {
			status = SetEvent(hEventoD);

			cout << "\n Evento D ocorreu \n";
			Tecla = 0;
		}
		else if (Tecla == e) {
			status = SetEvent(hEventoE);

			cout << "\n Evento E ocorreu \n";
			Tecla = 0;
		}
		else if (Tecla == a) {
			status = SetEvent(hEventoA);

			cout << "\n Evento A ocorreu \n";
			Tecla = 0;
		}
		else if (Tecla == l) {
			status = SetEvent(hEventoL);

			cout << "\n Evento L ocorreu \n";
			Tecla = 0;
		}
		else if (Tecla == c) {
			status = SetEvent(hEventoC);

			cout << "\n Evento C ocorreu \n";
			Tecla = 0;
		}
		else if (Tecla == ESC) {
			status = SetEvent(hEventoESC);
			cout << "\n Evento ESC ocorreu \n";
			//Tecla = 0;
		}
		else if (Tecla == 0) {
			cout << "\n nada aconteceu\n";
			status = SetEvent(hEventoNADA);
			
		}
		else {
			status = SetEvent(hEventoNADA);
			cout << "\n Evento nao cadastrado \n";
			Tecla = 0;
		}


	} while (Tecla != ESC);
	cout << "\nsaiu\n";


	// Encerrando as threads 
	//
	dwRet = WaitForMultipleObjects(2, hTarefa1, TRUE, INFINITE);
	//CheckForError((dwRet >= WAIT_OBJECT_0)&& (dwRet < WAIT_OBJECT_0 + 1));
	for (j = 0; j < 2; j++) {
		status =GetExitCodeThread(hTarefa1[j], &dwExitCode);
		cout << "thread"<<j<< "terminou: codigo"<< dwExitCode<<"\n";
		CloseHandle(hTarefa1[j]);	// apaga referência ao objeto
	}  // for 

	dwRet = WaitForMultipleObjects(2, hTarefa23, TRUE, INFINITE);
	//CheckForError((dwRet >= WAIT_OBJECT_0) && (dwRet < WAIT_OBJECT_0 + 2));
	for (j = 0; j < 2; j++) {
		status=GetExitCodeThread(hTarefa23[j], &dwExitCode);
		cout << "thread" << j << "terminou: codigo" << dwExitCode << "\n";
		CloseHandle(hTarefa23[j]);	// apaga referência ao objeto
	}  
	dwRet = WaitForMultipleObjects(2, hTarefa45, TRUE, INFINITE);
	//CheckForError((dwRet >= WAIT_OBJECT_0)&& (dwRet < WAIT_OBJECT_0 + 1));
	for (j = 0; j < 2; j++) {
		status = GetExitCodeThread(hTarefa45[j], &dwExitCode);
		cout << "thread" << j << "terminou: codigo" << dwExitCode << "\n";
		CloseHandle(hTarefa45[j]);	// apaga referência ao objeto
	}  // for 


	//Fecha todos os Handles
	CloseHandle(hMutexNSEQ);
	CloseHandle(hMutexINDICE);
	CloseHandle(hMutexOCUPADO);
	CloseHandle(hSemLISTAcheia11);
	CloseHandle(hSemLISTAcheia22);
	CloseHandle(hSemLISTAvazia);
	CloseHandle(hMutexPRODUTOR);
	CloseHandle(hMutexCOSNSUMIDOR);

	CloseHandle(hEventoA);
	CloseHandle(hEventoI);
	CloseHandle(hEventoD);
	CloseHandle(hEventoE);
	CloseHandle(hEventoL);
	CloseHandle(hEventoC);
	CloseHandle(hEventoNADA);
	CloseHandle(hEventoESC);




	cout << "\nAcione uma tecla para terminar\n";
	Tecla=_getch(); // // Pare aqui, caso não esteja executando no ambiente MDS

	return EXIT_SUCCESS;
}


// --------------- Execução relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI LeituraTipo11(LPVOID index) {
	BOOL status;
	DWORD ret;
	string aux = "erro";
	TIPO11 m1;
	int tipo;
	int nbloqueia = 1; // assume valor 0 quando hEventoI bloqueia e 1 quando hEventoI libera a thread
	HANDLE hEventos[2];

	//hEventoESC = OpenEvent(EVENT_ALL_ACCESS,FALSE,L"EventoESC");
	//hEventoI = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoI");


	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoI;



	do {
		ret = WaitForMultipleObjects(2, hEventos, FALSE, 500);
		tipo = ret - WAIT_OBJECT_0;

		if (tipo == 0) {
			break;
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0;
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1;
		}

		tipo = tipo + nbloqueia;
		if (ret==WAIT_TIMEOUT||tipo == 2) {
				// -------------recebe a mensagem do processo-------------//
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

				status = ReleaseSemaphore(hSemLISTAcheia11, 1, NULL); // Sinaliza que existe uma mensagem

				status = ReleaseMutex(hMutexPRODUTOR); // Libera Mutex
				cout << "\n esta tarefa 1 11 \n";
			}
			Sleep(400); // dorme
	} while (tipo!=0);
	cout << "\n Saiu tarefa 1 11 \n";
    

	_endthreadex((DWORD)index);
	return(0);

}

DWORD WINAPI LeituraTipo22(LPVOID index) {
	BOOL status;
	DWORD ret;
	string aux = "erro";
	TIPO22 m2;
	int tipo;
	int nbloqueia = 1; // assume valor 0 quando hEventoI bloqueia e 1 quando hEventoI libera a thread
	HANDLE hEventos[2];

	//hEventoESC = OpenEvent(EVENT_ALL_ACCESS,FALSE,L"EventoESC");
	//hEventoI = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoI");


	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoI;



	do {
		ret = WaitForMultipleObjects(2, hEventos, FALSE, 500);
		tipo = ret - WAIT_OBJECT_0;

		if (tipo == 0) {
			break;
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0;
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1;
		}

		tipo = tipo + nbloqueia;
		if (ret == WAIT_TIMEOUT || tipo == 2) {

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

			status = ReleaseSemaphore(hSemLISTAcheia22, 1, NULL); // Sonaliza que existe uma mensagem

			status = ReleaseMutex(hMutexPRODUTOR); // Libera Mutex
			cout << "\n esta tarefa 1 22 \n";
			Sleep(200); // dorme
		}
	} while (tipo != 0);
	cout << "\n Saiu tarefa 1 22 \n";

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

// ----------------------------------------------------------------------------------------------------- //

// --------------- Execução relacionadas a tarefa 2 e 3 Captura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI CapturaTipo11(LPVOID index) {
	BOOL status;
	int j;
	int aux = 0;
	string nada = " ";
	DWORD ret;
	int tipo;
	int nbloqueia = 1; // assume valor 0 quando hEventoI bloqueia e 1 quando hEventoI libera a thread
	HANDLE hEventos[2];

	//hEventoESC = OpenEvent(EVENT_ALL_ACCESS,FALSE,L"EventoESC");
	//hEventoI = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoI");


	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoD;



	do {
		ret = WaitForMultipleObjects(2, hEventos, FALSE, 500);
		tipo = ret - WAIT_OBJECT_0;

		if (tipo == 0) {
			break;
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0;
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1;
		}

		tipo = tipo + nbloqueia;
		if (ret == WAIT_TIMEOUT || tipo == 2) {


			//-------------Tenta Acessar o dado na lista-------------//

			status = WaitForSingleObject(hMutexCOSNSUMIDOR, INFINITE);  //Garante um consumidor por vez 
			status = WaitForSingleObject(hSemLISTAcheia11, INFINITE); // Aguarda um espaço preenchido;

			status = WaitForSingleObject(hMutexOCUPADO, INFINITE); // busca no vetor Ocupado uma mensagem do tipo 11
			for (j = 0; j < TAM_LIST; j++) {
				if (OCULPADO[j] == 11) {
					aux = j;
					OCULPADO[j] = 0;
					break;
				}
			}
			status = ReleaseMutex(hMutexOCUPADO);
			cout << "\n" << LISTA[aux] << "\n" << "LIDO" << "\n"; // Captura a mensagem da lista
			LISTA[aux] = nada;

			status = ReleaseSemaphore(hSemLISTAvazia, 1, NULL); // Sinaliza que uma mensagem foi lida 

			status = ReleaseMutex(hMutexCOSNSUMIDOR); // Libera Mutex
			cout << "\n esta tarefa 2 \n";
			Sleep(200); // dorme
		}
	} while (tipo != 0);
	cout << "\n Saiu tarefa 2 \n";
	

	
	_endthreadex((DWORD)index);

	return(0);
};

DWORD WINAPI CapturaTipo22(LPVOID index) {
	BOOL status;
	int j;
	int aux = 0;
	string nada=" ";
	DWORD ret;
	int tipo;
	int nbloqueia = 1; // assume valor 0 quando hEventoI bloqueia e 1 quando hEventoI libera a thread
	HANDLE hEventos[2];

	//hEventoESC = OpenEvent(EVENT_ALL_ACCESS,FALSE,L"EventoESC");
	//hEventoI = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoI");


	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoE;
	

	do {
		ret = WaitForMultipleObjects(2, hEventos, FALSE, INFINITE);
		tipo = ret - WAIT_OBJECT_0;

		if (tipo == 0) {
			break;
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0;
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1;
		}

		tipo = tipo + nbloqueia;
		if (ret == WAIT_TIMEOUT || tipo == 2) {

		//-------------Tenta Acessar o dado na lista-------------//

		status = WaitForSingleObject(hMutexCOSNSUMIDOR, INFINITE);  //Garante um consumidor por vez 
		status = WaitForSingleObject(hSemLISTAcheia22, INFINITE); // Aguarda um espaço preenchido;

		status = WaitForSingleObject(hMutexOCUPADO, INFINITE); // busca no vetor Ocupado uma mensagem do tipo 22
		for (j = 0; j < TAM_LIST; j++) {
			if (OCULPADO[j] == 22) {
				aux = j;
				OCULPADO[j] = 0;
				break;
			}
		}
		status = ReleaseMutex(hMutexOCUPADO);

		cout << "\n" << LISTA[aux] << "\n" << "LIDO" << "\n"; // Captura a mensagem da lista
		LISTA[aux] = nada;

		status = ReleaseSemaphore(hSemLISTAvazia, 1, NULL); // Sinaliza que uma mensagem foi lida 

		status = ReleaseMutex(hMutexCOSNSUMIDOR); // Libera Mutex

		cout << "\n esta tarefa 3 \n";
		Sleep(200); // dorme
		}
	} while (tipo != 0);
	cout << "\n Saiu tarefa 3 \n";

	_endthreadex((DWORD)index);
	return (0);
};

// ------------------------------------------------------------------------------------------------------- //

// --------------- Execução relacionadas a tarefa 4 e 5 : thread opemProcess --------------- //

DWORD WINAPI AbreTarefa4(LPVOID index) {

	BOOL status;
	STARTUPINFO si;				    // StartUpInformation para novo processo
	PROCESS_INFORMATION NewProcess;	// Informações sobre novo processo criado

	SetConsoleTitle("Programa Tarefa1");
	printf("entrou");


	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);	// Tamanho da estrutura em bytes

	status = CreateProcess(
		"D:\\arquivos ATR\\MeusProgramas\\TrabalhoFinal\\TrabalhoPrincipal\\tarefaaux.exe", // Caminho do arquivo executável
		NULL,                       // Apontador p/ parâmetros de linha de comando
		NULL,                       // Apontador p/ descritor de segurança
		NULL,                       // Idem, threads do processo
		FALSE,	                     // Herança de handles
		CREATE_NEW_CONSOLE,	     // Flags de criação
		NULL,	                     // Herança do amniente de execução
		"D:\\arquivos ATR\\MeusProgramas\\TrabalhoFinal\\TrabalhoPrincipal",              // Diretório do arquivo executável
		&si,			             // lpStartUpInfo
		&NewProcess);	             // lpProcessInformation
	if (!status) printf("Erro na criacao do Notepad! Codigo = %d\n", GetLastError());


	_endthreadex((DWORD)index);

	return(0);

}

DWORD WINAPI AbreTarefa5(LPVOID index) {

	return (0);
}
// ----------------------------------------------------------------------------------------- //