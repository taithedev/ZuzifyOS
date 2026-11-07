#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* BIOS-only 32-bit multiboot2 kernel, intentionally feature-limited. */
static volatile uint16_t *const video=(volatile uint16_t*)0xb8000;
static int col=0,row=0;
static uint8_t ink=0x1f;
static char username[20],secret[9],entry[40];
static unsigned int entry_len=0;
static unsigned int pin_digest=0;
static int stage=0;
static int wrong_attempts=0;

static inline void outb(uint16_t port,uint8_t value){
    __asm__ volatile("outb %0,%1"::"a"(value),"Nd"(port));
}
static inline uint8_t inb(uint16_t port){
    uint8_t value;
    __asm__ volatile("inb %1,%0":"=a"(value):"Nd"(port));
    return value;
}
static void serial_char(char c){
    for(int i=0;i<10000;i++)if(inb(0x3fd)&0x20)break;
    outb(0x3f8,(uint8_t)c);
}
static void serial(const char *s){
    while(*s)serial_char(*s++);
    serial_char('\r');serial_char('\n');
}
static void clear(void){
    for(int i=0;i<80*25;i++)video[i]=(uint16_t)(ink<<8)|' ';
    col=0;row=0;
}
static void put(char c){
    if(c=='\b'){if(col>0)col--;return;}
    if(c=='\n'){col=0;row++;}else{
        if(col>=80){col=0;row++;}
        if(row>=25){clear();row=0;}
        video[row*80+col]=(uint16_t)(ink<<8)|(uint8_t)c;
        col++;
    }
    if(row>=25){clear();row=0;}
}
static void print(const char *s){while(*s)put(*s++);}
static void println(const char *s){print(s);put('\n');}
static bool equals(const char *a,const char *b){
    while(*a && *a==*b){a++;b++;}
    return *a==*b;
}
static void copy(char *dst,const char *src,size_t cap){
    size_t i=0;while(src[i] && i+1<cap){dst[i]=src[i];i++;}
    dst[i]=0;
}
static unsigned int digest(const char *s){
    unsigned int h=2166136261u^0x5a92be41u;
    while(*s){h^=(uint8_t)*s++;h*=16777619u;}
    return h;
}
static void input_reset(void){
    for(size_t i=0;i<sizeof(entry);i++)entry[i]=0;
    entry_len=0;
}
static void prompt(void){
    if(stage==1)print("Username> ");
    else if(stage==2)print("New PIN> ");
    else if(stage==3)print("Confirm PIN> ");
    else if(stage==4)print("Unlock PIN> ");
    else print("ZuzifyOS32> ");
}
static void show_setup(void){
    clear();
    println("  +-------------------------------------------------------------+");
    println("  | ZUZIFY OS 32-BIT (LEGACY BIOS)  -  ALPHA 0.2                |");
    println("  +-------------------------------------------------------------+");
    println("");
    println("  YOUR OWN KERNEL. NOT LINUX OR WINDOWS.");
    println("  THIS 32-BIT EDITION USES A TEXT CONSOLE.");
    println("");
    println("  THIS DEMO STORES ACCOUNT/PIN ONLY IN MEMORY.");
    println("  ACCOUNT AND FILES RESET ON REBOOT. NOT SECURE AUTHENTICATION.");
    println("");
    if(stage==0)println("  Press ENTER to start first-boot setup.");
    if(stage==1)println("  Create username (2-19 letters or digits).");
    if(stage==2)println("  Choose a PIN (4-8 digits).");
    if(stage==3)println("  Confirm your PIN.");
    if(stage==4)println("  Enter your PIN to unlock.");
    println("");
    prompt();
}
static void desktop(void){
    clear();
    println("===============================================================");
    println(" ZUZIFY OS 32-BIT - PERSONAL DESKTOP / BIOS TEXT EDITION");
    println("===============================================================");
    print(" Account: ");println(username);
    println("");
    println(" Commands: help, about, uname, whoami, settings, echo, cls, lock");
    println(" No GUI, persistence, network, Windows EXE, or installed updates.");
    println("");
    prompt();
}
static void execute(const char *command){
    serial("ZUZIFYOS32_COMMAND_RECEIVED");
    if(equals(command,"help")){
        println("help about uname whoami settings cls clear lock echo <text>");
    }else if(equals(command,"about")){
        println("ZuzifyOS 0.2.0-alpha | IA-32 BIOS | Multiboot2 | native kernel");
    }else if(equals(command,"uname")){
        println("ZuzifyOS i686");
    }else if(equals(command,"whoami")){
        println(username);
    }else if(equals(command,"settings")){
        println("SETTINGS - VGA console, BIOS 32-bit, account in volatile RAM.");
        println("64-bit edition includes the graphical Settings application.");
    }else if(equals(command,"cls")||equals(command,"clear")){
        clear();
    }else if(equals(command,"lock")){
        stage=4;input_reset();show_setup();serial("ZUZIFYOS32_LOCKED");return;
    }else if(command[0]=='e'&&command[1]=='c'&&command[2]=='h'&&command[3]=='o'&&command[4]==' '){
        println(command+5);
    }else if(*command)println("Unknown command. Type help.");
    prompt();
}
static void submit(void){
    put('\n');
    if(stage==0){stage=1;input_reset();show_setup();return;}
    if(stage==1){
        if(entry_len<2||entry_len>19){println("Username must be 2-19 chars.");input_reset();prompt();return;}
        copy(username,entry,sizeof(username));stage=2;input_reset();show_setup();return;
    }
    if(stage==2){
        if(entry_len<4||entry_len>8){println("PIN must be 4-8 digits.");input_reset();prompt();return;}
        copy(secret,entry,sizeof(secret));stage=3;input_reset();show_setup();return;
    }
    if(stage==3){
        if(!equals(secret,entry)){
            println("PIN mismatch. Please choose another PIN.");
            stage=2;for(int i=0;i<9;i++)secret[i]=0;
            input_reset();show_setup();return;
        }
        pin_digest=digest(entry);
        for(int i=0;i<9;i++)secret[i]=0;
        stage=4;input_reset();
        serial("ZUZIFYOS32_ACCOUNT_CREATED");
        show_setup();return;
    }
    if(stage==4){
        unsigned int provided=digest(entry);
        input_reset();
        if(provided!=pin_digest){
            wrong_attempts++;println("Incorrect PIN.");prompt();
            serial("ZUZIFYOS32_LOGIN_REJECTED");return;
        }
        stage=5;wrong_attempts=0;
        serial("ZUZIFYOS32_LOGIN_OK");
        desktop();return;
    }
    if(stage==5){char copy_of_entry[40];copy(copy_of_entry,entry,sizeof(copy_of_entry));input_reset();execute(copy_of_entry);}
}
static const char keys[128]={
    [0x02]='1',[0x03]='2',[0x04]='3',[0x05]='4',[0x06]='5',
    [0x07]='6',[0x08]='7',[0x09]='8',[0x0a]='9',[0x0b]='0',
    [0x10]='q',[0x11]='w',[0x12]='e',[0x13]='r',[0x14]='t',[0x15]='y',
    [0x16]='u',[0x17]='i',[0x18]='o',[0x19]='p',
    [0x1e]='a',[0x1f]='s',[0x20]='d',[0x21]='f',[0x22]='g',
    [0x23]='h',[0x24]='j',[0x25]='k',[0x26]='l',
    [0x2c]='z',[0x2d]='x',[0x2e]='c',[0x2f]='v',[0x30]='b',
    [0x31]='n',[0x32]='m',[0x39]=' ',[0x0c]='-',[0x0d]='=',
    [0x33]=',',[0x34]='.',[0x35]='/'
};
static bool shift=false;
static void input_poll(void){
    uint8_t status=inb(0x64);
    if(!(status&1u))return;
    uint8_t sc=inb(0x60);
    if(status&0x20u)return;
    bool release=(sc&0x80u)!=0;
    sc&=0x7fu;
    if(sc==0x2a||sc==0x36){shift=!release;return;}
    if(release)return;
    if(sc==0x1c){submit();return;}
    if(sc==0x0e){if(entry_len){entry[--entry_len]=0;put('\b');put(' ');put('\b');}return;}
    int c=(unsigned char)keys[sc];
    if(c==0)return;
    if(shift && c>='a' && c<='z')c=c-'a'+'A';
    if(stage==2||stage==3||stage==4){
        if(c<'0'||c>'9'||entry_len>=8)return;
    }else if(stage==1){
        bool ok=(c>='a'&&c<='z')||(c>='A'&&c<='Z')||
            (c>='0'&&c<='9')||c=='-'||c=='_';
        if(!ok||entry_len>=19)return;
    }else if(stage==0)return;
    if(entry_len+1>=sizeof(entry))return;
    entry[entry_len++]=(char)c;entry[entry_len]=0;
    put(stage>=2&&stage<=4?'*':(char)c);
}
void kmain32(unsigned int magic,unsigned int info){
    (void)info;
    outb(0x3f9,0);outb(0x3fb,0x80);outb(0x3f8,3);
    outb(0x3f9,0);outb(0x3fb,3);outb(0x3fa,0xc7);outb(0x3fc,0x0b);
    if(magic!=0x36d76289u){
        serial("ZUZIFYOS32_BAD_BOOT_MAGIC");
        for(;;)__asm__ volatile("hlt");
    }
    clear();show_setup();
    serial("ZUZIFYOS32_BOOT_OK");
    serial("ZUZIFYOS32_OOBE_READY");
    for(;;){input_poll();__asm__ volatile("pause");}
}
