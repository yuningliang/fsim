$TOOLROOT/usr/mips-linux/bin/mipscc -O0 -L$TOOLROOT/usr/mips-linux/lib -U__i386__ -Wl,-Map -Wl, hello.map -Wl,-T -Wl, common.ld -o hello hello.c -keep -show

$TOOLROOT/usr/mips-linux/bin/mipsel-elf-linux-objdump -d -s -M,gpr-names=numeric hello > hello.dump.txt
