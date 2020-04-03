#ifndef MOCKIO_H
#define MOCKIO_H

typedef unsigned long alt_u32;

unsigned int IORD(int BASE, int REGNUM);
int alt_irq_register( alt_u32 id, void* context, void (*handler)(void*, alt_u32) );

#endif /* MOCKIO_H */
