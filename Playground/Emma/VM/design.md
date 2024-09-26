Instructions:
pushi ## - Pushes the input int to the stack
pushd ## - Pushes the input double to the stack
pushf ## - Pushes the input float to the stack 

addi - Adds the two elements at the top of the stack, treating them as ints
addf - Same as above, for two floats
addd - Same as above, for two doubles

Same add but for sub

pop <dest string> - Pop the top of the stack into memory keyed by the input string
pushh <src string> - Push from the given key on the heap to the stack

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
        - Just a collection of bits. Interpreted when read.

    - Can an object be a pointer? 
        - A pointer is just the name of another object

    - How are functions defined?
        - When a function is defined, it is stored in dict as (name, offset into file of start)

What can I push onto the stack?
    - More variable size byte arrays?