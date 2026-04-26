nasm -f bin -o loader.bin loader.asm
nasm -f elf64 -o entry.o entry.asm
nasm -f elf64 -o liba.o lib.asm
gcc -std=c99 -mcmodel=large -ffreestanding -fno-stack-protector -mno-red-zone -c main.c 
gcc -std=c99 -mcmodel=large -ffreestanding -fno-stack-protector -mno-red-zone -c print.c 
gcc -std=c99 -mcmodel=large -ffreestanding -fno-stack-protector -mno-red-zone -c debug.c 
gcc -std=c99 -mcmodel=large -ffreestanding -fno-stack-protector -mno-red-zone -c file.c 
ld -nostdlib -T link.lds -o entry entry.o main.o liba.o print.o debug.o file.o
objcopy -O binary entry entry.bin
dd if=entry.bin >> loader.bin
dd if=loader.bin of=../os.img bs=512 count=15 seek=1 conv=notrunc