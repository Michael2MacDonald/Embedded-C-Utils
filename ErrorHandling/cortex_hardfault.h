/**
 * 
 * For Cortex-M33
 * Google AI Overview
 */

#include "arm_cmse.h" // For TrustZone-aware CMSIS-Core
#include <stdio.h>

/**
 * C Handler to analyze fault type
 */
void HardFault_Handler_C(uint32_t *stack_frame) {
    // Read Status Registers from the System Control Block (SCB)
    uint32_t cfsr  = SCB->CFSR;
    uint32_t hfsr  = SCB->HFSR;
    uint32_t mmfar = SCB->MMFAR; // MemManage Fault Address
    uint32_t bfar  = SCB->BFAR;  // Bus Fault Address

    // Register values from the stack frame
    uint32_t pc = stack_frame[6]; 

    printf("--- Fault Detected ---\n");
    if (hfsr & SCB_HFSR_FORCED_Msk) {
        printf("HardFault: Escalated from another fault\n");
    }

    if (cfsr & 0xFFFF0000) {
        printf("UsageFault: Cause bits 0x%04lX\n", (cfsr >> 16));
    }
    if (cfsr & 0x0000FF00) {
        printf("BusFault: Cause bits 0x%02lX at Address: 0x%08lX\n", (cfsr >> 8 & 0xFF), bfar);
    }
    if (cfsr & 0x000000FF) {
        printf("MemManage: Cause bits 0x%02lX at Address: 0x%08lX\n", (cfsr & 0xFF), mmfar);
    }

    printf("Faulting Instruction PC: 0x%08lX\n", pc);
    while (1); // Halt
}

/**
 * Naked Assembly Wrapper
 * Determines if MSP or PSP was used and passes it to the C handler
 */
__attribute__((naked)) void HardFault_Handler(void) {
    __asm volatile (
        " tst lr, #4                                \n"
        " ite eq                                    \n"
        " mrseq r0, msp                             \n"
        " mrsne r0, psp                             \n"
        " b HardFault_Handler_C                     \n"
    );
}