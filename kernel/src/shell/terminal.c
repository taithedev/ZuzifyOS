#include "os.h"

static char screen_lines[TERM_LINES][TERM_COLS];
static int line_count;
static char prompt_input[TERM_COLS];
static size_t prompt_len;

static void push_line(const char *s){
    if(line_count==TERM_LINES){
        for(int i=1;i<TERM_LINES;i++)os_copy(screen_lines[i-1],screen_lines[i],TERM_COLS);
        line_count--;
    }
    os_copy(screen_lines[line_count++],s,TERM_COLS);
}
void term_line(const char *message){
    char line[TERM_COLS];size_t n=0;
    if(!message)message="";
    serial_write("TERM: ");
    serial_write(message);
    serial_write("\n");
    do{
        char c=*message++;
        if(c=='\n' || c==0 || n+1>=TERM_COLS){
            line[n]=0;push_line(line);n=0;
        }
        if(c && c!='\n' && n+1<TERM_COLS)line[n++]=c;
        if(!c)break;
    }while(1);
    term_redraw();
}
void term_clear(void){
    line_count=0;
    term_redraw();
}
void term_init(void){
    line_count=0;prompt_len=0;prompt_input[0]=0;
    term_line("ZUZIFYOS 0.2.0-ALPHA  /  X86-64 KERNEL");
    term_line("WELCOME. TYPE HELP AND PRESS ENTER.");
    term_line("F1:HELP  F2:FILES  F3:ABOUT  F4:APPS");
    term_line("");
}
void term_redraw(void){
    ui_draw();
}
void term_render(void){
    int w=gfx_width(),h=gfx_height();
    int x=w>840?185:20,y=101;
    int width=w-x-24,height=h-y-85;
    if(width<180 || height<150)return;
    gfx_round_rect(x+6,y+8,width,height,24,0x081123);
    gfx_glass(x,y,width,height,23,0x0e192b,221);
    gfx_round_rect(x+2,y+2,width-4,49,19,0x1e2b45);
    gfx_rect(x+16,y+17,14,14,0x7f91ff);
    gfx_rect(x+19,y+20,8,8,0x192338);
    gfx_text(x+42,y+15,"ZUZIFY TERMINAL",0xf5f7ff,2);
    gfx_text(x+width-105,y+20,"[ -  O  X ]",0xa0acca,1);
    gfx_rect(x+16,y+65,width-32,1,0x33415a);
    int visible=(height-142)/18;
    if(visible>TERM_LINES)visible=TERM_LINES;
    if(visible<0)visible=0;
    int from=line_count>visible?line_count-visible:0;
    int maxchars=(width-36)/12;
    if(maxchars>TERM_COLS-1)maxchars=TERM_COLS-1;
    for(int i=from;i<line_count;i++){
        char clipped[TERM_COLS];
        os_copy(clipped,screen_lines[i],(size_t)maxchars+1);
        uint32_t color=os_starts(clipped,"$")?0x8be0b4:0xcbd7f2;
        gfx_text(x+19,y+75+(i-from)*18,clipped,color,2);
    }
    int footer=y+height-57;
    gfx_rect(x+16,footer,width-32,1,0x30415c);
    gfx_text(x+18,footer+16,"$",0x79e8c3,2);
    int room=(width-65)/12;
    if(room>TERM_COLS-1)room=TERM_COLS-1;
    char cropped[TERM_COLS];
    os_copy(cropped,prompt_input,(size_t)room+1);
    gfx_text(x+44,footer+16,cropped,0xffffff,2);
    int cursor_x=x+44+(int)os_strlen(cropped)*12;
    gfx_rect(cursor_x,footer+17,8,13,0x80d1ff);
}
void term_key(int key){
    if(key>=KEY_F1 && key<=KEY_F4){
        const char *cmd=key==KEY_F1?"help":key==KEY_F2?"ls /home/guest":key==KEY_F3?"about":"apps";
        term_line(cmd);
        shell_exec(cmd);
        return;
    }
    if(key=='\b'){
        if(prompt_len)prompt_input[--prompt_len]=0;
        term_redraw();
        return;
    }
    if(key=='\n'){
        char entered[TERM_COLS],prefixed[TERM_COLS];
        os_copy(entered,prompt_input,sizeof(entered));
        os_copy(prefixed,"$ ",sizeof(prefixed));
        os_append(prefixed,entered,sizeof(prefixed));
        prompt_input[0]=0;prompt_len=0;
        term_line(prefixed);
        if(entered[0])shell_exec(entered);
        term_redraw();
        return;
    }
    if(key=='\t')key=' ';
    if(key>=32 && key<=126 && prompt_len+1<TERM_COLS){
        prompt_input[prompt_len++]=(char)key;
        prompt_input[prompt_len]=0;
        term_redraw();
    }
}
