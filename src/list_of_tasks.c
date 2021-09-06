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
#include "display.h"


#define TEMPO_TOTAL 3600	// Em segundos

// Executa pelo TEMPO_TOTAL
#define executa_milli(intervalo) for(int milli_index = 0;milli_index<TEMPO_TOTAL/(intervalo*1e-3);milli_index++)


/*used to control*/
double ni, q, na, nf;

/*  infos  */
float R = 0.001;		// 0.001 Grau/(J/s)
int S = 4184;			// 4184 J/KgC

/*  alarme  */
TaskHandle_t temp_alarme;

/*  Configuração do alarme  */
int limite_seguro = 30;		// 30C


/*-----------  Threads de alarme  ----------*/
void monitora_temperatura()
{
	/*  Prepara o relógio  */
	TickType_t xLastWakeTime;
	const TickType_t xTime = 10;
	xLastWakeTime = xTaskGetTickCount();


	executa_milli(xTime){
		vTaskDelayUntil(&xLastWakeTime, xTime);

		if(le_sensor(&T) > limite_seguro)
			xTaskNotifyGive(temp_alarme);

		//console_print(";\n");
	}
	xTaskNotifyGive(temp_alarme);	// Para liberar o escuta alarme (nada é escrito na tela depois de o programa ter-se encerrado)
	//console_print("Encerrou monitora_temperatura\n");		
}

void escuta_alarme()
{
	temp_alarme = xTaskGetCurrentTaskHandle();

	/*  Prepara o relógio  */
	TickType_t xLastWakeTime;
	const TickType_t xTime = 5;


	while(xTaskGetTickCount()*1e-3 <= TEMPO_TOTAL){
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		xLastWakeTime = xTaskGetTickCount();
		
		print_warning(limite_seguro);

		vTaskDelayUntil(&xLastWakeTime,xTime);

		erase_warning();
	
		//console_print("%lf\n",xTaskGetTickCount()*1e-3);
	}
	//console_print("Encerrou escuta_alarme\n");
}

/*-----------  Sequências de impressão  ----------*/
void imprime_dados()
{
	int index = 0;

	/*  Imprime no terminal  */
	inicializa_interface();
	

	/*  Prepara o relógio  */
	TickType_t xLastWakeTime;
	const TickType_t xTime= 1000;
	xLastWakeTime = xTaskGetTickCount();


	executa_milli(xTime){
        	vTaskDelayUntil(&xLastWakeTime, xTime);

		/*  Atualiza o terminal  */
		atualiza_valores_da_tela(index++);
	}
	finalizar_programa();
	//console_print("Encerrou imprime_dados\n");
}

/*-----------  Sequências de controle  ----------*/
void controla_temperatura()
{
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

		//console_print(",\n");
	}
	//console_print("Encerrou controla_temperatura\n");
}


void controla_nivel()
{
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

		//console_print(".\n");
	}
	//console_print("Encerrou controla_nivel\n");
}

void create_tasks()
{
	ni = .1;
	q = .1;
	na = .1;
	nf = .1;
	init_vars();

	xTaskCreate(&controla_nivel, "Controle de nivel", 1024, NULL, 1, NULL);
	xTaskCreate(&controla_temperatura, "Controle de temperatura", 1024, NULL, 1, NULL);
	xTaskCreate(&imprime_dados, "Imprime dados na tela", 1024, NULL, 1, NULL);
	xTaskCreate(&monitora_temperatura, "monitora a temperatura", 1024, NULL, 1, NULL);
	xTaskCreate(&escuta_alarme, "Imprime alarme na tela", 1024, NULL, 1, NULL);
}

