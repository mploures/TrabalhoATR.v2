// Trabalho de ATR
// 
//	Autor: Matheus Paiva 
//
//	Processo Principal 
//
//


#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT  0x0400	// Necessário para ativar novas funções da versão 4


#include <windows.h>
#include <process.h>	
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>
using namespace std;
#include<string>
using std::string;
#include <ctype.h>

#define _CHECKERROR	1
#include "CheckForError.h"

// Casting para terceiro e sexto parâmetros da função _beginthreadex
typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

// --------------- Declarações relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

//lista na memomiara ram
#define TAM_LIST 200
int indice=0;
string  LISTA[TAM_LIST];

typedef struct TIPO11 {
	int nseq = 1;
	int tipo = 11;
	int cad = 0;
	int gravidade = 1;
	int classe = 1;
	string arq = "arq123";
	string hora;
	string minuto;
	string segundo;
	string milesegundo;
}TIPO11; // definição do tipo 11
typedef struct TIPO22 {
	int nseq = 1;
	int tipo = 22;
	int cad = 1;
	string id = "";
	float temp = 1;
	float vel = 1;
	string hora;
	string minuto;
	string segundo;
	string milesegundo;
}TIPO22; // definição do tipo 22

// --------------- Declarações relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

HANDLE hMutexNSEQ, hMutexOCUPADO,hMutexINDICE; // handle do mutex que protege NSEQ,OCUPADO
HANDLE hSemLISTAcheia, hSemLISTAvazia; //handle do semaforo que verifica se a lsita tah cheia ou vazia;
HANDLE hMutexPRODUTOR, hMutexCOSNSUMIDOR; // handle do mutex que bloqueia o produtor e o consumidor;

TIPO11  novaMensagem11();
TIPO22  novaMensagem22();

int NSEQ = 1;

