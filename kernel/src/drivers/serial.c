#include "os.h"

static inline void port_out(uint16_t port,uint8_t data){
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}
static inline uint8_t port_in(uint16_t port){
    uint8_t v;
    __asm__ volatile("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}
void serial_init(void){
    port_out(0x3f9,0x00);
    port_out(0x3fb,0x80);
    port_out(0x3f8,0x03);
    port_out(0x3f9,0x00);
    port_out(0x3fb,0x03);
    port_out(0x3fa,0xc7);
    port_out(0x3fc,0x0b);
}
void serial_putc(char c){
    for(unsigned int i=0;i<100000;i++){
        if(port_in(0x3fd)&0x20u)break;
    }
    port_out(0x3f8,(uint8_t)c);
}
void serial_write(const char *s){
    while(*s){
        if(*s=='\n')serial_putc('\r');
        serial_putc(*s++);
    }
}
