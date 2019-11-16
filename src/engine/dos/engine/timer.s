; *******************************************************
; *
; * Modified 32-bit nasm version of
; * Preicision Zen Timer from Michael Abrash's book:
; *		"Graphics Programming Black Book"
; *
; * The special MPOPF macro has been replaced by an actual popfd
; * as HustleEngine only targets 386+
; *******************************************************
	bits 32
segment _BSS public class=DATA USE32

;; Variables
OriginalFlags	db 0
TimedCount		dw 0
ReferenceCount	dw 0
OverflowFlag	db 0

segment _TEXT public class=CODE USE32
	global timer_start
	global zen_timer_stop
	global zen_timer_get

%define BASE_8253		0x40
%define TIMER_0_8253	BASE_8253 + 0
%define MODE_8253		BASE_8253 + 3
%define OCW3			0x20
%define IRR				0x20

;; Macros
%macro DELAY 0
	jmp $+2
	jmp $+2
	jmp $+2
%endmacro

; ********************************************************************
; * Routine called to start timing.                                  *
; ********************************************************************
timer_start:
;
; Save the context of the program being timed.
;
	push   eax
	pushfd
	pop    eax                     ; get flags so we can keep
								   ; interrupts off when leaving
								   ; this routine
	mov    [OriginalFlags], ah     ; remember the state of the
								   ; Interrupt flag
	and    ah,0xfd                 ; set pushed interrupt flag
								   ; to 0
	push   eax
;
; Turn on interrupts, so the timer interrupt can occur if it's
; pending.
;
	sti
;
; Set timer 0 of the 8253 to mode 2 (divide-by-N), to cause
; linear counting rather than count-by-two counting. Also
; leaves the 8253 waiting for the initial timer 0 count to
; be loaded.
;
	mov  al,00110100b               ;mode 2
	out  MODE_8253,al
;
; Set the timer count to 0, so we know we won't get another
; timer interrupt right away.
; Note: this introduces an inaccuracy of up to 54 ms in the system
; clock count each time it is executed.
;
	DELAY
	sub     al,al
	out     TIMER_0_8253,al    		;lsb
	DELAY
	out     TIMER_0_8253,al     	;msb
;
; Wait before clearing interrupts to allow the interrupt generated
; when switching from mode 3 to mode 2 to be recognized. The delay
; must be at least 210 ns long to allow time for that interrupt to
; occur. Here, 10 jumps are used for the delay to ensure that the
; delay time will be more than long enough even on a very fast PC.
	DELAY
	DELAY
	DELAY
;
; Disable interrupts to get an accurate count.
;
	cli
;
; Set the timer count to 0 again to start the timing interval.
;
	mov  al,00110100b        ; set up to load initial
	out  MODE_8253,al        ; timer count
	DELAY
	sub  al,al
	out  TIMER_0_8253,al     ; load count lsb
	DELAY
	out  TIMER_0_8253,al; load count msb
;
; Restore the context and return.
;
	;MPOPF                   ; keeps interrupts off
	popfd 				     ; no guarantees that interrupts are off on 286
	pop   eax
	ret

;********************************************************************
;* Routine called to stop timing and get count.                     *
;********************************************************************
zen_timer_stop:
;
; Save the context of the program being timed.
;
	push    eax
	push    ecx
	pushfd
;
; Latch the count.
;
	mov  al,00000000b     	  ; latch timer 0
	out  MODE_8253,al
;
; See if the timer has overflowed by checking the 8259 for a pending
; timer interrupt.
;
	mov   al,00001010b        ; OCW3, set up to read
	out   OCW3,al             ; Interrupt Request register
	DELAY
	in    al,IRR              ; read Interrupt Request
	                          ; register
	and   al,1                ; set AL to 1 if IRQ0 (the
	                          ; timer interrupt) is pending
	mov   [OverflowFlag],al   ; store the timer overflow
	                          ; status
;
; Allow interrupts to happen again.
;
	sti
;
; Read out the count we latched earlier.
;
	in     al,TIMER_0_8253   ; least significant byte
	DELAY
	mov    ah,al
	in     al,TIMER_0_8253   ; most significant byte
	xchg   ah,al
	neg    ax                ; convert from countdown
	                         ; remaining to elapsed
	                         ; count
	mov    [TimedCount],ax
; Time a zero-length code fragment, to get a reference for how
; much overhead this routine has. Time it 16 times and average it,
; for accuracy, rounding the result.
;
	mov   word [ReferenceCount],0
	mov   cx,16
	cli                		; interrupts off to allow a
	                		; precise reference count
.RefLoop:
	call   ReferenceZTimerOn
	call   ReferenceZTimerOff
	loop   .RefLoop
	sti
	add    word [ReferenceCount],8	; total + (0.5 * 16)
	mov    cl,4
	shr    word [ReferenceCount],cl	; (total) / 16 + 0.5
;
; Restore original interrupt state.
;
	pop    ax                    ; retrieve flags when called
	mov    ch, [OriginalFlags]   ; get back the original upper
	                             ; byte of the FLAGS register
	and    ch, ~0xfd             ; only care about original
	                             ; interrupt flag...
	and    ah,0xfd               ; ...keep all other flags in
	                             ; their current condition
	or     ah,ch                 ; make flags word with original
	                             ; interrupt flag
	push   ax                    ; prepare flags to be popped
;
; Restore the context of the program being timed and return to it.
;
	;MPOPF                       ; restore the flags with the
	                             ; original interrupt state
	popfd
	pop    ecx
	pop    eax
	ret


;
; Called by ZTimerOff to start timer for overhead measurements.
;

ReferenceZTimerOn:
;
; Save the context of the program being timed.
;
	push  eax
	pushfd     			   ; interrupts are already off
;
; Set timer 0 of the 8253 to mode 2 (divide-by-N), to cause
; linear counting rather than count-by-two counting.
;
	mov    al,00110100b    ; set up to load
	out    MODE_8253,al    ; initial timer count
	DELAY
;
; Set the timer count to 0.
;
	sub    al,al
	out    TIMER_0_8253,al  ; load count lsb
	DELAY
	out    TIMER_0_8253,al  ; load count msb
;
; Restore the context of the program being timed and return to it.
;
	popfd
	pop    eax
	ret

;
; Called by ZTimerOff to stop timer and add result to ReferenceCount
; for overhead measurements.
;

ReferenceZTimerOff:
;
; Save the context of the program being timed.
;
	push   eax
	push   ecx
	pushfd
;
; Latch the count and read it.
;
	mov   al,00000000b        	; latch timer 0
	out   MODE_8253,al
	DELAY
	in    al,TIMER_0_8253   	; lsb
	DELAY
	mov   ah,al
	in    al,TIMER_0_8253    	; msb
	xchg  ah,al
	neg   ax                 	; convert from countdown
	                          	; remaining to amount
	                          	; counted down
	add   [ReferenceCount],ax
;
; Restore the context of the program being timed and return to it.
;
    popfd
    pop    ecx
    pop    eax
    ret

;********************************************************************
;* Routine to get time from timer                             *
;********************************************************************
zen_timer_get:
	cmp 	byte [OverflowFlag], 0
	jnz 	.overflowed
	mov		ax, word [TimedCount]
	sub 	ax, word [ReferenceCount]
	ret
.overflowed:
	mov 	eax, 0
	ret
