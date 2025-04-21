@ Filename: Sieve2.s
@ Author: Kevin Preston
@ Purpose: 
@ ARM Assembly to implement the Sieve of Erathothenes to find and print
@ the prime numbers from 1 to some upper value. 
@ The algothrim removes all the composite numbers leaving only the 
@ prime numbers in the final list. 
@ 
@ 1. Create a list of consecutive intergers from 2 to MAX.
@ 2. Initially, let p equal 2.
@ 3. Emumerate the multiples of p by counting to MAX from 2p in increments
@    of p, and mark them in the list. 
@ 4. Find the first number greater than p in the list that is not marked.
@    If there was no such number stop. Otherwise let p now equal the 
@    new number and repeat from step 3. 
@ 5. When the algothim terminates the numbers remaining not marked in the
@    list are all the primes below MAX. 
@ 
@  as -o sieve2.o sieve2.s 
@  gcc -o sieve2 sieve2.o
@  ./sieve2 ;echo $?

.equ  MAX, 4000  @Used for literal addressing so watch the limit. 

.global main

main: 

@ Use the C library to print the welcome string. 
    LDR  r0, =strWelcome @ Put address of string in r0
    BL   printf   @Make the call to printf

@ Initialize the array with the integers 2 to MAX. 

     LDR r5, =intprimes
     ADD r5, r5, #8    @skip over 0 and 1 locations. 
     MOV r4, #2

loop:
     CMP r4, #MAX
     CMP r4, #0x9c40
     BEQ endloop
     STR r4, [r5, #4]!
     ADD r4, r4, #1    @update counter     
     B   loop
endloop:


@ The intprimes array should now have sequential numbers stored in each location.
@ r5 has the address of the last element in the array. Save it in r10 to check 
@ for end of array in the loops below. 
@@@@ Verified that the above code works to setup the array. @@@@@

     MOV  r10, r5

@ Find the first non-zero array element

     LDR  r5, =intprimes
     MOV  r1, #0   @r1 is used to zero out locations in the array. 
     MOV  r4, #0   @start at first element.

NextElement:
     LDR  r3, [r5, #4]!
     ADD  r4, r4, #1   @Keep track of which element I'm looking at
     CMP  r4, #MAX     @If this is the end of the array printthelist. 
     BEQ  printthelist
     CMP  r5, r10      @Another check for the end of the array. 
     BGE  printthelist @Exceeded the arrary boundry printthelist
     CMP  r3, #0
     BEQ  NextElement  @Array element is Zero get the next one. 
@ r3 contains the next non-zero element and r5 contains the address. 

@Zero out all the mulitples of the currrent location until we hit the end of the array.    
@ Do not change r3, r4 or r5 in this next loop. 
     
     MOV  r6, r5  @Use r6 to loop through rest of array
     MOV  r7, r4  @Use r7 to keep track of which element I'm looking at.

BlankNonPrime:
     ADD  r7, r7, #1            @update inner loop subscript
     STR  r1, [r6, r3, LSL #2]! @Zero out the multiples of current number r3.
     CMP  r7, #MAX              @check to make sure counter does not exceed upper bound
     BEQ  NextElement
     CMP  r6, r10       @ Check address against end of array address. 
     BGE  NextElement   @ Went past end of array. See if more #'s left. 
     B    BlankNonPrime

printthelist:
@ Print all the non-zero array elements. This is the final prime list. 

@@@@ Verified that this works. @@@@
@ Print the final prime list of numbers. 
@   Print header for list 
@   Loop through the array 
@   If the number is not zero it is a prime so print it. 
@  
    LDR  r0, =strPrimeList @ Put address of string in r0
    BL   printf   @Make the call to printf

    LDR  r5, =intprimes   @ Array start address
    MOV  r4, #0           @ Init the loop counter. 

printloop:
     CMP  r4, #MAX        @ Array size is MAX. 
     BEQ  endprintloop
     LDR  r0, =strOutputint
     LDR  r1, [r5, #4]!   @ Get the arrary value and update index. 
     CMP  r1, #0
     ADD  r4, r4, #1      @update counter
     BEQ  printloop       @If the array contents are 0 skip print. 
     BL   printf
     B    printloop
endprintloop:
 
@ Use the C library to print goodbye text. 
    LDR  r0, =strGoodbye @ Put address of string in r0
    BL   printf   @Make the call to printf

@ Force the exit of this program and return command to OS

    MOV  r7, #0X01
    SVC  0

@ Declare the stings

.data
.balign 4   @ Force a word boundry.
strWelcome: .asciz "You are running the Sieve of Erathothenes.\n"

.balign 4   @ Force a word boundry
strPrimeList: .asciz "This is the start of the prime list.\n\n" 

.balign 4
strOutputint: .asciz " %d "

.balign 4   @ Force a word boundry
strGoodbye: .asciz "\n\n  This is the end of the program.\n\n" 

.balign 4 @ Force a word boundry
intprimes: .skip 2 *MAX * 4 @Set to 2 * MAX * 32 bits This is way too big but it works. 

.global printf

@ end of file.
