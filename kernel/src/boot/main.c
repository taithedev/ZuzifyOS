#include "os.h"

__attribute__((used,section(".limine_requests_start")))
static volatile uint64_t limine_start[] = LIMINE_REQUESTS_START_MARKER;
__attribute__((used,section(".limine_requests")))
static volatile uint64_t limine_revision[] = LIMINE_BASE_REVISION(6);
__attribute__((used,section(".limine_requests")))
static volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 0
};
__attribute__((used,section(".limine_requests_end")))
static volatile uint64_t limine_end[] = LIMINE_REQUESTS_END_MARKER;

static void halt_forever(void){for(;;)__asm__ volatile("cli; hlt");}

void kmain(void){
    serial_init();
    serial_write("ZuzifyOS kernel entry\n");
    if(!LIMINE_BASE_REVISION_SUPPORTED(limine_revision)){
        serial_write("ERROR: unsupported Limine base revision\n");
        halt_forever();
    }
    if(!fb_request.response || !fb_request.response->framebuffer_count){
        serial_write("ERROR: framebuffer unavailable\n");
        halt_forever();
    }
    struct limine_framebuffer *fb=fb_request.response->framebuffers[0];
    if(!fb || !fb->address || fb->bpp!=32 || fb->memory_model!=LIMINE_FRAMEBUFFER_RGB){
        serial_write("ERROR: requires a 32-bit RGB framebuffer\n");
        halt_forever();
    }
    gfx_init(fb);
    fs_init();
    term_init();
    term_redraw();
    serial_write("ZUZIFYOS_BOOT_OK\n");
    for(;;){
        int key=kbd_poll();
        if(key)term_key(key);
        __asm__ volatile("pause");
    }
}
