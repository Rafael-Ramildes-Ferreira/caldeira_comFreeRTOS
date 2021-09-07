//####################  Display do terminal  ####################//
#include "display.h"

#include <stdio.h>
#include <gconio.h>
#include "instrumentacao.h"
#include "console.h"


#define false 0
#define true  1

#define ESC "\033"

instrumentacao_mutex_t mutex_scr = NULL;

int finalizar = false;



/*-----------  Funções de impressão de saída  ----------*/
void atualiza_valores_da_tela(int tempo)
{
	instrumentacao_mutex_lock(mutex_scr);	
	console_print("%s", ESC "[?25l");			// Cursor invisível

	/*  Atualiza atuadores  */
	console_print("%s", ESC "[6A"); 			// Sobe 8 linhas

	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Q));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha
	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Ni));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha
	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Na));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha
	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Nf));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha

	/*  Atualiza sensores  */
	console_print("%s", ESC "[4A");			// Sobe 4 linhas

	console_print("%s", ESC "[28G");			// Anda 30 colunas para direita
	if(!finalizar)
		console_print("%09.6f",le_sensor(&T));		// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha
	console_print("%s", ESC "[28G");			// Anda 30 colunas para direita
	if(!finalizar)
		console_print("%09.6f",le_sensor(&H));		// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha
	console_print("\n\n\n");

	/*  Atualiza tempo  */
	int min = tempo/60;
	if(!finalizar)
		console_print("%s %02d:%02d\n", ESC "[6G",min, tempo - 60*min);

	//console_print("\n\n");
	console_print("%s", ESC "[?25h");			// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}

void inicializa_interface()
{
	mutex_scr = INSTRUMENTACAO_MUTEX_INITIALIZER;

	instrumentacao_mutex_lock(mutex_scr);	
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("\n");

	console_print("--------------------------------------\n");
	console_print(" Q  = \n");
	console_print(" Ni = \n");
	console_print(" Na = \n");
	console_print(" Nf = \n");

	console_print("%s", ESC "[4A");			// Sobe 4 linhas

	console_print("%sT = \n", ESC "[24G");		// Vai pra coluna 24
	console_print("%sH = \n", ESC "[24G");		// Vai pra coluna 24
	console_print("\n\n");
	console_print("--------------------------------------\n");

	console_print("Tempo   :  \n");

	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);	
}


/*---------- Funções de alarme ----------*/
void print_warning(int valor)
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("%s", ESC "[8A");			// Sobe 10 linhas
	console_print("%s", ESC "[1m");			// Põe em negrito
	console_print("%s", ESC "[38;5;196m");		// Põe em vermelho
	if(!finalizar){
		console_print("%s", ESC "[K");		// Limpa a linha
		console_print("#### WARNING: Temperatura Acima Do Limite Seguro: %d C. !!!!!!",valor);
	}
	console_print("%s", ESC "[0m");			// Reseta estilo da escrita
	console_print("\n\n\n\n\n\n\n\n");		// Desce todas as 10 linhas
	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}

void erase_warning()
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("%s", ESC "[8A");			// Sobe 10 linhas
	if(!finalizar)
		console_print("%s", ESC "[K");		// Limpa a linha
	console_print("\n\n\n\n\n\n\n\n");		// Desce todas as 10 linhas
	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}


/*---------- Finalização do programa ----------*/
void finalizar_programa(int number_of_blockedTasks, TaskHandle_t *blockedTaskHandle)
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("\nFim do programa ...\n");
	instrumentacao_mutex_unlock(mutex_scr);

	// Para liberar o escuta alarme (nada é escrito na tela depois de o programa ter-se encerrado)
	for(int i = 0; i < number_of_blockedTasks; i++)
		xTaskNotifyGive(*(blockedTaskHandle+i));
	
	finalizar = true;
}
