@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
     J.F. van der Bent Hogeschool Utrecht

     crt0.s:
            startup-code en interrupt afhandeling

     opmerkingen:
                 -Interrupt vectoren met MEMMAP worden automatisch ingesteld door het linkerscript
				 -Alleen IRQ en Reset exceptions zijn hier uitgewerkt. De andere vectoren
                  verwijzen naar een ander functie
                 -"A re-entrant interrupt handler (at least its 'top-level') *must* be written in assembler."
                  (aldus ARM). En ja, wij zijn re-entrant.
                 -Dit bestand is nogal afhankelijk van het Linkerscript; wil je precies begrijpen wat hier gebruikt,
                  dan moet je het linkerscript dus ook goed doornemen.
                 -Het systeem start in System-modus.
                 -__enable_interrupts en __disable_interrupts werken niet in user-modus. Let op FIQ staat dus uit
                 
				  met dank aan Bastiaan van Kesteren
*/
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.code 32
	.align
    @ De volgende worden in het linkerscript gedefineerd(daar wordt bepaald
    @ waar wat staat in het geheugen)
    .extern __bss_beg__ 
    .extern __bss_end__
    .extern __stack_end__
    .extern __data_beg__
    .extern __data_end__
    .extern __data_beg_src__

    .global _start
    .global endless_loop

@ ARM has 8 exception vectors

	.global	_start
_start:
	ldr		pc, =ResetHandler		@ reset vector
	ldr		pc, =ResetHandler		@ undefined instruction
	ldr		pc, =ResetHandler		@ software interrupt
	ldr		pc, =ResetHandler		@ prefetch abort
	ldr		pc, =ResetHandler		@ data abort
	ldr		pc, =ResetHandler		@ reserved
    ldr 	pc, =IRQHandler         @ IRQ
	ldr		pc, =FIQHandler			@ FIQ

	.pool							@ The ldr instructions load the jump
									@ addresses indirectly from a neareast pool.
									@ Because only the first 0x40 bytes of RAM
									@ are remapped to address 0, we have to put
									@ it here.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Definitie van ARM operating modes
    .set INIT_FIQ_MODE,         0x11
    .set INIT_IRQ_MODE,         0x12
    .set INIT_SUPERVISOR_MODE,  0x13
    .set INIT_ABORT_MODE,       0x17
    .set INIT_UNDEFINED_MODE,   0x1B
    .set INIT_SYSTEM_MODE,      0x1F
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global	FIQHandler
FIQHandler:
	stmfd	sp!, {lr}				@ sla het terugkeerpunt op... let op dit gaat naar de FIQ stack! 
									@ doe jezelf nu een lol en gebruik in de FIQ routine slechts de schaduw registers R7-R12
@	ldr		r0, =MyTimerInterrupt	@ voer hier een routine in die je uit wilt voeren in FIQ mode
	ldr		lr, =FIQHandler_resume  @ Bewaar de plaats van terugkeer (Remi) in het LR
	mov		pc, r0					@ helemaal klaar... kom maar op met die functie..

FIQHandler_resume:
	ldr		r1, =0xFFFFF030			@ VICVectAddr
	str		r0, [r1]				@ update priority hardware (required!)
	ldmfd	sp!, {lr}
	subs	pc, lr, #4				@ Op deze manier moeten we terug 4 bytes van waar we waren... 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ IRQ afhandeling
@ algemene afhandeling. Pas nadat de interrupt wordt aangeroepen wordt het adres
@ van de ISR uit het VIC register geladen. Hierdoor kunnen alle registers van
@ de VIC worden gezet in de routine VIC.c

IRQHandler:
    stmfd   sp!, {r0-r3,r12,lr}     @ registers op stack
    ldr     lr, =ExitISR            @ return-adres (voor later)
    ldr     r0, =0xFFFFF030         @ VICVectAddr
    ldr     pc, [r0]                @ spring naar (C-)functie van VICVectAddr

ExitISR:
    ldr     r1, =0xFFFFF030         @ VICVectAddr
    str     r0, [r1]                @ update priority hardware
    ldmfd   sp!, {r0-r3,r12,lr}
    subs    pc, lr, #4              @ return van IRQ-interrupt


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ __disable_interrupts
@ Disables the interrupts

	.global	__disable_interrupts
__disable_interrupts:
	mrs		r0, CPSR
	orr		r0, r0, #0xC0			@ disable IRQ/FIQ interrupts
	msr		CPSR_fsxc, r0
	mov		pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ __enable_interrupts
@ Interrupts aan. Let op FIQ staat uit en wordt hier niet aangezet! Hierboven wordt hij wel uitgezet...

	.global	__enable_interrupts
__enable_interrupts:
	mrs		r0, CPSR
	bic		r0, r0, #0x80			@ enable aleen de IRQ interrupts
	msr		CPSR_fsxc, r0
	mov		pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ The first things that need to be done

ResetHandler:

