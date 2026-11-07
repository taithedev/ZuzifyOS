#ifndef ZUZIFY_OS_H
#define ZUZIFY_OS_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "limine_abi.h"

#define ZUZU_VERSION "0.1.0-alpha"
#define ZUZU_NAME "ZuzifyOS"
#define TERM_LINES 56
#define TERM_COLS 100
#define FS_MAX 128
#define FS_DATA 512

size_t os_strlen(const char *s);
int os_strcmp(const char *a, const char *b);
int os_strncmp(const char *a, const char *b, size_t n);
void os_copy(char *dst, const char *src, size_t cap);
void os_append(char *dst, const char *src, size_t cap);
bool os_starts(const char *s, const char *prefix);
int os_atoi(const char *s);
void os_itoa(int n, char *out, size_t cap);

void serial_init(void);
void serial_putc(char c);
void serial_write(const char *text);

int kbd_poll(void);
#define KEY_F1 0x101
#define KEY_F2 0x102
#define KEY_F3 0x103
#define KEY_F4 0x104

void gfx_init(struct limine_framebuffer *fb);
int gfx_width(void);
int gfx_height(void);
void gfx_rect(int x,int y,int w,int h,uint32_t color);
void gfx_line(int x0,int y0,int x1,int y1,uint32_t color);
void gfx_text(int x,int y,const char *s,uint32_t color,int scale);
void gfx_icon(int x,int y,int kind);
void desktop_draw(void);

void term_init(void);
void term_key(int key);
void term_line(const char *line);
void term_redraw(void);
void term_clear(void);
void shell_exec(const char *command);

typedef struct {
    bool used;
    bool dir;
    int parent;
    char name[32];
    char data[FS_DATA];
} fs_node;
void fs_init(void);
int fs_resolve(const char *path);
int fs_create(const char *path, bool directory);
int fs_write(const char *path,const char *content);
int fs_delete(const char *path);
int fs_cd(const char *path);
void fs_pwd(char *buffer,size_t capacity);
int fs_cwd(void);
const fs_node *fs_node_at(int index);
#endif
