/*
 * envoi_can.h
 *
 * Created: 19/01/2016 12:01:06
 *  Author: CheikhnaAhmedTijane
 */ 


#ifndef ENVOI_CAN_H_
#define ENVOI_CAN_H_

//*****************************************************************************
//
//! \Fonction de réception d'un flow control suite à l'envoi d'un first frame.
//!
//! \param msg est le message sous le format Isotp.
//! \param fc est la stucture qui gère les flow control.
//!
//! \return status_linker qui indique si la cible est prête à recevoir les consecutive frames.
//
//*****************************************************************************
static int receive_flow_control(
IsoTpMessage msg,
flow_control fc
);

//*****************************************************************************
//
//! \Fonction d'envoi d'un single frame ou trame de 8 octets ou moins suivant la norme ISOTP (15765).
//!
//! \param msg est le message sous le format Isotp.
//! \param mess est le message que désire envoyer le host.
//!
//! \return 0 si la trame est bien envoyée. Sinon -1 s'il y a une erreur de transmission.
//
//*****************************************************************************
static int single_frame(
IsoTpMessage msg,
int mess[4095]
);

//*****************************************************************************
//
//! \Fonction d'envoi d'un consecutive frame ou trame de plus de 8 octets suivant la norme ISOTP (15765).
//!
//! \param msg est le message sous le format Isotp.
//! \param mess est le message que désire envoyer le host.
//! \param last_length est un pointeur sur la taille de données déjà envoyéee.
//! \param data_size est un pointeur sur la taille du message que le host souhaite envoyer.
//! \param identifier est un pointeur correspondant à l'indice du consecutive frame, variant de 0x21 à 0x2F
//!
//! \return 0 en cas de succès.
//
//*****************************************************************************
static int send_cf(
IsoTpMessage msg,
int mess[4095],
uint16_t *last_length,
uint16_t *data_size,
uint16_t *identifier
);

//*****************************************************************************
//
//! \Fonction d'envoi d'un first frame suivant la norme ISOTP (15765).
//!
//! \param msg est le message sous le format Isotp.
//! \param mess est le message que désire envoyer le host.
//! \param last_length est un pointeur sur la taille de données déjà envoyéee.
//! \param data_size est un pointeur sur la taille du message que le host souhaite envoyer.
//!
//! \Ne retourne pas de paramètre. Elle est typée void.
//
//*****************************************************************************
static void send_ff(
IsoTpMessage msg,
int mess[4095],
uint16_t *last_length,
uint16_t *data_size
);

//*****************************************************************************
//
//! \Fonction d'envoi de gestion de l'envoi de messages CAN suivant la norme ISOTP (15765).
//!
//! \param msg est le message sous le format Isotp.
//! \param mess est le message que désire envoyer le host.
//! \param last_length est un pointeur sur la taille de données déjà envoyéee.
//! \param data_size est un pointeur sur la taille du message que le host souhaite envoyer.
//! \param fc_mess est le message flow control
//!
//! \Ne retourne pas de valeurs. Elle est typée void.
//
//*****************************************************************************
static void send_message(
IsoTpMessage msg,
int mess[4095],
uint16_t *last_length,
uint16_t *data_size,
IsoTpMessage fc_mess
);




#endif /* ENVOI_CAN_H_ */
