// Trabalho de ATR
// 
//	Autor: Matheus Paiva 
//
//	Processo Exibe Defeitos 
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
    SetConsoleTitle(L"Trabalho de ATR - Exibe Defeitos");
    DWORD ret;
    HANDLE hEvento[2];
    HANDLE EventoMail;
    HANDLE hMail;
    DWORD dwRecebidos;

    string exibir;
    int tipo = 1;
    int nBloqueia = 1;
    int j;
    BOOL status;
    char texto[40];


    hEvento[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoESC");
    hEvento[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoA");

    EventoMail = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"EventoMail"); // Abre o Evento Criado pelo processo principal

    hMail = CreateMailslot(L"\\\\.\\mailslot\\MailATR", 0, MAILSLOT_WAIT_FOREVER, NULL); // Cria o MailSlot

    SetEvent(EventoMail);// Informa o Processo Principal que o Mailslot foi criado

    do {
        ret = WaitForMultipleObjects(2, hEvento, FALSE, 100);
        tipo = ret - WAIT_OBJECT_0;
        if (tipo == 0) {
            cout << "\n Evento ESC ocorreu- encerrando \n";
            break;
        }
        else if (tipo == 1 && nBloqueia == 0) {
            nBloqueia = 1;
        }
        else if (tipo == 1 && nBloqueia == 1) {
            nBloqueia = 0;
        }


        if (nBloqueia == 1) {
            WaitForSingleObject(EventoMail, INFINITE);
            //status=ReadFile(hMail, &recebido,sizeof(ENVIO11), &dwRecebidos, NULL);
            status = ReadFile(hMail, texto, 40 * sizeof(char), &dwRecebidos, NULL);
            if (status == FALSE) {
                printf("Erro na Leitura do MailSlotOP\n");
            }
            else {
                for (j = 0; j < 40; j++) {
                    cout << texto[j] << ' ';
                }
                cout << "\n ";
            }


        }




    } while (tipo != 0);

    return 0;
}