#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* author: wuxx */

enum GPIO_GROUP_E {
    PA = 0,
    PB = 1,
    PC = 2,
    PD = 3,
    PE = 4,
    PF = 5,
};

enum GPIO_MODE_E {
    IN = 0,
    OUT = 1,
};

#define GPIO_BASE   (0x01C20800)

#define GPIO_CFG0(n) (GPIO_BASE + ((n) * 0x24) + 0x0)
#define GPIO_CFG1(n) (GPIO_BASE + ((n) * 0x24) + 0x4)
#define GPIO_CFG2(n) (GPIO_BASE + ((n) * 0x24) + 0x8)
#define GPIO_CFG3(n) (GPIO_BASE + ((n) * 0x24) + 0xC)
#define GPIO_DATA(n) (GPIO_BASE + ((n) * 0x24) + 0x10)
#define GPIO_DRV0(n) (GPIO_BASE + ((n) * 0x24) + 0x14)
#define GPIO_DRV1(n) (GPIO_BASE + ((n) * 0x24) + 0x18)
#define GPIO_PUL0(n) (GPIO_BASE + ((n) * 0x24) + 0x1C)
#define GPIO_PUL1(n) (GPIO_BASE + ((n) * 0x24) + 0x20)

#define MAKE_PID(group, index) (#group#index), ((group) << 5 | index)

#define get_bit(x, index)       (((x) >> index) & 0x1)


typedef unsigned char u8;
typedef   signed char s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned int u32;
typedef   signed int s32;

/* pio */
/* |     |         */
/* 7 6 5 4 3 2 1 0 */
struct gpio_info {
    char name[16];  /* PA, PB, PC, PD, PE, PF */
    u8 pio;
    u32 cfg_addr;
    u32 cfg_off;
    u32 mode; /* 3 bit */
    char mode_desc[32];
};

/* 6 * 32 * 8 (PA-PF = 6, io0-io32,  8 mode) */
u32 gindex = 0;
struct gpio_info gi[2048];

/* ADD(PA, offset, mode, mode_desc)*/

void set_bit(unsigned int *x, unsigned int bit_index, unsigned int b)
{
    unsigned int _x;
    unsigned int bit_mask;
    _x = *x;
    if (get_bit(_x, bit_index) != b) {
        if (b == 0) {
            bit_mask = ~(0x1 << bit_index);
            *x = (_x) & bit_mask;
        } else {    /* b == 1 */
            bit_mask = (0x1 << bit_index);
            *x = (_x) | bit_mask;
        }
    }
}


u32 writel(u32 addr, u32 data)
{
    FILE *fp;
    u32 value = 0;
    char cmd[1024];

    snprintf(cmd, sizeof(cmd), "devmem 0x%08x 32 0x%08x", addr, data);

    fp = popen(cmd, "r");

    if (fp == NULL) {
        fprintf(stderr, "%s: popen(%s) fail!\n", __func__, cmd);
        goto err;
    }

err:
    pclose(fp);
    return value;

}

u32 readl(u32 addr)
{
    FILE *fp;
    u32 value = 0;
    char cmd[1024];
    char buf[32];

    snprintf(cmd, sizeof(cmd), "devmem 0x%08x", addr);

    fp = popen(cmd, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s: popen(%s) fail!\n", __func__, cmd);
    }

    fread(buf, sizeof(char), sizeof(buf), fp);
    sscanf(buf, "0x%x", &value);

err:
    pclose(fp);
    return value;
}

struct gpio_info * gi_get(char *name, char *mode)
{
    u32 i;
    for(i = 0; i < gindex; i++) {
        if ((strcmp(gi[i].name, name) == 0)) {
            if (mode == NULL) {
                return &(gi[i]);
            } else {
                if (strcmp(gi[i].mode_desc, mode) == 0) {
                    return &(gi[i]);
                }
            }
        }
    }
    return NULL;
}
char * gi_get_mode_desc(u8 pio, u32 mode)
{
    u32 i;
    for(i = 0; i < gindex; i++) {
        if (gi[i].pio == pio && gi[i].mode == mode) {
            return gi[i].mode_desc;
        }
    }

    fprintf(stdout, "%s(0x%x, 0x%x) fail\n", __func__, pio, mode);
    return NULL;
}

