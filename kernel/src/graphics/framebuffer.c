#include "os.h"

static struct limine_framebuffer *framebuffer;
static int theme_mode=0;
static uint32_t channel(uint32_t value,uint8_t size,uint8_t shift){
    if(!size)return 0;
    uint32_t max=(1u<<size)-1u;
    return ((value*max+127u)/255u)<<shift;
}
static uint32_t native(uint32_t rgb){
    return channel((rgb>>16)&255u,framebuffer->red_mask_size,framebuffer->red_mask_shift)
         | channel((rgb>>8)&255u,framebuffer->green_mask_size,framebuffer->green_mask_shift)
         | channel(rgb&255u,framebuffer->blue_mask_size,framebuffer->blue_mask_shift);
}
static uint32_t unchannel(uint32_t packed,uint8_t size,uint8_t shift){
    if(!size)return 0;
    uint32_t max=(1u<<size)-1u;
    return (((packed>>shift)&max)*255u+max/2u)/max;
}
static uint32_t unpack(uint32_t packed){
    return (unchannel(packed,framebuffer->red_mask_size,framebuffer->red_mask_shift)<<16)
        | (unchannel(packed,framebuffer->green_mask_size,framebuffer->green_mask_shift)<<8)
        | unchannel(packed,framebuffer->blue_mask_size,framebuffer->blue_mask_shift);
}
static uint32_t blend(uint32_t under,uint32_t over,int alpha){
    if(alpha<0)alpha=0;
    if(alpha>255)alpha=255;
    uint32_t r=((((under>>16)&255u)*(uint32_t)(255-alpha))+(((over>>16)&255u)*(uint32_t)alpha))/255u;
    uint32_t g=((((under>>8)&255u)*(uint32_t)(255-alpha))+(((over>>8)&255u)*(uint32_t)alpha))/255u;
    uint32_t b=(((under&255u)*(uint32_t)(255-alpha))+((over&255u)*(uint32_t)alpha))/255u;
    return (r<<16)|(g<<8)|b;
}
void gfx_init(struct limine_framebuffer *fb){framebuffer=fb;}
int gfx_width(void){return framebuffer?(int)framebuffer->width:0;}
int gfx_height(void){return framebuffer?(int)framebuffer->height:0;}
void gfx_set_theme(int mode){theme_mode=(mode>=0 && mode<=2)?mode:0;}
int gfx_get_theme(void){return theme_mode;}
void gfx_rect(int x,int y,int w,int h,uint32_t rgb){
    if(!framebuffer || w<=0 || h<=0)return;
    int x2=x+w,y2=y+h;
    if(x<0)x=0;if(y<0)y=0;
    if(x2>gfx_width())x2=gfx_width();
    if(y2>gfx_height())y2=gfx_height();
    if(x>=x2 || y>=y2)return;
    uint32_t px=native(rgb);
    for(int row=y;row<y2;row++){
        volatile uint32_t *out=(volatile uint32_t *)((uint8_t*)framebuffer->address+(uint64_t)row*framebuffer->pitch);
        for(int col=x;col<x2;col++)out[col]=px;
    }
}
static int rounded_inset(int row,int height,int radius){
    if(radius>height/2)radius=height/2;
    if(radius<=0)return 0;
    int dy=0;
    if(row<radius)dy=radius-row-1;
    else if(row>=height-radius)dy=row-(height-radius);
    else return 0;
    int rr=radius*radius;
    for(int dx=0;dx<radius;dx++){
        int d=radius-1-dx;
        if(d*d+dy*dy<=rr)return dx;
    }
    return radius-1;
}
void gfx_round_rect(int x,int y,int w,int h,int radius,uint32_t color){
    if(w<1||h<1)return;
    if(radius>w/2)radius=w/2;
    for(int row=0;row<h;row++){
        int inset=rounded_inset(row,h,radius);
        gfx_rect(x+inset,y+row,w-2*inset,1,color);
    }
}
static uint32_t fb_sample(int x,int y){
    if(x<0)x=0;if(y<0)y=0;
    if(x>=gfx_width())x=gfx_width()-1;
    if(y>=gfx_height())y=gfx_height()-1;
    volatile uint32_t *row=(volatile uint32_t *)((uint8_t*)framebuffer->address+(uint64_t)y*framebuffer->pitch);
    return unpack(row[x]);
}
/* Lightweight real sampled translucency; not a GPU compositor or macOS blur pipeline. */
void gfx_glass(int x,int y,int w,int h,int radius,uint32_t tint,int alpha){
    if(!framebuffer||w<1||h<1)return;
    if(radius>w/2)radius=w/2;
    for(int j=0;j<h;j++){
        int inset=rounded_inset(j,h,radius);
        int left=x+inset,right=x+w-inset;
        if(y+j<0 || y+j>=gfx_height())continue;
        if(left<0)left=0;
        if(right>gfx_width())right=gfx_width();
        if(left>=right)continue;
        volatile uint32_t *dst=(volatile uint32_t *)((uint8_t*)framebuffer->address+(uint64_t)(y+j)*framebuffer->pitch);
        for(int i=left;i<right;i++){
            uint32_t c0=fb_sample(i,y+j);
            uint32_t c1=fb_sample(i-3,y+j);
            uint32_t c2=fb_sample(i+3,y+j);
            uint32_t c3=fb_sample(i,y+j-3);
            uint32_t c4=fb_sample(i,y+j+3);
            uint32_t red=(((c0>>16)&255u)+((c1>>16)&255u)+((c2>>16)&255u)+((c3>>16)&255u)+((c4>>16)&255u))/5u;
            uint32_t green=(((c0>>8)&255u)+((c1>>8)&255u)+((c2>>8)&255u)+((c3>>8)&255u)+((c4>>8)&255u))/5u;
            uint32_t blue=((c0&255u)+(c1&255u)+(c2&255u)+(c3&255u)+(c4&255u))/5u;
            dst[i]=native(blend((red<<16)|(green<<8)|blue,tint,alpha));
        }
    }
    /* White edge reflection and low-opacity lower border. */
    gfx_rect(x+radius,y+1,w-radius*2,1,0x7381a4);
    gfx_rect(x+radius,y+h-2,w-radius*2,1,0x263653);
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
    const uint32_t white=0xf2f5ff;
    uint32_t tint=kind==0?0x5168cd:kind==1?0x287a9e:kind==2?0x775ab7:0x327e6d;
    gfx_round_rect(x,y,42,42,10,tint);
    gfx_round_rect(x+2,y+2,38,38,9,0x182844);
    if(kind==0){
        gfx_line(x+11,y+13,x+20,y+20,white);
        gfx_line(x+20,y+20,x+11,y+27,white);
        gfx_rect(x+23,y+27,11,2,0x88dbff);
    } else if(kind==1){
        gfx_round_rect(x+8,y+15,26,18,4,0xe7b35b);
        gfx_round_rect(x+8,y+12,13,7,3,0xf8d58d);
        gfx_rect(x+12,y+22,18,2,0xffe1a9);
    } else if(kind==2){
        gfx_round_rect(x+12,y+12,18,18,7,0xbd9cff);
        gfx_rect(x+9,y+19,24,5,0xbd9cff);
        gfx_rect(x+19,y+9,5,24,0xbd9cff);
        gfx_round_rect(x+18,y+18,7,7,3,0x182844);
    } else {
        gfx_round_rect(x+19,y+11,5,5,2,0x6be7c7);
        gfx_round_rect(x+19,y+19,5,14,2,0x6be7c7);
    }
}
void gfx_cursor(int x,int y){
    gfx_line(x,y,x+1,y+20,0x000715);
    gfx_line(x+1,y+20,x+6,y+15,0x000715);
    gfx_line(x+6,y+15,x+12,y+17,0x000715);
    gfx_line(x+12,y+17,x,y,0x000715);
    gfx_line(x+1,y+1,x+2,y+17,0xf8fbff);
    gfx_line(x+2,y+17,x+6,y+13,0xf8fbff);
    gfx_line(x+6,y+13,x+10,y+15,0xf8fbff);
}
void desktop_draw(void){
    int w=gfx_width(),h=gfx_height();
    const uint32_t base=theme_mode==1?0x261532:theme_mode==2?0x092d38:0x0b182e;
    gfx_rect(0,0,w,h,base);
    for(int y=0;y<h;y+=4){
        int k=(y*34)/(h?h:1);
        uint32_t r=(base>>16)&255u,g=(base>>8)&255u,b=base&255u;
        gfx_rect(0,y,w,4,((r+(uint32_t)k/2u)<<16)|((g+(uint32_t)k/2u)<<8)|(b+(uint32_t)k));
    }
    int cx=w*2/3,cy=h/3;
    for(int i=24;i>=1;i--){
        int rad=i*13;
        uint32_t c=theme_mode==1?0x35244c:theme_mode==2?0x17506a:0x1c345e;
        gfx_round_rect(cx-rad,cy-rad/2,rad*2,rad,rad/4,c);
    }
    gfx_glass(12,10,w-24,48,18,0x18283b,170);
    gfx_text(36,26,"ZUZIFY OS",0xf4f7ff,2);
    gfx_text(w-166,30,"ALPHA 0.2",0xaebfe0,1);
    gfx_text(30,89,"YOUR DESKTOP",0xc8d7ef,2);
    const char *labels[]={"TERMINAL","FILES","SETTINGS","APPS"};
    for(int i=0;i<4;i++){
        int top=135+i*94;
        gfx_icon(32,top,i);
        gfx_text(30,top+51,labels[i],0xe5edff,1);
    }
    gfx_glass(22,h-77,w-44,65,22,0x15243b,200);
    gfx_text(45,h-53,"ZUZIFYOS",0xe9efff,2);
    int start=w/2-108;
    for(int i=0;i<4;i++)gfx_icon(start+i*54,h-65,i);
    gfx_text(w-135,h-52,"F1 - F4",0xb4c0dd,1);
}
