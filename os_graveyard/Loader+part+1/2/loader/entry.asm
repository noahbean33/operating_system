section .text
extern EMain
global start

start:
    mov rsp,0xffff800000200000
    call EMain

End:
    hlt
    jmp End


