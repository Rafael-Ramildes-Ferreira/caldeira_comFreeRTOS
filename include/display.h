//####################  Cabeçalho do Display  ####################//
#ifndef DISPLAY_H
#define DISPLAY_H

#include "instrumentacao.h"


#ifdef __cplusplus
extern "C" {
#endif

void atualiza_valores_da_tela(int tempo);

void inicializa_interface();

void print_warning(int valor);

void erase_warning();

void finalizar_programa(int number_of_blockedTasks, TaskHandle_t *blockedTaskHandle);

#ifdef __cplusplus
}
#endif

#endif
