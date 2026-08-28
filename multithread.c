#include "multithread.h"

/*O trabalhador 1 pega os pedidos de bolo da lista de pedidos, faz a mistura e coloca na fila de bolos para assar*/
void *RotinaTrabalhador1 	(void *argumentos) 
{
	/*obtem os argumentos*/
	ArgumentosParaAsThrads *args 				= (ArgumentosParaAsThrads*) argumentos;
	Fila **fila_de_pedidos   					= (Fila**) args->array_de_ponteiros[0];
	Fila **fila_de_bolos_para_assar 			= (Fila**) args->array_de_ponteiros[2];
	pthread_mutex_t *m_fila_de_bolos_para_assar	= (pthread_mutex_t*) args->array_de_ponteiros[3];
	pthread_mutex_t *m_fila_de_pedidos 			= (pthread_mutex_t*) args->array_de_ponteiros[1];

	Fila *pool_interno = NULL;

	/*loop da thread*/
	while (1)
	{
		/*se desligamento da thread*/
		if (!global_controlador) 
		{	
			while (!VazioFila (pool_interno))
			{
				int *bolo = Desenfileirar	(&pool_interno);
				free (bolo);
			}
			while (!VazioFila (*fila_de_pedidos))
			{
				int *bolo = Desenfileirar	(fila_de_pedidos);
				free (bolo);
			}
			while (!VazioFila (*fila_de_bolos_para_assar))
			{
				int *bolo = Desenfileirar	(fila_de_bolos_para_assar);
				free (bolo);
			}
			printf("[ NOTA ] - Trabalhador1: encerramento idenficado. saindo\n");
			return NULL;
		}

		/*tenta pegar o mutex para abastarcer o pool*/ 
		if (pthread_mutex_trylock(m_fila_de_pedidos) == 0)
		{
			/*obtem o bolo do fila de pedidos*/
			int *bolo = Desenfileirar	(fila_de_pedidos);

			/*coloca o bolo no pool interno*/
			if (bolo != NULL) 
			{
				printf("[ NOTA ] - Trabalhador1: bolo %i recebido\n", *bolo);
				Enfileirar (&pool_interno, bolo, sizeof (int), 0, ENFILEIRAR_PONTEIRO);
			}

			/*libera o mutex*/
			pthread_mutex_unlock		(m_fila_de_pedidos);

		}

		/*tenta pegar o mutex da fila de bolos para assar*/
		if (pthread_mutex_trylock (m_fila_de_bolos_para_assar) == 0)
		{	
			/*obtem o bolo do pool interno*/
			int *bolo = Desenfileirar	(&pool_interno);

			/*coloca o bolo para assar*/
			if (bolo != NULL)
			{
				printf("[ NOTA ] - Trabalhador1: preparando mistura do bolo %i\n ...", *bolo);
				printf("[ NOTA ] - Trabalhador1: mistura do bolo %i preparada. Enviando para assagem\n", *bolo);
				Enfileirar (fila_de_bolos_para_assar, bolo, sizeof (int), 0, ENFILEIRAR_PONTEIRO);
			}

			else {  printf("[ NOTA ] - Trabalhador1: não há bolos pedidos de bolo para misturar\n");}

			/*libera o mutex*/
			pthread_mutex_unlock (m_fila_de_bolos_para_assar);

		}

	}		
}

