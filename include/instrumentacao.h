//####################  Cabeçalho dos sensores e atuadores  ####################//
#ifndef INSTRUMENTACAO_H
#define INSTRUMENTACAO_H

#include <FreeRTOS.h>
#include <semphr.h>

#define INSTRUMENTACAO_MUTEX_INITIALIZER	xSemaphoreCreateMutex()
#define instrumentacao_mutex_lock(mutex) 	xSemaphoreTake(mutex, portMAX_DELAY)
#define instrumentacao_mutex_unlock(mutex) 	xSemaphoreGive(mutex)
#define instrumentacao_mutex_t 			SemaphoreHandle_t


struct atuador
{
	char token[3];
	double max;
	double min;
	instrumentacao_mutex_t mutex;
	double valor;
};

struct sensor
{
	char token[3];
	instrumentacao_mutex_t mutex;
	double valor;
};

struct referencia
{
	double valor;
	instrumentacao_mutex_t mutex;
};

/*  Set points  */
extern struct referencia Tref;
extern struct referencia Href;

/*  inicializa_atuadores  */
extern struct atuador Q;
extern struct atuador Ni;
extern struct atuador Nf;
extern struct atuador Na;

/*  inicializa_sensores  */
extern struct sensor T;
extern struct sensor H;
extern struct sensor No;
extern struct sensor Ta; 
extern struct sensor Ti;

void init_vars();

double le_sensor(struct sensor *x);

void aciona_atuador(struct atuador *x,double valor);

double le_atuador(struct atuador *x);

double define_referencia(struct referencia *x,double val);

double le_referencia(struct referencia *x);

#endif
