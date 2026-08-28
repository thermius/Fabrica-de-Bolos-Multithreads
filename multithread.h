#ifndef M_THREAD_H
#define M_THREAD_H
#define MAX_ARGS 10

int global_controlador = 1;

typedef struct _ArgumentosParaAsThrads
{
	void *array_de_ponteiros [MAX_ARGS];
} ArgumentosParaAsThrads;

void *RotinaTrabalhador1 (void*);
void *RotinaTrabalhador2 (void*);
void *RotinaTrabalhador3 (void*);

#endif
