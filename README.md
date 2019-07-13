# f1c100s-gpio-tools
Command-line access to F1C100s GPIO
`
root@f1c100s:~#gpio
usage: gpio help
       gpio read  [PA0|PA1...]
       gpio write [PA0|PA1...]
       gpio mode  [PA0|PA1...] [in|out|...]
       gpio readall
       gpio dumpall
       gpio info
-- github.com/wuxx/f1c100s-gpio-tools --

root@f1c100s:~#gpio read PA0
PA0: 1

root@f1c100s:~#gpio write PA0 0
root@f1c100s:~#gpio read PA0
PA0: 0
root@f1c100s:~#gpio mode PA0 in

root@f1c100s:~#gpio readall
        value   mode
PA:
PA0:    0       [in]
PA1:    0       [disabled]
PA2:    0       [out]
PA3:    1       [out]
PB:
PB3:    0       [disabled]
PC:
PC0:    0       [spi0_clk]
PC1:    0       [spi0_cs]
PC2:    0       [spi0_miso]
PC3:    0       [spi0_mosi]
PD:
PD0:    0       [lcd_d2]
PD1:    0       [lcd_d3]
PD2:    0       [lcd_d4]
PD3:    0       [lcd_d5]
PD4:    0       [lcd_d6]
PD5:    0       [lcd_d7]
PD6:    0       [lcd_d10]
PD7:    0       [lcd_d11]
PD8:    0       [lcd_d12]
PD9:    0       [lcd_d13]
PD10:   0       [lcd_d14]
PD11:   0       [lcd_d15]
PD12:   0       [lcd_d18]
PD13:   0       [lcd_d19]
PD14:   0       [lcd_d20]
PD15:   0       [lcd_d21]
PD16:   0       [lcd_d22]
PD17:   0       [lcd_d23]
PD18:   0       [lcd_clk]
PD19:   0       [lcd_de]
PD20:   0       [lcd_hsync]
PD21:   0       [lcd_vsync]
PE:
PE0:    0       [uart0_rx]
PE1:    0       [uart0_tx]
PE2:    0       [out]
PE3:    1       [out]
PE4:    1       [out]
PE5:    1       [in]
PE6:    1       [out]
PE7:    0       [disabled]
PE8:    0       [disabled]
PE9:    0       [disabled]
PE10:   1       [out]
PE11:   0       [twi0_sck]
PE12:   0       [twi0_sda]
PF:
PF0:    0       [sdc0_d1]
PF1:    0       [sdc0_d0]
PF2:    0       [sdc0_clk]
PF3:    0       [sdc0_cmd]
PF4:    0       [sdc0_d3]
PF5:    0       [sdc0_d2]

root@f1c100s:~#gpio info
PA0:    (in out tp_x1 reserved da_bclk uart1_rts spi1_cs disabled )
PA1:    (in out tp_x2 reserved da_lrck uart1_cts spi1_miso disabled )
PA2:    (in out tp_y1 pwm0 da_in uart1_rx spi1_clk disabled )
PA3:    (in out tp_y2 ir_rx da_out uart1_tx spi1_miso disabled )
PB3:    (in out ddr_ref_d ir_rx reserved reserved reserved disabled )
PC0:    (reserved out spi0_clk sdc1_clk reserved reserved reserved disabled )
PC1:    (in out spi0_cs sdc1_cmd reserved reserved reserved disabled )
PC2:    (in out spi0_miso sdc1_d0 reserved reserved reserved disabled )
PC3:    (in out spi0_mosi uart0_tx reserved reserved reserved disabled )
PD0:    (in out lcd_d2 twi0_sda rsb_sda reserved eintd0 disabled )
PD1:    (in out lcd_d3 uart1_rts reserved reserved eintd1 disabled )
PD2:    (in out lcd_d4 uart1_cts reserved reserved eintd2 disabled )
PD3:    (in out lcd_d5 uart1_rx reserved reserved eintd3 disabled )
PD4:    (in out lcd_d6 uart1_tx reserved reserved eintd4 disabled )
PD5:    (in out lcd_d7 twi1_sck reserved reserved eintd5 disabled )
PD6:    (in out lcd_d10 twi1_sda reserved reserved eintd6 disabled )
PD7:    (in out lcd_d11 da_mclk reserved reserved eintd7 disabled )
PD8:    (in out lcd_d12 da_bclk reserved reserved eintd8 disabled )
PD9:    (in out lcd_d13 da_lrck reserved reserved eintd9 disabled )
PD10:   (in out lcd_d14 da_in reserved reserved eintd10 disabled )
PD11:   (in out lcd_d15 da_out reserved reserved eintd11 disabled )
PD12:   (in out lcd_d18 twi0_sck rsb_sck reserved eintd12 disabled )
PD13:   (in out lcd_d19 uart2_tx rsb_sck reserved eintd13 disabled )
PD14:   (in out lcd_d20 uart2_rx reserved reserved eintd14 disabled )
PD15:   (in out lcd_d21 uart2_rts twi2_sck reserved eintd15 disabled )
PD16:   (in out lcd_d22 uart2_cts twi2_sda reserved eintd16 disabled )
PD17:   (in out lcd_d23 owa_out reserved reserved eintd17 disabled )
PD18:   (in out lcd_clk spi0_cs reserved reserved eintd18 disabled )
PD19:   (in out lcd_de spi0_mosi reserved reserved eintd19 disabled )
PD20:   (in out lcd_hsync spi0_clk reserved reserved eintd20 disabled )
PD21:   (in out lcd_vsync spi0_miso reserved reserved eintd21 disabled )
PE0:    (in out csi_hsync lcd_d0 twi2_sck uart0_rx einte0 disabled )
PE1:    (in out csi_vsync lcd_d1 twi2_sda uart0_tx einte1 disabled )
PE2:    (in out csi_pclk lcd_d8 clk_out reserved einte2 disabled )
PE3:    (in out csi_d0 lcd_d9 da_bclk rsb_sck einte3 disabled )
PE4:    (in out csi_d1 lcd_d16 da_lrck rsb_sda einte4 disabled )
PE5:    (in out csi_d2 lcd_d17 da_in reserved einte5 disabled )
PE6:    (in out csi_d3 pwm1 da_out owa_out einte6 disabled )
PE7:    (in out csi_d4 uart2_tx spi1_cs reserved einte7 disabled )
PE8:    (in out csi_d5 uart2_rx spi1_mosi reserved einte8 disabled )
PE9:    (in out csi_d6 uart2_rts spi1_clk reserved einte9 disabled )
PE10:   (in out csi_d7 uart2_cts spi1_miso reserved einte10 disabled )
PE11:   (in out clk_out twi0_sck ir_rx reserved einte11 disabled )
PE12:   (in out da_mclk twi0_sda pwm0 reserved einte12 disabled )
PF0:    (in out sdc0_d1 dbg_ms ir_rx reserved eintf0 disabled )
PF1:    (in out sdc0_d0 dbg_di reserved reserved eintf1 disabled )
PF2:    (in out sdc0_clk uart0_tx reserved reserved eintf2 disabled )
PF3:    (in out sdc0_cmd dbg_do reserved reserved eintf3 disabled )
PF4:    (in out sdc0_d3 uart0_tx reserved reserved eintf4 disabled )
PF5:    (in out sdc0_d2 dbg_ck pwm1 reserved eintf5 disabled )

root@f1c100s:~#gpio dumpall
PA:
GPIO_CFG0: 0x00001170
GPIO_CFG1: 0x00000000
GPIO_CFG2: 0x00000000
GPIO_CFG3: 0x00000000
GPIO_DATA: 0x00000008
GPIO_DRV0: 0x00000055
GPIO_DRV1: 0x00000000
GPIO_PUL0: 0x00000000
GPIO_PUL1: 0x00000000
PB:
GPIO_CFG0: 0x00007222
GPIO_CFG1: 0x00000000
GPIO_CFG2: 0x00000000
GPIO_CFG3: 0x00000000
GPIO_DATA: 0x00000000
GPIO_DRV0: 0x00000055
GPIO_DRV1: 0x00000000
GPIO_PUL0: 0x00000000
GPIO_PUL1: 0x00000000
PC:
GPIO_CFG0: 0x00002222
GPIO_CFG1: 0x00000000
GPIO_CFG2: 0x00000000
GPIO_CFG3: 0x00000000
GPIO_DATA: 0x00000000
GPIO_DRV0: 0x00000000
GPIO_DRV1: 0x00000000
GPIO_PUL0: 0x00000000
GPIO_PUL1: 0x00000000
PD:
GPIO_CFG0: 0x22222222
GPIO_CFG1: 0x22222222
GPIO_CFG2: 0x00222222
GPIO_CFG3: 0x00000000
GPIO_DATA: 0x00000000
GPIO_DRV0: 0x55555555
GPIO_DRV1: 0x00000555
GPIO_PUL0: 0x00000000
GPIO_PUL1: 0x00000000
PE:
GPIO_CFG0: 0x71011155
GPIO_CFG1: 0x00033177
GPIO_CFG2: 0x00000000
GPIO_CFG3: 0x00000000
GPIO_DATA: 0x00000478
GPIO_DRV0: 0x01555555
GPIO_DRV1: 0x00000000
GPIO_PUL0: 0x00000004
GPIO_PUL1: 0x00000000
PF:
GPIO_CFG0: 0x00222222
GPIO_CFG1: 0x00000000
GPIO_CFG2: 0x00000000
GPIO_CFG3: 0x00000000
GPIO_DATA: 0x00000000
GPIO_DRV0: 0x00000aaa
GPIO_DRV1: 0x00000000
GPIO_PUL0: 0x00000555
GPIO_PUL1: 0x00000000
`