/*O trabalhador 2 pega os bolos da fila de assar, assa o bolo e coloca na fila de envio*/
void *RotinaTrabalhador2 (void *argumentos) 
{
	/*obtem os argumentos*/
	ArgumentosParaAsThrads *args 				= (ArgumentosParaAsThrads*) argumentos;
	Fila **fila_de_bolos_para_assar 			= (Fila**) args->array_de_ponteiros[0];
	Fila **fila_de_bolos_assados 				= (Fila**) args->array_de_ponteiros[2];
	pthread_mutex_t *m_fila_de_bolos_para_assar	= (pthread_mutex_t*) args->array_de_ponteiros[1];
	pthread_mutex_t *m_fila_de_bolos_assados 	= (pthread_mutex_t*) args->array_de_ponteiros[3];
	pthread_cond_t  *condicao					= (pthread_cond_t*)  args->array_de_ponteiros[4];
	Fila *pool_interno = NULL;
	int estoque = 0;

	/*loop da thread*/
	while (1)
	{
		/*se desligamento da thread*/
		if (!global_controlador) 
		{
			while (!VazioFila (pool_interno))
			{
				int *bolo = Desenfileirar	(&pool_interno);
				free (bolo);
			}

			while (!VazioFila (*fila_de_bolos_assados))
			{
				int *bolo = Desenfileirar	(fila_de_bolos_assados);
				free (bolo);
			}
			printf("[ NOTA ] - Trabalhador2: encerramento idenficado. saindo\n");
			return NULL;
		}

		/*tenta pegar o mutex de bolos para assar e colocar no pool*/ 
		if (pthread_mutex_trylock(m_fila_de_bolos_para_assar) == 0)
		{
			/*obtem o bolo do fila de pedidos*/
			int *bolo = Desenfileirar	(fila_de_bolos_para_assar);

			/*coloca o bolo no pool interno*/
			if (bolo != NULL)
			{ 
				printf("[ NOTA ] - Trabalhador2: bolo %i recebido\n", *bolo);
				Enfileirar (&pool_interno, bolo, sizeof (int), 0,ENFILEIRAR_PONTEIRO);
			}

			/*libera o mutex*/
			pthread_mutex_unlock		(m_fila_de_bolos_para_assar);

		}

		/*tenta pegar o mutex da fila de bolos para assar*/
		if (pthread_mutex_trylock (m_fila_de_bolos_assados) == 0)
		{	
			/*obtem o bolo do pool interno*/
			int *bolo = Desenfileirar	(&pool_interno);

			/*coloca o bolo para assar*/
			if (bolo != NULL) 
			{	
				printf("[ NOTA ] - Trabalhador2: assando bolo %i ...\n", *bolo);
				printf("[ NOTA ] - Trabalhador2: bolo %i assado. Enviado para a embalagem\n", *bolo);
				Enfileirar (fila_de_bolos_assados, bolo, sizeof (int), 0,ENFILEIRAR_PONTEIRO);
				estoque++;

				/*aqui a abordagem é diferente, acomulamoss 10 bolos em estoques antes de enviar para embalar, então o trabalhador 3 somente pode ser chamado quando houver estoque*/
				if (estoque >= 1)
				{ 
					estoque = 0;
					pthread_cond_signal (condicao);
				}				
			}

			/*libera o mutex antes de sair da região critica*/
			pthread_mutex_unlock (m_fila_de_bolos_assados);
			printf("[ NOTA ] - Trabalhador2: não há bolos para assar\n");

		}


	}
}

/*o trabalhador 3 embala o bolo, despanha e marca os bolos despachados*/
void *RotinaTrabalhador3 	(void *argumentos) 
{

	/*obtem os argumentos*/
	ArgumentosParaAsThrads *args 				= (ArgumentosParaAsThrads*) argumentos;
	Fila **fila_de_bolos_assados 				= (Fila**) args->array_de_ponteiros[0];
	Fila **bolos_finalizados 					= (Fila**) args->array_de_ponteiros[2];
	pthread_mutex_t *m_fila_de_bolos_assados 	= (pthread_mutex_t*) args->array_de_ponteiros[1];
	pthread_cond_t  *condicao					= (pthread_cond_t*)  args->array_de_ponteiros[3];


	Fila *pool_interno = NULL;

	/*o trabalhador "dorme" enquanto não chega bolos para embalar. quando houver bolos, automaticamente o mutex é capturado e o trabalho começa*/
	while (1)
	{
		pthread_mutex_lock(m_fila_de_bolos_assados);

		/*o trabalhador 3 depende de duas condições para acordar: controlador deslicado ou fila de bolos preenchida*/
		while (global_controlador && VazioFila(*fila_de_bolos_assados)) pthread_cond_wait(condicao, m_fila_de_bolos_assados);
       	
		/*se desligamento da thread*/
		if (!global_controlador) 
		{

			while (!VazioFila (pool_interno))
			{
				int *bolo = Desenfileirar	(&pool_interno);
				free (bolo);
			}
   			pthread_mutex_unlock(m_fila_de_bolos_assados);
   			printf("[ NOTA ] - Trabalhador3: encerramento idenficado. saindo\n");
			return NULL;
		}

		/*chegando aqui, temos bolo, esvazia o estoque*/
		while (!VazioFila(*fila_de_bolos_assados))
		{
			int *bolo = Desenfileirar	(fila_de_bolos_assados);
			printf("[ NOTA ] - Trabalhador3: bolo %i chegou para embalagem\n", *bolo);
			Enfileirar (&pool_interno, bolo, sizeof (int), 0,ENFILEIRAR_PONTEIRO);
		}

		/*libera o mutex*/
		pthread_mutex_unlock (m_fila_de_bolos_assados);

		/*embala os bolos*/
		while (!VazioFila (pool_interno))
		{
			int *bolo = Desenfileirar	(&pool_interno);
			if (bolo == NULL) break;
			printf ("[ NOTA ] - Trabalhador3(): embalando bolo %i\n",*bolo);
			printf ("[ NOTA ] - Trabalhador3(): bolo %i embalado e enviado. Registrando envio\n",*bolo);
			Enfileirar (bolos_finalizados, bolo, sizeof (int), 0,ENFILEIRAR_PONTEIRO);
		}
		printf ("[ NOTA ] - Trabalhador3(): Todo o estoque foi embalado\n");


	}
}
