/*
 * interface.h
 *
 *  Created on: 3 Jun 2020
 *      Author: wsadzik
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

extern struct Interface interface; // To prevent warnings about declaring Interface structure as parameter in below typedefs

typedef enum {
	CAN = 0x00U,
	ETHERNET = 0x01U
} Interface_type;

typedef void (*interface_output)(struct Interface *interface, uint8_t *data);

typedef void (*interface_msg_len)(struct Interface *interface, uint16_t len);

struct Interface {
	Interface_type type;

	interface_output output;

	interface_msg_len len;

	void *msg;

};

#endif /* INTERFACE_H_ */
