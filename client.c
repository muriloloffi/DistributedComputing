#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <winbase.h>
#include "Funcoes.h"

void modoDeUsar();

int main(int argc, char *argv[])
{
  SOCKET s;
  SOCKADDR_IN enderecoLocal;
  SOCKADDR_IN enderecoRemoto;
  char *buffer, nome[50];
  
  WORD wVersionRequested;
  WSADATA wsaData;
  int err, tam, opcao;
  
  /* Inicializar Servi�o de Comunica��o do Windows 
     Criar Socket
     Ligar o socket criado ao endere�o da m�quina local
     Aguardar chamadas
  */
  
  opcao = 0;
  if(argc == 3)
  {
    /* Inicializar Servi�o de Comunica��o do Windows */
    wVersionRequested = MAKEWORD( 2, 0 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if(err == 0) /* Verificar se o servi�o de Comunica��o do Windows Aceita WinSockets 2.0 */
    {
      if(LOBYTE( wsaData.wVersion ) == 2 && HIBYTE( wsaData.wVersion ) == 0 )
      { 
         if((s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) != INVALID_SOCKET)
          {           	  
            enderecoLocal.sin_family = AF_INET;
            enderecoLocal.sin_port = htons(0); 
            gethostname(nome,50);
            enderecoLocal.sin_addr = *((IN_ADDR *)*gethostbyname(nome)->h_addr_list);
            if(bind(s,(SOCKADDR *)&enderecoLocal,sizeof(enderecoLocal)) == 0)
            {
                printf("Ligacao do socket concluida com sucesso...\n");
                enderecoRemoto.sin_family = AF_INET;
                enderecoRemoto.sin_port = htons(1500);
                enderecoRemoto.sin_addr.s_addr = inet_addr(argv[1]);           
                WSASetLastError(0);
                buffer = (char *) malloc(100*sizeof(char));
                strcpy(buffer,argv[2]);
                ioctlsocket(s,FIONBIO,(long *)&opcao);
                if(connect(s,(SOCKADDR *)&enderecoRemoto,sizeof(enderecoRemoto)) == 0)
                {
                    for(;;)
                    {
                        if((tam = send(s,buffer,strlen(buffer),MSG_DONTROUTE)) > SOCKET_ERROR)
                        {
                            printf("Numero de bytes enviados por send: %i\n",tam);
                            printf("Mensagem Enviada: %s\n",buffer);
                            memset(buffer,'\0',100);
                            tam = recv(s,buffer,100,0);
                            if(tam != SOCKET_ERROR && tam != 0)
                                printf("Mensagem recebida do servidor: %s\n",buffer);
                            else
                            {
                                printf("\tErro na recepcao da mensagem. Abortando cliente!\n");
                                trataErro();
                                closesocket(s);
                                break;
                            }
                        }
                        else
                        {
                            printf("Erro de envio de mensagem. Abortando cliente!\n");
                            if(tam == SOCKET_ERROR)
                                trataErro();
                            closesocket(s);
                            break;
                        }
                        printf("Deseja enviar nova mensagem? (0 - Sim, 1 - Nao): ");
                        fflush(stdin);
                        scanf("%i",&err);
                        if(err == 0)
                        {
                            fflush(stdin);
                            printf("Digite a mensagem (<= 100 caracteres): ");
                            gets(buffer);
                        }
                        else
                        {
                        closesocket(s);
                        break;
                        }
                    }
                }
                else
                {
                    printf("Erro na tentativa de conexao. Abortando cliente!\n");
                    trataErro();
                    closesocket(s);
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
            printf("Nao foi possivel criar o socket!!!\n");
            trataErro();
         }
   	  }
      else
        WSACleanup();
    }
    else
        printf("\n Nao foi possivel inicializar o servico de comunicacao do windows!!");
  }
  else
    modoDeUsar();
	 
  system("PAUSE");	
  return 0;
}


void modoDeUsar()
{
    printf("---Modo de usar:---\n");
    printf("\tClienteTCP <enderecoIPServidor> <mensagem>\n");
}

