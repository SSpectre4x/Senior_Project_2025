/* ARM Lab 2 */
/* Kevin Preston */

/* Soft Drink Machine */
@
@ as -o ARM_Lab2_CokeMachine.o ARM_Lab2_CokeMachine.s
@ gcc -o ARM_Lab2_CokeMachine ARM_Lab2_CokeMachine.o
@ ./ARM_Lab2_CokeMachine ;echo $?
@ gdb --args ./ARM_Lab2_CokeMachine
@
/* ----- Data section ------ */
.data
.balign 4

strWelcome:
.asciz "Welcome to Mr. Preston's soft drink vending machine. \n"

strDrinkCost:
.asciz "The cost of a soft drink is 55 cents (US). \n"

strDrinkChoices:
.asciz "Drink choices are Coke(c), Sprite(s), Dr Pepper(p) or Mellow Yellow(m). \n"

strInputChoices:
.asciz "Enter coin (n, d, q, f, b-1$ bill) or r to return change and quit program.\n"

strBalance:
.asciz "Total balance is %d cents \n"

strReturnBalance:
.asciz "Balance returned in change slot is %d cents \n"

strInvalidCoin:
.asciz "Invalid coin. Please try another input.\n"

strMakeDrinkChoices:
.asciz "Make drink choice of Coke(c), Sprite(s), Dr Pepper(p) or Mellow Yellow(m). \n"

strDrinkverifyCoke:
.asciz "Are you sure you want a Coke(c)? \n  Enter y for Yes or n for No to make another selection. \n"

strDrinkverifySprite:
.asciz "Are you sure you want a Sprite(s)?\n  Enter y for Yes or n for No to make another selection. \n"

strDrinkverifyDrPepper:
.asciz "Are you sure you want a Dr Pepper(p)?\n  Enter y for Yes or n for No to make another selection. \n"

strDrinkverifyMellowYellow:
.asciz "Are you sure you want a Mellow Yellow(m)?\n  Enter y for Yes or n for No to make another selection. \n"

strDrinkOutputCoke:
.asciz "A Coke(c) has been dispensed and with change of %d cents. \n"

strDrinkOutputSprite:
.asciz "A Sprite(s) has been dispensed and with change of %d cents. \n"

strDrinkOutputDrPepper:
.asciz "A Dr Pepper(p) has been dispensed and with change of %d cents. \n"

strDrinkOutputMellowYellow:
.asciz "A Mellow Yellow(m) has been dispensed and with change of %d cents. \n"

strEndingMessage:
.asciz "Have a nice day! \n"

strInventoryCoke:
.asciz "The Coke(c) inventory is %d. \n"

strInventorySprite:
.asciz "The Sprite(s) inventory is %d. \n"

strInventoryDrPepper:
.asciz "The Dr Pepper(p) inventory is %d. \n"

strInventoryMellowYellow:
.asciz "The Mellow Yellow(m) inventory is %d. \n"

strNoCoke:
.asciz "No Coke(c) remaining make another selection. \n"

strNoSprite:
.asciz "No Sprite(s) remaining make another selection. \n"

strNoDrPepper:
.asciz "No Dr Pepper(p) remaining make another selection. \n"

strNoMellowYellow:
.asciz "No Mellow Yellow(m) remaining make another selection. \n"

strNoDrinks:
.asciz "No Drinks remain. Returning change of %d cents. \n"

strCoinInput:
.asciz "The input coin was : %x \n"

/* Format pattern for scanf call */
.balign 4
inputPattern: .asciz "%s"

/* Address for the charater read from scanf */
.balign 4
inputChar: .word 0

.balign 4
return: .word 0 

/* ------ Code Section ----*/
/* Since we are using printf and scanf do you use r0 though r4 for this program */

.text
.balign 4
.global main 

main: /* Program entry point */

/* Before making a function call (bl) save the current return address. */
   ldr r1, addr_of_return 
   str lr, [r1]              /* Store the return address at addr_of_return */

/* zero out the sums and the counter */
/* Using r0 and r1 for the calls to printf and scanf */

   mov r5,  #0 /* Deposit_Balance = 0 */

/**********************************************/
/* Set up the drink inventory. Keep it small. */
/**********************************************/
   mov r9,  #1 /* Coke Inventory */
   mov r10, #1 /* Sprite Inventory */
   mov r11, #1 /* Dr Pepper Inventory */
   mov r8,  #1 /* Mellow Yellow Inventory */

/*********************************/
/* Print the welcome information */
/*********************************/

   ldr r0, addr_strWelcome   /* Put the address of the into the first parameter */
   bl printf                 /* Call the C print command */

   ldr r0, addr_strDrinkCost /* Put the address of my string into the first parameter */
   bl printf                 /* Call the C print command */
 
   ldr r0, addr_strDrinkChoices /* Put the address of my string into the first parameter */
   bl printf                    /* Call the C print command */

/****************************/
/*    Get the coin input    */
/****************************/

