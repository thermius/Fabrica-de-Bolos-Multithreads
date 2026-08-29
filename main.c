#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#define PRODUCAO 	1000
#include "fila.c"
#include "multithread.c"
                                       
int main ()
{
	/*Iremos criar 4 threads que junto com a thread main, dá ao programa 4 threads que iráo compartilhar de filas para fazer a produção de bolos
	* 1. thread main crair uma fila de pedidos de bolo; (1s)
	* 2. trabalhador_1 mistura os ingredientes;  (1s)
	* 3. trabalhador_2 coloca o bolo para assar; (1s)
	* 4. trabalhador_3 embala o bolo para viagem; (1s)
	*/

	/*trabalhadores*/
	pthread_t trabalhador_1, trabalhador_2, trabalhador_3;

	/*argumentos*/
    ArgumentosParaAsThrads 	args_trabalhador_1, args_trabalhador_2,args_trabalhador_3;	


	/*cada trabalhador busca por demanda em uma fila especifica da esteira*/
	Fila *fila_de_pedidos 			= NULL;			/*o main recebe e insere pedidos aqui para que trabalhador_1 possa misturar os ingredientes*/
	Fila *fila_de_bolos_para_assar 	= NULL;			/*o trabalhador_1 recebe o pedido de bolo, faz a mistura e coloca na fila de bolos para assar*/
	Fila *fila_de_bolos_assados 	= NULL;			/*o trabalhador_2 vai até a fila de bolos para assar, pega o bolo, assa e coloca na fila de bolos para embalar*/
	Fila *bolos_finalizados 		= NULL;			/*o trabalhador_3 vai até a fila de bolos para viagem, embala, despanha e registra que o pedido foi enviado*/

	/*main enfileira uma quantidade de bolos para produção*/
	for (int i = 0; i < PRODUCAO; i++) Enfileirar (&fila_de_pedidos, &i, sizeof (int), 0,ENFILEIRAR_DADO_APONTADO); 
	printf("[ NOTA ] - main(): produção de %i bolos foi iniciada\n", PRODUCAO);

	/*cria os mutex*/
	pthread_mutex_t m_fila_de_pedidos;
	pthread_mutex_t m_fila_de_bolos_para_assar;
	pthread_mutex_t m_fila_de_bolos_assados;

	/*inicializa*/
    pthread_mutex_init(&m_fila_de_pedidos, NULL);
    pthread_mutex_init(&m_fila_de_bolos_para_assar, NULL);
    pthread_mutex_init(&m_fila_de_bolos_assados, NULL);

    /*cria as condionais*/
    pthread_cond_t cond_bolo_misturado	= PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_bolo_assado		= PTHREAD_COND_INITIALIZER;

    /*argumentos do trabalhador 1*/
    args_trabalhador_1.array_de_ponteiros[0]  = (void*) &fila_de_pedidos;
    args_trabalhador_1.array_de_ponteiros[1]  = (void*) &m_fila_de_pedidos;
    args_trabalhador_1.array_de_ponteiros[2]  = (void*) &fila_de_bolos_para_assar;
    args_trabalhador_1.array_de_ponteiros[3]  = (void*) &m_fila_de_bolos_para_assar;
    args_trabalhador_1.array_de_ponteiros[4]  = NULL;

    /*argumentos do trabalhador 2*/
    args_trabalhador_2.array_de_ponteiros[0]  = (void*) &fila_de_bolos_para_assar;
    args_trabalhador_2.array_de_ponteiros[1]  = (void*) &m_fila_de_bolos_para_assar;
    args_trabalhador_2.array_de_ponteiros[2]  = (void*) &fila_de_bolos_assados;
    args_trabalhador_2.array_de_ponteiros[3]  = (void*) &m_fila_de_bolos_assados;
    args_trabalhador_2.array_de_ponteiros[4]  = (void*) &cond_bolo_assado;
	args_trabalhador_2.array_de_ponteiros[5]  = NULL;

     /*argumentos do trabalhador 3*/
    args_trabalhador_3.array_de_ponteiros[0]  = (void*) &fila_de_bolos_assados;
    args_trabalhador_3.array_de_ponteiros[1]  = (void*) &m_fila_de_bolos_assados;
    args_trabalhador_3.array_de_ponteiros[2]  = (void*) &bolos_finalizados;
    args_trabalhador_3.array_de_ponteiros[3]  = (void*) &cond_bolo_assado;
	args_trabalhador_3.array_de_ponteiros[4]  = NULL;


	/*cria os trabalhadores e aponta cada um para suas funções*/
	pthread_create(&trabalhador_1, 		NULL,&RotinaTrabalhador1, 	(void*)&args_trabalhador_1);
	pthread_create(&trabalhador_2, 		NULL,&RotinaTrabalhador2, 	(void*)&args_trabalhador_2);
	pthread_create(&trabalhador_3, 		NULL,&RotinaTrabalhador3, 	(void*)&args_trabalhador_3);

	/*loop que acompanha a produção*/
	global_controlador = 1;

	while (1)
	{
			char c = 0;
			printf("[ ENTRADA ] - main(): digite 'a' para finalizar as threads\n");
			scanf (" %c",&c);
			if (c == 'a') break;			
	}

	global_controlador = 0;
	pthread_mutex_lock(&m_fila_de_bolos_assados);
	pthread_cond_broadcast(&cond_bolo_assado);
	pthread_mutex_unlock(&m_fila_de_bolos_assados);
	pthread_join(trabalhador_1, NULL);
	pthread_join(trabalhador_2, NULL);
	pthread_join(trabalhador_3, NULL);
	pthread_mutex_destroy(&m_fila_de_pedidos);
	pthread_mutex_destroy(&m_fila_de_bolos_para_assar);
	pthread_mutex_destroy(&m_fila_de_bolos_assados);
	pthread_cond_destroy(&cond_bolo_assado);
	DestruirFila (&fila_de_pedidos);
	DestruirFila (&fila_de_pedidos);
	DestruirFila (&fila_de_bolos_para_assar);
	DestruirFila (&fila_de_bolos_assados);
	while (!VazioFila(bolos_finalizados)) {int *x = (int*) Desenfileirar (&bolos_finalizados); free (x);}
	DestruirFila (&bolos_finalizados);

	return 0;
}
