#ifndef MOCKIO_H
#define MOCKIO_H

#define IORD_ALTERA_AVALON_PIO_DATA(base) IORD(base, 0)
#define IORD_ALTERA_AVALON_PIO_EDGE_CAP(base) IORD(base, 0)

#define IOWR_ALTERA_AVALON_PIO_EDGE_CAP(base, data) IOWR(base, 3, data)
#define IOWR_ALTERA_AVALON_PIO_IRQ_MASK(base, data) IOWR(base, 2, data)

typedef unsigned long alt_u32;

unsigned int IORD(int base, int regnum);
void IOWR(int base, int regnum, unsigned int data);

int alt_irq_register(alt_u32 id, void *context, void (*handler)(void *, alt_u32));
void IOWR_ALTERA_AVALON_PIO_DATA(unsigned int base, unsigned int data);

void IOWR_8DIRECT(int base, int offset, int data);
void IOWR_16DIRECT(int base, int offset, int data);
void IOWR_32DIRECT(int base, int offset, int data);

#endif /* MOCKIO_H */
