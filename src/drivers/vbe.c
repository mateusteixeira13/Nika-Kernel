#include <multiboot.h>
#include <stdint.h>
#include <vga.h>
#include <vbe.h>
#include <pit.h>

// GD = Graphical Driver

int gd_ok = 0;

// My struct yay!

typedef struct gd_framebuffer{
    uintptr_t addr;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
} gd_framebuffer_t;


static gd_framebuffer_t fb_info;

GD_STATUS get_framebuffer(multiboot_info_t* mbd, uint32_t magic){
    INFO("Initializing GD... getting the framebuffer\n");
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
        WARN("GD: Error! The magic is invalid!\n");
        return GERROR;
    }

    if(!(mbd->flags & (1 << 12))){
        WARN("GD: Error! No framebuffer\n");
        return GERROR;
    }

    fb_info.addr = (uintptr_t) mbd->framebuffer_addr;
    fb_info.width = mbd->framebuffer_width;
    fb_info.height = mbd->framebuffer_height;
    fb_info.pitch = mbd->framebuffer_pitch;
    fb_info.bpp = mbd->framebuffer_bpp;

    return GSUCCESS;
}

GD_STATUS gd_init(multiboot_info_t* mbd, uint32_t magic){
    if(get_framebuffer(mbd, magic) != 0){
        WARN("GD Failed\n");
        return GERROR;
    }

    gd_ok = 1;
    return GSUCCESS;
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color){
    if(gd_ok == 0){
        WARN("GD is not initialized\n");
        return;
    }

    if(x >= fb_info.width || y >= fb_info.height){
        return;
    }

    uint8_t* pixel = (uint8_t*)fb_info.addr + y * fb_info.pitch + x * (fb_info.bpp / 8);
    if(fb_info.bpp == 32){
        *(uint32_t*)pixel = color;
    } else if(fb_info.bpp == 24){
        pixel[0] = color & 0xFF;
        pixel[1] = (color >> 8) & 0xFF;
        pixel[2] = (color >> 16) & 0xFF;
    }
}

void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color){
    if(gd_ok == 0){
        WARN("GD is not initialized\n");
        return;
    }

    for(uint32_t i = 0; i < h; i++){
        for(uint32_t j = 0; j < w; j++){
            put_pixel(x + j, y + i, color);
        }
    }
}

void clear(uint32_t color){
    if(gd_ok == 0){
        WARN("GD is not initialized\n");
        return;
    }

    draw_rect(0, 0, fb_info.width, fb_info.height, color);
}

// Test the VBE

void init_graphical_test(){
    if(gd_ok == 0){
        WARN("GD is not initialized\n");
        return;
    }
    for(uint32_t y = 0; y < fb_info.height; y++){
        for(uint32_t x = 0; x < fb_info.width; x++){
            put_pixel(x, y, 0xFF0000);
        }
    }
    
}
