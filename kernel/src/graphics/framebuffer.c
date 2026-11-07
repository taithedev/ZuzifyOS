#include "os.h"

static struct limine_framebuffer *framebuffer;
static int theme_mode=0;

static uint32_t channel(uint32_t value,uint8_t size,uint8_t shift){
    if(!size)return 0;
    uint32_t max=(1u<<size)-1u;
    return ((value*max+127u)/255u)<<shift;
}
static uint32_t rgb_native(uint32_t rgb){
    return channel((rgb>>16)&255u,framebuffer->red_mask_size,framebuffer->red_mask_shift)
         | channel((rgb>>8)&255u,framebuffer->green_mask_size,framebuffer->green_mask_shift)
         | channel(rgb&255u,framebuffer->blue_mask_size,framebuffer->blue_mask_shift);
}
void gfx_init(struct limine_framebuffer *fb){framebuffer=fb;}
int gfx_width(void){return framebuffer?(int)framebuffer->width:0;}
int gfx_height(void){return framebuffer?(int)framebuffer->height:0;}
void gfx_set_theme(int mode){theme_mode=mode&1;}

void gfx_rect(int x,int y,int w,int h,uint32_t rgb){
    if(!framebuffer || w<=0 || h<=0)return;
    int x2=x+w,y2=y+h;
    if(x<0)x=0;
    if(y<0)y=0;
    if(x2>gfx_width())x2=gfx_width();
    if(y2>gfx_height())y2=gfx_height();
    if(x>=x2 || y>=y2)return;
    uint32_t native=rgb_native(rgb);
    for(int row=y;row<y2;row++){
        volatile uint32_t *out=(volatile uint32_t *)((uint8_t*)framebuffer->address+(uint64_t)row*framebuffer->pitch);
        for(int col=x;col<x2;col++)out[col]=native;
    }
}

void gfx_line(int x0,int y0,int x1,int y1,uint32_t color){
    int dx=x1>x0?x1-x0:x0-x1;
    int sx=x0<x1?1:-1;
    int dy=y1>y0?y0-y1:y1-y0;
    int sy=y0<y1?1:-1;
    int err=dx+dy;
    while(1){
        gfx_rect(x0,y0,1,1,color);
        if(x0==x1&&y0==y1)break;
        int e=err*2;
        if(e>=dy){err+=dy;x0+=sx;}
        if(e<=dx){err+=dx;y0+=sy;}
    }
}

void gfx_icon(int x,int y,int kind){
    /* Pixel/vector icon primitives. No external image decoding required. */
    const uint32_t white=0xf2f5ff;
    gfx_rect(x,y,42,42,kind==0?0x4153bf:kind==1?0x246b90:kind==2?0x62439f:0x2f6e62);
    gfx_rect(x+2,y+2,38,38,0x171f37);
    if(kind==0){ /* Terminal */
        gfx_line(x+11,y+13,x+20,y+20,white);
        gfx_line(x+20,y+20,x+11,y+27,white);
        gfx_rect(x+23,y+27,11,2,0x88dbff);
    }else if(kind==1){ /* Folder */
        gfx_rect(x+8,y+15,26,18,0xe7b35b);
        gfx_rect(x+8,y+12,12,5,0xf8d58d);
        gfx_rect(x+11,y+18,20,11,0x916c39);
    }else if(kind==2){ /* Gear */
        gfx_rect(x+12,y+12,18,18,0xbd9cff);
        gfx_rect(x+9,y+19,24,5,0xbd9cff);
        gfx_rect(x+19,y+9,5,24,0xbd9cff);
        gfx_rect(x+18,y+18,7,7,0x171f37);
    }else{ /* Info */
        gfx_rect(x+19,y+12,4,4,0x6be7c7);
        gfx_rect(x+19,y+19,4,13,0x6be7c7);
    }
}

void desktop_draw(void){
    int w=gfx_width(),h=gfx_height();
    uint32_t base=theme_mode?0x20132e:0x091427;
    gfx_rect(0,0,w,h,base);
    for(int y=0;y<h;y+=3){
        uint32_t shade=(uint32_t)((y*18)/(h?h:1));
        uint32_t r=theme_mode?32u:9u;
        uint32_t g=theme_mode?19u:20u;
        uint32_t b=theme_mode?46u:39u;
        gfx_rect(0,y,w,3,((r+shade/3)<<16)|((g+shade/2)<<8)|(b+shade));
    }
    gfx_rect(0,0,w,52,0x101b31);
    gfx_rect(0,51,w,1,0x30415f);
    gfx_rect(22,17,18,18,0x6e8bff);
    gfx_rect(26,21,10,10,0x0e172b);
    gfx_text(49,16,"ZUZIFY OS",0xe9eeff,2);
    gfx_text(w-190,20,"ALPHA 0.1",0xaeb9d3,1);
    gfx_text(28,80,"YOUR DESKTOP",0xb7c6e1,2);
    const char *labels[]={"TERMINAL F1","FILES F2","SETTINGS F3","APPS F4"};
    for(int i=0;i<4;i++){
        int top=136+i*93;
        gfx_icon(30,top,i);
        gfx_text(30,top+48,labels[i],0xc6d5ef,1);
    }
    int dock_y=h-66;
    gfx_rect(0,dock_y,w,66,0x101b31);
    gfx_rect(0,dock_y,w,1,0x344565);
    gfx_text(25,dock_y+24,"ZUZIFYOS",0xe5ecff,2);
    int start=w/2-105;
    for(int i=0;i<4;i++)gfx_icon(start+i*52,dock_y+12,i);
    gfx_text(w-145,dock_y+27,"F1 - F4",0x8f9dbf,1);
}
