//####################  Cabeçalho do Display  ####################//
#ifndef DISPLAY_H
#define DISPLAY_H

#include "instrumentacao.h"

#define displayREF_PORCENT 5.0

#ifdef __cplusplus
extern "C" {
#endif

void fast_inicializa_interface();

void imprime_valore_na_tela(int tempo);

void informa_tempo_levado(double time);

void informa_alarme(int valor);

void termina_alarme();


//###############  DEPRICATED   ###############//
void inicializa_interface();

//###############  DEPRICATED   ###############//
void atualiza_valores_da_tela(int tempo);

//###############  DEPRICATED   ###############//
void tempo_levado(double time);

//###############  DEPRICATED   ###############//
void print_warning(int valor);

//###############  DEPRICATED   ###############//
void erase_warning();

//###############  DEPRICATED   ###############//
void finalizar_programa(int number_of_blockedTasks, TaskHandle_t *blockedTaskHandle);

#ifdef __cplusplus
}
#endif

#endif
