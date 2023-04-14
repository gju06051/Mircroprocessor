/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (1)
#endif
/* ISR prototypes */
void R_IRQ11_ISR(void);

/* Vector table allocations */
#define VECTOR_NUMBER_ICU_IRQ11 ((IRQn_Type) 0) /* ICU IRQ11 (External Interrupt: PORD_IRQ11) */
#define ICU_IRQ11_IRQn          ((IRQn_Type) 0) /* ICU IRQ11 (External Interrupt: PORD_IRQ11) */
#endif /* VECTOR_DATA_H */
