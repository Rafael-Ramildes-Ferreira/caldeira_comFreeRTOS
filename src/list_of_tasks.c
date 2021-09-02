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
#include "console.h"
#include "communication.h"
#include "list_of_tasks.h"
#include "instrumentacao.h"


#define TEMPO_TOTAL 3600

// Executa por uma hora (3600 s) então o loop ocorre 3600s/intervalo em segundos (espera intervalo em nano)
#define executa_nano(intervalo) for(int index = 0;index<TEMPO_TOTAL/(intervalo*1e-9);index++)


/*used to control*/
double ni, q, na, nf;

/*  infos  */
float R = 0.001;		// 0.001 Grau/(J/s)
int S = 4184;			// 4184 J/KgC


/*-----------  Sequências de controle  ----------*/
void controla_temperatura()
{
	console_print("Iniciou o ciclo de controle da temperatura\n");
	int intervalo = 0.05e+9;	// 0.05s = 50ms = 50 000 000ns

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


	executa_nano(intervalo){
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
	int intervalo = 0.07e+9; 	  // 0.07s = 70ms = 70 000 000ns

	/*  Variáveis de controle  */
	// Parâmetros
	double kp = 50;
	double u;			// Atuação

	// Variável
	double erro;

	TickType_t xLastWakeTime;
	const TickType_t xTime= 70;
	xLastWakeTime = xTaskGetTickCount();


	executa_nano(intervalo){
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