DWORD WINAPI LeituraTipo11(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 11
DWORD WINAPI LeituraTipo22(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 22 

// ----------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 2 e 3 Captura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI CapturaTipo11(LPVOID);	// declaração da thread  que  gerencia a captura do tipo 11
DWORD WINAPI CapturaTipo22(LPVOID);	// declaração da thread  que  gerencia a captura do tipo 22 


// ------------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 6: Eventos de Bloqueio e Encerramento --------------- //

#define	i       0x69    //Tecla para habilitar e desabilitar a leitura do sistema de inspeção de defeitos---------------------- hEventoI
#define	d       0x64    //Tecla para habilitar e desabilitar a captura de mensagens de defeitos de superficíe das tiras-------- hEventoD
#define	e       0x65    //Tecla para habilitar e desabilitar a captura de mensagens de dados do processo de laminação---------- hEventoE
#define	a       0x61    //Tecla para habilitar e desabilitar a exibição de defeitos de tiras----------------------------------- hEventoA
#define	l       0x6C    //Tecla para habilitar e desabilitar a a exibição de dados do processo de laminação-------------------- hEventoL
#define	c       0x63    //Tecla para limpar janela de console que exibe a - hEventoC
#define	ESC 	0x1B	//Tecla para encerrar o programa

HANDLE hEventoI11,hEventoI22, hEventoD, hEventoE, hEventoA, hEventoL, hEventoC; // eventos i,d,e,a,l,c
HANDLE hEventoESC; // evento de encerramento geral

// ------------------------------------------------------------------------------------------------------- //

// --------------- Declarações relacionadas a tarefa 4 e 5 : thread opemProcess --------------- //

DWORD WINAPI AbreTarefa4(LPVOID);	// declaração da thread  que  gerencia da tarefa 4
DWORD WINAPI AbreTarefa5(LPVOID);	// declaração da thread  que  gerencia da tarefa 5

PROCESS_INFORMATION NewTarefa4,NewTarefa5;
// -------------------------------------------------------------------------------------------- //

//variaveis que informa se exite mensagens do tipo 11 e do tipo 22 na lista

HANDLE hMutex11, hMutex22;
int contP11 = 0;
int contP22 = 0;

// --------------- Declarações relacionadas asTemporizações da Tarefa 1 --------------- //

HANDLE hTimerDefeitos;
HANDLE hTimerDados;

// ------------------------------------------------------------------------------------ //

void GuardarEmArquivo(char* msg);
string FormatarMensagem(string msg);
HANDLE hMail;
HANDLE hEventoMail;
HANDLE hSemARQLivres, hSemARQOcupado;
HANDLE hEventoARQFimLeitura, hEventoARQFimEscrita;
HANDLE hSemARQ;


int main() {

	SetConsoleTitle(L"Trabalho de ATR - Principal");
	//variaveis e Handles

	HANDLE hTarefas[6]; // handle para todas as tarefas
	DWORD dwLeitura11ID, dwLeitura22ID, dwCaptura11ID, dwCaptura22ID, dwExibe11ID, dwExibe22ID;
	DWORD dwExitCode = 0;
	DWORD dwRet;
	int aux,j,status,Tecla=0;
	LARGE_INTEGER Preset;

	//Mutex e Semaforos
	hMutexNSEQ = CreateMutex(NULL, FALSE, L"ProtegeNSEQ");
	hMutexINDICE = CreateMutex(NULL, FALSE, L"ProtegeINDICE");
	hMutexOCUPADO = CreateMutex(NULL, FALSE, L"ProtegeOCUPADO");
	hMutexPRODUTOR = CreateMutex(NULL, FALSE, L"ProtegePRODUTOR");
	hMutexCOSNSUMIDOR = CreateMutex(NULL, FALSE, L"ProtegeCOSNSUMIDOR");
	hSemLISTAcheia = CreateSemaphore(NULL, 0, TAM_LIST,L"SemLISTAcheia");
 	hSemLISTAvazia = CreateSemaphore(NULL, TAM_LIST, TAM_LIST, L"SemLISTAvazia"); 
	hMutex11= CreateMutex(NULL, FALSE, L"Protege11");
	hMutex22 = CreateMutex(NULL, FALSE, L"Protege22");
	hSemARQ = CreateSemaphore(NULL, 100, 100, L"SemARQ");

	// Eventos
	hEventoARQFimLeitura = CreateEvent(NULL, FALSE, TRUE, L"EventoARQFimLeitura");// reset automatico
	hEventoARQFimEscrita = CreateEvent(NULL, FALSE, FALSE, L"EventoARQFimEscrita");// reset automatico
	hEventoMail = CreateEvent(NULL, FALSE, FALSE, L"EventoMail");// reset automatico
	hEventoI11 = CreateEvent(NULL, FALSE, FALSE, L"EventoI11"); // reset automatico
	hEventoI22 = CreateEvent(NULL, FALSE, FALSE, L"EventoI22"); // reset automatico
	hEventoD = CreateEvent(NULL, FALSE, FALSE,L"EventoD"); // reset automatico
	hEventoE = CreateEvent(NULL, FALSE, FALSE, L"EventoE"); // reset automatico
	hEventoA = CreateEvent(NULL, FALSE, FALSE, L"EventoA"); // reset automatico
	hEventoL = CreateEvent(NULL, FALSE, FALSE, L"EventoL"); // reset automatico
	hEventoC = CreateEvent(NULL, FALSE, FALSE, L"EventoC"); // reset automatico
	hEventoESC = CreateEvent(NULL, TRUE, FALSE,L"EventoESC"); // reset manual


	//Timer
	hTimerDados = CreateWaitableTimer(NULL, FALSE, L"TimerDados");
	hTimerDefeitos = CreateWaitableTimer(NULL, FALSE, L"TimerDefeitos");;


	// Criando Threads

	//tarefa 1
	hTarefas[0] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo11, NULL, 0, (CAST_LPDWORD)&dwLeitura11ID);
	if (hTarefas[0]) 	cout <<"Tarefa 1 leitura 11 criada com Id="<< dwLeitura11ID<< "\n";
	hTarefas[1] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)LeituraTipo22, NULL, 0, (CAST_LPDWORD)&dwLeitura22ID);
	if (hTarefas[1]) 	cout << "Tarefa 1 leitura 22 criada com Id=" << dwLeitura22ID << "\n";
	// tarefa 2 e 3 
	hTarefas[2] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)CapturaTipo11, NULL, 0, (CAST_LPDWORD)&dwCaptura11ID);
	if (hTarefas[2]) 	cout << "Tarefa 2 captura 11 criada com Id=" << dwCaptura11ID << "\n";
	hTarefas[3] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)CapturaTipo22, NULL, 0, (CAST_LPDWORD)&dwCaptura22ID);
	if (hTarefas[3]) 	cout << "Tarefa 3 captura 22 criada com Id=" << dwCaptura22ID << "\n";
	//tarefa 4 e 5
	hTarefas[4] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)AbreTarefa4, NULL, 0, (CAST_LPDWORD)&dwExibe11ID);
	if (hTarefas[4]) 	cout << "Tarefa 4 exibe 11 criada com Id=" << dwExibe11ID << "\n";
	hTarefas[5] = (HANDLE)_beginthreadex(NULL, 0, (CAST_FUNCTION)AbreTarefa5, NULL, 0, (CAST_LPDWORD)&dwExibe22ID);
    if (hTarefas[5]) 	cout << "Tarefa 5 exibe 22 criada com Id=" << dwExibe22ID << "\n";



	// Execução continua da tarefa 6 : Disparo dos Eventos 

	do {

		// ajuste dos timers
		aux=rand() % 2000 + 100;
		Preset.QuadPart = -(10000*aux);
		status= SetWaitableTimer(hTimerDefeitos, &Preset, aux, NULL, NULL, FALSE);
		Preset.QuadPart = -(10000 * 500);
		status= SetWaitableTimer(hTimerDados, &Preset, 500, NULL, NULL, FALSE);
		

		
		cout << "\n Tecle <i> para gerar evento que bloqueia ou retoma a leitura do sistema de inspecao de defeitos,\n Tecle <d> para gerar evento que bloqueoa ou retoma a captura de mensagens de defeitos de superficie das tiras,\n Tecle <e> para gerar evento que bloqueia ou retoma a captura de mensagens de dados do processo de laminacao,\n Tecle <a> para gerar evento que bloqueia ou retoma a exibicao de defeitos de tiras,\n Tecle <l> para gerar evento que  bloqueia ou retoma a a exibicao de dados do processo de laminacao,\n Tecle <c> para gerar evento que notifica a tarefa de exibicao de dados do processo para limpar sua janela de console,\n ou <Esc> para terminar\n";
		Tecla = _getch();

		if (Tecla == i) {
			status = SetEvent(hEventoI11);
			status = SetEvent(hEventoI22);

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
		else{
			cout << "\n Evento nao cadastrado \n";
			Tecla = 0;
		}

	


	} while (Tecla != ESC);
	


	// Encerrando as threads 
	dwRet = WaitForMultipleObjects(6, hTarefas, TRUE, INFINITE);
	//CheckForError((dwRet >= WAIT_OBJECT_0)&& (dwRet < WAIT_OBJECT_0 + 5));
	for (j = 0; j < 6; j++) {
		status =GetExitCodeThread(&hTarefas[j], &dwExitCode);
		cout << "thread "<<j<< " terminou: codigo "<< dwExitCode<<"\n";
		CloseHandle(hTarefas[j]);	// apaga referência ao objeto
	}  // for 
	ResetEvent(hEventoESC);

	CloseHandle(NewTarefa4.hProcess);
	CloseHandle(NewTarefa5.hProcess);

	//Fecha todos os Handles
	CloseHandle(hMutexNSEQ);
	CloseHandle(hMutexINDICE);
	CloseHandle(hMutexOCUPADO);
	CloseHandle(hSemLISTAcheia);
	CloseHandle(hSemLISTAvazia);
	CloseHandle(hMutexPRODUTOR);
	CloseHandle(hMutexCOSNSUMIDOR);
	CloseHandle(hMutex11);
	CloseHandle(hMutex22);

	CloseHandle(hEventoMail);
	CloseHandle(hSemARQ);
	CloseHandle(hEventoARQFimLeitura);
	CloseHandle(hEventoARQFimEscrita);

	CloseHandle(hEventoA);
	CloseHandle(hEventoI11);
	CloseHandle(hEventoI22);
	CloseHandle(hEventoD);
	CloseHandle(hEventoE);
	CloseHandle(hEventoL);
	CloseHandle(hEventoC);
	CloseHandle(hEventoESC);





	cout << "\nAcione uma tecla para terminar\n";
	Tecla=_getch(); // // Pare aqui, caso não esteja executando no ambiente MDS

	return EXIT_SUCCESS;
}


