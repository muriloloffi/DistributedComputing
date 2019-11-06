#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <winbase.h>
#include "Funcoes.h"

#define MAXCON 64

DWORD WINAPI ThreadFunc( LPVOID lpParam )
{
    SOCKET s;
    char *buffer;
    int retorno, tam;
    
    s = *(SOCKET *)lpParam;
    buffer = NULL;
    retorno = 1;
    tam = 100;
    for(;;)
    {
        free(buffer);
        buffer = (char *) malloc(100*sizeof(char));
        memset(buffer,'\0',100);
        tam = recv(s,buffer,100,0);
        if(tam > 0)
        {
            printf("Tam mesg recebida: %i\n",tam);
            printf("Mensagem recebida do cliente: %s\n",buffer);
            if(send(s,buffer,strlen(buffer),MSG_DONTROUTE) == SOCKET_ERROR)
            {
                 trataErro();
                 retorno = -1;
                 break;
            }
        }
        else
        { /* Conexao quebrada ou outro erro */
            retorno = -1;
            break;
        }
    }  
    if(buffer != NULL)
        free(buffer);

    closesocket(s);
    printf("Encerrando Thread com retorno = %i\n",retorno);
    
    return retorno;
}

int main(int argc, char *argv[])
{
  SOCKET s,socketCliente;
  SOCKADDR_IN enderecoLocal;
  SOCKADDR_IN enderecoRemoto;
  HANDLE thread, vetorThreads[MAXCON];

  WORD wVersionRequested;
  WSADATA wsaData;
  DWORD threadID,indexThread;
  int err, tam, i;
  char nome[50];
  
  /* Inicializar Servi�o de Comunica��o do Windows 
     Criar Socket
     Ligar o socket criado ao endere�o da m�quina local
     Ouvir conex�es (listen)
     Aceitar conex�es (accept)
     Criar Thread para cada conex�o
  */
    
  threadID = 0;
  indexThread = -1;
  i = 0;
  /* Inicializar Servi�o de Comunica��o do Windows */
  wVersionRequested = MAKEWORD( 2, 0 );
  err = WSAStartup( wVersionRequested, &wsaData );
  if(err == 0) /* Verificar se o servi�o de Comunica��o do Windows Aceita WinSockets 2.0 */
  {
    if(LOBYTE( wsaData.wVersion ) == 2 && HIBYTE( wsaData.wVersion ) == 0 )
    { 
           enderecoLocal.sin_family = AF_INET;
           enderecoLocal.sin_port = htons(1500);
           gethostname(nome,50);
           enderecoLocal.sin_addr = *((IN_ADDR *)*gethostbyname(nome)->h_addr_list);  
/*           enderecoLocal.sin_addr.s_addr = ADDR_ANY; */
           WSASetLastError(0);
           if((s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) != INVALID_SOCKET)
           {
             if(bind(s,(SOCKADDR *)&enderecoLocal,sizeof(enderecoLocal)) == 0)              
             {
                printf("Servidor %s, endereco: %s, aguardando na porta 1500\n",nome,inet_ntoa(enderecoLocal.sin_addr));
                listen(s,MAXCON); /* Max de MAXCON conex�es */
                for(;;)
                {              		 
                     tam = sizeof(enderecoRemoto);
                     if((socketCliente = accept(s,(SOCKADDR *) &enderecoRemoto, &tam)) != SOCKET_ERROR)
                     {
                      	 printf("Conexao estabelecida com: %s\n",inet_ntoa(enderecoRemoto.sin_addr));
                      	 threadID++;
                      	 indexThread++;
                      	 thread = CreateThread(NULL,0,ThreadFunc,(LPVOID)&socketCliente,0,&threadID);
                      	 if(thread == NULL)
                         {
                             printf("Erro de criacao da thread no.: %i. Abortando servidor...!\n",threadID);
                             trataErro();
                             closesocket(s);
                             break;
                         }
                         vetorThreads[indexThread] = thread;
                     }
                     else
                     {
                          printf("Erro na aceitacao da conexao do cliente: %i\n.",threadID+1);
                          trataErro();
                          closesocket(s);
                          break;
                     }
                }
             }
             else
             {
                printf("N�o foi poss�vel realizar a liga��o do socket (bind)\n");
                trataErro();
                closesocket(s);
             }
           }
           else
           {
                printf("Nao foi possivel criar o socket ouvidor (Listener)!!!\n");
                trataErro();
           }
       }
       else
            WSACleanup();
    }
    else
        printf("\n Nao foi possivel inicializar o servico de comunicacao do windows!!");
    
    for(i=0;i<indexThread;i++)
        CloseHandle(vetorThreads[i]);
        
    system("PAUSE");	
    return 0; 	 
}

