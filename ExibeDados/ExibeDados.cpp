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

int LerArquivo();

int main()
{
    SetConsoleTitle(L"Trabalho de ATR - Exibe Dados");

    //Variaveis relacionadas ao sincronismo
    DWORD ret;
    HANDLE hEvento[3];
    int tipo = 5;
    int nBloqueia = 1;

    //Variaveis relacionadas ao Arquivo
    HANDLE hSemARQ;
    HANDLE hEventoARQFimLeitura, hEventoARQFimEscrita;
    HANDLE hEventosInterno[2];
    int tipoInterno = 1;
    int size=0;
   
    //Abre os Eventos de Sincronismo
    hEvento[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoESC");
    hEvento[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoL");
    hEvento[2] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoC");

    //Abre o meaforo de 100 posições 
    hSemARQ = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"SemARQ");

    // Abre os Eventos que garantem que so um processo terá acesso ao arquivo
    hEventoARQFimLeitura = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoARQFimLeitura");
    hEventoARQFimEscrita = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoARQFimEscrita");

    // Organiza os ventos que ocorrerão internamente
    hEventosInterno[0]= OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoESC"); // Encerra o processo;
    hEventosInterno[1]= OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoARQFimEscrita"); // Espera o fim da escrita;
    
    do {
        ret = WaitForMultipleObjects(3, hEvento, FALSE, 100);
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
        }else if(tipo==2){
            system("cls");
        }
   
        if (nBloqueia == 1) {
            ret = WaitForMultipleObjects(2, hEventosInterno,FALSE,100);
            tipoInterno= ret - WAIT_OBJECT_0;
           
            if (tipoInterno == 0) {
                cout << "\n Evento ESC ocorreu- encerrando \n";
                break;
            }
            else if (tipoInterno == 1) {
                
                size = LerArquivo();
                ReleaseSemaphore(hSemARQ, size, NULL);
                SetEvent(hEventoARQFimLeitura);

            }
        }


    } while (tipo != 0);

    CloseHandle(hEvento[0]);
    CloseHandle(hEvento[1]);
    CloseHandle(hEvento[2]);
    CloseHandle(hSemARQ);
    CloseHandle(hEventoARQFimLeitura);
    CloseHandle(hEventoARQFimEscrita);
    CloseHandle(hEventosInterno[0]);
    CloseHandle(hEventosInterno[1]);

    return 0;

}


int LerArquivo() {
    int size=0;
    char texto[83];
    string exibir;
    FILE* arq;
    errno_t err;
    
        do{
        err = fopen_s(&arq, "..\\Release\\dados.txt", "r");
        if (arq == NULL) {
            printf(".");
        }
        else { 
           
            break; }
        } while (arq == NULL);

        while ((fgets(texto, sizeof(texto), arq)) != NULL) {
            exibir = texto;
            cout << "\n" << exibir ;
            size++;
        }

 
    fclose(arq);

    //limpa o arquivo
  
      
       
    do {
        err = fopen_s(&arq, "..\\Release\\dados.txt", "w");
        if (arq == NULL) {
            printf(".");
        }
        else {
           
            break;
        }
    } while (arq == NULL);
        fclose(arq);
   
    



    return size;
}