// --------------- Execução relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI LeituraTipo11(LPVOID index) {

	// Variaveis de controle de thread e elementos de sincronização
	int status;
	DWORD ret;
	DWORD dwRet;
	//Variaveis de construção da mensagem
	string aux = "erro";
	char Print[5];
	TIPO11 m1;
	//Variaveis que gerem a parte de evento da thread
	int tipo;
	int nbloqueia = 1; // assume valor 0 quando hEventoI bloqueia e 1 quando hEventoI libera a thread
	HANDLE hEventos[3];

	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoI11;
	hEventos[2] = hTimerDefeitos;

	//Rotina Principal
	do {
		// Espera a ocorrencia de um evento
		ret = WaitForMultipleObjects(3, hEventos, FALSE, INFINITE);

		// retona qual a posição do evento que ocorreu 0 para ESC e 1 para I e 2 Para o Temporizador
		tipo = ret - WAIT_OBJECT_0;

		if (tipo == 0) {
			break;// encerra o while
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0; // bloqueia
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1; //libera 
		}

		if (tipo == 2 && nbloqueia == 1) {

			// -------------recebe a mensagem do processo-------------//
			dwRet = WaitForSingleObject(hMutexNSEQ, INFINITE); // mutex pra proteger a variavel NSEQ
			//CheckForError((dwRet >= WAIT_OBJECT_0));
			m1 = novaMensagem11();
			status = ReleaseMutex(hMutexNSEQ);

			// -------------Produz Mensagem-------------//

			status = sprintf(Print, "%05d", m1.nseq);
			aux = Print;
			aux += "/";
			aux += to_string(m1.tipo) + "/0";
			if (m1.gravidade == 10) {
				aux += to_string(m1.cad) + "/";
			}
			else {
				aux += to_string(m1.cad) + "/0";
			}
			aux += to_string(m1.gravidade) + "/0";
			aux += to_string(m1.classe) + "/";
			aux += m1.arq;
			aux += "/";
			aux += m1.hora + ":";
			aux += m1.minuto + ":";
			aux += m1.segundo + ":";
			aux += m1.milesegundo + "\0";

			//-------------Tenta guardar o dado produzido-------------//

			dwRet = WaitForSingleObject(hMutexPRODUTOR, INFINITE);  //Garante um produtor por vez 

			dwRet = WaitForSingleObject(hSemLISTAvazia, INFINITE); // Aguarda um espaço vazio

			dwRet = WaitForSingleObject(hMutexINDICE, INFINITE);//atualiza o indice


			indice = (indice + 1) % TAM_LIST;
			LISTA[indice] = aux; // Armazena a mensagem na lista

			dwRet = WaitForSingleObject(hMutex11, INFINITE);
			contP11++; //atualiza o numero de produtos tipo 11 na lista			
			status = ReleaseMutex(hMutex11);

			status = ReleaseMutex(hMutexINDICE);
			status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL); // Sinaliza que existe uma mensagem
			status = ReleaseMutex(hMutexPRODUTOR); // Libera Mutex

		}

	} while (tipo != 0);


	//Encerramento da thread

	_endthreadex((DWORD)index);
	return(0);
}

