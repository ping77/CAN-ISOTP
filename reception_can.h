/*
 * reception_can.h
 *
 * Created: 19/01/2016 11:59:34
 *  Author: CheikhnaAhmedTijane
 */ 


#ifndef RECEPTION_CAN_H_
#define RECEPTION_CAN_H_

//*****************************************************************************
//
//! \Fonction de gestion de l'envoi de flow control can suivant la norme ISOTP (15765).
//!
//! \param can_mb_conf_t est la structure de configuration d'une mailbox.
//! \g_ul_recv_status est le descripteur du statut de l'interruption sur le RX du CAN.
//!
//! \Ne retourne pas de valeurs. Elle est typée void.
//
//*****************************************************************************

void message_received(
can_mb_conf_t can0_mailbox,
volatile uint32_t g_ul_recv_status
);

//*****************************************************************************
//
//! \Fonction de gestion de l'envoi de flow control can suivant la norme ISOTP (15765).
//!
//! \param size est la taille globale du message à reçevoir.
//! \param fc est la stucture qui gère les flow control.
//!
//! \Ne retourne pas de valeurs. Elle est typée void.
//
//*****************************************************************************

static void send_fc(
uint16_t *size,
flow_control fc
);

//*****************************************************************************
//
//! \Fonction de gestion de la réception de single frames can suivant la norme ISOTP (15765).
//!
//! \param msg est le message sous le format Isotp.
//! \param data est le champ de données à renvoyer au host.
//!
//! \return 0 en cas de succès.
//
//*****************************************************************************

static int receive_single_frame(
IsoTpMessage msg,
int data[4095]
);

//*****************************************************************************
//
//! \Fonction de gestion de la réception de first frames can suivant la norme ISOTP (15765).
//!
//! \param msg_in est le message sous le format Isotp.
//! \param complete_message est le buffer de stockage du message complet.
//! \param data_tracer est le pointeur sur la taille des données déjà envoyée.
//! \param mess_size est la taille du message global.
//!
//! \return last_length qui correspond au nombre d'octets déjà reçus.
//
//*****************************************************************************

static uint16_t receive_first_frame(
int complete_message[4095],
IsoTpMessage msg_in,
uint16_t *data_tracer,
uint16_t *mess_size
);

//*****************************************************************************
//
//! \Fonction de gestion de la réception de consecutive frames can suivant la norme ISOTP (15765).
//!
//! \param msg_in est le message sous le format Isotp.
//! \param complete_message est le buffer de stockage du message complet.
//! \param data_tracer est le pointeur sur la taille des données déjà envoyée.
//! \param number_interrupt est le pointeur sur le nombre d'interruptions.
//! \param mess_size est la taille du message global.
//!
//! \Pas de valeur retournée. La fonction est typée void.
//
//*****************************************************************************

static void receive_consecutive_frame(
int complete_message[4095],
uint8_t *number_interrupt,
IsoTpMessage msg_in,
uint16_t *data_tracer,
uint16_t *mess_size
);

//*****************************************************************************
//
//! \Fonction de gestion globale de la partie réception de messages can suivant la norme ISOTP (15765).
//!
//! \param msg_in est le message sous le format Isotp.
//! \param complete_message est le buffer de stockage du message complet.
//! \param last_length est le pointeur sur la taille des données déjà envoyée
//!
//! \Pas de valeur retournée. La fonction est typée void.
//
//*****************************************************************************

void receive_message(
IsoTpMessage msg_in,
int complete_message[4095],
uint16_t *last_length
);

#endif /* RECEPTION_CAN_H_ */
