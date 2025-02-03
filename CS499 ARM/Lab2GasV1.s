@ ARM Lab 2 Gasoline Pump
@ Kevin Preston
@ V1 - Make more modular with functions. 

@----- Code Section -----

.text
.balign 4
.global main

main: @program entry point

@Zero out the counters and set the prices an fuel limits.

     MOV r7,  #0     @Current amount of gas dispensed.
     MOV r6,  #0     @User entered amount of gas. 
     MOV r5,  #''    @Grade to dispense.  

@Registers for the fuel quanity in tenths of gallons

     MOV r8,  #5    @Unleaded
     MOV r9,  #15   @Mid-Grade
     MOV r10, #23   @Premium

Welcome:
     LDR  r0, =strWelcome
     BL   printf

Instruction:    @Print instruction and get user entry.
     LDR  r0, =strInstruction
     BL   printf
     
     LDR  r0, =chrUserInput
     SUB  sp, sp, #4   @update stack pointer to save registers.
     MOV  r1, sp
     BL   scanf
     LDR  r11, [sp, #0] @input letter stored in r11. 
     ADD  sp, sp, #4

     CMP  r11, #'Q'  @Secret code to quit program. 
     BEQ  ShutDown

     CMP  r11, #'I'  @Secret code to get inventory. 
     BLEQ  Inventory
     BEQ  Instruction

     CMP  r11, #'P'  @Code to start pumping. 
     BEQ  StartPumping

     CMP  r11, #'S'  @Code to enter dollar amount
     BEQ  UserEnterMoney

     B    Instruction @Invalid entry try again. 


UserEnterMoney:
     LDR  r0, =strEnterAmount
     BL   printf

     LDR  r0, =intRequestAmount
     SUB  sp, sp, #4   @update stack pointer to save registers.
     MOV  r1, sp
     BL   scanf
     LDR  r11, [sp, #0] @input value stored in r11. 
     ADD  sp, sp, #4
     MOV  r6, r11       @Amount entered in r6

     BL   UserSelectFuel @Grade selected in r5

@ Calculate gallons to dispense. The calculated gallons is 
@ returned in r2. 
     BL  CalculateGallons

@ Is there enough grade in r5 to meet request?
@ Get the amount of selected grade into r0.  
     CMP   r5, #'U'
     MOVEQ r0, r8
     CMP   r5, #'M'
     MOVEQ r0, r9
     CMP   r5, #'P'
     MOVEQ r0, r10
    
     CMP   r0, #0  @Zero amount left in grade. 
     BLE   CannotMeetRequest
     CMP   r2, r0  @Not enough left in grade to make request.
     BGT   CannotMeetRequest

     STMFD sp!, {r2} @save r2
     LDR   r0, =strEnterYToStart
     BL    printf

     LDR   r0, =chrUserInput
     SUB   sp, sp, #4   @update stack pointer to save registers.
     MOV   r1, sp
     BL    scanf
     LDR   r11, [sp, #0] @input letter stored in r11. 
     ADD   sp, sp, #4
     
     CMP   r11, #'Y'
     BNE   Instruction      @Did not say Yes go to menu. 

@Print gallons to dispense.
     LDMFD sp!, {r2} @restore r2     
     STMFD sp!, {r2} @save r2
     MOV  r1, r2 @Tenths of gallons dispensed. 
     LDR  r0, =strGasDispensed
     BL   printf
     LDMFD sp!, {r2} @restore r2

@ Decrement gas that was dispensed. 
     BL   UpdateInventory

     B    Instruction @go back to user prompt. 

@----------------------------------------
@This is for the P - Start Pumping Entry. 
@ r5 - Grade selected by user
@ r6 - Cost of fuel dispensed 
@ r2 - Amount of fuel dispensed so far.
@ r0 - Amount of fuel in selected grade. 
@ Dispense a gallon at a time. 

StartPumping:

     MOV  r2, #0  @Pump a gallon at a time.
     MOV  r6, #0  @Zero out cost of fuel so far.
     BL   UserSelectFuel  @Grade selected in r5

@ Is there a gallon left in the selected grade? 

@ Is there enough grade in r5 to meet request?
@ Get the amount of selected grade into r0.  
PumpOneGallon:
     CMP   r5, #'U'
     MOVEQ r0, r8
     CMP   r5, #'M'
     MOVEQ r0, r9
     CMP   r5, #'P'
     MOVEQ r0, r10

     CMP   r0, #0  @Zero amount left in grade. 
     BLE   CannotMeetRequest
     CMP   r0, #10  @Not enough left in grade to make request.
     BLT   PumpRemaining @ fix this to pump remaining. 

@ Calculate the dollar amount of gas dispensed so far. 

     CMP   r5, #'U'
     ADDEQ r6, r6, #200
     CMP   r5, #'M'
     ADDEQ r6, r6, #300
     CMP   r5, #'P'
     ADDEQ r6, r6, #400

     ADD   r2, r2, #10  @Pump another gallon. 
     STMFD sp!, {r0, r2, r6} @save registers 
     MOV   r1, r2 @Tenths of gallons dispensed. 
     LDR   r0, =strGasDispensedSoFar
     BL    printf
     LDMFD sp!, {r0,r2, r6} @restore registers

     STMFD sp!, {r0,r2, r6} @save registers 
     MOV   r1, r6 @Cents of gallons dispensed. 
     LDR   r0, =strValueDispensedSoFar
     BL    printf
     LDMFD sp!, {r0,r2, r6} @restore registers

@ Decrement amount of gas dispensed. 
     CMP   r5, #'U'
     SUBEQ r8, r8, #10
     CMP   r5, #'M'
     SUBEQ r9, r9, #10
     CMP   r5, #'P'
     SUBEQ r10,r10,#10

ContinuePumping:
     STMFD sp!, {r0, r2} @save r2
     LDR   r0, =strEnterYToStart
     BL    printf

     LDR   r0, =chrUserInput
     SUB   sp, sp, #4   @update stack pointer to save registers.
     MOV   r1, sp
     BL    scanf
     LDR   r11, [sp, #0] @input letter stored in r11. 
     ADD   sp, sp, #4
     LDMFD sp!, {r0, r2} @restore registers
     
     CMP   r11, #'Y'
     BNE   FinishStartPumping  @Did not say Yes Finish.  
     B     PumpOneGallon

     LDR  r0, =strWantToContinue
     BL   printf

FinishStartPumping:
     B    Instruction

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
PumpRemaining:
@Not a whole gallon remaining pump the fractional parts left then
@print transaction totals and go to instruction prompt.
@ r0 has the remaining fuel left in grade.
@ r2 gallons of gas pumped. 
@ r5 grade of gas selected.
@ r6 Dollar amount of gas pumped.

TenthsLeft:
     CMP   r5, #'U'
     ADDEQ r6, r6, #20 @$ for tenth
     ADDEQ r2, r2, #1  @Pump tenth 
     SUBEQ r8, r8, #1  @Subtract tenth
     CMP   r5, #'M'
     ADDEQ r6, r6, #30 @$ for tenth
     ADDEQ r2, r2, #1  @Pump tenth
     SUBEQ r9, r9, #1  @Subtract tenth
     CMP   r5, #'P'
     ADDEQ r6, r6, #40  @$ for tenth
     ADDEQ r2, r2, #1   @Pump tenth
     SUBEQ r10, r10, #1 @Subtract tenth

@ If still tenths left pump another tenth.   
     CMP   r5, #'U'
     MOVEQ r0, r8
     CMP   r5, #'M'
     MOVEQ r0, r9
     CMP   r5, #'P'
     MOVEQ r0, r10

     CMP   r0, #0
     BGT   TenthsLeft

@ Dispensed all the tenths.  

     STMFD sp!, {r0, r2, r6} @save registers 
     MOV   r1, r2 @Tenths of gallons dispensed. 
     LDR   r0, =strGasDispensedSoFar
     BL    printf
     LDMFD sp!, {r0,r2, r6} @restore registers

     STMFD sp!, {r0,r2, r6} @save registers 
     MOV   r1, r6 @Cents of gallons dispensed. 
     LDR   r0, =strValueDispensedSoFar
     BL    printf
     LDR   r0, =strNoneLeftInGrade
     BL    printf
     LDMFD sp!, {r0,r2, r6} @restore registers
     B    Instruction @Go to instruction prompt. 

@end PumpRemaining
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ConfirmAmount:
     LDR  r0, =strConfirmAmount
     BL   printf
     LDR  r0, =strOKWithAmount
     BL   printf

@ if not confirmed - entered N go to UserEnterMoney

CannotMeetRequest:
     LDR  r0, =strCannotMeetRequest
     BL   printf
     B    Instruction

EndTransaction:
     LDR  r0, =strGasDispensed
     BL   printf
     LDR  r0, =strValueDispensed
     BL   printf

ShutDown:
    LDR  r0, =strShuttingDown
    BL   printf
    LDR  r0, =strTotalSales
    BL   printf


@ Return control to OS.

exit:
     MOV  r7, #0x01  @doing an exit when using 1. 
     SVC  0          @do the system call. 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Function to update inventory
@ Inputs:
@   r2 - Amount of gas in this transaction. 
@   r8 - Quanity of Unleaded 
@   r9 - Quanity of Mid-Grade
@   r10 - Quanity of Premium 
@ Outputs:
@   None.
@ Registers Used:
@   r1 - used to save return address.
@ Decrement gas that was dispensed. 
UpdateInventory:
     STMFD sp!, {r1} @ Save the registers 
     LDR  r1, =return2  @Save return address 
     STR  lr, [r1]

     CMP   r5, #'U'
     SUBEQ r8, r8, r2
     CMP   r5, #'M'
     SUBEQ r9, r9, r2
     CMP   r5, #'P'
     SUBEQ r10,r10,r2
     LDR  lr, =return2  @Restore return address
     LDR  lr, [lr]
     LDMFD sp!, {r1}    @Restore the registers
     BX   lr            @Return
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Function to print current Inventory information.
@ Inputs:
@   r8 - Quanity of Unleaded 
@   r9 - Quanity of Mid-Grade
@   r10 - Quanity of Premium 
@ Outputs:
@   None.
@ Registers Used:
@   r0 - used to print string
@   r1 - used to print quanity of gas. 
Inventory:
     STMFD sp!, {r0-r12} @ Save the registers 
     LDR  r1, =return2  @Save return address 
     STR  lr, [r1]

     LDR  r0, =strInventory
     BL   printf
     MOV  r1, r8 @Unleaded quanity
     LDR  r0, =strUnleaded
     BL   printf
     MOV  r1, r9 @Mid-Grade quanity
     LDR  r0, =strMidGrade
     BL   printf
     MOV  r1, r10 @Premium quanity
     LDR  r0, =strPremium
     BL   printf
 
     LDR  lr, =return2  @Restore return address
     LDR  lr, [lr]
     LDMFD sp!, {r0-r12}    @Restore the registers
     BX   lr            @Return
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Function to get user selected fuel grade.
@ Inputs: 
@    None. 
@ Outputs: 
@    r5 - User selected fuel grade U, M or S. 
@ Registers Used:
@  r1 - Save return address and get inputs
@  r0 - address for print strings.
@  r11 - used to get inputs letter. 
@ 
@ Keep prompting until valid fuel grade is selected. 
UserSelectFuel:
     STMFD sp!, {r0-r4, r6-r12} @ Save the registers 
     LDR  r1, =return2  @Save return address 
     STR  lr, [r1]

PromptAgain:
     LDR  r0, =strGradePrompt
     BL   printf

     LDR  r0, =chrUserInput
     SUB  sp, sp, #4   @update stack pointer to save registers.
     MOV  r1, sp
     BL   scanf
     LDR  r11, [sp, #0] @input letter stored in r11. 
     ADD  sp, sp, #4

     MOV  r5, r11 
     CMP  r5, #'U'
     BEQ  USFReturn
     CMP  r5, #'M'
     BEQ  USFReturn
     CMP  r5, #'P'
     BEQ  USFReturn
     CMP  r5, #'Q'
     BEQ  Instruction @ Go back to Instruction without restoring registers.
     B    PromptAgain  @invalid entry try again

USFReturn:
     LDR  lr, =return2  @Restore return address
     LDR  lr, [lr]
     LDMFD sp!,  {r0-r4, r6-r12}   @Restore the registers
     BX   lr            @Return
 
@ End function UserSelectFuel
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Works to calculate gallons based on cents entered. 
@ Function to calculate gallons (tenths) requested.
@ Inputs:
@   r6 is dollar amount in cents requested.
@   r5 is Grade to dispense.
@ Output:
@   r2 - Number of gallons to dispense to meet request.
@ Registers used:
@   r0 - cost of one gallon of selected grade.
@   r1 - How much money remains 
@ 
CalculateGallons:
     STMFD sp!, {r0, r1, r3-r12} @ Save the registers 

     LDR  r1, =return2  @Save return address 
     STR  lr, [r1]

@ Use r0 for price of grade to dispense.
     CMP   r5, #'U'
     MOVEQ r0, #200
     CMP   r5, #'M'
     MOVEQ r0, #300
     CMP   r5, #'P'
     MOVEQ r0, #400

@ Start the loop to calculate amount to dispense.

     MOV   r2, #0 @zero out calculated gallons. 
     MOV   r1, r6 @copy money to working register
@ if there is enough money remaining dispense a whole gallon.
WholeGallon:
     CMP   r1, r0 @ Is there enough money to dispense whole gallon? 
     SUBGE r1, r1, r0 @sub whole gallon worth
     ADDGE r2, r2, #10 @ 10 tenths is one gallon.
     BGE   WholeGallon 
@when we get here start taking tenths off. 
@ Divide r0 by 10. 

     STMFD sp!, {r1} @save r1 so I do not have to rewrite this code. 
@copied code to divide by 10
 add 	r1,r0,r0,lsl #1 @ 3* r0 in r1
 add 	r0,r0,r1,lsl #2 @ 13* r0 in r0
 add 	r0,r0,r1,lsl #6 @ 205*r0 in r0
 add 	r0,r0,r1,lsl #10 @ 3,277* r0 in r0
 mov	r0,r0,lsr #15    @divide by 32,768
@end of copied code.
     LDMFD sp!, {r1} @restore r1

TenthsGallon:
     CMP   r1, r0 @ Is there enough to dispense 1 tenth. 
     SUBGE r1, r1, r0 @ Sub tenth gallon worth
     ADDGE r2, r2, #1 @ tenth added to total
     BGE   TenthsGallon

CGReturn:
     LDR  lr, =return2  @Restore return address
     LDR  lr, [lr]
     LDMFD sp!, {r0, r1, r3-r12}  @Restore the registers
     BX   lr            @Return
@ End function CaclulateGallons
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@----- Data Section ------
.data
.balign 4

return2: .word @used for return address from sub. 
.balign 4

strWelcome:
.asciz "Welcome to Mr. Preston's Gasoline Pump.\n\n"

.balign 4
strInstruction:
.asciz "Enter P to start pumping gasoline \n   or S to enter dollar amount you want to dispense.\n"

.balign 4
strInventory:
.asciz "Current inventory of gasoline (in tenths of gallons)\nper fuel grade are as follows: \n"

.balign 4
strUnleaded:
.asciz "Unleaded  %d \n"

.balign 4
strMidGrade:
.asciz "Mid-Grade %d \n"

.balign 4
strPremium:
.asciz "Premium   %d \n"

.balign 4
strGradePrompt:
.asciz "What grade do you want dispensed? (U, M, P) or Q to Quit. \n"

.balign 4
strEnterAmount:
.asciz "Enter amount to dispense in Cents. \n"

.balign 4
strEnterYToStart:
.asciz "Enter Y to start dispense of gasoline. \n"

.balign 4
strGasDispensedSoFar:
.asciz "Quanity of gasoline dispense so far is: %d \n"

.balign 4
strValueDispensedSoFar:
.asciz "$Amount of gasoline dispense so far is: %d \n"

.balign 4
strWantToContinue:
.asciz "Enter Y to continue pumping or N to stop. \n"

.balign 4
strGasDispensed:
.asciz "Quanity of gasoline dispense is: %d \n"

.balign 4
strValueDispensed:
.asciz " Amount of gasoline dispense is: \n"

.balign 4
strConfirmAmount:
.asciz "Is this the amount (in Cents) you want dispense: ?\n"

.balign 4
strOKWithAmount:
.asciz "Enter Y to continue on N to enter another amount. \n"

.balign 4
strCannotMeetRequest:
.asciz "Not enough gasoline in that grade to meet the request. \n"

.balign 4
strNoneLeftInGrade:
.asciz "No gasoline remaining in selected Grade. \n\n"

.balign 4
strShuttingDown:
.asciz "No gasoline remaining. Shutting down pump. \n\n"

.balign 4
strTotalSales:
.asciz "Total sales (in Cents) is: \n\n"

.balign 4
chrUserInput:
.asciz "%s"

.balign 4
intRequestAmount:
.asciz "%d"

@ end of file. 