DWORD WINAPI LeituraTipo22(LPVOID index) {

	// Variaveis de controle de thread e elementos de sincronização
	int status;
	DWORD ret;
	DWORD dwRet;
	//Variaveis de construção da mensagem
	string aux = "erro";
	char Print[5];
	TIPO22 m2;
	//Variaveis que gerem a parte de evento da thread
	int tipo;
	int nbloqueia = 1; // assume valor 0 quando hEventoI bloqueia e 1 quando hEventoI libera a thread
	HANDLE hEventos[3];


	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoI22;
	hEventos[2] = hTimerDados;


	// Rotina principal
	do {

		ret = WaitForMultipleObjects(3, hEventos, FALSE, INFINITE);
		// retona qual a posição do evento que ocorreu 0 para ESC e 1 para I e 2 para o Temporizador
		tipo = ret - WAIT_OBJECT_0;

		if (tipo == 0) {
			break;
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0; // bloqueia
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1; //libera 
		}


		if (tipo == 2 && nbloqueia == 1) {

			// -------------recebe a mensagem-------------//
			dwRet = WaitForSingleObject(hMutexNSEQ, INFINITE); // mutex pra proteger a variavel NSEQ
			m2 = novaMensagem22();
			status = ReleaseMutex(hMutexNSEQ);

			// -------------Produz Mensagem-------------//
			int real;

			status = sprintf(Print, "%05d", m2.nseq);
			aux = Print;
			aux += "/";
			aux += to_string(m2.tipo) + "/0";
			aux += to_string(m2.cad) + "/";
			aux += m2.id + "/";
			status = sprintf(Print, "%03d", (int)m2.temp);
			aux += Print;
			real = (int)((m2.temp - (int)(m2.temp)) * 10);
			status = sprintf(Print, ".%d", real);
			aux += Print;
			aux += "/";
			status = sprintf(Print, "%03d", (int)m2.vel);
			aux += Print;
			real = (int)((m2.vel - (int)(m2.vel)) * 10);
			status = sprintf(Print, ".%d", real);
			aux += Print;
			aux += "/";
			aux += m2.hora + ":";
			aux += m2.minuto + ":";
			aux += m2.segundo + ":";
			aux += m2.milesegundo +"\0";


			//-------------Tenta guardar o dado produzido-------------//

			dwRet = WaitForSingleObject(hMutexPRODUTOR, INFINITE);  //Garante um produtor por vez 

			dwRet = WaitForSingleObject(hSemLISTAvazia, INFINITE); // Aguarda um espaço vazio

			dwRet = WaitForSingleObject(hMutexINDICE, INFINITE);//atualiza o indice

			indice = (indice + 1) % TAM_LIST;
			LISTA[indice] = aux; // Armazena a mensagem na lista

			dwRet = WaitForSingleObject(hMutex22, INFINITE);
			contP22++; // atualiza o numero de mensagens tipo 22 na lista
			status = ReleaseMutex(hMutex22);


			status = ReleaseMutex(hMutexINDICE);
			status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL); // Sinaliza que existe uma mensagem
			status = ReleaseMutex(hMutexPRODUTOR); // Libera Mutex
		}

	} while (tipo != 0);

	//Encerramento da Thread

	_endthreadex((DWORD)index);
	return(0);
}

