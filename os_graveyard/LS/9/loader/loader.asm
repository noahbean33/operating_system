[BITS 16]
[ORG 0x7e00]

start:
    mov [DriveId],dl

    mov eax,0x80000000
    cpuid
    cmp eax,0x80000001
    jb NotSupport

    mov eax,0x80000001
    cpuid
    test edx,(1<<29)
    jz NotSupport
    test edx,(1<<26)
    jz NotSupport

    mov ax,0x2000
    mov es,ax

GetMemInfoStart:
    mov eax,0xe820
    mov edx,0x534d4150
    mov ecx,20
    mov dword[es:0],0

    mov edi,8
    xor ebx,ebx
    int 0x15
    jc NotSupport

GetMemInfo:
    cmp dword[es:di+16],1
    jne Cont
    cmp dword[es:di+4],0
    jne Cont
    mov eax,[es:di]
    cmp eax,0x30000000
    ja Cont
    cmp dword[es:di+12],0
    jne Find
    add eax,[es:di+8]
    cmp eax,0x30000000 + 100*1024*1024
    jb Cont
    
Find:
    mov byte[LoadImage],1

Cont:
    add edi,20
    inc dword[es:0]
    test ebx,ebx
    jz GetMemDone

    mov eax,0xe820
    mov edx,0x534d4150
    mov ecx,20
    int 0x15
    jnc GetMemInfo

GetMemDone:
    cmp byte[LoadImage],1
    jne ReadError

TestA20:
    mov ax,0xffff
    mov es,ax

    mov word[0x7c00],0xa200
    cmp word[es:0x7c10],0xa200
    jne SetA20LineDone
    mov word[0x7c00],0xb200
    cmp word[es:0x7c10],0xb200
    je End
    
SetA20LineDone:
    xor ax,ax
    mov es,ax

SetVideoMode:
    mov ax,3
    int 0x10

    cli
    lgdt [Gdt32Ptr]

    mov eax,cr0
    or eax,1
    mov cr0,eax

LoadFS:
    mov ax,0x10
    mov fs,ax

    mov eax,cr0
    and al,0xfe
    mov cr0,eax

BigRealMode:
    sti
    mov cx,203*16*63/100
    xor ebx,ebx
    mov edi,0x30000000
    xor ax,ax
    mov fs,ax

ReadFAT:
    push ecx
    push ebx
    push edi
    push fs
    
    mov ax,100
    call ReadSectors
    test al,al
    jnz  ReadError

    pop fs
    pop edi
    pop ebx

    mov cx,512*100/4
    mov esi,0x60000
    
CopyData:
    mov eax,[fs:esi]
    mov [fs:edi],eax

    add esi,4
    add edi,4
    loop CopyData

    pop ecx

    add ebx,100
    loop ReadFAT

ReadRemainingSectors:
    push edi
    push fs

    mov ax,(203*16*63) % 100
    call ReadSectors
    test al,al
    jnz  ReadError

    pop fs
    pop edi
    
    mov cx,(((203*16*63) % 100) * 512)/4
    mov esi,0x60000

CopyRemainingData: 
    mov eax,[fs:esi]
    mov [fs:edi],eax

    add esi,4
    add edi,4
    loop CopyRemainingData


    cli
    lidt [Idt32Ptr]

    mov eax,cr0
    or eax,1
    mov cr0,eax

    jmp 08:PMEntry

ReadSectors:
    mov si,ReadPacket
    mov word[si],0x10
    mov word[si+2],ax
    mov word[si+4],0
    mov word[si+6],0x6000
    mov dword[si+8],ebx
    mov dword[si+0xc],0
    mov dl,[DriveId]
    mov ah,0x42
    int 0x13
    
    setc al
    ret

ReadError:
NotSupport:
    mov ah,0x13
    mov al,1
    mov bx,0xa
    xor dx,dx
    mov bp,Message
    mov cx,MessageLen 
    int 0x10

End:
    hlt
    jmp End

[BITS 32]
PMEntry:
    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,0x7c00

    cld
    mov edi,0x70000
    xor eax,eax
    mov ecx,0x10000/4
    rep stosd
    
    mov dword[0x70000],0x71007
    mov dword[0x71000],10000111b

    mov eax,(0xffff800000000000>>39)
    and eax,0x1ff
    mov dword[0x70000+eax*8],0x72003
    mov dword[0x72000],10000011b

    lgdt [Gdt64Ptr]

    mov eax,cr4
    or eax,(1 << 5)
    mov cr4,eax

    mov eax,0x70000
    mov cr3,eax

    mov ecx,0xc0000080
    rdmsr
    or eax,(1 << 8)
    wrmsr

    mov eax,cr0
    or eax,(1<<31)
    mov cr0,eax

    jmp 08:LMEntry

PEnd:
    hlt
    jmp PEnd

[BITS 64]
LMEntry:
    mov rsp,0x7c00

    cld
    mov rdi,0x100000
    mov rsi,CModule
    mov rcx,512*15/8
    rep movsq

    mov rax,0xffff800000100000
    jmp rax

LEnd:
    hlt
    jmp LEnd

Message:    db "We have an error in boot process"
MessageLen: equ $-Message

ReadPacket: times 16 db 0
DriveId: db 0
LoadImage: db 0

Gdt32:
    dq 0
Code32:
    dw 0xffff
    dw 0
    db 0
    db 0x9a
    db 0xcf
    db 0
Data32:
    dw 0xffff
    dw 0
    db 0
    db 0x92
    db 0xcf
    db 0
    
Gdt32Len: equ $-Gdt32

Gdt32Ptr: dw Gdt32Len-1
          dd Gdt32

Idt32Ptr: dw 0
          dd 0

Gdt64:
    dq 0
    dq 0x0020980000000000

Gdt64Len: equ $-Gdt64

Gdt64Ptr: dw Gdt64Len-1
          dd Gdt64

CModule:
    
