/* ***************************************************************************************************************

	crt.s						STARTUP  ASSEMBLY  CODE 
								-----------------------


	Module includes the interrupt vectors and start-up code.

  *************************************************************************************************************** */

/* Stack Sizes */
.set  UND_STACK_SIZE, 0x00000004		/* stack for "undefined instruction" interrupts is 4 bytes  */
.set  ABT_STACK_SIZE, 0x00000004		/* stack for "abort" interrupts is 4 bytes                  */
.set  FIQ_STACK_SIZE, 0x00000004		/* stack for "FIQ" interrupts  is 4 bytes         			*/
.set  IRQ_STACK_SIZE, 0X00000004		/* stack for "IRQ" normal interrupts is 4 bytes    			*/
.set  SVC_STACK_SIZE, 0x00000004		/* stack for "SVC" supervisor mode is 4 bytes  				*/



/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers) */
.set  MODE_USR, 0x10            		/* Normal User Mode 										*/
.set  MODE_FIQ, 0x11            		/* FIQ Processing Fast Interrupts Mode 						*/
.set  MODE_IRQ, 0x12            		/* IRQ Processing Standard Interrupts Mode 					*/
.set  MODE_SVC, 0x13            		/* Supervisor Processing Software Interrupts Mode 			*/
.set  MODE_ABT, 0x17            		/* Abort Processing memory Faults Mode 						*/
.set  MODE_UND, 0x1B            		/* Undefined Processing Undefined Instructions Mode 		*/
.set  MODE_SYS, 0x1F            		/* System Running Priviledged Operating System Tasks  Mode	*/

.set  I_BIT, 0x80               		/* when I bit is set, IRQ is disabled (program status registers) */
.set  F_BIT, 0x40               		/* when F bit is set, FIQ is disabled (program status registers) */


.text
.arm

        
.global	Reset_Handler
.global _start

.func   _start

_start:


# Reset Handler

Reset_Handler:  

				/* Setup a stack for each mode - note that this only sets up a usable stack
				for User mode.   Also each mode is setup with interrupts initially disabled. */
    			  
    			ldr   r0, =_stack_end
    			msr   CPSR_c, #MODE_UND|I_BIT|F_BIT 	/* Undefined Instruction Mode  */
    			mov   sp, r0
    			sub   r0, r0, #UND_STACK_SIZE
    			msr   CPSR_c, #MODE_ABT|I_BIT|F_BIT 	/* Abort Mode */
    			mov   sp, r0
    			sub   r0, r0, #ABT_STACK_SIZE
    			msr   CPSR_c, #MODE_FIQ|I_BIT|F_BIT 	/* FIQ Mode */
    			mov   sp, r0	
   				sub   r0, r0, #FIQ_STACK_SIZE
    			msr   CPSR_c, #MODE_IRQ|I_BIT|F_BIT 	/* IRQ Mode */
    			mov   sp, r0
    			sub   r0, r0, #IRQ_STACK_SIZE
    			msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT 	/* Supervisor Mode */
    			mov   sp, r0
    			sub   r0, r0, #SVC_STACK_SIZE
    			msr   CPSR_c, #MODE_SYS|I_BIT|F_BIT 	/* User Mode */
    			mov   sp, r0

        /*
         * Data initialization.
         * NOTE: It assumes that the DATA size is a multiple of 4.
         */
        ldr     r1, =_textdata
        ldr     r2, =_data
        ldr     r3, =_edata
dataloop:
        cmp     r2, r3
        ldrlo   r0, [r1], #4
        strlo   r0, [r2], #4
        blo     dataloop
        /*
         * BSS initialization.
         * NOTE: It assumes that the BSS size is a multiple of 4.
         */
        mov     r0, #0
        ldr     r1, =_bss_start
        ldr     r2, =_bss_end
bssloop:
        cmp     r1, r2
        strlo   r0, [r1], #4
        blo     bssloop
        
				/* Enter the C code  */
                b       main

.endfunc
.end
