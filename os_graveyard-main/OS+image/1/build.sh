nasm -f bin -o boot.bin boot.asm
dd if=boot.bin of=os.img bs=512 count=1 conv=notrunc