@
@ Initialize stack pointers of IRQ and supervisor mode to end of RAM
@

    @ We beginnen met stack-initialisatie, voor elke mode een
    @ Grootes van stack zijn gedefineerd in het linkerscript
    msr     cpsr_c, #INIT_FIQ_MODE
    ldr     sp, =__stack_end_fiq__
    msr     cpsr_c, #INIT_IRQ_MODE
    ldr     sp, =__stack_end_irq__
    msr     cpsr_c, #INIT_SUPERVISOR_MODE
    ldr     sp, =__stack_end_supervisor__
    msr     cpsr_c, #INIT_ABORT_MODE
    ldr     sp, =__stack_end_abort__
    msr     cpsr_c, #INIT_UNDEFINED_MODE
    ldr     sp, =__stack_end_undefined__
    msr     cpsr_c, #INIT_SYSTEM_MODE
    ldr     sp, =__stack_end__

    sub     sl, sp, #512        @ StackLimit zit 512 bits boven de StackPointer

    mov     a2, #0              @ FramePointer..
    mov     fp, a2              @ ..op nul

    @ Volgens de C-standaard moeten nieuwe variables NULL zijn (als in 'int i;')
    @ De compiler doet dit zelf niet, maar gaat ervanuit dat dit door het systeem wordt geregeld
    @ Laten wij nu eens het systeem zijn... dus, hier ongebruikt geheugen wissen.
    @
    ldr     r1, .LC1            @  __bss_beg__ laden; gedefineerd in het linkerscript
    ldr     r3, .LC2            @  __bss_end__ laden;        ''
    subs    r3, r3, r1          @ bepaal lengte van BSS (__bss_end__ - __bss_beg__)
    beq     .end_clear_loop     @ Als er geen ongeinitialiseerd geheugen is, is er hier 
                                @ ook niets te doen.. branch naar eind v. functie

    mov     r2, #0              @ En we vullen met...
.clear_loop:
    strb    r2, [r1], #1        @ Vul byte in r1, en schuif meteen door naar het volgende byte
    subs    r3, r3, #1          @ Lengte van BSS minus 1
    bgt     .clear_loop         @ Zijn we al klaar ?
.end_clear_loop:

    @ En vervolgens kopieren we de initialisatie-waardes voor .data van flash naar RAM
    @
    @ init-waardes voor variables worden in flash opgeslagen, en tijdens startup op de
    @ juiste plek(in RAM dus) geladen.
    @
    @ En inderdaad, dat laatste gebeurt hier
    @
    ldr     r1, .LC3            @ __data_beg__ laden; ook deze staat in het linkerscript
    ldr     r2, .LC4            @ __data_beg_src__ laden;   ''
    ldr     r3, .LC5            @ __data_end__ laden;       ''
    subs    r3, r3, r1          @ En bepaal lengte van dit blok (__data_end__ - __data_beg__)
    beq     .end_set_loop       @ Als er geen .data blok is om over te kopieren, is er hier 
                                @ ook niets te doen.. branch naar eind v. functie
.set_loop:
    ldrb    r4, [r2], #1        @ Byte inlezen van flash (en __data_beg_src__++)..
    strb    r4, [r1], #1        @ ..en opslaan in RAM(en __data_beg__++)
    subs    r3, r3, #1          @ hoeveelheid over te kopieren data min een
    bgt     .set_loop           @ tot er niets meer van over is
.end_set_loop:
 

@ O, wacht.. die interrupts moeten ook nog in het juiste deel staan...
@ Hier wordt gewerkt de LC6 definitie uit het linkerscipt
@ Wanneer er read only (ro) data wordt geschreven naar het ram
@ zet deze routine de MEMMAP op SRAM en anders op ROM (flash)

	ldr     r1, .LC6			@ load ro_end from linkerscript
	ands	r1,#0x40000000		@ maskeer voor RAM
	bne		.remap_ram
    mov     r1, #1              @ interruptvectors staan in flash
 	b		.setmemmap
.remap_ram:
   mov     r1, #2              @ interruptvectors staan in SRAM
.setmemmap:
	ldr     r0, =0xE01FC040     @ MEMMAP
    str     r1, [r0]


@
@ Clear BSS (non-initialized variables)
@

	mov		r0, #0
	ldr		r1, =__data_end
	ldr		r2, =__bss_end

ClearBSSLoop:
	cmp	r1, r2
	beq		ClearBSSLoopDone		@ check if we're done yet
	str		r0, [r1], #4			@ fill zero's
	b		ClearBSSLoop	
ClearBSSLoopDone:

@
@ Jump to user code
@

	b		main					@ assume we're not going to return

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Definitie van constanten.
@
@   "For Thumb, constants must be after the code since only
@    positive offsets are supported for PC relative addresses."
@
@ dusch...

    .align 0
.LC1: 
.word   __bss_start
.LC2:
.word   __bss_end__
.LC3:
.word   __data_beg__
.LC4:
.word   __data_beg_src__
.LC5:
.word   __data_end__
.LC6:	
.word	__ro_end

	.end

