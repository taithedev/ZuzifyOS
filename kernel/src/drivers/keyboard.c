#include "os.h"

static inline uint8_t inb(uint16_t port){
    uint8_t value;
    __asm__ volatile("inb %1, %0":"=a"(value):"Nd"(port));
    return value;
}
static const char standard[128]={
    [0x02]='1',[0x03]='2',[0x04]='3',[0x05]='4',[0x06]='5',[0x07]='6',
    [0x08]='7',[0x09]='8',[0x0a]='9',[0x0b]='0',[0x0c]='-',[0x0d]='=',
    [0x10]='q',[0x11]='w',[0x12]='e',[0x13]='r',[0x14]='t',[0x15]='y',
    [0x16]='u',[0x17]='i',[0x18]='o',[0x19]='p',[0x1a]='[',[0x1b]=']',
    [0x1e]='a',[0x1f]='s',[0x20]='d',[0x21]='f',[0x22]='g',[0x23]='h',
    [0x24]='j',[0x25]='k',[0x26]='l',[0x27]=';',[0x28]='\'',[0x29]='`',
    [0x2b]='\\',[0x2c]='z',[0x2d]='x',[0x2e]='c',[0x2f]='v',[0x30]='b',
    [0x31]='n',[0x32]='m',[0x33]=',',[0x34]='.',[0x35]='/',[0x39]=' '
};
static const char shifted[128]={
    [0x02]='!',[0x03]='@',[0x04]='#',[0x05]='$',[0x06]='%',[0x07]='^',
    [0x08]='&',[0x09]='*',[0x0a]='(',[0x0b]=')',[0x0c]='_',[0x0d]='+',
    [0x1a]='{',[0x1b]='}',[0x27]=':',[0x28]='"',[0x29]='~',
    [0x2b]='|',[0x33]='<',[0x34]='>',[0x35]='?'
};
static bool shift_down=false, caps_on=false, extended=false;
int kbd_poll(void){
    uint8_t status=inb(0x64);
    if(!(status&1u))return 0;
    uint8_t raw=inb(0x60);
    if(status&0x20u){mouse_feed(raw);return 0;}
    if(raw==0xe0){extended=true;return 0;}
    if(extended){extended=false;return 0;}
    bool released=(raw&0x80u)!=0;
    uint8_t code=(uint8_t)(raw&0x7fu);
    if(code==0x2a||code==0x36){shift_down=!released;return 0;}
    if(released)return 0;
    if(code==0x3a){caps_on=!caps_on;return 0;}
    if(code==0x01)return 27;
    if(code==0x1c)return '\n';
    if(code==0x0e)return '\b';
    if(code==0x0f)return '\t';
    if(code>=0x3b && code<=0x3e)return KEY_F1+(code-0x3b);
    char ch=standard[code];
    if(ch>='a' && ch<='z'){
        if(shift_down^caps_on)ch=(char)(ch-'a'+'A');
    } else if(shift_down && shifted[code])ch=shifted[code];
    return (unsigned char)ch;
}