void gi_add(char *name, u8 pio, u32 cfg_addr, u32 offset, u32 mode, char *mode_desc)
{
    if (gindex >= (sizeof(gi) / sizeof(gi[0]))) {
        fprintf(stderr, "gindex overflow!\n");
        exit(-1);
    }

    strncpy(gi[gindex].name, name, sizeof(gi[gindex].name));
    gi[gindex].pio = pio;
    gi[gindex].cfg_addr = cfg_addr;
    gi[gindex].cfg_off  = offset;
    gi[gindex].mode = mode;
    strncpy(gi[gindex].mode_desc, mode_desc, sizeof(gi[gindex].mode_desc));

    gindex++;
}

void gi_init()
{
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 0, "in");
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 1, "out");
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 2, "tp_x1");
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 3, "reserved");
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 4, "da_bclk");
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 5, "uart1_rts");
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 6, "spi1_cs");
    gi_add(MAKE_PID(PA, 0), GPIO_CFG0(PA), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 0, "in");
    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 1, "out");
    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 2, "tp_x2");
    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 3, "reserved");
    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 4, "da_lrck");
    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 5, "uart1_cts");
    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 6, "spi1_miso");
    gi_add(MAKE_PID(PA, 1), GPIO_CFG0(PA), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 0, "in");
    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 1, "out");
    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 2, "tp_y1");
    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 3, "pwm0");
    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 4, "da_in");
    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 5, "uart1_rx");
    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 6, "spi1_clk");
    gi_add(MAKE_PID(PA, 2), GPIO_CFG0(PA), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 0, "in");
    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 1, "out");
    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 2, "tp_y2");
    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 3, "ir_rx");
    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 4, "da_out");
    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 5, "uart1_tx");
    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 6, "spi1_miso");
    gi_add(MAKE_PID(PA, 3), GPIO_CFG0(PA), 0xc, 7, "disabled");


    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 0, "in");
    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 1, "out");
    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 2, "ddr_ref_d");
    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 3, "ir_rx");
    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 4, "reserved");
    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 5, "reserved");
    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 6, "reserved");
    gi_add(MAKE_PID(PB, 3), GPIO_CFG0(PB), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 0, "reserved");
    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 1, "out");
    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 2, "spi0_clk");
    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 3, "sdc1_clk");
    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 4, "reserved");
    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 5, "reserved");
    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 6, "reserved");
    gi_add(MAKE_PID(PC, 0), GPIO_CFG0(PC), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 0, "in");
    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 1, "out");
    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 2, "spi0_cs");
    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 3, "sdc1_cmd");
    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 4, "reserved");
    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 5, "reserved");
    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 6, "reserved");
    gi_add(MAKE_PID(PC, 1), GPIO_CFG0(PC), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 0, "in");
    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 1, "out");
    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 2, "spi0_miso");
    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 3, "sdc1_d0");
    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 4, "reserved");
    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 5, "reserved");
    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 6, "reserved");
    gi_add(MAKE_PID(PC, 2), GPIO_CFG0(PC), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 0, "in");
    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 1, "out");
    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 2, "spi0_mosi");
    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 3, "uart0_tx");
    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 4, "reserved");
    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 5, "reserved");
    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 6, "reserved");
    gi_add(MAKE_PID(PC, 3), GPIO_CFG0(PC), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 0, "in");
    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 1, "out");
    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 2, "lcd_d2");
    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 3, "twi0_sda");
    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 4, "rsb_sda");
    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 5, "reserved");
    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 6, "eintd0");
    gi_add(MAKE_PID(PD, 0), GPIO_CFG0(PD), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 0, "in");
    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 1, "out");
    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 2, "lcd_d3");
    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 3, "uart1_rts");
    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 4, "reserved");
    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 5, "reserved");
    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 6, "eintd1");
    gi_add(MAKE_PID(PD, 1), GPIO_CFG0(PD), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 0, "in");
    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 1, "out");
    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 2, "lcd_d4");
    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 3, "uart1_cts");
    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 4, "reserved");
    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 5, "reserved");
    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 6, "eintd2");
    gi_add(MAKE_PID(PD, 2), GPIO_CFG0(PD), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 0, "in");
    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 1, "out");
    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 2, "lcd_d5");
    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 3, "uart1_rx");
    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 4, "reserved");
    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 5, "reserved");
    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 6, "eintd3");
    gi_add(MAKE_PID(PD, 3), GPIO_CFG0(PD), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 0, "in");
    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 1, "out");
    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 2, "lcd_d6");
    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 3, "uart1_tx");
    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 4, "reserved");
    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 5, "reserved");
    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 6, "eintd4");
    gi_add(MAKE_PID(PD, 4), GPIO_CFG0(PD), 0x10, 7, "disabled");

    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 0, "in");
    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 1, "out");
    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 2, "lcd_d7");
    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 3, "twi1_sck");
    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 4, "reserved");
    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 5, "reserved");
    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 6, "eintd5");
    gi_add(MAKE_PID(PD, 5), GPIO_CFG0(PD), 0x14, 7, "disabled");

    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 0, "in");
    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 1, "out");
    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 2, "lcd_d10");
    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 3, "twi1_sda");
    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 4, "reserved");
    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 5, "reserved");
    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 6, "eintd6");
    gi_add(MAKE_PID(PD, 6), GPIO_CFG0(PD), 0x18, 7, "disabled");

    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 0, "in");
    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 1, "out");
    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 2, "lcd_d11");
    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 3, "da_mclk");
    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 4, "reserved");
    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 5, "reserved");
    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 6, "eintd7");
    gi_add(MAKE_PID(PD, 7), GPIO_CFG0(PD), 0x1c, 7, "disabled");

    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 0, "in");
    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 1, "out");
    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 2, "lcd_d12");
    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 3, "da_bclk");
    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 4, "reserved");
    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 5, "reserved");
    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 6, "eintd8");
    gi_add(MAKE_PID(PD, 8), GPIO_CFG1(PD), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 0, "in");
    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 1, "out");
    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 2, "lcd_d13");
    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 3, "da_lrck");
    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 4, "reserved");
    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 5, "reserved");
    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 6, "eintd9");
    gi_add(MAKE_PID(PD, 9), GPIO_CFG1(PD), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 0, "in");
    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 1, "out");
    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 2, "lcd_d14");
    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 3, "da_in");
    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 4, "reserved");
    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 5, "reserved");
    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 6, "eintd10");
    gi_add(MAKE_PID(PD, 10), GPIO_CFG1(PD), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 0, "in");
    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 1, "out");
    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 2, "lcd_d15");
    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 3, "da_out");
    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 4, "reserved");
    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 5, "reserved");
    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 6, "eintd11");
    gi_add(MAKE_PID(PD, 11), GPIO_CFG1(PD), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 0, "in");
    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 1, "out");
    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 2, "lcd_d18");
    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 3, "twi0_sck");
    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 4, "rsb_sck");
    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 5, "reserved");
    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 6, "eintd12");
    gi_add(MAKE_PID(PD, 12), GPIO_CFG1(PD), 0x10, 7, "disabled");

    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 0, "in");
    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 1, "out");
    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 2, "lcd_d19");
    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 3, "uart2_tx");
    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 4, "rsb_sck");
    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 5, "reserved");
    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 6, "eintd13");
    gi_add(MAKE_PID(PD, 13), GPIO_CFG1(PD), 0x14, 7, "disabled");

    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 0, "in");
    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 1, "out");
    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 2, "lcd_d20");
    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 3, "uart2_rx");
    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 4, "reserved");
    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 5, "reserved");
    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 6, "eintd14");
    gi_add(MAKE_PID(PD, 14), GPIO_CFG1(PD), 0x18, 7, "disabled");

    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 0, "in");
    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 1, "out");
    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 2, "lcd_d21");
    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 3, "uart2_rts");
    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 4, "twi2_sck");
    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 5, "reserved");
    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 6, "eintd15");
    gi_add(MAKE_PID(PD, 15), GPIO_CFG1(PD), 0x1c, 7, "disabled");

    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 0, "in");
    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 1, "out");
    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 2, "lcd_d22");
    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 3, "uart2_cts");
    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 4, "twi2_sda");
    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 5, "reserved");
    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 6, "eintd16");
    gi_add(MAKE_PID(PD, 16), GPIO_CFG2(PD), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 0, "in");
    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 1, "out");
    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 2, "lcd_d23");
    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 3, "owa_out");
    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 4, "reserved");
    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 5, "reserved");
    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 6, "eintd17");
    gi_add(MAKE_PID(PD, 17), GPIO_CFG2(PD), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 0, "in");
    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 1, "out");
    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 2, "lcd_clk");
    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 3, "spi0_cs");
    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 4, "reserved");
    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 5, "reserved");
    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 6, "eintd18");
    gi_add(MAKE_PID(PD, 18), GPIO_CFG2(PD), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 0, "in");
    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 1, "out");
    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 2, "lcd_de");
    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 3, "spi0_mosi");
    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 4, "reserved");
    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 5, "reserved");
    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 6, "eintd19");
    gi_add(MAKE_PID(PD, 19), GPIO_CFG2(PD), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 0, "in");
    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 1, "out");
    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 2, "lcd_hsync");
    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 3, "spi0_clk");
    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 4, "reserved");
    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 5, "reserved");
    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 6, "eintd20");
    gi_add(MAKE_PID(PD, 20), GPIO_CFG2(PD), 0x10, 7, "disabled");

    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 0, "in");
    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 1, "out");
    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 2, "lcd_vsync");
    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 3, "spi0_miso");
    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 4, "reserved");
    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 5, "reserved");
    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 6, "eintd21");
    gi_add(MAKE_PID(PD, 21), GPIO_CFG2(PD), 0x14, 7, "disabled");

    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 0, "in");
    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 1, "out");
    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 2, "csi_hsync");
    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 3, "lcd_d0");
    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 4, "twi2_sck");
    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 5, "uart0_rx");
    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 6, "einte0");
    gi_add(MAKE_PID(PE, 0), GPIO_CFG0(PE), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 0, "in");
    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 1, "out");
    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 2, "csi_vsync");
    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 3, "lcd_d1");
    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 4, "twi2_sda");
    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 5, "uart0_tx");
    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 6, "einte1");
    gi_add(MAKE_PID(PE, 1), GPIO_CFG0(PE), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 0, "in");
    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 1, "out");
    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 2, "csi_pclk");
    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 3, "lcd_d8");
    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 4, "clk_out");
    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 5, "reserved");
    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 6, "einte2");
    gi_add(MAKE_PID(PE, 2), GPIO_CFG0(PE), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 0, "in");
    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 1, "out");
    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 2, "csi_d0");
    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 3, "lcd_d9");
    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 4, "da_bclk");
    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 5, "rsb_sck");
    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 6, "einte3");
    gi_add(MAKE_PID(PE, 3), GPIO_CFG0(PE), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 0, "in");
    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 1, "out");
    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 2, "csi_d1");
    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 3, "lcd_d16");
    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 4, "da_lrck");
    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 5, "rsb_sda");
    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 6, "einte4");
    gi_add(MAKE_PID(PE, 4), GPIO_CFG0(PE), 0x10, 7, "disabled");

    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 0, "in");
    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 1, "out");
    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 2, "csi_d2");
    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 3, "lcd_d17");
    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 4, "da_in");
    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 5, "reserved");
    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 6, "einte5");
    gi_add(MAKE_PID(PE, 5), GPIO_CFG0(PE), 0x14, 7, "disabled");

    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 0, "in");
    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 1, "out");
    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 2, "csi_d3");
    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 3, "pwm1");
    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 4, "da_out");
    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 5, "owa_out");
    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 6, "einte6");
    gi_add(MAKE_PID(PE, 6), GPIO_CFG0(PE), 0x18, 7, "disabled");

    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 0, "in");
    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 1, "out");
    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 2, "csi_d4");
    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 3, "uart2_tx");
    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 4, "spi1_cs");
    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 5, "reserved");
    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 6, "einte7");
    gi_add(MAKE_PID(PE, 7), GPIO_CFG0(PE), 0x1c, 7, "disabled");

    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 0, "in");
    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 1, "out");
    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 2, "csi_d5");
    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 3, "uart2_rx");
    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 4, "spi1_mosi");
    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 5, "reserved");
    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 6, "einte8");
    gi_add(MAKE_PID(PE, 8), GPIO_CFG1(PE), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 0, "in");
    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 1, "out");
    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 2, "csi_d6");
    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 3, "uart2_rts");
    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 4, "spi1_clk");
    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 5, "reserved");
    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 6, "einte9");
    gi_add(MAKE_PID(PE, 9), GPIO_CFG1(PE), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 0, "in");
    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 1, "out");
    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 2, "csi_d7");
    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 3, "uart2_cts");
    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 4, "spi1_miso");
    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 5, "reserved");
    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 6, "einte10");
    gi_add(MAKE_PID(PE, 10), GPIO_CFG1(PE), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 0, "in");
    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 1, "out");
    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 2, "clk_out");
    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 3, "twi0_sck");
    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 4, "ir_rx");
    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 5, "reserved");
    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 6, "einte11");
    gi_add(MAKE_PID(PE, 11), GPIO_CFG1(PE), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 0, "in");
    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 1, "out");
    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 2, "da_mclk");
    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 3, "twi0_sda");
    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 4, "pwm0");
    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 5, "reserved");
    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 6, "einte12");
    gi_add(MAKE_PID(PE, 12), GPIO_CFG1(PE), 0x10, 7, "disabled");

    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 0, "in");
    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 1, "out");
    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 2, "sdc0_d1");
    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 3, "dbg_ms");
    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 4, "ir_rx");
    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 5, "reserved");
    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 6, "eintf0");
    gi_add(MAKE_PID(PF, 0), GPIO_CFG0(PF), 0x0, 7, "disabled");

    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 0, "in");
    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 1, "out");
    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 2, "sdc0_d0");
    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 3, "dbg_di");
    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 4, "reserved");
    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 5, "reserved");
    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 6, "eintf1");
    gi_add(MAKE_PID(PF, 1), GPIO_CFG0(PF), 0x4, 7, "disabled");

    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 0, "in");
    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 1, "out");
    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 2, "sdc0_clk");
    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 3, "uart0_tx");
    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 4, "reserved");
    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 5, "reserved");
    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 6, "eintf2");
    gi_add(MAKE_PID(PF, 2), GPIO_CFG0(PF), 0x8, 7, "disabled");

    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 0, "in");
    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 1, "out");
    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 2, "sdc0_cmd");
    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 3, "dbg_do");
    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 4, "reserved");
    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 5, "reserved");
    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 6, "eintf3");
    gi_add(MAKE_PID(PF, 3), GPIO_CFG0(PF), 0xc, 7, "disabled");

    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 0, "in");
    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 1, "out");
    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 2, "sdc0_d3");
    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 3, "uart0_tx");
    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 4, "reserved");
    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 5, "reserved");
    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 6, "eintf4");
    gi_add(MAKE_PID(PF, 4), GPIO_CFG0(PF), 0x10, 7, "disabled");

    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 0, "in");
    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 1, "out");
    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 2, "sdc0_d2");
    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 3, "dbg_ck");
    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 4, "pwm1");
    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 5, "reserved");
    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 6, "eintf5");
    gi_add(MAKE_PID(PF, 5), GPIO_CFG0(PF), 0x14, 7, "disabled");
}

