//####################  Implementação dos sensores e atuadores  ####################//
#include "instrumentacao.h"

#include <stdio.h>
#include <stdlib.h>
#include "communication.h"


#define toFloat(x) atof(x+3)
#define limitado(val,inf,sup) ((val<=inf)?inf:(val>=sup)?sup:val)


/*  Set points  */
struct referencia Tref = {30, NULL};
struct referencia Href = {1.5,NULL};

/*  inicializa_atuadores  */
struct atuador Q  = {"aq-",1000000,0,NULL,0000};
struct atuador Ni = {"ani",100,0,NULL,0000};
struct atuador Nf = {"anf",100,0,NULL,0000};
struct atuador Na = {"ana",10,0,NULL,0000};

/*  inicializa_sensores  */
struct sensor T  = {"st-",NULL,0000};
struct sensor H  = {"sh-",NULL,0000};
struct sensor No = {"sno",NULL,0000};
struct sensor Ta = {"sta",NULL,0000}; 
struct sensor Ti = {"sti",NULL,0000};

void init_vars()
{

	/*  Set points  */
	Tref.mutex = xSemaphoreCreateMutex();
	Href.mutex = xSemaphoreCreateMutex();

	/*  inicializa_atuadores  */
	Q.mutex  = xSemaphoreCreateMutex();
	Ni.mutex = xSemaphoreCreateMutex();
	Nf.mutex = xSemaphoreCreateMutex();
	Na.mutex = xSemaphoreCreateMutex();

	/*  inicializa_sensores  */
	T.mutex  = xSemaphoreCreateMutex();
	H.mutex  = xSemaphoreCreateMutex();
	No.mutex = xSemaphoreCreateMutex();
	Ta.mutex = xSemaphoreCreateMutex();
	Ti.mutex = xSemaphoreCreateMutex();
}

/* Funções para manipular as estruturas  */
double le_sensor(struct sensor *x)  
{
	instrumentacao_mutex_lock(x->mutex);
        send_request_message(x->token);
        receive_message(x->token, &x->valor,COMMUNICATION_BLOCKING);
	double val = x->valor;
	instrumentacao_mutex_unlock(x->mutex);

	return val;
}

void aciona_atuador(struct atuador *x,double valor)
{
	instrumentacao_mutex_lock(x->mutex);
        send_set_message(x->token,limitado(valor,x->min,x->max));
	receive_message(x->token, &x->valor, COMMUNICATION_BLOCKING);
	instrumentacao_mutex_unlock(x->mutex);

}

double le_atuador(struct atuador *x)
{
	instrumentacao_mutex_lock(x->mutex);
	double val = x->valor;
	instrumentacao_mutex_unlock(x->mutex);

	return val;
}

double define_referencia(struct referencia *x,double val)
{
	instrumentacao_mutex_lock(x->mutex);
	*(&x->valor) = val;
	instrumentacao_mutex_unlock(x->mutex);
}

double le_referencia(struct referencia *x)
{
	instrumentacao_mutex_lock(x->mutex);
	double val = x->valor;
	instrumentacao_mutex_unlock(x->mutex);

	return val;
}
