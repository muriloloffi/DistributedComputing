#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "Funcoes.h"

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
  
  
  buffer = NULL;
  opcao = 0;
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
      		 enderecoLocal.sin_addr = *((IN_ADDR *) *gethostbyname(nome)->h_addr_list);

/*      		 enderecoLocal.sin_addr.s_addr = inet_addr(argv[1]);*/
           ioctlsocket(s,FIONBIO,(long *)&opcao);     
  		     WSASetLastError(0);
            if((s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) != INVALID_SOCKET)
            {
              if(bind(s,(SOCKADDR *)&enderecoLocal,sizeof(enderecoLocal)) == 0)              
              {
                printf("Ligacao do socket concluida com sucesso...\n");              
                for(;;)
                {
                    free(buffer);
                    buffer = (char *) malloc(100*sizeof(char));
                    memset(buffer,'\0',100);
                    
                    tam = sizeof(enderecoRemoto);
                    err = tam;
/* O grande problema residia em que o 4o. par�metro devia ser 0 e n�o MSG_PEEK como na implementa��o abaixo */                    
                    tam = recvfrom(s,buffer,100,0,(SOCKADDR *)&enderecoRemoto,&tam);                       
                    sendto(s,buffer,strlen(buffer),MSG_DONTROUTE,(SOCKADDR *)&enderecoRemoto,err);                       
                }
                closesocket(s);
                WSACleanup();

