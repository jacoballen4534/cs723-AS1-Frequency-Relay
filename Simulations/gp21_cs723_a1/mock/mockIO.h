#ifndef MOCKIO_H
#define MOCKIO_H

typedef unsigned long alt_u32;

unsigned int IORD(int base, int regnum);
int alt_irq_register(alt_u32 id, void *context, void (*handler)(void *, alt_u32));

void IOWR_8DIRECT(int base, int offset, int data);
void IOWR_16DIRECT(int base, int offset, int data);
void IOWR_32DIRECT(int base, int offset, int data);

#endif /* MOCKIO_H */
