SHELL := /bin/bash
.SUFFIXES:
.DELETE_ON_ERROR:

CC := clang
LD := ld.lld
CFLAGS := --target=x86_64-unknown-none-elf -std=c11 -O2 -g -Wall -Wextra -Werror \
  -ffreestanding -fno-builtin -fno-stack-protector -fno-stack-check \
  -fno-pic -fno-pie -fno-asynchronous-unwind-tables -fno-unwind-tables \
  -fno-omit-frame-pointer -fno-exceptions -fno-common -m64 -march=x86-64 \
  -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel \
  -Ikernel/include
LDFLAGS := -nostdlib -static -z max-page-size=0x1000 --gc-sections -T kernel/linker.ld
SOURCES := $(shell find kernel/src -type f -name '*.c' | LC_ALL=C sort)
OBJECTS := $(patsubst kernel/src/%.c,build/obj/%.o,$(SOURCES))

CFLAGS32 := --target=i686-unknown-none-elf -std=c11 -O2 -g -Wall -Wextra -Werror \
  -ffreestanding -fno-builtin -fno-stack-protector -fno-stack-check \
  -fno-pic -fno-pie -fno-asynchronous-unwind-tables -fno-unwind-tables \
  -fno-omit-frame-pointer -fno-exceptions -fno-common -m32 -march=i686 \
  -mno-mmx -mno-sse -mno-sse2
LDFLAGS32 := -m elf_i386 -nostdlib -static -z max-page-size=0x1000 -T kernel32/linker.ld

.PHONY: all kernel kernel32 iso iso32 run run32 test test32 test-interactive clean
all: iso iso32
kernel: build/kernel.elf
kernel32: build/kernel32.elf

build/obj/%.o: kernel/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel.elf: $(OBJECTS) kernel/linker.ld
	@mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

build/obj32/main.o: kernel32/main.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS32) -c $< -o $@

build/obj32/entry.o: kernel32/entry.S
	@mkdir -p $(dir $@)
	$(CC) --target=i686-unknown-none-elf -m32 -c $< -o $@

build/kernel32.elf: build/obj32/main.o build/obj32/entry.o kernel32/linker.ld
	@mkdir -p build
	$(LD) $(LDFLAGS32) -o $@ build/obj32/entry.o build/obj32/main.o

iso: build/ZuzifyOS.iso
iso32: build/ZuzifyOS-x86.iso

build/ZuzifyOS.iso: build/kernel.elf limine.conf scripts/build-iso.sh
	bash scripts/build-iso.sh x64

build/ZuzifyOS-x86.iso: build/kernel32.elf limine32.conf scripts/build-iso.sh
	bash scripts/build-iso.sh x86

run: iso
	qemu-system-x86_64 -m 512M -machine q35 -cdrom build/ZuzifyOS.iso -boot d -serial stdio

run32: iso32
	qemu-system-i386 -m 256M -cpu pentium3 -cdrom build/ZuzifyOS-x86.iso -boot d -serial stdio

test: iso
	bash scripts/smoke-test.sh

test32: iso32
	bash scripts/smoke-test-32.sh

test-interactive: iso
	bash scripts/terminal-test.sh

clean:
	rm -rf build .iso-work
