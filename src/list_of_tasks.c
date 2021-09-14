//####################  Implementação das threads  ####################//
#include "list_of_tasks.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"
#include "instrumentacao.h"
#include "display.h"



/*  infos  */
float R = 0.001;		// 0.001 Grau/(J/s)
int S = 4184;			// 4184 J/KgC

/*  alarme  */
TaskHandle_t temp_alarme;
TaskHandle_t temp_porcent;

/*  Configuração do alarme  */
int limite_seguro = 30;		// 30C


/*-----------  Threads de alarme  ----------*/
void monitora_temperatura()
{
	/*  Prepara o relógio  */
	TickType_t xLastWakeTime;
	const TickType_t xTime = 10;
	xLastWakeTime = xTaskGetTickCount();

	double t;
	double ref;


	for(;;){
		vTaskDelayUntil(&xLastWakeTime, xTime);

		t = le_sensor(&T);
		if(t > limite_seguro)
			xTaskNotifyGive(temp_alarme);
		ref = le_referencia(&Tref);
		if(t > ref - ref * displayREF_PORCENT/100)
			xTaskNotifyGive(temp_porcent);
	}
}

void escuta_alarme()
{
	temp_alarme = xTaskGetCurrentTaskHandle();


	for(;;){
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		
		informa_alarme(limite_seguro);
	}
}

void imprime_tempo_ate_porcent()
{
	temp_porcent = xTaskGetCurrentTaskHandle();

	/*  Prepara o relógio  */
	TickType_t xInitTime = xTaskGetTickCount();
	TickType_t xEndTime;

	/*  Espera ser avisado que o valor foi atingido  */
	ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
	xEndTime = xTaskGetTickCount();
	informa_tempo_levado((xEndTime - xInitTime)*1e-3);

	vTaskDelete(xTaskGetCurrentTaskHandle());	// Task suicide
}

/*-----------  Sequências de impressão  ----------*/
void imprime_dados()
{
	int index = 0;
	

	/*  Prepara o relógio  */
	TickType_t xLastWakeTime;
	const TickType_t xTime= 1000;
	xLastWakeTime = xTaskGetTickCount();


	for(;;){
        	vTaskDelayUntil(&xLastWakeTime, xTime);

		/*  Atualiza o terminal  */
		imprime_valore_na_tela(index++);
	}
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


	for(;;){
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
	/*  Variáveis de controle  */
	// Parâmetros
	double kp = 50;
	double u;			// Atuação

	// Variável
	double erro;

	TickType_t xLastWakeTime;
	const TickType_t xTime= 70;
	xLastWakeTime = xTaskGetTickCount();


	for(;;){
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
	init_vars();
	fast_inicializa_interface();

	xTaskCreate(&controla_nivel, "Controle de nivel", 1024, NULL, 1, NULL);
	xTaskCreate(&controla_temperatura, "Controle de temperatura", 1024, NULL, 1, NULL);
	xTaskCreate(&imprime_dados, "Imprime dados na tela", 1024, NULL, 1, NULL);
	xTaskCreate(&monitora_temperatura, "Monitora a temperatura", 1024, NULL, 1, NULL);
	xTaskCreate(&escuta_alarme, "Imprime alarme na tela", 1024, NULL, 1, NULL);
	xTaskCreate(&imprime_tempo_ate_porcent, "Imprime o tempo até que a temperatura esteja a N%% da referencia", 1024, NULL, 1, NULL);
}

