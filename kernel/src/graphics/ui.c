#include "os.h"

/* The alpha account store is VOLATILE and NOT a secure auth system. */
enum state { WELCOME, USERNAME, PICK_PIN, CONFIRM_PIN, LOCKED, DESKTOP };
static enum state state_now=WELCOME;
static char username[24],input[32],candidate[12],notice[96];
static int input_len,failures;
static uint32_t pin_hash;
static int active_app=UI_NONE;
static int mouse_x=760,mouse_y=440;
static bool mouse_down=false,ready=false;

static uint32_t pin_digest(const char *pin){
    /* Non-cryptographic toy verifier, intentionally not represented as secure. */
    uint32_t h=2166136261u ^ 0x5a92be41u;
    for(size_t i=0;pin[i];i++){h^=(uint8_t)pin[i];h*=16777619u;}
    return h;
}
static void erase(char *text,size_t count){
    volatile char *p=text;
    for(size_t i=0;i<count;i++)p[i]=0;
}
static void clear_input(void){erase(input,sizeof(input));input_len=0;}
static void banner(const char *s){os_copy(notice,s,sizeof(notice));}
static void draw_input(int x,int y,int width,bool secret){
    gfx_round_rect(x,y,width,51,14,0x273751);
    gfx_round_rect(x+2,y+2,width-4,47,12,0x111e34);
    char display[40];os_copy(display,input,sizeof(display));
    if(secret)for(size_t i=0;i<os_strlen(display);i++)display[i]='*';
    if(!display[0])os_copy(display,secret?"ENTER 4-8 DIGITS":"TYPE HERE",sizeof(display));
    gfx_text(x+18,y+17,display,input_len?0xf0f5ff:0x657994,2);
    gfx_rect(x+20+(int)os_strlen(display)*12,y+17,3,14,0x85d5ff);
}
static void button(int x,int y,int width,const char *label,bool highlighted){
    gfx_round_rect(x,y,width,43,13,highlighted?0x5b72da:0x34435c);
    gfx_text(x+18,y+15,label,0xffffff,2);
}
static void draw_setup(void){
    int w=gfx_width(),h=gfx_height();
    int cw=w>780?646:w-32, ch=428;
    int x=(w-cw)/2,y=(h-ch)/2;
    gfx_round_rect(x+6,y+10,cw,ch,30,0x071326);
    gfx_glass(x,y,cw,ch,30,0x102039,188);
    gfx_round_rect(x+33,y+29,57,57,19,0x566feb);
    gfx_text(x+50,y+48,"Z",0xffffff,3);
    gfx_text(x+111,y+36,"ZUZIFY OS",0xf5f6ff,3);
    gfx_text(x+113,y+72,"FIRST BOOT EXPERIENCE",0x9dadce,1);
    for(int i=0;i<4;i++)gfx_round_rect(x+37+i*76,y+112,61,5,3,
        (state_now==WELCOME || (int)state_now>=i+1)?0x7a91ff:0x34455d);
    const char *headline=state_now==WELCOME?"LET'S SET THINGS UP":
      state_now==USERNAME?"CREATE AN ACCOUNT":
      state_now==PICK_PIN?"CHOOSE A PIN":
      state_now==CONFIRM_PIN?"CONFIRM YOUR PIN":"WELCOME BACK";
    gfx_text(x+36,y+148,headline,0xffffff,2);
    const char *description=state_now==WELCOME?"WELCOME TO YOUR NEW DESKTOP.":
      state_now==USERNAME?"ENTER A LOCAL USERNAME (2-19 CHARACTERS).":
      state_now==PICK_PIN?"SET A 4-8 DIGIT PIN FOR THIS SESSION.":
      state_now==CONFIRM_PIN?"ENTER YOUR PIN AGAIN TO CONFIRM.":
      "TYPE YOUR PIN TO UNLOCK.";
    gfx_text(x+36,y+184,description,0xc2cfe4,1);
    if(state_now==WELCOME){
        gfx_text(x+36,y+228,"CUSTOM KERNEL. NO LINUX OR WINDOWS REQUIRED.",0xdce7fc,1);
        gfx_text(x+36,y+251,"YOUR FILES AND ACCOUNT RESET ON REBOOT.",0xf4c38e,1);
    }else{
        if(state_now==LOCKED){
            gfx_text(x+36,y+220,username,0x8bd9ff,2);
        }
        draw_input(x+35,y+263,cw-70,state_now==PICK_PIN ||
          state_now==CONFIRM_PIN || state_now==LOCKED);
    }
    button(x+35,y+350,cw-70,state_now==WELCOME?"START SETUP":
           state_now==USERNAME?"NEXT":state_now==PICK_PIN?"CONTINUE":
           state_now==CONFIRM_PIN?"CREATE ACCOUNT":"UNLOCK",true);
    gfx_text(x+37,y+402,notice,0xffd3ae,1);
}
static void draw_panel_title(int x,int y,const char *title){
    gfx_round_rect(x+17,y+16,13,13,6,0x7a93ff);
    gfx_text(x+43,y+15,title,0xf5f7ff,2);
    gfx_round_rect(x+18,y+51,553,1,0,0x475777);
    gfx_text(x+530,y+17,"[X]",0xffa5b2,1);
}
static void draw_settings(int x,int y,int w,int h){
    (void)h;
    draw_panel_title(x,y,"SETTINGS");
    gfx_text(x+30,y+83,"PERSONALIZATION",0x8fb1ff,2);
    gfx_text(x+30,y+116,"CHOOSE A DESKTOP THEME",0xc5d5ec,1);
    button(x+30,y+147,145,"MIDNIGHT",gfx_get_theme()==0);
    button(x+190,y+147,137,"VIOLET",gfx_get_theme()==1);
    button(x+342,y+147,130,"OCEAN",gfx_get_theme()==2);
    gfx_text(x+30,y+221,"ACCOUNTS",0x8fb1ff,2);
    gfx_text(x+30,y+253,"LOCAL USER:",0xc5d5ec,1);
    gfx_text(x+145,y+253,username,0xf5f7ff,1);
    gfx_text(x+30,y+281,"PIN: SESSION ONLY - RESET ON REBOOT",0xf0be90,1);
    button(x+30,y+305,208,"LOCK SESSION",false);
    gfx_text(x+30,y+374,"SYSTEM",0x8fb1ff,2);
    gfx_text(x+30,y+401,"NATIVE X86-64 KERNEL - 0.2 ALPHA",0xe6ecf6,1);
    gfx_text(x+30,y+420,"UPDATES: MANUAL ISO REPLACEMENT ONLY",0xa9b9d2,1);
    gfx_text(x+30,y+440,"MOUSE: PS/2 EMULATION  |  KEYS: F1-F4",0xa9b9d2,1);
    if(w<530)gfx_text(x+30,y+459,"SMALL DISPLAY - CONTENT MAY CLIP",0xf0be90,1);
}
static void draw_files(int x,int y,int w,int h){
    (void)w;
    draw_panel_title(x,y,"FILE EXPLORER");
    char cwd[160];fs_pwd(cwd,sizeof(cwd));
    gfx_text(x+27,y+82,"CURRENT DIRECTORY:",0xa2b9dd,1);
    gfx_text(x+27,y+103,cwd,0xf5f7ff,2);
    int dir=fs_cwd(),count=0;
    for(int i=1;i<FS_MAX && count<16;i++){
        const fs_node *node=fs_node_at(i);
        if(node && node->parent==dir){
            int ly=y+147+count*21;
            if(ly+20>y+h-24)break;
            gfx_text(x+35,ly,node->dir?"DIR":"TXT",node->dir?0xe6c280:0x9ac4fd,1);
            gfx_text(x+104,ly,node->name,0xf4f7ff,1);
            count++;
        }
    }
    if(!count)gfx_text(x+28,y+151,"FOLDER IS EMPTY",0xb9c9df,1);
    gfx_text(x+27,y+h-45,"USE TERMINAL: LS / CD / CAT / WRITE",0x97a6c0,1);
}
static void draw_apps(int x,int y){
    draw_panel_title(x,y,"APPLICATIONS");
    gfx_text(x+30,y+80,"INCLUDED IN THIS BUILD",0xa5b8d6,1);
    const char *names[]={"TERMINAL - F1","FILE EXPLORER - F2","SETTINGS - F3","APP LAUNCHER - F4"};
    for(int i=0;i<4;i++){
        gfx_icon(x+32,y+113+i*81,i);
        gfx_text(x+93,y+129+i*81,names[i],0xf4f7ff,1);
    }
    gfx_text(x+30,y+464,"MORE NATIVE APPS ARE PLANNED.",0x92a4c4,1);
}
static void desktop_window(void){
    int w=gfx_width(),h=gfx_height();
    if(active_app==UI_NONE)return;
    if(active_app==UI_TERMINAL){term_render();return;}
    int width=w>840?580:w-34,height=h>690?510:h-128;
    int x=(w-width)/2+55,y=(h-height)/2-5;
    if(x+width>w-10)x=w-width-10;
    gfx_round_rect(x+7,y+13,width,height,26,0x091324);
    gfx_glass(x,y,width,height,24,0x11213c,204);
    if(active_app==UI_SETTINGS)draw_settings(x,y,width,height);
    else if(active_app==UI_FILES)draw_files(x,y,width,height);
    else draw_apps(x,y);
}
void ui_draw(void){
    if(!ready)return;
    desktop_draw();
    if(state_now!=DESKTOP)draw_setup();
    else {
        desktop_window();
        gfx_round_rect(34,70,160,30,13,0x172842);
        gfx_text(47,80,username,0xaad8ff,1);
    }
    gfx_cursor(mouse_x,mouse_y);
}
void ui_init(void){
    username[0]=0;clear_input();candidate[0]=0;
    failures=0;state_now=WELCOME;active_app=UI_NONE;
    mouse_x=gfx_width()/2;mouse_y=gfx_height()/2;
    mouse_down=false;banner("PRESS ENTER TO CONTINUE");
    ready=true;
    serial_write("ZUZIFYOS_OOBE_READY\n");
    ui_draw();
}
bool ui_ready(void){return ready;}
const char *ui_username(void){return *username?username:"guest";}
void ui_open(int app){
    if(state_now!=DESKTOP)return;
    if(app>=UI_TERMINAL && app<=UI_NONE)active_app=app;
    ui_draw();
}
void ui_lock(void){
    if(state_now!=DESKTOP)return;
    state_now=LOCKED;active_app=UI_NONE;clear_input();
    banner("ENTER YOUR PIN TO UNLOCK");
    serial_write("ZUZIFYOS_LOCKED\n");
    ui_draw();
}
static void proceed(void){
    if(state_now==WELCOME){
        state_now=USERNAME;banner("USERNAME WILL BE LOCAL");
    }else if(state_now==USERNAME){
        if(input_len<2 || input_len>19){banner("USERNAME MUST HAVE 2-19 CHARACTERS");return;}
        os_copy(username,input,sizeof(username));clear_input();
        state_now=PICK_PIN;banner("4 TO 8 DIGITS REQUIRED");
    }else if(state_now==PICK_PIN){
        if(input_len<4 || input_len>8){banner("PIN MUST CONTAIN 4-8 DIGITS");return;}
        os_copy(candidate,input,sizeof(candidate));clear_input();
        state_now=CONFIRM_PIN;banner("CONFIRM YOUR NEW PIN");
    }else if(state_now==CONFIRM_PIN){
        if(os_strcmp(candidate,input)!=0){
            erase(candidate,sizeof(candidate));clear_input();
            state_now=PICK_PIN;banner("PINS DID NOT MATCH - TRY AGAIN");return;
        }
        pin_hash=pin_digest(input);
        erase(candidate,sizeof(candidate));clear_input();
        state_now=LOCKED;banner("ACCOUNT CREATED. UNLOCK TO CONTINUE.");
        serial_write("ZUZIFYOS_ACCOUNT_CREATED\n");
    }else if(state_now==LOCKED){
        uint32_t entered=pin_digest(input);clear_input();
        if(entered==pin_hash){
            state_now=DESKTOP;active_app=UI_TERMINAL;
            failures=0;banner("");
            serial_write("ZUZIFYOS_LOGIN_OK\n");
        }else{
            failures++;banner("INCORRECT PIN - TRY AGAIN");
            serial_write("ZUZIFYOS_LOGIN_REJECTED\n");
        }
    }
}
void ui_key(int key){
    if(!ready)return;
    if(state_now==DESKTOP){
        if(key>=KEY_F1 && key<=KEY_F4){ui_open(key-KEY_F1);return;}
        if(key==27){ui_open(UI_NONE);return;}
        if(key=='\t'){ui_open((active_app+1)%5);return;}
        if(active_app==UI_TERMINAL){term_key(key);return;}
        if(active_app==UI_SETTINGS){
            if(key=='1')gfx_set_theme(0);
            if(key=='2')gfx_set_theme(1);
            if(key=='3')gfx_set_theme(2);
            if(key=='l'||key=='L'){ui_lock();return;}
        }
        ui_draw();return;
    }
    if(key=='\n'){proceed();ui_draw();return;}
    if(key=='\b'){
        if(input_len){input[--input_len]=0;}
        ui_draw();return;
    }
    bool is_pin=state_now==PICK_PIN||state_now==CONFIRM_PIN||state_now==LOCKED;
    bool okay=is_pin?(key>='0' && key<='9'):
        ((key>='a' && key<='z')||(key>='A' && key<='Z')||
         (key>='0' && key<='9')||key=='_'||key=='-');
    int max=is_pin?8:19;
    if(okay && input_len<max && input_len<31){
        input[input_len++]=(char)key;input[input_len]=0;
    }
    ui_draw();
}
static void click(int x,int y){
    int w=gfx_width(),h=gfx_height();
    if(state_now!=DESKTOP){
        int cw=w>780?646:w-32,ch=428;
        int cx=(w-cw)/2,cy=(h-ch)/2;
        if(x>=cx+35 && x<=cx+cw-35 &&
           y>=cy+350 && y<=cy+393)proceed();
        ui_draw();return;
    }
    int dock_start=w/2-108;
    if(y>=h-75 && y<h-6){
        for(int i=0;i<4;i++)
            if(x>=dock_start+i*54 && x<dock_start+i*54+48){
                active_app=i;ui_draw();return;
            }
    }
    if(x>=25 && x<154 && y>=128 && y<510){
        for(int i=0;i<4;i++){
            if(y>=135+i*94 && y<200+i*94){active_app=i;ui_draw();return;}
        }
    }
    if(active_app==UI_NONE){ui_draw();return;}
    if(active_app==UI_TERMINAL){ui_draw();return;}
    int width=w>840?580:w-34,height=h>690?510:h-128;
    int px=(w-width)/2+55,py=(h-height)/2-5;
    if(px+width>w-10)px=w-width-10;
    if(x>=px+524 && y>=py+12 && y<py+48){active_app=UI_NONE;ui_draw();return;}
    if(active_app==UI_SETTINGS){
        if(y>=py+147 && y<=py+190){
            if(x>=px+30 && x<px+175)gfx_set_theme(0);
            else if(x>=px+190 && x<px+327)gfx_set_theme(1);
            else if(x>=px+342 && x<px+472)gfx_set_theme(2);
        }
        if(x>=px+30 && x<px+238 && y>=py+305 && y<py+348){ui_lock();return;}
    } else if(active_app==UI_APPS){
        for(int i=0;i<4;i++)
            if(x>=px+28 && x<px+width-28 &&
               y>=py+105+i*81 && y<py+173+i*81){active_app=i;break;}
    }
    ui_draw();
}
void ui_mouse_move(int dx,int dy,bool left){
    if(!ready)return;
    mouse_x+=dx;mouse_y-=dy;
    if(mouse_x<0)mouse_x=0;
    if(mouse_y<0)mouse_y=0;
    if(mouse_x>=gfx_width())mouse_x=gfx_width()-1;
    if(mouse_y>=gfx_height())mouse_y=gfx_height()-1;
    bool clicked=left && !mouse_down;
    mouse_down=left;
    if(clicked){click(mouse_x,mouse_y);return;}
    /* The glass reflections and cursor are recomputed on movement. */
    ui_draw();
}
