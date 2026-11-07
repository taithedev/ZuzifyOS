#include "os.h"

size_t os_strlen(const char *s) {
    size_t n=0;
    if (!s) return 0;
    while (s[n]) n++;
    return n;
}

int os_strcmp(const char *a,const char *b) {
    while (*a && *a==*b) {a++;b++;}
    return (unsigned char)*a - (unsigned char)*b;
}

int os_strncmp(const char *a,const char *b,size_t n) {
    for(size_t i=0;i<n;i++){
        if(a[i]!=b[i] || !a[i])return (unsigned char)a[i]-(unsigned char)b[i];
    }
    return 0;
}

void os_copy(char *dst,const char *src,size_t cap) {
    if (!cap) return;
    size_t i=0;
    if (src) for(;i+1<cap && src[i];i++)dst[i]=src[i];
    dst[i]=0;
}

void os_append(char *dst,const char *src,size_t cap) {
    size_t n=os_strlen(dst);
    if(n<cap)os_copy(dst+n,src,cap-n);
}

bool os_starts(const char *s,const char *prefix) {
    return os_strncmp(s,prefix,os_strlen(prefix))==0;
}

int os_atoi(const char *s) {
    int sign=1,value=0;
    if(*s=='-'){sign=-1;s++;}
    while(*s>='0' && *s<='9') {value=value*10+(*s-'0');s++;}
    return value*sign;
}

void os_itoa(int value,char *dst,size_t cap) {
    char tmp[16];size_t n=0;bool neg=value<0;
    unsigned int u=neg?(unsigned int)(-(long long)value):(unsigned int)value;
    do {tmp[n++]=(char)('0'+u%10u);u/=10u;} while(u && n<sizeof(tmp));
    if(neg && n<sizeof(tmp))tmp[n++]='-';
    size_t j=0;
    while(n && j+1<cap)dst[j++]=tmp[--n];
    if(cap)dst[j]=0;
}

/* Compiler may emit freestanding block memory operations. */
void *memcpy(void *d,const void *s,size_t n) {
    unsigned char *dst=d; const unsigned char *src=s;
    for(size_t i=0;i<n;i++)dst[i]=src[i];
    return d;
}
void *memset(void *d,int c,size_t n) {
    unsigned char *dst=d;
    for(size_t i=0;i<n;i++)dst[i]=(unsigned char)c;
    return d;
}
void *memmove(void *d,const void *s,size_t n) {
    unsigned char *dst=d; const unsigned char *src=s;
    if(dst<src)for(size_t i=0;i<n;i++)dst[i]=src[i];
    else for(size_t i=n;i>0;i--)dst[i-1]=src[i-1];
    return d;
}
int memcmp(const void *a,const void *b,size_t n){
    const unsigned char *x=a,*y=b;
    for(size_t i=0;i<n;i++)if(x[i]!=y[i])return (int)x[i]-(int)y[i];
    return 0;
}
