//####################  Display do terminal  ####################//
#include "display.h"

#include <math.h>
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
	console_print("%s", ESC "[8A"); 			// Sobe 8 linhas

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

	console_print("\n\n");
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

	console_print("%s", ESC "[4A");		// Sobe 4 linhas

	console_print("%sT = \n", ESC "[24G");		// Vai pra coluna 24
	console_print("%sH = \n", ESC "[24G");		// Vai pra coluna 24
	console_print("\n\n");
	console_print("--------------------------------------\n");

	console_print("Tempo   :  \n");

	console_print("Escreva T 123.45 ou H 123.45 para redefinir o set-point\nde T ou H para 123.45 ou qualquer outro valor\n");
	console_print("==>");
	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);	
}


/*---------- Funções de alarme ----------*/
void print_warning(int valor)
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("%s", ESC "[10A");		// Sobe 10 linhas
	console_print("%s", ESC "[1m");			// Põe em negrito
	console_print("%s", ESC "[38;5;196m");		// Põe em vermelho
	if(!finalizar){
		console_print("%s", ESC "[K");		// Limpa a linha
		console_print("#### WARNING: Temperatura Acima Do Limite Seguro: %d C. !!!!!!",valor);
	}
	console_print("%s", ESC "[0m");			// Reseta estilo da escrita
	console_print("\n\n\n\n\n\n\n\n\n\n");		// Desce todas as 10 linhas
	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}

void erase_warning()
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("%s", ESC "[?25l");		// Cursor invisível
	console_print("%s", ESC "[10A");		// Sobe 10 linhas
	if(!finalizar)
		console_print("%s", ESC "[K");		// Limpa a linha
	console_print("\n\n\n\n\n\n\n\n\n\n");		// Desce todas as 10 linhas
	console_print("%s", ESC "[?25h");		// Cursor visível
	instrumentacao_mutex_unlock(mutex_scr);
}


/*---------- Finalização do programa ----------*/
void finalizar_programa(int number_of_blockedTasks, TaskHandle_t *blockedTaskHandle)
{
	instrumentacao_mutex_lock(mutex_scr);
	console_print("\nPressione ENTER para finalizar . . .\n");
	instrumentacao_mutex_unlock(mutex_scr);
	
	finalizar = true;

	for(int i = 0; i < number_of_blockedTasks; i++)
		xTaskNotifyGive(*(blockedTaskHandle+i));	// Para liberar o escuta alarme (nada é escrito na tela depois de o programa ter-se encerrado)
}



