#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

int main(int argc, char *argv[])
{
  SOCKET s;
  SOCKADDR_IN enderecoLocal;
  SOCKADDR_IN enderecoRemoto; /* Inicializar Servi�o de Comunica��o do Windows*/              
  char *buffer;               /* Criar Socket                                 */            
                              /* Ligar o socket criado ao endere�o da m�quina local */
  WORD wVersionRequested;     /* Enviar mensagem */
  WSADATA wsaData;            /* Aguardar retorno */
  int err, tam, opcao;
  
  opcao = 0;
  if(argc == 4)
  {
    /* Inicializar Servi�o de Comunica��o do Windows */
    wVersionRequested = MAKEWORD( 2, 0 );
    err = WSAStartup( wVersionRequested, &wsaData );
/* Verificar se o servi�o de Comunica��o do Windows Aceita WinSockets 2.0 */
    if(err == 0) 
    {
     if(LOBYTE( wsaData.wVersion ) == 2 && HIBYTE( wsaData.wVersion ) == 0 )
     { 
      enderecoLocal.sin_family = AF_INET;
	  enderecoLocal.sin_port = htons(0); 
/* 	  enderecoLocal.sin_addr.s_addr = inet_addr(argv[1]); */
 	  enderecoLocal.sin_addr = *((IN_ADDR*) *gethostbyname(nome)->h_addr_list);

      enderecoRemoto.sin_family = AF_INET;
      enderecoRemoto.sin_port = htons(1500);
/*      enderecoRemoto.sin_addr.s_addr = inet_addr(argv[2]); */
      enderecoRemoto.sin_addr.s_addr = inet_addr(argv[1]);
           
      WSASetLastError(0);
      buffer = (char *) malloc(100*sizeof(char));
      strcpy(buffer,argv[3]);
      ioctlsocket(s,FIONBIO,(long *)&opcao);        
      if((s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) != INVALID_SOCKET)
      {           	  
   		 if(bind(s,(SOCKADDR *)&enderecoLocal,sizeof(enderecoLocal)) == 0)              
   		 {
              for(;;)
              {                              
                 printf("Ligacao do socket concluida com sucesso...\n");
                 printf("buffer: %s\n",buffer);
                 tam = sendto(s,buffer,strlen(buffer),MSG_DONTROUTE,
                                   (SOCKADDR *)&enderecoRemoto,sizeof(enderecoRemoto));
                 printf("Numero de bytes enviados por sendto: %i\n",tam);
                 if(tam != SOCKET_ERROR)
                 {
                 printf("Buffer apos o sendto: %s\n",buffer);
                 memset(buffer,'\0',100);
                 err = sizeof(enderecoRemoto);
                 tam = recvfrom(s,buffer,100,MSG_PEEK,(SOCKADDR *)&enderecoRemoto,&err);
        		 if(tam != SOCKET_ERROR && tam != 0)
                  printf("Buffer apos receber a mensagem do servidor: %s\n",buffer);
                 else
                 {
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

