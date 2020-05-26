/*
 * html-page.h
 *
 *  Created on: 20 May 2020
 *      Author: wsadzik
 */

#ifndef HTML_PAGE_H_
#define HTML_PAGE_H_

#include "stdint.h"

extern const unsigned char TEST_PAGE[];
extern const unsigned char HTTP_OK[];

#define TEST_PAGE_LEN 202U

uint16_t TEST_PAGE_SUM(void);

#endif /* HTML_PAGE_H_ */