/*---------- Função de interpretação de entrada ----------*/
void interpreta_escrita()
{
	static int index;
	static double val;

	static int index_ponto;
	static int var;		// 0 -> T; 1 -> H
	

	/*  Funções auxiliares  */
	// -1 -> resetar; 0 -> T; e 1 -> H
	void escolhe_desativa_variavel(int command)
	{
		index_ponto = -1;
		index = 0;
		val = 0;
		var = command;
	}

	void limpa_linha_comando()
	{
		instrumentacao_mutex_lock(mutex_scr);
		if(!finalizar){
			console_print("%s", ESC "[4G");		// Move o cursor para coluna 4
			console_print("%s", ESC "[0K");		// apaga dala pra frente
		}
		instrumentacao_mutex_unlock(mutex_scr);

		escolhe_desativa_variavel(-1);	//  Reseta completamente		
	}

	void backspace()
	{
		index -= 1;

		instrumentacao_mutex_lock(mutex_scr);
		if(!finalizar){
			console_print("%s[%dG", ESC, 6 + index);	// Move o cursor para trás
			console_print("%s", ESC "[0K");		// apaga dala pra frente
		}
		instrumentacao_mutex_unlock(mutex_scr);
	}

	void comando_invalido()
	{
		instrumentacao_mutex_lock(mutex_scr);
		console_print("%s", ESC "[4G");		// Vai pra coluna 4
		console_print("%s", ESC "[?25l");		// Cursor invisível
		console_print("%s", ESC "[38;5;196m");		// Põe em vermelho
		if(!finalizar)
			console_print("Comando invalido!!");
		console_print("%s", ESC "[0m");		// Reseta estilo da escrita
		console_print("%s", ESC "[?25h");		// Cursor visível
		instrumentacao_mutex_unlock(mutex_scr);

		delay(1500);

		instrumentacao_mutex_lock(mutex_scr);
		if(!finalizar){
			console_print("%s", ESC "[0G");	// Vai para coluna 0
			console_print("%s", ESC "[0K");	// Limpa a linha
			console_print("==>");
		}
		instrumentacao_mutex_unlock(mutex_scr);

		escolhe_desativa_variavel(-1);		//  Reseta completamente
	}

	void interpreta_texto(char input)
	{
		instrumentacao_mutex_lock(mutex_scr);
		if(!finalizar){
			console_print("%s", ESC "[4G");	// Vai para coluna 4
			console_print("%s", ESC "[0K");	// Limpa a linha
			console_print("%c", input);
		}
		instrumentacao_mutex_unlock(mutex_scr);
	}

	double interpreta_numero(char input)
	{
		instrumentacao_mutex_lock(mutex_scr);
		if(!finalizar){
			console_print("%s[%dG", ESC, 6 + index);	// Vai para coluna 4
			console_print("%s", ESC "[0K");		// Limpa a linha dali pra frente
			console_print("%c", input);
		}
		instrumentacao_mutex_unlock(mutex_scr);

		index++;
		
		return 10*val + input - 48;		// Converte de char pra int e faz ser o número menos significativo de val
	}

	/*  Controla a leitura  */
	char rec = getch();
	if(!finalizar){
		switch(rec){
			/*--------- Definem a variável ----------*/
			case 't':{
				interpreta_texto(rec-32);	// Em maiúsculo
				escolhe_desativa_variavel(0);	//  Reseta parcialmente e define a variavel
			} break;
			case 'T':{
				interpreta_texto(rec);
				escolhe_desativa_variavel(0);	//  Reseta parcialmente e define a variavel
			} break;
			case 'h':{
				interpreta_texto(rec-32);	// Em maiúsculo
				escolhe_desativa_variavel(1);	//  Reseta parcialmente e define a variavel
			} break;
			case 'H':{
				interpreta_texto(rec);
				escolhe_desativa_variavel(1);	//  Reseta parcialmente e define a variavel
			} break;

			/*--------- Inúteis ----------*/
			case ' ':{	// Do nothing
			} break;

			/*--------- Pegam o valor ----------*/
			case '0':{
				val = interpreta_numero(rec);
			}break;
			case '1':{
				val = interpreta_numero(rec);
			}break;
			case '2':{
				val = interpreta_numero(rec);
			}break;
			case '3':{
				val = interpreta_numero(rec);
			}break;
			case '4':{
				val = interpreta_numero(rec);
			}break;
			case '5':{
				val = interpreta_numero(rec);
			}break;
			case '6':{
				val = interpreta_numero(rec);
			}break;
			case '7':{
				val = interpreta_numero(rec);
			}break;
			case '8':{
				val = interpreta_numero(rec);
			}break;
			case '9':{
				val = interpreta_numero(rec);
			}break;

			/*--------- Marca o ponto ----------*/
			case '.':{
				if(index_ponto < 0){
					interpreta_numero('.');
					index_ponto = index;
				}else{
					comando_invalido();	//  Imprime aviso
				}
			} break;
			case ',':{
				if(index_ponto < 0){
					interpreta_numero('.');
					index_ponto = index;
				}else{
					comando_invalido();	//  Imprime aviso
				}
			} break;

			/*--------- Apaga escrita ----------*/
			case 127:{	// --- Backspace
				if(index==0)
					limpa_linha_comando();
				else{
					if(index==index_ponto)
						index_ponto = -1;
					else val = (int) val/10;

					backspace();	// Imprime o backspace na tela
				}
			} break;

			/*--------- Termina a coleta ----------*/
			case '\n':{
				if(var>=0){
					// O índice do ponto só vale se for positivo
					index_ponto = (index_ponto>=0)?index_ponto:index;
					val *= pow(10,index_ponto-index);
					if(var)
						define_referencia(&Href,val);
					else define_referencia(&Tref,val);

					limpa_linha_comando();		//  Limpa a tela

				}else comando_invalido();		//  Imprime aviso
			} break;

			/*--------- Comando inválido ----------*/
			default:{
				comando_invalido();	//  Imprime aviso
			} break;
		}
	}
}
