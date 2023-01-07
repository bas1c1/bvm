# bvm
This is very simple virtual machine written by C

Example of code:

```
0a01 0fee 1bee 1ca0 0e
```

The same in assembly:

```
lbl 0xee
push 1
call a0 //call putc
pushlbl lbl
jmp
```
