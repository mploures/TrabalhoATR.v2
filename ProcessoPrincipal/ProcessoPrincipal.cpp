// Trabalho de ATR
// 
//	Autor: Matheus Paiva 
//
//	Processo Principal 
//
//

#define TAM_LIST 10
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>	
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>	
#include<string>
using std::string;


// Casting para terceiro e sexto parâmetros da função _beginthreadex
typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

// --------------- Declarações relacionadas a tarefa 1 Leitura de mensagem tipo 11 e 22 --------------- //

//lista na memomiara ram
string  lista[TAM_LIST];

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
}TIPO22; // definição do tipo 11

HANDLE hMutexNSEQ;
TIPO11  novaMensagem11();
TIPO22  novaMensagem22();

int NSEQ = 1;

DWORD WINAPI LeituraTipo11(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 11
DWORD WINAPI LeituraTipo22(LPVOID);	// declaração da thread  que  gerencia a leitura do tipo 22 

// ----------------------------------------------------------------------------------------------------- //


int main() {
	return(0);
}