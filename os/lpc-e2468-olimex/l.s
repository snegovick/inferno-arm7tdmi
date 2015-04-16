//.global setlabel
//.global splhi
.global getcallerpc
//.global _tas
        
/*setlabel:*/
/*	MOV		R0, sp*/		 /* sp */
/*  ADD   R1, R0, #4*/
/*	MOV		R1, pc*/		 /* pc */
/*	MOV		R0, #0*/
/*	MOV   pc, lr*/


/* splhi: */
/* 	MOV R0, CPSR */
/* 	ORR	$(PsrDirq), R0, R1 */
/* 	MOV	CPSR,	R1 */
/* 	MOV	R6, $(MACHADDR) */
/* 	MOV	(R6), R14	/\* m->splpc *\/ */
/* 	MOV pc, lr */

getcallerpc:
       MOV r0, sp
       MOV pc, lr

/* _tas: */
/*         MOV R1, R0 */
/*         ldr R2, =0xDEADDEAD */
/*         SWP R2, R1, [R0] */
/*         MOV pc, lr */