void usage(char *program_name)
{
    fprintf(stdout, "usage: %s help \n", program_name);
    fprintf(stdout, "       %s read  [PA0|PA1...] \n", program_name);
    fprintf(stdout, "       %s write [PA0|PA1...] \n", program_name);
    fprintf(stdout, "       %s mode  [PA0|PA1...] [in|out|...]\n", program_name);
    fprintf(stdout, "       %s readall\n", program_name);
    fprintf(stdout, "       %s dumpall\n", program_name);
    fprintf(stdout, "       %s info\n", program_name);
    fprintf(stdout, "-- github.com/wuxx/f1c100s-gpio-tools --\n");
    exit(0);
}

void dump_gpio_info()
{
    u32 i;
    u32 group, index;
    u32 cfg_value, cfg_mask;
    u32 dvalue;
    u32 current_group = PA;
    u32 current_index = 0;
    u32 mode = 0;
    u32 b = 0;

    fprintf(stdout, "\tvalue\tmode\n");
    fprintf(stdout, "PA:\n");

    for(i = 0; i < gindex; i = i + 8) {

        group = gi[i].pio >> 5;
        index = gi[i].pio & 0x1f;

        if (group != current_group) {
            fprintf(stdout, "P%c:\n", 'A' + group);
            current_group = group;
        }

        fprintf(stdout, "P%c%d:", 'A' + group, index);

        cfg_value = readl(gi[i].cfg_addr);
        mode = (cfg_value >> gi[i].cfg_off) & 0x7;

        dvalue = readl(GPIO_DATA(group));
        b = get_bit(dvalue, index);

        fprintf(stdout, "\t%d\t[%s]\n", b, gi_get_mode_desc(gi[i].pio, mode));
    }

}

