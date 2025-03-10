@ Calculations
@ 15 Unique Operators: LDR, BL, MOV, CMP, BEQ, B, BLE, BLT, BGT, BGE, ADD, BNE, SUB, BX, SVC
@ 27 Unique Operands (not including labels)
@ Total Operators
@ Total Operands
@ Cyclo Complexity
@ Number of full-line comments
@ Lines of ARM code Total
@ Lines of code with comments
@ Lines of code without comments 
@ Number of directives
@ Registers
@ Calls to subroutines BL 
@ SVC instruction
@ Addressing Modes
@ Assembler Directives 


@ Kaiden Robinson
@ CS 309-02
@ Lab 5
@ This program simulates operation of a gasoline pump
@
@
@ To run the program:
@ 
@ as -o KR_Lab5.o KR_Lab5.s
@ gcc -o KR_Lab5 KR_Lab5.o
@ ./KR_Lab5 ;echo $?
@ 

.equ 		READERROR, 0

.global 	main

main:

welcome:
		LDR		R0, =strWelcome
		BL		printf
		MOV		R4, #0
		MOV		R5, #0
		MOV		R6, #0			
		MOV		R7, #0	
		MOV		R8, #0		
		MOV		R9, #500		@ inv: regular
		MOV		R10, #500		@ inv: mid-grade
		MOV		R11, #500		@ inv: premium

		BL		inventory		@ print inventory
		BL		printf
		BL		dollars
		BL		printf
		

prompt:
		LDR		R0, =strInputPrompt
		BL		printf

get_input:
		LDR		R0, =charInputPattern
		LDR		R1, =intInput
		BL		scanf
		CMP		R0, #READERROR
		BEQ		readerror
		LDR		R1, =intInput
		LDR		R1, [R1]

grade:		
		CMP		R1, #'I'
		BEQ		secret_code
		CMP		R1, #'R'
		BEQ		reg
		CMP		R1, #'M'
		BEQ 	mid
		CMP		R1, #'P'
		BEQ		pre
		
		B		prompt
		
reg:		
		LDR		R0, =strRegular
		MOV		R5, #4
		MOV		R6, R9
		B		cont

mid:
		LDR		R0, =strMidGrade
		MOV		R5, #3
		MOV		R6, R10
		CMP		R6, #5
		BLE		empty
		B		cont
		
pre:		
		LDR		R0, =strPremium
		MOV		R5, #2
		MOV		R6, R11
		
cont:		
		CMP		R6, #0
		BLE		empty
		BL		printf

dispense:
		LDR		R0, =strEnterDollar
		BL		printf
		LDR		R0, =intInputPattern
		LDR		R1, =intInput
		BL		scanf
		CMP		R0, #READERROR
		BEQ		readerror
		LDR		R1, =intInput
		LDR		R1, [R1]
		MOV		R4, R1			@ copy input to R4
		MOV		R7, #0
		MOV		R8, #0

		CMP		R4, #1
		BLT		dispense
		CMP		R4, #50
		BGT		dispense

checkLoop:
		CMP		R7, R6
		BGT		enterLower
		CMP		R8, R4
		BGE		checkComplete
		
		ADD		R7, R7, R5
		ADD		R8, R8, #1
		B		checkLoop
		
checkComplete:
		MOV		R6, #0
		MOV		R7, #0

loop:		
		CMP		R6, R4
		BGE		exitLoop

		CMP		R5, #4
		BNE		skip1
		SUB		R9, R9, R5
		ADD		R7, R7, #4
skip1:		
		CMP		R5, #3
		BNE		skip2
		SUB		R10, R10, R5
		ADD		R7, R7, #3
skip2:
		CMP		R5, #2
		BNE		skip3
		SUB		R11, R11, R5
		ADD		R7, R7, #2
skip3:		
		ADD		R6, R6, #1
		B		loop
		
