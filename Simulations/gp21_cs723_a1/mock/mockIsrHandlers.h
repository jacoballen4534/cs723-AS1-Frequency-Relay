
#ifndef MOCK_ISR_HANDLERS_H
#define MOCK_ISR_HANDLERS_H
typedef unsigned long alt_u32;

// ISR handler function pointers
void (*freq_analyser_isr_handler)(void*, alt_u32);


#endif /* MOCK_ISR_HANDLERS_H */
