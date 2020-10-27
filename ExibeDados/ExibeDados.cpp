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
void ExibirArquivo(char* msg);

int main()
{
    SetConsoleTitle(L"Trabalho de ATR - Exibe Dados");
    DWORD ret;
    HANDLE hEvento[3];
    HANDLE hmutexARQ;
    HANDLE hSemARQ;
    HANDLE hEventoARQ;
    int tipo = 5;
    int nBloqueia = 1;
    int size=0;
   
    hSemARQ = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,L"SemARQ");
    hmutexARQ = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"MutexARQ");
    hEvento[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoESC");
    hEvento[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoL");
    hEvento[2] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventoC");
    hEventoARQ = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"EventoARQ"); // Abre o Evento Criado pelo processo principal
    
    do {
        ret = WaitForMultipleObjects(3, hEvento, FALSE, 100);
        tipo = ret - WAIT_OBJECT_0;
        cout << tipo << "\n";
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
      
           /*
            WaitForSingleObject(hEventoARQ, INFINITE);
            size=LerArquivo();
            ReleaseSemaphore(hSemARQ,size, NULL);
            SetEvent(hEventoARQ);
            Sleep(500);
           
            //Sleep(500);
           */
        
        }


    } while (tipo != 0);

    return 0;

}


int LerArquivo() {
    int size=0;
    char texto[45];
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
            cout << exibir << "\n";
            //ExibirArquivo(texto);
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

void ExibirArquivo(char* msg) {

    string mensagem;
    string aux[7];

    mensagem = msg;
  

    aux[0] = mensagem.substr(0, 5);  // nseq
    aux[1] = mensagem.substr(6, 2);  // tipo
    aux[2] = mensagem.substr(9, 2);  // cad
    aux[3] = mensagem.substr(12, 8); // placa
    aux[4] = mensagem.substr(19, 5); // temp
    aux[5] = mensagem.substr(25, 5); // vel
    aux[6] = mensagem.substr(31, 12);// tempo

    if (stoi(aux[0]) > 1 && stoi(aux[0]) <= 99999) {
 
        mensagem = aux[6];
        mensagem += " | NSEQ:" + aux[0];
        mensagem += " | CAD: " + aux[2];
        mensagem += " | ID PLACA: " + aux[3];
        mensagem += " | TEMP: " + aux[4];
        mensagem += " | VEL " + aux[5];
        cout << mensagem << "\n";
        
    }
    else {
        
    }

};