exitLoop:
		LDR		R0, =strDispense
		MOV		R1, R7
		BL		printf
				
		CMP		R9, #0
		BGT		prompt
		CMP		R10, #5
		BGT		prompt
		CMP		R11, #0
		BGT		prompt
		
		B		pumpOff
			
secret_code:
		BL		inventory
		BL		printf
		BL		dollars
		BL		printf
		B		prompt
	
inventory:	
		LDR		R0, =strInv
		MOV		R1, R9
		MOV		R2, R10
		MOV		R3, R11
		BX		LR

dollars:
		MOV		R4, #500
		MOV		R6, R9
		MOV		R7, R10
		MOV		R8, R11
		LDR 	R0, =strDollar
		
		SUB		R6, R4, R6
		SUB		R7, R4, R7
		SUB		R8, R4, R8
		
regDollars:
		MOV		R4, #0
		MOV		R5, #0
regLoop:
		CMP		R5, R6
		BGE		midDollars
		ADD		R5, R5, #4
		ADD		R4, R4, #1
		B		regLoop
midDollars:
		MOV		R1, R4
		MOV		R4, #0
		MOV 	R5, #0
midLoop:
		CMP		R5, R7
		BGE		preDollars
		ADD		R5, R5, #3
		ADD		R4, R4, #1
		B		midLoop
preDollars:
		MOV		R2, R4
		MOV		R4, #0
		MOV 	R5, #0
preLoop:
		CMP		R5, R8
		BGE		printDollars
		ADD		R5, R5, #2
		ADD		R4, R4, #1
		B		preLoop
printDollars:
		MOV		R3, R4
		BX		LR

enterLower:
		LDR		R0, =strEnterLower
		BL		printf
		B		dispense
		
empty:
		LDR		R0, =strEmpty
		BL		printf
		B		get_input
		
pumpOff:
		LDR		R0, =strEnd
		BL		printf
		BL		inventory
		BL		printf
		BL		dollars
		BL		printf
		B		myexit
		
readerror:
		LDR		R0, =strInputPattern
		LDR		R1, =strInputError
		BL 		scanf
		B		prompt
		
myexit:
		MOV		R7, #0x01
		SVC		0
		
		
.data

.balign 4
strWelcome: .asciz "\nWelcome to the teller machine!\n"

.balign 4
strInputPrompt: .asciz "\nSelect Grade of gas to dispense (R, M, or P)\n"

.balign 4
strRegular: .asciz "\nYou have selected Regular \n"

.balign 4
strMidGrade: .asciz "\nYou have selected Mid-Grade \n"

.balign 4
strPremium: .asciz "\nYou have selected Premium \n"

.balign 4
strEnterDollar: .asciz "\nEnter Dollar amount to dispense (At\nleast 1 and no more than 50)\n"

.balign 4
strEnterLower: .asciz "\nNot enough gasoline. Select a lower amount\n"

.balign 4
strEmpty: .asciz "\nThis gas is empty. Select another one\n"

.balign 4
strDispense: .asciz "\n%d tenths of a gallon have been dispensed\n"

.balign 4
strTryAgain: .asciz "\nPlease check the requirements and try agin\n"

.balign 4
strInv: .asciz "\n*****\n  Inventory of gasoline (in tenths of gallons):\n  Regular: %d \n  Mid-Grade: %d \n  Premium: %d \n*****\n"

.balign 4
strDollar: .asciz "\n*****\nDollar amount dispensed by grade:\n  Regular  $%d\n  Mid-Grade  $%d\n  Premium  $%d\n*****\n"

.balign 4
strEnd: .asciz "\nAll pumps are empty. Have a nice day!\n"

.balign 4
charInputPattern: .asciz "%c"

.balign 4
intInputPattern: .asciz "%d"

.balign 4
strInputPattern: .asciz "%[^\n]"

.balign 4
strInputError: .skip 100*4

.balign 4
intInput: .word 0

.balign 4
count: .word

.global printf

.global scanf
