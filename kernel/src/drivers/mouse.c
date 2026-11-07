#include "os.h"

static inline void outb(uint16_t port,uint8_t val){
    __asm__ volatile("outb %0,%1"::"a"(val),"Nd"(port));
}
static inline uint8_t inb(uint16_t port){
    uint8_t value;
    __asm__ volatile("inb %1,%0":"=a"(value):"Nd"(port));
    return value;
}
static void wait_write(void){
    for(unsigned int i=0;i<100000u;i++)if(!(inb(0x64)&2u))return;
}
static uint8_t packet[3];
static int packet_index=0;
void mouse_init(void){
    /* PS/2 mouse via 8042 controller. Works on common QEMU i8042 setups. */
    wait_write();outb(0x64,0xa8);
    wait_write();outb(0x64,0xd4);
    wait_write();outb(0x60,0xf4);
    serial_write("ZUZIFYOS_PS2_MOUSE_ATTEMPTED\n");
}
void mouse_feed(uint8_t data){
    if(packet_index==0){
        if(data==0xfau || data==0xfeu)return;
        if(!(data&0x08u))return;
    }
    packet[packet_index++]=data;
    if(packet_index<3)return;
    packet_index=0;
    if(packet[0]&0xc0u)return; /* Overflow flag. */
    int dx=(int)(int8_t)packet[1];
    int dy=(int)(int8_t)packet[2];
    bool left=(packet[0]&1u)!=0;
    ui_mouse_move(dx,dy,left);
}
