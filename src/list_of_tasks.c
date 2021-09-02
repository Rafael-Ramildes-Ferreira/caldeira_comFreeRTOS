/********** Code informaation **********/
/**** Creation date: June 29, 2021 *****/
/**** Developer: Alan Kunz Cechinel ****/
/**** e-mail: cechinel.a.k@gmail.com ***/
/***************************************/

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Local includes. */
#include "list_of_tasks.h"
#include "console.h"
#include "instrumentacao.h"


#define TEMPO_TOTAL 3600	// Em segundos

// Executa pelo TEMPO_TOTAL
#define executa_nano(intervalo) for(int nano_index = 0;executa_nano_index<TEMPO_TOTAL/(intervalo*1e-9);nano_index++)
// Executa pelo TEMPO_TOTAL
#define executa_milli(intervalo) for(int milli_index = 0;milli_index<TEMPO_TOTAL/(intervalo*1e-3);milli_index++)


/*used to control*/
double ni, q, na, nf;

/*  infos  */
float R = 0.001;		// 0.001 Grau/(J/s)
int S = 4184;			// 4184 J/KgC


/*-----------  Sequências de controle  ----------*/
void controla_temperatura()
{
	console_print("Iniciou o ciclo de controle da temperatura\n");

	/*  Variáveis do controle  */
	// Parâmetros
	double kp = 300000;		// Ganho proporcional
	double u;			// Atuação

	// Variáveis
	double erro;


	/*  Prepara o relógio  */
	TickType_t xLastWakeTime;
	const TickType_t xTime= 50;
	xLastWakeTime = xTaskGetTickCount();


	executa_milli(xTime){
        	vTaskDelayUntil(&xLastWakeTime, xTime);
		
		/*  Define a atuação  */
		erro = le_referencia(&Tref) - le_sensor(&T);
		u = kp*(erro) + (le_sensor(&T)-le_sensor(&Ta))/R - S*(le_sensor(&Ti)-le_sensor(&T))*le_atuador(&Ni);

		/*  Envia a mensagem  */
		aciona_atuador(&Q,u);
		aciona_atuador(&Na,(u - le_atuador(&Q))/(S*(80-le_sensor(&T))));
	}
}


void controla_nivel()
{
	console_print("Iniciou o ciclo de controle do nivel\n");

	/*  Variáveis de controle  */
	// Parâmetros
	double kp = 50;
	double u;			// Atuação

	// Variável
	double erro;

	TickType_t xLastWakeTime;
	const TickType_t xTime= 70;
	xLastWakeTime = xTaskGetTickCount();


	executa_milli(xTime){
        	vTaskDelayUntil(&xLastWakeTime, xTime);

		/*  Define a atuação  */
		erro = le_referencia(&Href) - le_sensor(&H);
		u = kp*erro + le_sensor(&No) - le_atuador(&Na);
		
		/*  Envia mensagem  */
		aciona_atuador(&Ni,u);
		aciona_atuador(&Nf,-u);
	}
}

void create_tasks()
{
	console_print("Iniciando a criacao de tasks\n");
	ni = .1;
	q = .1;
	na = .1;
	nf = .1;
	init_vars();

	xTaskCreate(&controla_nivel, "Controle de nivel", 1024, NULL, 1, NULL);
	xTaskCreate(&controla_temperatura, "Controle de temperatura", 1024, NULL, 1, NULL);
}

