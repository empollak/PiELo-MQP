Instructions:
pushi ## - Pushes the input int to the stack
pushd ## - Pushes the input double to the stack
pushf ## - Pushes the input float to the stack 

push <src string> - Push the element keyed by the string to the stack
pop <dest string> - Pop the top of the stack into memory keyed by the input string

addi - Adds the two elements at the top of the stack, treating them as ints
addf - Same as above, for two floats
addd - Same as above, for two doubles

Same add but for sub


ret - Pops the top of the stack and goes there in the file to start running code
call <string> - Push the location of next line to stack. Then get the location in file from <string> in memory and jump there to start running code. 
func <string> - Defines a function start. Before code is run, all funcs are stored in the heap as (string, location).

print - print the top of the stack


Problems
- How does the VM get the code?
    - File pointer allows jumping around
    - Treat it as a list of tokens

- How do I have memory?
    - A dict. Every object is named and stored in a dict
        - How do I give names in assembly?
            - Just do. The VM treats the assembly as a list of tokens, whether bytecode or not.
            - This has absolutely no safety.

    - What type are values in the dict?
        - Just a collection of bits. 
            - Requires using the correct instructions for storing or reading!
            - This adds a whole lot of assembly instructions for all combos of types
        - Everything which is not immediately used for calculation goes in the dict
        - Garbage collection? Scoping?
            - Handled in the compiler or in the VM?
                - Could make the compiler compile local scope variables with a different name?
                - Anything that's local to a function can go on the stack

    - Can an object be a pointer? 
        - A pointer is just the name of another object

    - How are functions defined?
        - When a function is defined, it is stored in dict as (name, offset into file of start)
        - Don't need to store the end of a file as it's expected that there will be a return
        - Return will pop the top of the stack and treat that as where in the file to go
        - When calling a function, need to push next line address onto stack
        - This means that all functions must leave the stack how they found it
    
    What about conditionals and such?
        - Support jumping rather than calling
        - Allow you to compare to values on the stack and jump or not jump based on the result
        

What can I push onto the stack?
    - More variable size byte arrays?

Also supports comments starting with #