TIPO11 novaMensagem11() {
	TIPO11 m1;
	SYSTEMTIME tempo = { 0 };
	GetLocalTime(&tempo); // recebe o horario corrente

	// Gera valores aleatorios para a cadeira, a classe e a gravidade 	
	m1.cad = rand() % 6 + 1;
	m1.classe = rand() % 9 + 1;
	m1.gravidade = rand() % 10 + 1;

	// gera uma sequencia aleatoria de 3 caracteres e 3 numeros
	m1.arq = (char)(rand() % 25 + 65);
	m1.arq += (char)(rand() % 25 + 65);
	m1.arq += (char)(rand() % 25 + 65);
	m1.arq += to_string(rand() % 9);
	m1.arq += to_string(rand() % 9);
	m1.arq += to_string(rand() % 9);

	// Atribui o valor de NSEQ e o atualiza
	m1.nseq = NSEQ;
	NSEQ++;
	if (NSEQ == 99999) {
		NSEQ = 1;
	}
	// A variavel NSEQ é protegida antes de chamar a função

	// atribui o horario local;
	m1.hora = to_string(tempo.wHour);
	m1.minuto = to_string(tempo.wMinute);
	m1.segundo = to_string(tempo.wSecond);
	m1.milesegundo = to_string(tempo.wMilliseconds);

	return m1;
};

TIPO22 novaMensagem22()
{
	TIPO22 m2;
	int aux = rand() % 9999;
	int aux2 = rand() % 9999;
	SYSTEMTIME tempo = { 0 };
	GetLocalTime(&tempo); // recebe o horario corrente

	// Gera valores aleatorios para a cadeira, a temperatura e a velocidade 	
	m2.cad = rand() % 9 + 1;
	m2.temp = (float)aux / 10;
	m2.vel = (float)aux2 / 10;

	// gera uma sequencia aleatoria de 2 caracteres e 4 numeros
	m2.id = (char)(rand() % 25 + 65);
	m2.id += (char)(rand() % 25 + 65);
	m2.id += (char)(rand() % 25 + 65);
	m2.id += "-";
	m2.id += to_string(rand() % 9);
	m2.id += to_string(rand() % 9);
	m2.id += to_string(rand() % 9);
	m2.id += to_string(rand() % 9);

	// Atribui o valor de NSEQ e o atualiza
	m2.nseq = NSEQ;
	NSEQ++;
	if (NSEQ == 99999) {
		NSEQ = 1;
	}
	// A variavel NSEQ é protegida antes de chamar a função


	// atribui o horario local;
	m2.hora = to_string(tempo.wHour);
	m2.minuto = to_string(tempo.wMinute);
	m2.segundo = to_string(tempo.wSecond);
	m2.milesegundo = to_string(tempo.wMilliseconds);

	return m2;
};

