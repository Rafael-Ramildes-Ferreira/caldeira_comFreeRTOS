//####################  Display do terminal  ####################//
#include "display.h"

#include <stdio.h>
#include <gconio.h>
#include "instrumentacao.h"
#include "console.h"


#define TAM_MEDIA 10

#define false 0
#define true  1

#define ESC "\033"

instrumentacao_mutex_t mutex_scr = NULL;

int finalizar = false;

int passou_limiar = false;
int min_limiar;
double sec_limiar;

double ultimas_temperaturas[TAM_MEDIA] = {0};
int index_temp = 0;

double limite_excedido;
int alarme = false;


double array_mean(int size, double array[], int consider_zero);


/*-----------  Funções de inicialização do display  ----------*/
void fast_inicializa_interface()
{
	mutex_scr = INSTRUMENTACAO_MUTEX_INITIALIZER;
}


/*-----------  Funções de impressão de saída  ----------*/
void imprime_valore_na_tela(int tempo)
{
	instrumentacao_mutex_lock(mutex_scr);	
	console_print("%s", ESC "[?25l");		// Cursor invisível
	if(alarme){
		console_print("%s", ESC "[1m");			// Põe em negrito
		console_print("%s", ESC "[38;5;196m");		// Põe em vermelho
		console_print("#### WARNING: Temperatura Acima Do Limite Seguro: %.2lf C. !!!!!!",limite_excedido);  //#10	
		termina_alarme();
		console_print("%s", ESC "[0m");			// Reseta estilo da escrita
	}
	console_print("\n");										//#10->9

	console_print("--------------------------------------\n");					//#9->8
	console_print(" Q  = %09.6f\n", le_atuador(&Q));						//#8->7
	console_print(" Ni = %09.6f\n", le_atuador(&Ni));						//#7->6
	console_print(" Na = %09.6f\n", le_atuador(&Na));						//#6->5
	console_print(" Nf = %09.6f\n", le_atuador(&Nf));						//#5->4

	console_print("%s", ESC "[4A");			// Sobe 4 linhas				//#4->8

	console_print("%sT = %09.6f\n", ESC "[24G", le_sensor(&T));		// Vai pra coluna 24	//#8->7
	console_print("%sH = %09.6f\n", ESC "[24G", le_sensor(&T));		// Vai pra coluna 24	//#7->6
	console_print("\n\n");										//#6->5->4
	console_print("--------------------------------------\n");					//#4->3

	/*  Imprime tempo levado para alcançar o limiar  */
	console_print("Tempo levado até %5.1f %% do set point",displayREF_PORCENT);
	if(passou_limiar)
		console_print(" %02d:%07.4lf\n", min_limiar, sec_limiar);				//#3->2
	else
		console_print(": Ainda nao chegou a dada porcentagem\n");				//#3->2

	/*  Imprime média móvel  */
	ultimas_temperaturas[(index_temp++)%TAM_MEDIA] = le_sensor(&T);
	console_print("Média móvel da temperatura %lf\n", array_mean(TAM_MEDIA,ultimas_temperaturas,false));							//#2->1	

	/*  Imprime tempo  */
	int min = tempo/60;
	console_print("Tempo %02d:%02d\n",min, tempo - 60*min);						//#1->0

	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}


/*-----------  Funções auxiliares de impressão  ----------*/
void informa_tempo_levado(double time)
{
	min_limiar = time/60;
	sec_limiar = time - min_limiar*60;

	passou_limiar = true;
}


void informa_alarme(int valor)
{
	limite_excedido = valor;

	alarme = true;
}


void termina_alarme()
{
	limite_excedido = 0;

	alarme = false;	
}

double array_mean(int size, double array[], int consider_zero)
{
	double sum = 0;
	int virtual_size = size;

	for(int i = 0;i < size; i++)
		if(array[i]!=0 || consider_zero)
			sum += array[i];
		else virtual_size--;

	return sum/virtual_size;
}


