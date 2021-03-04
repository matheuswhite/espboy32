idf_component_register(SRCS
                        "src/clock.c"
                        "src/cartridge.c"
                        "src/memory.c"
                        "src/rom_only.c"
                        "src/mbc1.c"
                        "src/mbc2.c"
                        "src/mbc3.c"
                        "src/mbc5.c"
                       INCLUDE_DIRS "include"
                       REQUIRES lcd)