// ----------------------------------------------------------------------------------------------------- //

// --------------- Execução relacionadas a tarefa 2 e 3 Captura de mensagem tipo 11 e 22 --------------- //

DWORD WINAPI CapturaTipo11(LPVOID index) {
	// Variaveis de controle de thread e elementos de sincronização
	BOOL status;
	DWORD ret;
	DWORD dwRet;

	//Variaveis de captura da mensagem
	string nada = "nada 11";
	TIPO22 m2;
	int j;
	int index11=0;
	int size = 0;

	//Variaveis que gerem a parte de evento da thread
	int tipo;    // tipo do evento
	int nbloqueia = 1; // assume valor 0 quando hEventoD bloqueia e 1 quando hEventoI libera a thread
	HANDLE hEventos[2];
	int AJUDA = 0; // variavel que manipula o numero de produtos 11

	// Variaveis Para o envio de Mensagem atraves do Mailslot
	DWORD dwEnviados;
	char texto[38];
	string msg;

	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoD;


	// Espera sincronismo para servidor e mailslot serem criados
	ret=WaitForSingleObject(hEventoMail, INFINITE);

	hMail= CreateFile(L"\\\\.\\mailslot\\MailATR",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	do {
		// Espera a ocorrencia de um evento; para não travar nessa linha o time_out deve ser  diferente de INFINITE
		ret = WaitForMultipleObjects(2, hEventos, FALSE, 100);

		tipo = ret - WAIT_OBJECT_0;// retona qual a posição do evento que ocorreu 0 para ESC e 1 para D

		if (tipo == 0) {
			break;// encerra o do while
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0; // bloqueia
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1; //libera 
		}

		// A variavel globa contP11 contem o numero de mensagem 11 na lista de mensagem
		dwRet = WaitForSingleObject(hMutex11, INFINITE);
		AJUDA = contP11;
		ReleaseMutex(hMutex11);

		if (nbloqueia == 1 && AJUDA >1) {


			//-------------Tenta Acessar o dado na lista-------------//


			dwRet = WaitForSingleObject(hMutexCOSNSUMIDOR, INFINITE);  //Garante um consumidor por vez 
			dwRet = WaitForSingleObject(hSemLISTAcheia, INFINITE); // Aguarda um espaço preenchido;

			// Encontra a Primeira Mensagem tipo 11 na lista
			for (j = 0; j < TAM_LIST; j++) {
				msg = LISTA[j];
				if (msg != "") {
					if (msg.substr(6, 2) == "11") {
						index11 = j;
						break;
					}
				}
			}
			if (msg != "") {
				if (msg.substr(6, 2) == "11") {
					// Grava a mensagem em um vetor de caracter
					for (j = 0; j < msg.size(); j++) {
						texto[j] = msg[j];
					}

					// Envia para o mailslot
					status = WriteFile(hMail, texto, 38 * sizeof(char), &dwEnviados, NULL);
					//cout << texto << " - 11 \n";

					//Sinaliza que uma mensagem foi escrita
					SetEvent(hEventoMail);

					//limpa o vetor de texto
					for (j = 0; j < 37; j++) {
						texto[j] = '0';
					}
					texto[37] = '\0';

					//Coloca nada dentro da lista;
					nada = texto;
					LISTA[index11] = nada;
					dwRet = WaitForSingleObject(hMutex11, INFINITE);
					contP11--; //Atualiza o numero de mensagens tipo 11 na lista
					status = ReleaseMutex(hMutex11);

					status = ReleaseSemaphore(hSemLISTAvazia, 1, NULL); // Sinaliza que uma mensagem foi lida 
					status = ReleaseMutex(hMutexCOSNSUMIDOR); // Libera Mutex

				}
				else {
					// não existia mensagem do tipo 11 na lista
					status = ReleaseMutex(hMutexCOSNSUMIDOR);
					status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL);
				}
			}
			else {
				// não existia mensagem do tipo 11 na lista
				status = ReleaseMutex(hMutexCOSNSUMIDOR);
				status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL);
			}
		}

	} while (tipo != 0);
	
	
	_endthreadex((DWORD)index);

	return(0);
};

