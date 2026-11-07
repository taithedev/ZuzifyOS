#include "os.h"

static fs_node nodes[FS_MAX];
static int cwd_index=0;

static int child_of(int parent,const char *name){
    for(int i=1;i<FS_MAX;i++){
        if(nodes[i].used && nodes[i].parent==parent && os_strcmp(nodes[i].name,name)==0)return i;
    }
    return -1;
}
int fs_cwd(void){return cwd_index;}
const fs_node *fs_node_at(int id){
    return (id>=0 && id<FS_MAX && nodes[id].used)?&nodes[id]:0;
}
int fs_resolve(const char *path){
    if(!path || !*path)return cwd_index;
    int at=(*path=='/')?0:cwd_index;
    while(*path){
        while(*path=='/')path++;
        if(!*path)break;
        char segment[32];size_t n=0;
        while(*path && *path!='/'){
            if(n+1>=sizeof(segment))return -1;
            segment[n++]=*path++;
        }
        segment[n]=0;
        if(os_strcmp(segment,".")==0)continue;
        if(os_strcmp(segment,"..")==0){at=nodes[at].parent;continue;}
        if(!nodes[at].dir)return -1;
        at=child_of(at,segment);
        if(at<0)return -1;
    }
    return at;
}
int fs_create(const char *path,bool directory){
    if(!path || !*path || os_strcmp(path,"/")==0)return -1;
    if(fs_resolve(path)>=0)return -2;
    char work[160],name[32];
    os_copy(work,path,sizeof(work));
    size_t len=os_strlen(work);
    while(len>1 && work[len-1]=='/')work[--len]=0;
    char *last=0;
    for(size_t i=0;i<len;i++)if(work[i]=='/')last=&work[i];
    int parent=cwd_index;
    if(last){
        os_copy(name,last+1,sizeof(name));
        if(last==work){parent=0;}
        else{*last=0;parent=fs_resolve(work);}
    }else{os_copy(name,work,sizeof(name));}
    if(parent<0 || !nodes[parent].dir || !*name || os_strlen(name)>=32)return -1;
    for(int i=1;i<FS_MAX;i++){
        if(!nodes[i].used){
            nodes[i].used=true;nodes[i].dir=directory;nodes[i].parent=parent;
            os_copy(nodes[i].name,name,sizeof(nodes[i].name));
            nodes[i].data[0]=0;
            return i;
        }
    }
    return -3;
}
int fs_write(const char *path,const char *content){
    int id=fs_resolve(path);
    if(id<0)id=fs_create(path,false);
    if(id<0 || nodes[id].dir)return -1;
    os_copy(nodes[id].data,content,sizeof(nodes[id].data));
    return 0;
}
int fs_delete(const char *path){
    int id=fs_resolve(path);
    if(id<=0)return -1;
    for(int i=1;i<FS_MAX;i++)if(nodes[i].used && nodes[i].parent==id)return -2;
    if(id==cwd_index)return -3;
    nodes[id].used=false;
    return 0;
}
int fs_cd(const char *path){
    int id=fs_resolve(path);
    if(id<0 || !nodes[id].dir)return -1;
    cwd_index=id;return 0;
}
void fs_pwd(char *buffer,size_t capacity){
    if(!capacity)return;
    char parts[24][32];int count=0,at=cwd_index;
    while(at>0 && count<24){
        os_copy(parts[count++],nodes[at].name,32);
        at=nodes[at].parent;
    }
    os_copy(buffer,"/",capacity);
    for(int i=count-1;i>=0;i--){
        os_append(buffer,parts[i],capacity);
        if(i)os_append(buffer,"/",capacity);
    }
}
void fs_init(void){
    for(int i=0;i<FS_MAX;i++)nodes[i].used=false;
    nodes[0].used=true;nodes[0].dir=true;nodes[0].parent=0;
    os_copy(nodes[0].name,"/",sizeof(nodes[0].name));
    cwd_index=0;
    fs_create("/home",true);
    fs_create("/home/guest",true);
    fs_create("/home/guest/projects",true);
    fs_create("/etc",true);
    fs_create("/dev",true);
    fs_create("/tmp",true);
    fs_create("/usr",true);
    fs_create("/usr/bin",true);
    fs_create("/usr/share",true);
    fs_create("/var",true);
    fs_create("/var/log",true);
    fs_write("/home/guest/welcome.txt","Welcome to ZuzifyOS. This is a REAL kernel.\nType help in the terminal to explore.\nAll files currently live in RAM.");
    fs_write("/etc/os-release","NAME=ZuzifyOS\nVERSION=0.1.0-alpha\nARCH=x86_64\nSTATUS=hobby-os");
    fs_write("/usr/bin/hello.zs","echo Hello from a ZuzifyOS shell script!\necho This runs inside our kernel command interpreter.");
    fs_write("/usr/bin/tour.zs","echo Starting the system tour...\nuname\nls /\ncat /etc/os-release\napps");
    fs_write("/var/log/boot.log","boot stage: Limine -> kernel -> framebuffer -> ramfs -> shell");
    fs_cd("/home/guest");
}
