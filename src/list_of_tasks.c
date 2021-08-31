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

/*used to control*/
double ni, q, na, nf;

void create_tasks()
{
    ni = .1;
    q = .1;
    na = .1;
    nf = .1;

    xTaskCreate(&vTA, "Task A", 1024, NULL, 1, NULL);
    xTaskCreate(&vTB, "Task B", 1024, NULL, 2, NULL);
}

void vTA(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xTime = 100;
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xTime);
        ni *= 1.01;

        q *= 1.01;

        na *= 1.01;

        nf *= 1.01;
    }
}

void vTB(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xTime= 1000;
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xTime);

        double read;

        /*Sets*/

        send_set_message(SET_NI, ni);
        console_print("SET_NI sent: %lf\n", 10);

        if (receive_message(SET_NI, &read, COMMUNICATION_BLOCKING)){
            console_print("SET_NI received: %lf\n", read);
	}

        send_set_message(SET_Q, q);
        console_print("SET_Q sent: %lf\n", q);

        if (receive_message(SET_Q, &read, COMMUNICATION_BLOCKING))
            console_print("SET_Q received: %lf\n", read);

        send_set_message(SET_NA, na);
        console_print("SET_NA sent: %lf\n", na);

        if (receive_message(SET_NA, &read, COMMUNICATION_BLOCKING))
            console_print("SET_NA received: %lf\n", read);

        send_set_message(SET_NF, nf);
        console_print("SET_NF sent: %lf\n", nf);

        if (receive_message(SET_NF, &read, COMMUNICATION_BLOCKING))
            console_print("SET_NF received: %lf\n\n", read);

        /*Requests*/
        send_request_message(REQUEST_TA);
        if (receive_message(REQUEST_TA, &read, COMMUNICATION_BLOCKING))
            console_print("REQUEST_TA received: %lf\n", read);

        send_request_message(REQUEST_T);
        if (receive_message(REQUEST_T, &read, COMMUNICATION_BLOCKING))
            console_print("REQUEST_T received: %lf\n", read);

        send_request_message(REQUEST_TI);
        if (receive_message(REQUEST_TI, &read, COMMUNICATION_BLOCKING))
            console_print("REQUEST_TI received: %lf\n", read);

        send_request_message(REQUEST_NO);
        if (receive_message(REQUEST_NO, &read, COMMUNICATION_BLOCKING))
            console_print("REQUEST_NO received: %lf\n", read);

        send_request_message(REQUEST_H);
        if (receive_message(REQUEST_H, &read, COMMUNICATION_BLOCKING))
            console_print("REQUEST_H received: %lf\n", read);

        console_print("============================\n");
    }
}