DWORD WINAPI CapturaTipo22(LPVOID index) {
	// Variaveis de controle de thread e elementos de sincronização
	BOOL status;
	DWORD ret;
	DWORD dwRet;

	//Variaveis de captura da mensagem
	string nada = " nada 22";
	TIPO22 m2;
	int j;
	int index22=0;

	//Variaveis que gerem a parte de evento da thread
	int tipo;    // tipo do evento
	int nbloqueia = 1; // assume valor 0 quando hEventoE bloqueia e 1 quando hEventoE libera a thread
	HANDLE hEventos[2];
	int AJUDA = 0; // variavel que manipula o numero de produtos 11

	// Variaveis Para o envio de Mensagem atraves do Arquivo dados.txt
	char texto[82];
	string msg;
	string mensagem;
	string aux[7];

	hEventos[0] = hEventoESC;
	hEventos[1] = hEventoE;

	do {
		// Espera a ocorrencia de um evento; para não travar nessa linha o time_out deve ser  diferente de INFINITE
		ret = WaitForMultipleObjects(2, hEventos, FALSE, 100);

		tipo = ret - WAIT_OBJECT_0;// retona qual a posição do evento que ocorreu 0 para ESC e 1 para E

		if (tipo == 0) {
			break;// encerra o do while
		}
		else if (tipo == 1 && nbloqueia == 1) {
			nbloqueia = 0; // bloqueia
		}
		else if (tipo == 1 && nbloqueia == 0) {
			nbloqueia = 1; //libera 
		}

		// A variavel global contP22 contem o numero de mensagem 22 na lista de mensagem
		dwRet = WaitForSingleObject(hMutex22, INFINITE);
		AJUDA = contP22;
		ReleaseMutex(hMutex22);

		if (nbloqueia == 1 && AJUDA > 1) {

			//-------------Tenta Acessar o dado na lista-------------//

			dwRet = WaitForSingleObject(hMutexCOSNSUMIDOR, INFINITE);  //Garante um consumidor por vez 
			dwRet = WaitForSingleObject(hSemLISTAcheia, INFINITE); // Aguarda um espaço preenchido;

			// Encontra a Primeira Mensagem tipo 22 na lista
			for (j = 0; j < TAM_LIST; j++) {
				msg = LISTA[j];
				if (msg != "") {
					if (msg.substr(6, 2) == "22") {
						index22 = j;
						break;
					}
				}
			}

			if (msg != "") {
				if (msg.substr(6, 2) == "22") {

					msg = FormatarMensagem(msg);
					// Grava a mensagem em um vetor de caracter
					for (j = 0; j < msg.size(); j++) {
						texto[j] = msg[j];
					}

					// Envia para o Arquivo
					WaitForSingleObject(hEventoARQFimLeitura,100);
					WaitForSingleObject(hSemARQ,INFINITE);
					GuardarEmArquivo(texto);
					SetEvent(hEventoARQFimEscrita);
					//cout << texto << "- 22\n";


					//limpa o vetor de texto
					for (j = 0; j < msg.size(); j++) {
						texto[j] = '0';
					}
					texto[81] = '\0';

					//Coloca nada dentro da lista;
					nada = texto;
					LISTA[index22] = nada;

					dwRet = WaitForSingleObject(hMutex22, INFINITE);
					contP22--; // Atualiza o numero de produtos tipo 22
					status = ReleaseMutex(hMutex22);

					status = ReleaseSemaphore(hSemLISTAvazia, 1, NULL); // Sinaliza que uma mensagem foi lida 
					status = ReleaseMutex(hMutexCOSNSUMIDOR); // Libera Mutex
				}
				else {
					// não existia mensagem do tipo 22 na lista
					status = ReleaseMutex(hMutexCOSNSUMIDOR);
					status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL);
				}
			}else {
				// não existia mensagem do tipo 22 na lista
				status = ReleaseMutex(hMutexCOSNSUMIDOR);
				status = ReleaseSemaphore(hSemLISTAcheia, 1, NULL);
			}


		}
			
	
	} while (tipo != 0);

	//cout << "\n Saiu tarefa 3 \n";
	
	_endthreadex((DWORD)index);
	return (0);
};