//###############  DEPRICATED   ###############//
void atualiza_valores_da_tela(int tempo)
{
	static double ultimas_temperaturas[TAM_MEDIA] = {0};
	static int index_temp = 0;

	instrumentacao_mutex_lock(mutex_scr);	
	console_print("%s", ESC "[?25l");			// Cursor invisível

	/*  Atualiza atuadores  */
	console_print("%s", ESC "[8A"); 			// Sobe 8 linhas		#0->8

	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Q));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha	#8->7
	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Ni));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha	#7->6
	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Na));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha	#6->5
	console_print("%s", ESC "[7G"); 			// Anda 7 colunas para direita
	if(!finalizar)
		console_print("%09.6f", le_atuador(&Nf));	// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha	#5->4

	/*  Atualiza sensores  */
	console_print("%s", ESC "[4A");				// Sobe 4 linhas		#4->8

	console_print("%s", ESC "[28G");			// Anda 30 colunas para direita
	if(!finalizar)
		console_print("%09.6f",le_sensor(&T));		// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha	#8->7
	console_print("%s", ESC "[28G");			// Anda 30 colunas para direita
	if(!finalizar)
		console_print("%09.6f",le_sensor(&H));		// Extrai e escreve o valor
	console_print("\n");					// Passa pra próxima linha	#7->6
	console_print("\n\n\n");				// 				#6->5->4->3

	/*  Atualiza média móvel  */
	ultimas_temperaturas[(index_temp++)%TAM_MEDIA] = le_sensor(&T);
	console_print("\n");					//				#3->2
	if(!finalizar)
		console_print("%s %lf", ESC "[27G", array_mean(TAM_MEDIA,ultimas_temperaturas,false));
	console_print("\n");					//				#2->1

	/*  Atualiza tempo  */
	int min = tempo/60;
	if(!finalizar)
		console_print("%s %02d:%02d", ESC "[6G",min, tempo - 60*min);
	console_print("\n");					//				#1->0
	

	console_print("%s", ESC "[?25h");			// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}

//###############  DEPRICATED   ###############//
void tempo_levado(double time)
{
	int min = time/60;
	double sec = time - min*60;

	instrumentacao_mutex_lock(mutex_scr);
	console_print("%s", ESC "[3A");		// Sobe três linha		#0->3
	console_print("%s", ESC "[38G");	// Vai para a coluna 39
	console_print("%s", ESC "[K");		// Limpa a linha
	if(!finalizar)
		console_print(" %02d:%07.4lf", min, sec);	// Imprime o tempo que levou
	console_print("\n\n\n");		// Desce as três linhas		#3->2->1->0
	instrumentacao_mutex_unlock(mutex_scr);
}

//###############  DEPRICATED   ###############//
void inicializa_interface()
{
	mutex_scr = INSTRUMENTACAO_MUTEX_INITIALIZER;

	instrumentacao_mutex_lock(mutex_scr);	
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("\n");							//#10->9

	console_print("--------------------------------------\n");		//#9->8
	console_print(" Q  = \n");						//#8->7
	console_print(" Ni = \n");						//#7->6
	console_print(" Na = \n");						//#6->5
	console_print(" Nf = \n");						//#5->4

	console_print("%s", ESC "[4A");			// Sobe 4 linhas	//#4->8

	console_print("%sT = \n", ESC "[24G");		// Vai pra coluna 24	//#8->7
	console_print("%sH = \n", ESC "[24G");		// Vai pra coluna 24	//#7->6
	console_print("\n\n");							//#6->5->4
	console_print("--------------------------------------\n");		//#4->3

	console_print("Tempo levado até %5.1f %% do set point: Ainda nao chegou a dada porcentagem\n",displayREF_PORCENT);//#3->2
	console_print("Média móvel da temperatura \n");				//#2->1
	console_print("Tempo   :  \n");						//#1->0

	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}

//###############  DEPRICATED   ###############//
void print_warning(int valor)
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("%s", ESC "[10A");		// Sobe 10 linhas		#0->10
	console_print("%s", ESC "[1m");			// Põe em negrito
	console_print("%s", ESC "[38;5;196m");		// Põe em vermelho
	if(!finalizar){
		console_print("%s", ESC "[K");		// Limpa a linha
		console_print("#### WARNING: Temperatura Acima Do Limite Seguro: %d C. !!!!!!",valor);
	}
	console_print("%s", ESC "[0m");			// Reseta estilo da escrita
	console_print("\n\n\n\n\n\n\n\n\n\n");		// Desce todas as 10 linhas	#10->0
	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}

//###############  DEPRICATED   ###############//
void erase_warning()
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("%s", ESC "[10A");		// Sobe 10 linhas		#0->10
	if(!finalizar)
		console_print("%s", ESC "[K");		// Limpa a linha
	console_print("\n\n\n\n\n\n\n\n\n\n");		// Desce todas as 10 linhas	#10->0
	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}

//###############  DEPRICATED   ###############//
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
