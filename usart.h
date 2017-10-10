#ifndef USART_H
#define USART_H

#include <htc.h>

void USARTInit();

void USARTWriteByte(char);

void USARTWriteString(const char *);

void USARTWriteLine(const char *);

void USARTWriteInt(int ,unsigned char);

unsigned char USARTReadByte();

#endif