void GuardarEmArquivo(char* msg) {
	FILE* arq;
	int j;
	errno_t err;
	

	do {
		err = fopen_s(&arq,"..\\Release\\dados.txt", "a");
		if (arq == NULL) {
			printf(".");
		}else {
			break;
		}
	} while (arq == NULL);

	for (j = 0; j < 82; j++) {
		fputc(msg[j], arq);
	}
	fputc('\n', arq);

	fclose(arq);

}

string FormatarMensagem(string msg) {

	string mensagem;
	string aux[7];
	


	
	mensagem = msg;

	if (msg != "") {
		aux[0] = mensagem.substr(0, 5);  // nseq
		aux[1] = mensagem.substr(6, 2);  // tipo
		aux[2] = mensagem.substr(9, 2);  // cad
		aux[3] = mensagem.substr(12, 8); // placa
		aux[4] = mensagem.substr(21, 5); // temp
		aux[5] = mensagem.substr(27, 5); // vel
		aux[6] = mensagem.substr(33, 12);// tempo

		if (stoi(aux[0]) > 1 && stoi(aux[0]) <= 99999) {
			mensagem = aux[6];
			mensagem += " | NSEQ:" + aux[0];
			mensagem += " | CAD: " + aux[2];
			mensagem += " | ID PLACA: " + aux[3];
			mensagem += " | TEMP: " + aux[4];
			mensagem += " | VEL " + aux[5];
			mensagem += "\0";


		}
	
	}
	return mensagem;
}
// ------------------------------------------------------------------------------------------------------- //

// --------------- Execução relacionadas a tarefa 4 e 5 : thread opemProcess --------------- //

DWORD WINAPI AbreTarefa4(LPVOID index) {

	BOOL status;
	STARTUPINFO si;				    // StartUpInformation para novo processo
	//PROCESS_INFORMATION NewProcess;	// Informações sobre novo processo criado

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);	// Tamanho da estrutura em bytes

	status=CreateProcess(
		L"..\\Release\\ExibicaoDefeito.exe", // Caminho do arquivo executável
		NULL,                       // Apontador p/ parâmetros de linha de comando
		NULL,                       // Apontador p/ descritor de segurança
		NULL,                       // Idem, threads do processo
		FALSE,	                     // Herança de handles
		CREATE_NEW_CONSOLE,	     // Flags de criação
		NULL,	                     // Herança do amniente de execução
		L"..\\Release",              // Diretório do arquivo executável
		&si,			             // lpStartUpInfo
		&NewTarefa4);	             // lpProcessInformation
	if (!status) printf("Erro na criacao do Notepad! Codigo = %d\n", GetLastError());



	_endthreadex((DWORD)index);

	return(0);

}

DWORD WINAPI AbreTarefa5(LPVOID index) {

	BOOL status;
	STARTUPINFO si;				    // StartUpInformation para novo processo
	//PROCESS_INFORMATION NewProcess;	// Informações sobre novo processo criado


	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);	// Tamanho da estrutura em bytes

	status = CreateProcess(
		L"..\\Release\\ExibeDados.exe", // Caminho do arquivo executável
		NULL,                       // Apontador p/ parâmetros de linha de comando
		NULL,                       // Apontador p/ descritor de segurança
		NULL,                       // Idem, threads do processo
		FALSE,	                     // Herança de handles
		CREATE_NEW_CONSOLE,	     // Flags de criação
		NULL,	                     // Herança do amniente de execução
		L"..\\Release",              // Diretório do arquivo executável
		&si,			             // lpStartUpInfo
		&NewTarefa5);	             // lpProcessInformation
	if (!status) printf("Erro na criacao do Notepad! Codigo = %d\n", GetLastError());



	_endthreadex((DWORD)index);

	return(0);

}

// ----------------------------------------------------------------------------------------- //