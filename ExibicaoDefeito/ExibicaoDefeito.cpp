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

void ExibirDefeitos(char* msg);

int main()
{
    SetConsoleTitle(L"Trabalho de ATR - Exibe Defeitos");

    //Variaveis relacionadas ao MailSlot
    HANDLE EventoMail;
    DWORD dwRecebidos;
    BOOL status;
    HANDLE hEventosInterno[2]; 
    HANDLE hMail;
    string exibir;
    char texto[40];
    int tipoInterno = 1;

    //Variaveis relacionadas ao sincronismo
    DWORD ret;
    HANDLE hEvento[3];
    int tipo = 1;
    int nBloqueia = 1;

   


    //Abre os Eventos de Sincronismo
    hEvento[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoESC");
    hEvento[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoA");

    // Abre o Evento Criado pelo processo principal relacionado ao Mailslot
    EventoMail = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"EventoMail"); 

    // Cria o MailSlot
    hMail = CreateMailslot(L"\\\\.\\mailslot\\MailATR", 38, 100, NULL); 
    // Informa o Processo Principal que o Mailslot foi criado
    ret=SetEvent(EventoMail);

    //Internamente esse evento tambem significa o encerramento da thread
    hEventosInterno[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoESC");
    //Internamete esse evento significa que existe um novo conteudo no mailslot
    hEventosInterno[1] = EventoMail;

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

            ret = WaitForMultipleObjects(2, hEventosInterno, FALSE, INFINITE);
            tipoInterno = ret - WAIT_OBJECT_0;
           
            if (tipoInterno == 0) {
                cout << "\n Evento ESC ocorreu- encerrando \n";
                break;
            }
            else if(tipoInterno == -1) {

             status = ReadFile(hMail, texto, 38 * sizeof(char), &dwRecebidos, NULL);
             if (status == FALSE) {
                 printf(".");
             }
             else {
                 
                 ExibirDefeitos(texto);
                 Sleep(100);
                 
             }


                

            }

            /*
            SetEvent(EventoMail);
            */
        }

    } while (tipo != 0);

    CloseHandle(hMail);
    CloseHandle(hEvento[0]);
    CloseHandle(hEvento[1]);
    CloseHandle(EventoMail);
    CloseHandle(hEventosInterno[0]);
    CloseHandle(hEventosInterno[1]);
    CloseHandle(EventoMail);

    return 0;
}

void ExibirDefeitos(char* msg) {

    string mensagem;
    string aux[7];

    mensagem = msg;

    aux[0] = mensagem.substr(0, 5);  // nseq
    aux[1] = mensagem.substr(6, 2);  // tipo
    aux[2] = mensagem.substr(9, 2);  // cad
    aux[3] = mensagem.substr(12, 2); // grav    
    aux[4] = mensagem.substr(15, 2); // classe
    aux[5] = mensagem.substr(18, 6); // foto
    aux[6] = mensagem.substr(25, 12);// tempo

    if (stoi(aux[0]) > 1 && stoi(aux[0]) <= 99999) {
        mensagem = aux[6];
        mensagem += " | NSEQ:" + aux[0];
        mensagem += " | CAD: " + aux[2];
        mensagem += " | ID FOTO: " + aux[5];
        mensagem += " | GRAV: " + aux[3];
        mensagem += " | CLASSE " + aux[4];
        cout << "\n" << mensagem << "\n";
    }
    else {

    }
}