void dump_gpio_mode()
{
    u32 i, x;
    u32 group, index;

    for(i = 0; i < gindex; i = i + 8) {
        group = gi[i].pio >> 5;
        index = gi[i].pio & 0x1f;

        fprintf(stdout, "P%c%d:\t(", 'A' + group, index);

        for(x = 0; x < 8; x++) {
            fprintf(stdout, "%s ", gi[i + x].mode_desc);
        }

        fprintf(stdout, ")\n");
    }
}

void dump_reg(u32 pio)
{
    fprintf(stdout, "GPIO_CFG0: 0x%08x\n", readl(GPIO_CFG0(pio)));
    fprintf(stdout, "GPIO_CFG1: 0x%08x\n", readl(GPIO_CFG1(pio)));
    fprintf(stdout, "GPIO_CFG2: 0x%08x\n", readl(GPIO_CFG2(pio)));
    fprintf(stdout, "GPIO_CFG3: 0x%08x\n", readl(GPIO_CFG3(pio)));
    fprintf(stdout, "GPIO_DATA: 0x%08x\n", readl(GPIO_DATA(pio)));
    fprintf(stdout, "GPIO_DRV0: 0x%08x\n", readl(GPIO_DRV0(pio)));
    fprintf(stdout, "GPIO_DRV1: 0x%08x\n", readl(GPIO_DRV1(pio)));
    fprintf(stdout, "GPIO_PUL0: 0x%08x\n", readl(GPIO_PUL0(pio)));
    fprintf(stdout, "GPIO_PUL1: 0x%08x\n", readl(GPIO_PUL1(pio)));
}

