# bvm
This is very simple virtual machine written by C

Example of code:

```
0a01 0fee 1bee 1ca0 0e
```

The same in assembly:

```
push 1
lbl 0xee
pushlbl 0xee
call a0 //call putc
jmp
```
