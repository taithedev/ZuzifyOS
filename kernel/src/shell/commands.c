#include "os.h"
extern void gfx_set_theme(int mode);

static int script_depth=0;

static void print_number(const char *prefix,int value){
    char number[24],msg[96];
    os_itoa(value,number,sizeof(number));
    os_copy(msg,prefix,sizeof(msg));
    os_append(msg,number,sizeof(msg));
    term_line(msg);
}
static void listing(const char *path){
    int dir=fs_resolve(path && *path?path:".");
    const fs_node *node=fs_node_at(dir);
    if(!node){term_line("ls: no such path");return;}
    if(!node->dir){term_line(node->name);return;}
    int count=0;
    for(int i=1;i<FS_MAX;i++){
        const fs_node *item=fs_node_at(i);
        if(item && item->parent==dir){
            char name[64];
            os_copy(name,item->dir?"[DIR] ":"      ",sizeof(name));
            os_append(name,item->name,sizeof(name));
            term_line(name);
            count++;
        }
    }
    if(!count)term_line("(empty)");
}
static void tree_list(int parent,int indent,int *total){
    if(indent>8)return;
    for(int i=1;i<FS_MAX;i++){
        const fs_node *node=fs_node_at(i);
        if(node && node->parent==parent){
            char output[110];int at=0;
            for(int j=0;j<indent*2 && at<30;j++)output[at++]=' ';
            output[at]=0;
            os_append(output,node->name,sizeof(output));
            if(node->dir)os_append(output,"/",sizeof(output));
            term_line(output);
            (*total)++;
            if(node->dir)tree_list(i,indent+1,total);
        }
    }
}
static void run_script(const char *path){
    int id=fs_resolve(path);
    const fs_node *node=fs_node_at(id);
    if(!node || node->dir){term_line("run: script not found");return;}
    size_t len=os_strlen(node->name);
    if(len<3 || os_strcmp(node->name+len-3,".zs")!=0){
        term_line("run: only .zs scripts are supported right now");
        return;
    }
    if(script_depth>=3){term_line("run: script nesting limit exceeded");return;}
    char copy[FS_DATA];
    os_copy(copy,node->data,sizeof(copy));
    script_depth++;
    char *start=copy;
    for(char *p=copy;;p++){
        if(*p=='\n' || *p==0){
            char saved=*p;*p=0;
            if(*start && *start!='#')shell_exec(start);
            if(!saved)break;
            start=p+1;
        }
    }
    script_depth--;
}
static void launch(const char *app){
    if(os_strcmp(app,"terminal")==0){term_line("Terminal is already open.");return;}
    if(os_strcmp(app,"files")==0){term_line("FILES: /home/guest");listing("/home/guest");return;}
    if(os_strcmp(app,"settings")==0){term_line("SETTINGS: use 'theme midnight' or 'theme violet'.");return;}
    if(os_strcmp(app,"about")==0){term_line("ZuzifyOS native x86-64 early alpha.");return;}
    if(os_strcmp(app,"calculator")==0){term_line("Calculator: type 'calc 12 * 3'.");return;}
    if(os_strcmp(app,"clock")==0){term_line("Clock requires an RTC or time-service driver.");return;}
    size_t length=os_strlen(app);
    if(length>=4 && os_strcmp(app+length-4,".exe")==0){
        term_line("Windows .exe is NOT supported. PE loader and Win32 APIs needed.");
        return;
    }
    run_script(app);
}
static void calculator(const char *expr){
    while(*expr==' ')expr++;
    int a=os_atoi(expr);
    if(*expr=='-')expr++;
    while(*expr>='0'&&*expr<='9')expr++;
    while(*expr==' ')expr++;
    char operation=*expr++;
    while(*expr==' ')expr++;
    int b=os_atoi(expr);
    int answer=0;
    if(operation=='+')answer=a+b;
    else if(operation=='-')answer=a-b;
    else if(operation=='*')answer=a*b;
    else if(operation=='/'&&b)answer=a/b;
    else{term_line("calc: use 'calc 12 + 3' (supports + - * /)");return;}
    print_number("RESULT: ",answer);
}
void shell_exec(const char *input){
    while(*input==' ')input++;
    if(!*input)return;
    char command[32];int n=0;
    while(*input && *input!=' ' && n<31)command[n++]=*input++;
    command[n]=0;
    while(*input==' ')input++;
    const char *arg=input;
    if(os_strcmp(command,"help")==0){
        term_line("COMMANDS:");
        term_line("help clear about version uname whoami echo");
        term_line("pwd ls cd mkdir touch cat write rm tree");
        term_line("apps run calc theme update reboot");
        term_line("EXAMPLES: ls /   cat /etc/os-release");
        term_line("write notes.txt hello   run /usr/bin/hello.zs");
    }else if(os_strcmp(command,"clear")==0)term_clear();
    else if(os_strcmp(command,"echo")==0)term_line(arg);
    else if(os_strcmp(command,"about")==0){
        term_line("ZUZIFYOS / REAL 64-BIT HOBBY OPERATING SYSTEM");
        term_line("Limine boot / framebuffer UI / PS2 input / RAMFS");
        term_line("Files do not persist. No Windows .exe support.");
    }else if(os_strcmp(command,"version")==0 || os_strcmp(command,"ver")==0){
        term_line("ZuzifyOS " ZUZU_VERSION);
    }else if(os_strcmp(command,"uname")==0)term_line("ZuzifyOS x86_64 native-kernel");
    else if(os_strcmp(command,"whoami")==0)term_line("guest (demo identity; no security model)");
    else if(os_strcmp(command,"pwd")==0){
        char cwd[160];fs_pwd(cwd,sizeof(cwd));term_line(cwd);
    }else if(os_strcmp(command,"ls")==0)listing(arg);
    else if(os_strcmp(command,"cd")==0){
        if(fs_cd(*arg?arg:"/home/guest")<0)term_line("cd: directory not found");
    }else if(os_strcmp(command,"mkdir")==0){
        if(!*arg){term_line("usage: mkdir <path>");return;}
        if(fs_create(arg,true)<0)term_line("mkdir: cannot create directory");
        else term_line("Directory created (RAM only).");
    }else if(os_strcmp(command,"touch")==0){
        if(!*arg){term_line("usage: touch <path>");return;}
        int id=fs_resolve(arg);
        if(id<0 && fs_create(arg,false)<0)term_line("touch: cannot create file");
        else term_line("File ready.");
    }else if(os_strcmp(command,"cat")==0){
        const fs_node *node=fs_node_at(fs_resolve(arg));
        if(!node || node->dir)term_line("cat: not a readable file");
        else term_line(node->data);
    }else if(os_strcmp(command,"write")==0){
        char path[128];int i=0;
        while(*arg && *arg!=' ' && i<127)path[i++]=*arg++;
        path[i]=0;
        while(*arg==' ')arg++;
        if(!*path || !*arg){term_line("usage: write <path> <text>");return;}
        if(fs_write(path,arg)<0)term_line("write: unable to write file");
        else term_line("Saved in RAM (lost on reboot).");
    }else if(os_strcmp(command,"rm")==0){
        if(!*arg || fs_delete(arg)<0)term_line("rm: not found, in use, or not empty");
        else term_line("Entry removed.");
    }else if(os_strcmp(command,"tree")==0){
        int id=fs_resolve(*arg?arg:"/");
        if(id<0){term_line("tree: path not found");return;}
        const fs_node *node=fs_node_at(id);
        term_line(node->name);
        int total=0;if(node->dir)tree_list(id,1,&total);
        print_number("ENTRIES: ",total);
    }else if(os_strcmp(command,"apps")==0){
        term_line("BUILT-IN APPS:");
        term_line("run terminal / files / settings / about");
        term_line("run calculator / clock");
        term_line("run /usr/bin/hello.zs");
        term_line("run /usr/bin/tour.zs");
    }else if(os_strcmp(command,"run")==0 || os_strcmp(command,"open")==0){
        if(!*arg)term_line("usage: run <app-name or .zs path>");
        else launch(arg);
    }else if(os_strcmp(command,"calc")==0)calculator(arg);
    else if(os_strcmp(command,"theme")==0){
        if(os_strcmp(arg,"midnight")==0)gfx_set_theme(0);
        else if(os_strcmp(arg,"violet")==0)gfx_set_theme(1);
        else{term_line("usage: theme midnight|violet");return;}
        term_redraw();term_line("Theme changed.");
    }else if(os_strcmp(command,"update")==0){
        if(!*arg || os_strcmp(arg,"check")==0){
            term_line("UPDATE: offline alpha; no network updater installed.");
            term_line("Releases: github.com/taithedev/ZuzifyOS/releases");
            term_line("Do not auto-flash unverified system images.");
        }else term_line("usage: update check");
    }else if(os_strcmp(command,"reboot")==0){
        term_line("Requesting PS/2 controller reset (QEMU only).");
        __asm__ volatile("outb %0, %1"::"a"((uint8_t)0xfe),"Nd"((uint16_t)0x64));
    }else if(os_strcmp(command,"mem")==0)term_line("Memory manager not initialized; use QEMU -m to configure.");
    else if(os_strcmp(command,"ps")==0)term_line("Kernel command loop only; userspace scheduler not built.");
    else if(os_strcmp(command,"date")==0)term_line("RTC/time service not implemented.");
    else term_line("Unknown command. Type help.");
}