void dump_gpio_reg()
{
    fprintf(stdout, "PA:\n");
    dump_reg(PA);

    fprintf(stdout, "PB:\n");
    dump_reg(PB);

    fprintf(stdout, "PC:\n");
    dump_reg(PC);

    fprintf(stdout, "PD:\n");
    dump_reg(PD);

    fprintf(stdout, "PE:\n");
    dump_reg(PE);

    fprintf(stdout, "PF:\n");
    dump_reg(PF);
}


int main(int argc, char **argv)
{
    u32 group, index;
    u32 cfg_value, cfg_mask;
    u32 dvalue, b;
    char *m;

    struct gpio_info *pgi;

   	if (argc == 1) {
        usage(argv[0]);
	}

    gi_init();

    if (strcmp("read", argv[1]) == 0) {
        /* e.g. gpio read PA0 */
        if(argc != 3) {
            fprintf(stderr, "%s read pio, e.g. %s read PA0\n", argv[0], argv[0]);
            exit(-1);
        }
        pgi = gi_get(argv[2], NULL);

        if (pgi == NULL) {
            fprintf(stderr, "gi_get(%s) fail!\n", argv[2]);
            exit(-1);
        }

        group = pgi->pio >> 5;      /* high 3 bit */
        index = pgi->pio & 0x1f;    /* low 5 bit */
        dvalue = readl(GPIO_DATA(group));
        fprintf(stdout, "%s: %d\n", argv[2], get_bit(dvalue, index));

    } else if (strcmp("write", argv[1]) == 0) {
        /* e.g. gpio write PA0 1 */
        if (argc != 4) {
            fprintf(stderr, "%s write pio [0|1], e.g. %s write PA 1\n", argv[0], argv[0]);
            exit(-1);
        }

        pgi = gi_get(argv[2], NULL);
		b   = atoi(argv[3]);

        if (pgi == NULL) {
            fprintf(stderr, "gi_get(%s) fail!\n", argv[2]);
            exit(-1);
        }

        group = pgi->pio >> 5;      /* high 3 bit */
        index = pgi->pio & 0x1f;    /* low 5 bit */
            
        cfg_value = readl(pgi->cfg_addr);
        cfg_mask  = 0x7 << pgi->cfg_off;
        cfg_value = cfg_value & (~cfg_mask);
        cfg_value = cfg_value | OUT << pgi->cfg_off;
        writel(pgi->cfg_addr, cfg_value);

        dvalue = readl(GPIO_DATA(group));
        set_bit(&dvalue, index, b);
		writel(GPIO_DATA(group), dvalue);
    } else if (strcmp("mode", argv[1]) == 0) {
        /* e.g. gpio mode PA0 in|out|... */
        if (argc != 4) {
            fprintf(stderr, "%s mode pio [in|out|...], e.g. %s mode PA in\n", argv[0], argv[0]);
            exit(-1);
        }

		m   = argv[3];
        pgi = gi_get(argv[2], m);

        if (pgi == NULL) {
            fprintf(stderr, "gi_get(%s, %s) fail!\n", argv[2], m);
            exit(-1);
        }

        group = pgi->pio >> 5;      /* high 3 bit */
        index = pgi->pio & 0x1f;    /* low 5 bit */

        cfg_value = readl(pgi->cfg_addr);
        cfg_mask  = 0x7 << pgi->cfg_off;
        cfg_value = cfg_value & (~cfg_mask);
        cfg_value = cfg_value | pgi->mode << pgi->cfg_off;
        writel(pgi->cfg_addr, cfg_value);

    } else if (strcmp("readall", argv[1]) == 0) {
        dump_gpio_info();

    } else if (strcmp("dumpall", argv[1]) == 0) {
        dump_gpio_reg();
    } else if (strcmp("info", argv[1]) == 0) {
        dump_gpio_mode();
    } else {
        fprintf(stderr, "unknown cmd [%s]\n", argv[1]);
        usage(argv[0]);
    }


    return 0;
}
