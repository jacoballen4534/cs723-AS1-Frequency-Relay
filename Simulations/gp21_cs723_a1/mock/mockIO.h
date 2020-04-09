#ifndef MOCKIO_H
#define MOCKIO_H

#define IORD_ALTERA_AVALON_PIO_DATA(base) IORD(base, 0)

typedef unsigned long alt_u32;

unsigned int IORD(int base, int regnum);
int alt_irq_register(alt_u32 id, void *context, void (*handler)(void *, alt_u32));

void IOWR_8DIRECT(int base, int offset, int data);
void IOWR_16DIRECT(int base, int offset, int data);
void IOWR_32DIRECT(int base, int offset, int data);

#endif /* MOCKIO_H */
