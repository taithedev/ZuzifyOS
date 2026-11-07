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

.PHONY: all kernel iso run test test-interactive clean
all: iso
kernel: build/kernel.elf

build/obj/%.o: kernel/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel.elf: $(OBJECTS) kernel/linker.ld
	@mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

iso: build/ZuzifyOS.iso

build/ZuzifyOS.iso: build/kernel.elf limine.conf scripts/build-iso.sh
	bash scripts/build-iso.sh

run: iso
	qemu-system-x86_64 -m 512M -machine q35 -cdrom build/ZuzifyOS.iso -boot d -serial stdio

test: iso
	bash scripts/smoke-test.sh

test-interactive: iso
	bash scripts/terminal-test.sh

clean:
	rm -rf build .iso-work