coin_input:
   ldr r0, addr_strInputChoices /* Put the address of my string into the first parameter */
   bl printf                    /* Call the C print command */

   ldr r0, addr_inputPattern /* Setup to read in one character */
   sub sp, sp, #4
   mov r1, sp
   bl scanf
   ldr r3, [sp, #0]              /* This is where the character will be stored */
   add sp, sp, #4

check_input: 
/* r3 contains the input character */
/* r5 is the deposit balance */

check_n:       /*Nickle input */
   mov r0, #'n'
   cmp r3, r0
   bne check_d
   add r5, r5, #5
   b show_balance

check_d:        /*Dime input */
   mov r0, #'d'
   cmp r3, r0
   bne check_q
   add r5, r5, #10
   b show_balance

check_q:       /*Quarter input */
   mov r0, #'q'
   cmp r3, r0
   bne check_f
   add r5, r5, #25
   b show_balance

check_f:       /*Fifty cent input */
   mov r0, #'f'
   cmp r3, r0
   bne check_b
   add r5, r5, #50
   b show_balance

check_b:        /* 1$ bill input */
   mov r0, #'b'  
   cmp r3, r0
   bne check_r
   add r5, r5, #100
   b show_balance

check_r:       /* Return change and exit */
   mov r0, #'r'
   cmp r3, r0
   bne check_i

   ldr r0, addr_strReturnBalance /* Put the address of my string into the first parameter */
   mov r1, r5              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   b exit

check_i:        /* Give the inventory.  */
   mov r0, #'i'  
   cmp r3, r0
   bne check_junk
   b show_inventory


check_junk:   /* invalid coin input */
   ldr r0, addr_strInvalidCoin  /* Put the address of my string into the first parameter */
   bl printf               /* Call the C print command */
   b coin_input

show_balance:
   ldr r0, addr_strBalance /* Put the address of my string into the first parameter */
   mov r1, r5              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   mov r0, #55
   cmp r5, r0
   blt coin_input  /* Not enough $ has been entered. Get more */

/*************************************************************/
/* Enough money has been entered.                            */
/* Check the total inventory and if >1 get the drink choice. */
/*************************************************************/  
drink_choice:
   add r0, r9, r10
   add r0, r0, r11
   adds r0, r0, r8 /* No remain drinks */ 
   beq NoDrinks
   
   ldr r0, addr_strMakeDrinkChoices /* Put the address of my string into the first parameter */
   bl printf     /* Call the C print command */

   ldr r0, addr_inputPattern /* Setup to read in one character */
   sub sp, sp, #4
   mov r1, sp
   bl scanf
   ldr r3, [sp, #0]              /* This is where the character will be stored */
   add sp, sp, #4

/****************************************/
/* Drink selection has been entered.    */
/* Validate the selection and dispense. */
/****************************************/

/* remember r5 is balance         */
/*          r3 is the input drink */

check_drink:  /*Coke first */
   mov r0, #'c'
   cmp r3, r0
   bne check_sprite /* Not a coke try next. */

   mov r0, #0
   cmp r0, r9
   beq NoCoke

   ldr r0, addr_strDrinkverifyCoke /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */

   ldr r0, addr_inputPattern /* Setup to read in one character */
   sub sp, sp, #4
   mov r1, sp
   bl scanf
   ldr r3, [sp, #0]              /* This is where the character will be stored */
   add sp, sp, #4

   mov r0, #'y'
   cmp r3, r0
   bne drink_choice /* Did not want coke try again. */
   /* Calculate remaining balance and dispense drink. */
   mov r0, #55
   sub r5, r0
   ldr r0, addr_strDrinkOutputCoke /* Put the address of my string into the first parameter */
   mov r1, r5              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   mov r5,  #0 /* Deposit_Balance = 0 */
   sub r9, #1  /* Adjust inventory balance. */
   b coin_input

check_sprite:    /*  Sprite */
   mov r0, #'s'
   cmp r3, r0
   bne check_DrPepper /* Not a Sprite try next one. */
 
   mov r0, #0
   cmp r0, r10
   beq NoSprite

   ldr r0, addr_strDrinkverifySprite /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */

   ldr r0, addr_inputPattern /* Setup to read in one character */
   sub sp, sp, #4
   mov r1, sp
   bl scanf
   ldr r3, [sp, #0]              /* This is where the character will be stored */
   add sp, sp, #4

   mov r0, #'y' 
   cmp r3, r0
   bne drink_choice  /* Did not want a Sprite try again. */
   /* Calculate remaining balance and dispense drink. */
   mov r0, #55
   sub r5, r0
   ldr r0, addr_strDrinkOutputSprite /* Put the address of my string into the first parameter */
   mov r1, r5              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   mov r5,  #0 /* Deposit_Balance = 0 */
   sub r10, #1  /* Adjust inventory balance. */
   b coin_input

check_DrPepper:    /* Dr Pepper */
   mov r0, #'p'
   cmp r3, r0
   bne check_MellowYellow /* Not a Dr Pepper try the next one. */

   mov r0, #0
   cmp r0, r11
   beq NoDrPepper
 
   ldr r0, addr_strDrinkverifyDrPepper /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */

   ldr r0, addr_inputPattern /* Setup to read in one character */
   sub sp, sp, #4
   mov r1, sp
   bl scanf
   ldr r3, [sp, #0]              /* This is where the character will be stored */
   add sp, sp, #4

   mov r0, #'y'
   cmp r3, r0
   bne drink_choice /* Did not want a Dr Pepper try again. */
   /* Calculate remaining balance and output drink. */
   mov r0, #55
   sub r5, r0
   ldr r0, addr_strDrinkOutputDrPepper /* Put the address of my string into the first parameter */
   mov r1, r5              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   mov r5,  #0 /* Deposit_Balance = 0 */
   sub r11, #1  /* Adjust inventory balance. */
   b coin_input


check_MellowYellow:
   mov r0, #'m'
   cmp r3, r0
   bne drink_choice  /*invalid entry rerun them again */

   mov r0, #0
   cmp r0, r8
   beq NoMellowYellow

   ldr r0, addr_strDrinkverifyMellowYellow /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   ldr r0, addr_inputPattern /* Setup to read in one character */
   sub sp, sp, #4
   mov r1, sp
   bl scanf
   ldr r3, [sp, #0]     /* This is where the character will be stored */
   add sp, sp, #4

   mov r0, #'y'
   cmp r3, r0
   bne drink_choice  /* Did not want a Mellow Yellow */
   /* Calculate remaining balance and output drink. */
   mov r0, #55
   sub r5, r0
   ldr r0, addr_strDrinkOutputMellowYellow /* Put the address of my string into the first parameter */
   mov r1, r5              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   mov r5,  #0 /* Deposit_Balance = 0 */
   sub r8, #1  /* Adjust inventory balance. */
   b coin_input

show_inventory:
/****************************/
/* Show the drink inventory */
/****************************/

   ldr r0, addr_strInventoryCoke  /* Put the address of my string into the first parameter */
   mov r1, r9              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */

   ldr r0, addr_strInventorySprite  /* Put the address of my string into the first parameter */
   mov r1, r10              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */

   ldr r0, addr_strInventoryDrPepper  /* Put the address of my string into the first parameter */
   mov r1, r11             /* The counter is the second parameter. */
   bl printf               /* Call the C print command */

   ldr r0, addr_strInventoryMellowYellow  /* Put the address of my string into the first parameter */
   mov r1, r8             /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   b coin_input
/* end of show inventory */

/*******************************/
/* Start of NO drink messages. */
/*******************************/

NoCoke:
   ldr r0, addr_strNoCoke  /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   b drink_choice 

NoSprite:
   ldr r0, addr_strNoSprite  /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   b drink_choice

NoDrPepper:
   ldr r0, addr_strNoDrPepper  /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   b drink_choice

NoMellowYellow:
   ldr r0, addr_strNoMellowYellow  /* Put the address of my string into the first parameter */
   mov r1, r6              /* The counter is the second parameter. */
   bl printf               /* Call the C print command */
   b drink_choice

NoDrinks:
   ldr r0, addr_strNoDrinks
   mov r1, r5
   bl printf

/* End of the program */
exit:
   ldr r0, addr_strEndingMessage  /* Put the address of my string into the first parameter */
   bl printf               /* Call the C print command */

   ldr lr, addr_of_return
   ldr lr, [lr]           /* Restore the return address. */ 
   bx  lr /*Return from main */


/* Labels needed to access data */

addr_of_return      : .word return

addr_strWelcome                : .word strWelcome
addr_strDrinkCost              : .word strDrinkCost
addr_strDrinkChoices           : .word strDrinkChoices
addr_strInputChoices           : .word strInputChoices
addr_strInvalidCoin            : .word strInvalidCoin
addr_strBalance                : .word strBalance
addr_strReturnBalance          : .word strReturnBalance
addr_strMakeDrinkChoices       : .word strMakeDrinkChoices
addr_strDrinkverifyCoke        : .word strDrinkverifyCoke
addr_strDrinkverifySprite      : .word strDrinkverifySprite
addr_strDrinkverifyDrPepper    : .word strDrinkverifyDrPepper
addr_strDrinkverifyMellowYellow: .word strDrinkverifyMellowYellow
addr_strDrinkOutputCoke        : .word strDrinkOutputCoke
addr_strDrinkOutputSprite      : .word strDrinkOutputSprite 
addr_strDrinkOutputDrPepper    : .word strDrinkOutputDrPepper
addr_strDrinkOutputMellowYellow: .word strDrinkOutputMellowYellow
addr_strEndingMessage          : .word strEndingMessage
addr_strInventoryCoke          : .word strInventoryCoke
addr_strInventorySprite        : .word strInventorySprite
addr_strInventoryDrPepper      : .word strInventoryDrPepper
addr_strInventoryMellowYellow  : .word strInventoryMellowYellow
addr_strNoCoke                 : .word strNoCoke
addr_strNoSprite               : .word strNoSprite
addr_strNoDrPepper             : .word strNoDrPepper
addr_strNoMellowYellow         : .word strNoMellowYellow
addr_strNoDrinks               : .word strNoDrinks
addr_strCoinInput              : .word strCoinInput

addr_inputPattern              : .word inputPattern
addr_inputChar                 : .word inputChar

/* External */

.global printf
.global scanf

