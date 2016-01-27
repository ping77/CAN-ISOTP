/*
 * can_handler.h
 *
 * Created: 20/01/2016 09:36:16
 *  Author: CheikhnaAhmedTijane
 */ 


#ifndef CAN_HANDLER_H_
#define CAN_HANDLER_H_

volatile uint32_t g_ul_recv_status;
void can_initialiser(
can_mb_conf_t can0_mailbox0,
can_mb_conf_t can0_mailbox1
);

void choice_handler(
uint16_t our_id
);



#endif /* CAN_HANDLER_H_ */
