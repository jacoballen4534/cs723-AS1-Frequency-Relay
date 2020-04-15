
#ifndef MOCK_ISR_HANDLERS_H
#define MOCK_ISR_HANDLERS_H
typedef unsigned long alt_u32;

// ISR handler function pointers
void (*freq_analyser_isr_handler)(void *, alt_u32);
void *freq_analyser_context;

void (*keyboard_isr_handler)(void *, alt_u32);
void *keyboard_context;

void (*button_isr_handler)(void *, alt_u32);
void *button_context;

#endif /* MOCK_ISR_HANDLERS_H */
