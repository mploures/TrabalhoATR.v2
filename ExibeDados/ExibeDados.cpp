// Trabalho de ATR
// 
//	Autor: Matheus Paiva 
//
//	Processo Exibe Dados 
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

int main()
{
    DWORD ret;
    HANDLE hEvento[3];
    int tipo = 5;

    hEvento[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoESC");
    hEvento[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoL");
    hEvento[2] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoC");

    do {
        ret = WaitForMultipleObjects(2, hEvento, FALSE, INFINITE);
        tipo = ret - WAIT_OBJECT_0;

        if (tipo == 0) {
            cout << "\n Evento ESC ocorreu- encerrando \n";
            break;
        }
        else if(tipo==1) {
            cout << "\n Evento L ocorreu \n";
        }
        else if(tipo==2){
            cout << "\n Evento C ocorreu \n";
            Sleep(500);
            system("clear");
        }


    } while (tipo != 0);

    return 0;

}