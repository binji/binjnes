typedef enum {
  SIZE_0K,
  SIZE_256B,
  SIZE_1K,
  SIZE_2K,
  SIZE_4K,
  SIZE_8K,
  SIZE_16K,
  SIZE_24K,
  SIZE_32K,
  SIZE_40K,
  SIZE_48K,
  SIZE_56K,
  SIZE_64K,
  SIZE_72K,
  SIZE_88K,
  SIZE_96K,
  SIZE_112K,
  SIZE_128K,
  SIZE_232K,
  SIZE_256K,
  SIZE_512K,
  SIZE_520K,
  SIZE_1024K,
  SIZE_2048K,
} Size;

static const u32 s_sizes[] = {
  0,
  256,
  1024,
  2048,
  4096,
  8192,
  16384,
  24576,
  32768,
  40960,
  49152,
  57344,
  65536,
  73728,
  90112,
  98304,
  114688,
  131072,
  237568,
  262144,
  524288,
  532480,
  1048576,
  2097152,
};

typedef struct {
  u16 mapper;
  u8 submapper;
  Size prgrom, prgram, prgnvram;
  Size chrrom, chrram, chrnvram;
  System system;
  Mirror mirror;
  bool battery;
} Cart;

static const Cart s_carts[] = {
  /* 0 */ {.mapper=0,.prgrom=SIZE_32K,.prgnvram=SIZE_2K,.chrrom=SIZE_8K,.mirror=MIRROR_FOUR_SCREEN,.battery=1,},
  /* 1 */ {.mapper=0,.prgrom=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 2 */ {.mapper=0,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 3 */ {.mapper=0,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 4 */ {.mapper=0,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 5 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 6 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 7 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 8 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 9 */ {.mapper=0,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 10 */ {.mapper=0,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 11 */ {.mapper=0,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 12 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 13 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 14 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 15 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 16 */ {.mapper=0,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 17 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 18 */ {.mapper=0,.prgrom=SIZE_16K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 19 */ {.mapper=0,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 20 */ {.mapper=0,.prgrom=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 21 */ {.mapper=0,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 22 */ {.mapper=0,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 23 */ {.mapper=0,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 24 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 25 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 26 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 27 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 28 */ {.mapper=0,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 29 */ {.mapper=0,.prgrom=SIZE_16K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 30 */ {.mapper=0,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 31 */ {.mapper=0,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 32 */ {.mapper=0,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 33 */ {.mapper=0,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 34 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 35 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 36 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 37 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 38 */ {.mapper=0,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 39 */ {.mapper=0,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 40 */ {.mapper=0,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 41 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 42 */ {.mapper=0,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 43 */ {.mapper=0,.prgrom=SIZE_48K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 44 */ {.mapper=0,.prgrom=SIZE_32K,.prgnvram=SIZE_2K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 45 */ {.mapper=0,.prgrom=SIZE_32K,.prgnvram=SIZE_4K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 46 */ {.mapper=0,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 47 */ {.mapper=0,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_PAL,},
  /* 48 */ {.mapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 49 */ {.mapper=1,.prgrom=SIZE_64K,.prgram=SIZE_2K,.chrrom=SIZE_32K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 50 */ {.mapper=1,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 51 */ {.mapper=1,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 52 */ {.mapper=1,.prgrom=SIZE_16K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 53 */ {.mapper=1,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 54 */ {.mapper=1,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 55 */ {.mapper=1,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 56 */ {.mapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 57 */ {.mapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 58 */ {.mapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 59 */ {.mapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 60 */ {.mapper=1,.prgrom=SIZE_64K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 61 */ {.mapper=1,.prgrom=SIZE_64K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 62 */ {.mapper=1,.prgrom=SIZE_64K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 63 */ {.mapper=1,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 64 */ {.mapper=1,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 65 */ {.mapper=1,.prgrom=SIZE_64K,.prgram=SIZE_8K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 66 */ {.mapper=1,.prgrom=SIZE_64K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 67 */ {.mapper=1,.prgrom=SIZE_64K,.prgram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 68 */ {.mapper=1,.prgrom=SIZE_64K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 69 */ {.mapper=1,.prgrom=SIZE_64K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 70 */ {.mapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 71 */ {.mapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 72 */ {.mapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 73 */ {.mapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 74 */ {.mapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 75 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 76 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 77 */ {.mapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 78 */ {.mapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 79 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 80 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_72K,.mirror=MIRROR_HORIZONTAL,},
  /* 81 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 82 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 83 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 84 */ {.mapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 85 */ {.mapper=1,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 86 */ {.mapper=1,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 87 */ {.mapper=1,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 88 */ {.mapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 89 */ {.mapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 90 */ {.mapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 91 */ {.mapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 92 */ {.mapper=1,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 93 */ {.mapper=1,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 94 */ {.mapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 95 */ {.mapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 96 */ {.mapper=1,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 97 */ {.mapper=1,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 98 */ {.mapper=1,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 99 */ {.mapper=1,.prgrom=SIZE_512K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 100 */ {.mapper=1,.prgrom=SIZE_512K,.prgram=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 101 */ {.mapper=1,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 102 */ {.mapper=1,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 103 */ {.mapper=1,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 104 */ {.mapper=1,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 105 */ {.mapper=1,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 106 */ {.mapper=1,.prgrom=SIZE_64K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 107 */ {.mapper=1,.prgrom=SIZE_64K,.prgnvram=SIZE_8K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 108 */ {.mapper=1,.prgrom=SIZE_64K,.prgnvram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 109 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 110 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 111 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 112 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 113 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 114 */ {.mapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.prgnvram=SIZE_8K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 115 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 116 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_112K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 117 */ {.mapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 118 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 119 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 120 */ {.mapper=1,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 121 */ {.mapper=1,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 122 */ {.mapper=1,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 123 */ {.mapper=1,.prgrom=SIZE_256K,.prgnvram=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 124 */ {.mapper=1,.prgrom=SIZE_256K,.prgram=SIZE_8K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 125 */ {.mapper=1,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_48K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 126 */ {.mapper=1,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 127 */ {.mapper=1,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 128 */ {.mapper=1,.prgrom=SIZE_512K,.prgnvram=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 129 */ {.mapper=1,.submapper=5,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 130 */ {.mapper=1,.submapper=5,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 131 */ {.mapper=1,.submapper=5,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 132 */ {.mapper=1,.submapper=5,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 133 */ {.mapper=1,.submapper=5,.prgrom=SIZE_32K,.chrrom=SIZE_40K,.mirror=MIRROR_HORIZONTAL,},
  /* 134 */ {.mapper=1,.submapper=5,.prgrom=SIZE_32K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 135 */ {.mapper=1,.submapper=5,.prgrom=SIZE_32K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 136 */ {.mapper=1,.submapper=5,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 137 */ {.mapper=2,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 138 */ {.mapper=2,.prgrom=SIZE_48K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 139 */ {.mapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 140 */ {.mapper=2,.submapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 141 */ {.mapper=2,.submapper=1,.prgrom=SIZE_1024K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 142 */ {.mapper=2,.submapper=1,.prgrom=SIZE_64K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 143 */ {.mapper=2,.submapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 144 */ {.mapper=2,.submapper=1,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 145 */ {.mapper=2,.submapper=1,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 146 */ {.mapper=2,.submapper=1,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 147 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.prgram=SIZE_2K,.chrram=SIZE_8K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 148 */ {.mapper=2,.submapper=2,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 149 */ {.mapper=2,.submapper=2,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 150 */ {.mapper=2,.submapper=2,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 151 */ {.mapper=2,.submapper=2,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 152 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 153 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 154 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 155 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 156 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 157 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 158 */ {.mapper=2,.submapper=2,.prgrom=SIZE_16K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 159 */ {.mapper=2,.submapper=2,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 160 */ {.mapper=2,.submapper=2,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 161 */ {.mapper=2,.submapper=2,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 162 */ {.mapper=2,.submapper=2,.prgrom=SIZE_64K,.chrram=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 163 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 164 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 165 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 166 */ {.mapper=2,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 167 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 168 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 169 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 170 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 171 */ {.mapper=2,.submapper=2,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 172 */ {.mapper=2,.submapper=2,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 173 */ {.mapper=2,.submapper=2,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 174 */ {.mapper=3,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 175 */ {.mapper=3,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 176 */ {.mapper=3,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 177 */ {.mapper=3,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 178 */ {.mapper=3,.submapper=1,.prgrom=SIZE_32K,.prgram=SIZE_2K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 179 */ {.mapper=3,.submapper=1,.prgrom=SIZE_16K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 180 */ {.mapper=3,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 181 */ {.mapper=3,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 182 */ {.mapper=3,.submapper=2,.prgrom=SIZE_16K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 183 */ {.mapper=3,.submapper=2,.prgrom=SIZE_16K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 184 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 185 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 186 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_24K,.mirror=MIRROR_HORIZONTAL,},
  /* 187 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 188 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 189 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 190 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.prgram=SIZE_2K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 191 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 192 */ {.mapper=3,.submapper=2,.prgrom=SIZE_16K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 193 */ {.mapper=3,.submapper=2,.prgrom=SIZE_16K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 194 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 195 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 196 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 197 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 198 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 199 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 200 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.prgram=SIZE_2K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 201 */ {.mapper=3,.submapper=2,.prgrom=SIZE_32K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 202 */ {.mapper=3,.submapper=2,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 203 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_FOUR_SCREEN,.system=SYSTEM_NTSC,},
  /* 204 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 205 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 206 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_FOUR_SCREEN,.system=SYSTEM_NTSC,},
  /* 207 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_FOUR_SCREEN,.system=SYSTEM_NTSC,},
  /* 208 */ {.mapper=4,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 209 */ {.mapper=4,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 210 */ {.mapper=4,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 211 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 212 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 213 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 214 */ {.mapper=4,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 215 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 216 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 217 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 218 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_40K,.mirror=MIRROR_HORIZONTAL,},
  /* 219 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_56K,.mirror=MIRROR_HORIZONTAL,},
  /* 220 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 221 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 222 */ {.mapper=4,.prgrom=SIZE_32K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 223 */ {.mapper=4,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 224 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 225 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 226 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 227 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 228 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 229 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 230 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 231 */ {.mapper=4,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 232 */ {.mapper=4,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 233 */ {.mapper=4,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 234 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 235 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 236 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 237 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 238 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 239 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 240 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 241 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_88K,.mirror=MIRROR_HORIZONTAL,},
  /* 242 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_96K,.mirror=MIRROR_HORIZONTAL,},
  /* 243 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 244 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 245 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 246 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 247 */ {.mapper=4,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 248 */ {.mapper=4,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 249 */ {.mapper=4,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 250 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_232K,.mirror=MIRROR_HORIZONTAL,},
  /* 251 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 252 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 253 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 254 */ {.mapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 255 */ {.mapper=4,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 256 */ {.mapper=4,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 257 */ {.mapper=4,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 258 */ {.mapper=4,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 259 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 260 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 261 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 262 */ {.mapper=4,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 263 */ {.mapper=4,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 264 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 265 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 266 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 267 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 268 */ {.mapper=4,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 269 */ {.mapper=4,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 270 */ {.mapper=4,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 271 */ {.mapper=4,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 272 */ {.mapper=4,.prgrom=SIZE_512K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 273 */ {.mapper=4,.prgrom=SIZE_512K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 274 */ {.mapper=4,.prgrom=SIZE_512K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 275 */ {.mapper=4,.prgrom=SIZE_512K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 276 */ {.mapper=4,.prgrom=SIZE_512K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 277 */ {.mapper=4,.prgrom=SIZE_512K,.chrrom=SIZE_512K,.mirror=MIRROR_HORIZONTAL,},
  /* 278 */ {.mapper=4,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_40K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 279 */ {.mapper=4,.prgrom=SIZE_32K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 280 */ {.mapper=4,.prgrom=SIZE_64K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 281 */ {.mapper=4,.prgrom=SIZE_64K,.prgnvram=SIZE_8K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 282 */ {.mapper=4,.prgrom=SIZE_64K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 283 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 284 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 285 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 286 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 287 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 288 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_16K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 289 */ {.mapper=4,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 290 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 291 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 292 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 293 */ {.mapper=4,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 294 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 295 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 296 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 297 */ {.mapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 298 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 299 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 300 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 301 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 302 */ {.mapper=4,.prgrom=SIZE_256K,.prgnvram=SIZE_16K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 303 */ {.mapper=4,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 304 */ {.mapper=4,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 305 */ {.mapper=4,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 306 */ {.mapper=4,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 307 */ {.mapper=4,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 308 */ {.mapper=4,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 309 */ {.mapper=4,.prgrom=SIZE_1024K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 310 */ {.mapper=4,.submapper=1,.prgrom=SIZE_256K,.prgnvram=SIZE_1K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 311 */ {.mapper=4,.submapper=1,.prgrom=SIZE_256K,.prgnvram=SIZE_1K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_PAL,},
  /* 312 */ {.mapper=4,.submapper=2,.prgrom=SIZE_256K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 313 */ {.mapper=4,.submapper=2,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 314 */ {.mapper=4,.submapper=3,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 315 */ {.mapper=4,.submapper=3,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 316 */ {.mapper=4,.submapper=3,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 317 */ {.mapper=4,.submapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 318 */ {.mapper=5,.prgrom=SIZE_16K,.chrrom=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 319 */ {.mapper=5,.prgrom=SIZE_16K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 320 */ {.mapper=5,.prgrom=SIZE_32K,.prgram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 321 */ {.mapper=5,.prgrom=SIZE_64K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 322 */ {.mapper=5,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 323 */ {.mapper=5,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 324 */ {.mapper=5,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 325 */ {.mapper=5,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 326 */ {.mapper=5,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 327 */ {.mapper=5,.prgrom=SIZE_512K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 328 */ {.mapper=5,.prgrom=SIZE_512K,.chrrom=SIZE_512K,.mirror=MIRROR_HORIZONTAL,},
  /* 329 */ {.mapper=5,.prgrom=SIZE_512K,.chrrom=SIZE_512K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 330 */ {.mapper=5,.prgrom=SIZE_1024K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 331 */ {.mapper=5,.prgrom=SIZE_1024K,.chrrom=SIZE_512K,.mirror=MIRROR_HORIZONTAL,},
  /* 332 */ {.mapper=5,.prgrom=SIZE_1024K,.chrrom=SIZE_520K,.mirror=MIRROR_HORIZONTAL,},
  /* 333 */ {.mapper=5,.prgrom=SIZE_1024K,.prgram=SIZE_8K,.chrrom=SIZE_1024K,.mirror=MIRROR_HORIZONTAL,},
  /* 334 */ {.mapper=5,.prgrom=SIZE_1024K,.prgram=SIZE_8K,.chrrom=SIZE_1024K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 335 */ {.mapper=5,.prgrom=SIZE_128K,.prgram=SIZE_8K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 336 */ {.mapper=5,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 337 */ {.mapper=5,.prgrom=SIZE_256K,.prgram=SIZE_8K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 338 */ {.mapper=5,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 339 */ {.mapper=5,.prgrom=SIZE_256K,.prgnvram=SIZE_32K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 340 */ {.mapper=5,.prgrom=SIZE_512K,.prgram=SIZE_8K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 341 */ {.mapper=5,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 342 */ {.mapper=5,.prgrom=SIZE_512K,.prgnvram=SIZE_32K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 343 */ {.mapper=5,.prgrom=SIZE_1024K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 344 */ {.mapper=5,.prgrom=SIZE_1024K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 345 */ {.mapper=5,.prgrom=SIZE_1024K,.prgnvram=SIZE_32K,.chrrom=SIZE_1024K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 346 */ {.mapper=7,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 347 */ {.mapper=7,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 348 */ {.mapper=7,.submapper=1,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 349 */ {.mapper=7,.submapper=1,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 350 */ {.mapper=7,.submapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 351 */ {.mapper=7,.submapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 352 */ {.mapper=7,.submapper=1,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 353 */ {.mapper=7,.submapper=1,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 354 */ {.mapper=7,.submapper=2,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 355 */ {.mapper=7,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 356 */ {.mapper=7,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 357 */ {.mapper=7,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 358 */ {.mapper=7,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 359 */ {.mapper=7,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 360 */ {.mapper=7,.submapper=2,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 361 */ {.mapper=9,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 362 */ {.mapper=9,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 363 */ {.mapper=9,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 364 */ {.mapper=10,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 365 */ {.mapper=10,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 366 */ {.mapper=10,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 367 */ {.mapper=11,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 368 */ {.mapper=11,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 369 */ {.mapper=11,.prgrom=SIZE_128K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 370 */ {.mapper=11,.prgrom=SIZE_512K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 371 */ {.mapper=11,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 372 */ {.mapper=11,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 373 */ {.mapper=11,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 374 */ {.mapper=11,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 375 */ {.mapper=11,.prgrom=SIZE_64K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 376 */ {.mapper=11,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,},
  /* 377 */ {.mapper=11,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 378 */ {.mapper=11,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 379 */ {.mapper=16,.submapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 380 */ {.mapper=16,.submapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 381 */ {.mapper=16,.submapper=4,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 382 */ {.mapper=16,.submapper=5,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 383 */ {.mapper=16,.submapper=5,.prgrom=SIZE_256K,.prgnvram=SIZE_256B,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 384 */ {.mapper=18,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 385 */ {.mapper=18,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 386 */ {.mapper=18,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 387 */ {.mapper=18,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 388 */ {.mapper=18,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 389 */ {.mapper=19,.submapper=2,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 390 */ {.mapper=19,.submapper=2,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 391 */ {.mapper=19,.submapper=2,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 392 */ {.mapper=19,.submapper=2,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 393 */ {.mapper=19,.submapper=2,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 394 */ {.mapper=19,.submapper=2,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 395 */ {.mapper=19,.submapper=2,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 396 */ {.mapper=19,.submapper=3,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 397 */ {.mapper=19,.submapper=3,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 398 */ {.mapper=19,.submapper=3,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 399 */ {.mapper=19,.submapper=3,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 400 */ {.mapper=19,.submapper=4,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 401 */ {.mapper=19,.submapper=5,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 402 */ {.mapper=19,.submapper=5,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 403 */ {.mapper=21,.submapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 404 */ {.mapper=21,.submapper=2,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 405 */ {.mapper=23,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_40K,.mirror=MIRROR_HORIZONTAL,},
  /* 406 */ {.mapper=23,.submapper=1,.prgrom=SIZE_128K,.prgram=SIZE_2K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 407 */ {.mapper=23,.submapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_512K,.mirror=MIRROR_HORIZONTAL,},
  /* 408 */ {.mapper=23,.submapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_512K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 409 */ {.mapper=23,.submapper=2,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 410 */ {.mapper=23,.submapper=2,.prgrom=SIZE_128K,.prgram=SIZE_2K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 411 */ {.mapper=23,.submapper=2,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 412 */ {.mapper=23,.submapper=2,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 413 */ {.mapper=23,.submapper=2,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 414 */ {.mapper=23,.submapper=2,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 415 */ {.mapper=23,.submapper=3,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 416 */ {.mapper=23,.submapper=3,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 417 */ {.mapper=23,.submapper=3,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 418 */ {.mapper=23,.submapper=3,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 419 */ {.mapper=24,.prgrom=SIZE_32K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 420 */ {.mapper=24,.prgrom=SIZE_64K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 421 */ {.mapper=24,.prgrom=SIZE_128K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 422 */ {.mapper=24,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 423 */ {.mapper=24,.prgrom=SIZE_256K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 424 */ {.mapper=24,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 425 */ {.mapper=24,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 426 */ {.mapper=24,.prgrom=SIZE_256K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 427 */ {.mapper=25,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 428 */ {.mapper=25,.submapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 429 */ {.mapper=25,.submapper=1,.prgrom=SIZE_128K,.prgram=SIZE_2K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 430 */ {.mapper=25,.submapper=2,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 431 */ {.mapper=25,.submapper=2,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 432 */ {.mapper=25,.submapper=2,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 433 */ {.mapper=25,.submapper=3,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 434 */ {.mapper=26,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 435 */ {.mapper=26,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 436 */ {.mapper=28,.prgrom=SIZE_32K,.chrram=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 437 */ {.mapper=28,.prgrom=SIZE_64K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 438 */ {.mapper=28,.prgrom=SIZE_64K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 439 */ {.mapper=28,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 440 */ {.mapper=28,.prgrom=SIZE_1024K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 441 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_SINGLE_1,},
  /* 442 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_SINGLE_1,.system=SYSTEM_NTSC,},
  /* 443 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_SINGLE_1,.battery=1,.system=SYSTEM_NTSC,},
  /* 444 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_FOUR_SCREEN,.battery=1,},
  /* 445 */ {.mapper=30,.prgrom=SIZE_32K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 446 */ {.mapper=30,.prgrom=SIZE_64K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 447 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 448 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 449 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 450 */ {.mapper=30,.prgrom=SIZE_2048K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 451 */ {.mapper=30,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 452 */ {.mapper=30,.prgrom=SIZE_128K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 453 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 454 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 455 */ {.mapper=30,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 456 */ {.mapper=30,.prgrom=SIZE_256K,.chrram=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 457 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 458 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 459 */ {.mapper=30,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 460 */ {.mapper=30,.prgrom=SIZE_128K,.chrram=SIZE_32K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 461 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 462 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 463 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_PAL,},
  /* 464 */ {.mapper=30,.prgrom=SIZE_512K,.chrram=SIZE_32K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 465 */ {.mapper=30,.submapper=1,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 466 */ {.mapper=33,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 467 */ {.mapper=33,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 468 */ {.mapper=33,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 469 */ {.mapper=33,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 470 */ {.mapper=34,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 471 */ {.mapper=34,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 472 */ {.mapper=34,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 473 */ {.mapper=34,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 474 */ {.mapper=34,.submapper=1,.prgrom=SIZE_64K,.prgram=SIZE_8K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,},
  /* 475 */ {.mapper=34,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,},
  /* 476 */ {.mapper=34,.submapper=2,.prgrom=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 477 */ {.mapper=34,.submapper=2,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 478 */ {.mapper=34,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 479 */ {.mapper=34,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 480 */ {.mapper=34,.submapper=2,.prgrom=SIZE_128K,.chrram=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 481 */ {.mapper=34,.submapper=2,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 482 */ {.mapper=34,.submapper=2,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 483 */ {.mapper=34,.submapper=2,.prgrom=SIZE_512K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 484 */ {.mapper=66,.prgrom=SIZE_64K,.chrrom=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 485 */ {.mapper=66,.prgrom=SIZE_64K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 486 */ {.mapper=66,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 487 */ {.mapper=66,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 488 */ {.mapper=66,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 489 */ {.mapper=66,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 490 */ {.mapper=66,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 491 */ {.mapper=66,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,},
  /* 492 */ {.mapper=66,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 493 */ {.mapper=66,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 494 */ {.mapper=66,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 495 */ {.mapper=66,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 496 */ {.mapper=69,.prgrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 497 */ {.mapper=69,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 498 */ {.mapper=69,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 499 */ {.mapper=69,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 500 */ {.mapper=69,.prgrom=SIZE_128K,.prgram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 501 */ {.mapper=69,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 502 */ {.mapper=69,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 503 */ {.mapper=69,.prgrom=SIZE_128K,.prgnvram=SIZE_32K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 504 */ {.mapper=69,.prgrom=SIZE_128K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 505 */ {.mapper=69,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 506 */ {.mapper=71,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 507 */ {.mapper=71,.prgrom=SIZE_64K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 508 */ {.mapper=71,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 509 */ {.mapper=71,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,},
  /* 510 */ {.mapper=71,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 511 */ {.mapper=71,.submapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 512 */ {.mapper=71,.submapper=1,.prgrom=SIZE_128K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 513 */ {.mapper=76,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 514 */ {.mapper=76,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 515 */ {.mapper=78,.submapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 516 */ {.mapper=78,.submapper=3,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 517 */ {.mapper=79,.prgrom=SIZE_16K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 518 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 519 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 520 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 521 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 522 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_NTSC,},
  /* 523 */ {.mapper=79,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 524 */ {.mapper=79,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 525 */ {.mapper=79,.prgrom=SIZE_16K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 526 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 527 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 528 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_PAL,},
  /* 529 */ {.mapper=79,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 530 */ {.mapper=79,.prgrom=SIZE_64K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 531 */ {.mapper=79,.prgrom=SIZE_64K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 532 */ {.mapper=79,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,},
  /* 533 */ {.mapper=79,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 534 */ {.mapper=85,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 535 */ {.mapper=85,.submapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 536 */ {.mapper=85,.submapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 537 */ {.mapper=85,.submapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 538 */ {.mapper=85,.submapper=2,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 539 */ {.mapper=87,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 540 */ {.mapper=87,.prgrom=SIZE_16K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 541 */ {.mapper=87,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,},
  /* 542 */ {.mapper=87,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 543 */ {.mapper=118,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 544 */ {.mapper=118,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.system=SYSTEM_PAL,},
  /* 545 */ {.mapper=118,.prgrom=SIZE_128K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 546 */ {.mapper=118,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,},
  /* 547 */ {.mapper=118,.prgrom=SIZE_256K,.prgnvram=SIZE_8K,.chrrom=SIZE_128K,.mirror=MIRROR_HORIZONTAL,.battery=1,},
  /* 548 */ {.mapper=163,.prgrom=SIZE_2048K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_HORIZONTAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 549 */ {.mapper=163,.prgrom=SIZE_512K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 550 */ {.mapper=163,.prgrom=SIZE_1024K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 551 */ {.mapper=163,.prgrom=SIZE_2048K,.prgnvram=SIZE_8K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.battery=1,.system=SYSTEM_NTSC,},
  /* 552 */ {.mapper=206,.prgrom=SIZE_64K,.prgram=SIZE_2K,.chrrom=SIZE_32K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 553 */ {.mapper=206,.prgrom=SIZE_64K,.prgram=SIZE_2K,.chrrom=SIZE_64K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 554 */ {.mapper=206,.prgrom=SIZE_128K,.prgram=SIZE_2K,.chrrom=SIZE_32K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 555 */ {.mapper=206,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 556 */ {.mapper=206,.prgrom=SIZE_128K,.prgram=SIZE_2K,.chrrom=SIZE_64K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 557 */ {.mapper=206,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 558 */ {.mapper=206,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 559 */ {.mapper=206,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 560 */ {.mapper=206,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 561 */ {.mapper=206,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_HORIZONTAL,},
  /* 562 */ {.mapper=206,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 563 */ {.mapper=206,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 564 */ {.mapper=206,.prgrom=SIZE_64K,.chrrom=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 565 */ {.mapper=206,.prgrom=SIZE_64K,.chrrom=SIZE_24K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 566 */ {.mapper=206,.prgrom=SIZE_64K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 567 */ {.mapper=206,.prgrom=SIZE_64K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,},
  /* 568 */ {.mapper=206,.prgrom=SIZE_128K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 569 */ {.mapper=206,.prgrom=SIZE_128K,.chrrom=SIZE_64K,.mirror=MIRROR_VERTICAL,},
  /* 570 */ {.mapper=206,.submapper=1,.prgrom=SIZE_32K,.prgram=SIZE_2K,.chrrom=SIZE_32K,.mirror=MIRROR_FOUR_SCREEN,},
  /* 571 */ {.mapper=206,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_16K,.mirror=MIRROR_HORIZONTAL,},
  /* 572 */ {.mapper=206,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_HORIZONTAL,},
  /* 573 */ {.mapper=206,.submapper=1,.prgrom=SIZE_32K,.chrrom=SIZE_32K,.mirror=MIRROR_VERTICAL,},
  /* 574 */ {.mapper=210,.submapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 575 */ {.mapper=210,.submapper=1,.prgrom=SIZE_512K,.chrrom=SIZE_256K,.mirror=MIRROR_HORIZONTAL,},
  /* 576 */ {.mapper=210,.submapper=1,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 577 */ {.mapper=210,.submapper=1,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 578 */ {.mapper=210,.submapper=1,.prgrom=SIZE_512K,.prgnvram=SIZE_2K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,.battery=1,},
  /* 579 */ {.mapper=210,.submapper=2,.prgrom=SIZE_128K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 580 */ {.mapper=210,.submapper=2,.prgrom=SIZE_256K,.chrrom=SIZE_128K,.mirror=MIRROR_VERTICAL,},
  /* 581 */ {.mapper=210,.submapper=2,.prgrom=SIZE_256K,.chrrom=SIZE_256K,.mirror=MIRROR_VERTICAL,},
  /* 582 */ {.mapper=232,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
  /* 583 */ {.mapper=232,.submapper=1,.prgrom=SIZE_256K,.chrram=SIZE_8K,.mirror=MIRROR_VERTICAL,.system=SYSTEM_NTSC,},
};

static const u32 s_crcs[] = {
/**** 0 mapper=0 prgrom=32K prgnvram=2K chrrom=8K mirror=4 battery=1 */
    0xB6AFFFF8, //  Homebrew\From Below (Vs. v0.10.0).nes 
/**** 1 mapper=0 prgrom=8K chrrom=8K mirror=H */
    0x084F61CD, //  Licensed Japan\Galaxian (rev0).nes 
    0x76A6A813, //  Licensed Japan\Galaxian (rev1).nes 
/**** 2 mapper=0 prgrom=16K chrram=8K mirror=H */
    0x522EE20F, //  Homebrew\Blastered Cats 1K.nes 
    0x4E2118E4, //  Homebrew\Bomb Array.nes 
    0x0EA5E3FA, //  Homebrew\Box Boy.nes 
    0x8BC43536, //  Homebrew\Color a Deer [pubby].nes 
    0x929790DE, //  Homebrew\Demos\Xmas 2006 [Nerds 'R' Us].nes 
    0x6C251132, //  Homebrew\Flight Minigames.nes 
    0xE43FCC70, //  Homebrew\Function [Red Moon Games].nes 
    0x10650239, //  Homebrew\Memory (Sly Dog Studios).nes 
    0x21BA8CE6, //  Homebrew\Midnight Jogger.nes 
    0xEE31C528, //  Homebrew\Munchie Attack꞉ Fast Food Clone.nes 
    0x65699715, //  Homebrew\NSF Players\Mega Man.nes 
    0xEC6BFDF8, //  Homebrew\Pong (2015-12).nes 
    0x36F3E5FA, //  Homebrew\Simone Says.nes 
    0xEFF3ECB5, //  Homebrew\The Game [Calvin Wiermaa].nes 
    0x90893CF0, //  Homebrew\The One with the Walls.nes 
    0x0EDB2A70, //  Homebrew\Tic-Tac-Two-Ps.nes 
    0x54E99165, //  Homebrew\Tools\Controller Test (10-2016)[JRoatch].nes 
    0xE18120C3, //  Homebrew\Tools\SFX Editor.nes 
    0x9B3B1CF4, //  Homebrew\Whack-A-Mario.nes 
    0xAD1DB0DE, //  Homebrew\Wrecking Balls (NESdev 2016)[Rahsennor].nes 
/**** 3 mapper=0 prgrom=16K chrram=8K mirror=H system=SYSTEM_PAL */
    0xD1837AD3, //  pal_apu_test/01.len_ctr.nes 
    0x87483778, //  pal_apu_test/02.len_table.nes 
    0x47086DB9, //  pal_apu_test/03.irq_flag.nes 
    0x42D39BDC, //  pal_apu_test/04.clock_jitter.nes 
    0x9464BB7A, //  pal_apu_test/05.len_timing_mode0.nes 
    0x966EA9A4, //  pal_apu_test/06.len_timing_mode1.nes 
    0xE4E03A3D, //  pal_apu_test/07.irq_flag_timing.nes 
    0xABF7617D, //  pal_apu_test/08.irq_timing.nes 
    0xED21658A, //  pal_apu_test/10.len_halt_timing.nes 
    0xA487CD04, //  pal_apu_test/11.len_reload_timing.nes 
/**** 4 mapper=0 prgrom=16K chrram=8K mirror=H system=SYSTEM_NTSC */
    0xBB1B892A, //  Homebrew\2048.nes 
    0xB8637E90, //  Homebrew\Escape from Pong (rev2).nes 
    0x24B00036, //  Homebrew\Escape from Pong (rev5, normal controls).nes 
    0xA3A74BCF, //  Homebrew\Escape from Pong (rev5, reversed controls).nes 
    0x3CC9E65A, //  Homebrew\Galaxy Patrol Enhanced.nes 
/**** 5 mapper=0 prgrom=16K chrrom=8K mirror=H */
    0xD9893080, //  Bad Dumps\Battletank 2000 [bad CHR].nes 
    0xC44D4FAA, //  Bootleg Hacks\Hassle.nes 
    0xD49342CC, //  Bootleg Hacks\Inventor\Sea War 2000.nes 
    0x4E12DA59, //  Bootleg Hacks\Teletubbies.nes 
    0x7ABB83D5, //  Bootleg Hacks\星空飛前 - Night Arrow.nes 
    0x400858B9, //  Bootleg Singles\Galaga (LA25).nes 
    0x31D755B3, //  Bootleg Singles\Ice Climber (LA20).nes 
    0x711CCD04, //  Bootleg Singles\Mario Bros. (LA01).nes 
    0xE4E3D2ED, //  Bootleg Singles\Pac-Man (LA34).nes 
    0xBAD43FD6, //  Bootleg Singles\Pinball (LA10).nes 
    0x63DDD219, //  Bootleg Singles\Popeye (LA02).nes 
    0xB22C41CD, //  Bootleg Singles\Road Fighter (LA42).nes 
    0x38C1CF0D, //  Bootleg Singles\Tennis.nes 
    0x9387A2AC, //  Bootleg Singles\TouchGamePlayer\Battle City.nes 
    0x74C0FD0F, //  Bootleg Singles\TouchGamePlayer\Bomb Sweeper.nes 
    0xFAE795F7, //  Bootleg Singles\TouchGamePlayer\Chack 'n Pop.nes 
    0x2B33B9CC, //  Bootleg Singles\TouchGamePlayer\Devil World.nes 
    0xFC70C88A, //  Bootleg Singles\TouchGamePlayer\Dig Dug.nes 
    0x01024AEF, //  Bootleg Singles\TouchGamePlayer\Front Line.nes 
    0x758F6867, //  Bootleg Singles\TouchGamePlayer\Galaga.nes 
    0x9BFCAEE5, //  Bootleg Singles\TouchGamePlayer\Galaxian.nes 
    0x070DF06F, //  Bootleg Singles\TouchGamePlayer\Golf.nes 
    0x9EB71CF9, //  Bootleg Singles\TouchGamePlayer\Joust.nes 
    0x56EFE20D, //  Bootleg Singles\TouchGamePlayer\Mario Bros..nes 
    0x5922D8AE, //  Bootleg Singles\TouchGamePlayer\Nuts &amp; Milk.nes 
    0x57342C40, //  Bootleg Singles\TouchGamePlayer\Pinball.nes 
    0xE2BD6CEB, //  Bootleg Singles\TouchGamePlayer\Super Arabian.nes 
    0x1D54B1D6, //  Bootleg Singles\小瑪琍.nes 
    0xE5D11921, //  Compatibility Hacks\Galaxian (rev0)[PRG doubled to 16 KiB].nes 
    0x44C11847, //  Compatibility Hacks\Galaxian (rev1)[PRG doubled to 16 KiB].nes 
    0x4AC46E46, //  Compatibility Hacks\小瑪琍 (C&amp;E)[PRG doubled to 16 KiB].nes 
    0xAE0119B3, //  Homebrew\81ex.nes 
    0x9F2B9182, //  Homebrew\Aspect Star N [Nicole Express].nes 
    0x6DFB1B97, //  Homebrew\Bomb Sweeper.nes 
    0x41324F94, //  Homebrew\CYO.nes 
    0x8F426CAB, //  Homebrew\Demos\BioForceApe.nes 
    0xEA39CD0F, //  Homebrew\Demos\Ceci n'est pas une NES.nes 
    0xB071B0D4, //  Homebrew\Demos\Danmaku 100 Bullet Demo [pubby].nes 
    0x9622900E, //  Homebrew\Demos\Kap3del4.nes 
    0xA3EC6542, //  Homebrew\Demos\Michael Martin's Diffusion Chamber.nes 
    0x841DE589, //  Homebrew\Demos\NES Jack-o-Lantern.nes 
    0xE3907336, //  Homebrew\Demos\NROM-Template.nes 
    0x1956F2CC, //  Homebrew\Demos\Nothing.nes 
    0x0B1A8E70, //  Homebrew\Demos\Unchained Nostalgia (v1.3.1)[VEG].nes 
    0x85B0336F, //  Homebrew\Demos\Zounds.nes 
    0x95EC8617, //  Homebrew\Double Action Blaster Guys.nes 
    0x1CBD5116, //  Homebrew\FHBG [NovaYoshi].nes 
    0x0CBB550D, //  Homebrew\Galaxy Nes.nes 
    0xAA939381, //  Homebrew\Get It! [Gravel Sudios].nes 
    0x7EC289BF, //  Homebrew\Greedy Snake.nes 
    0x5B854ED5, //  Homebrew\Hacks\Alien Pinball [ROMs and Robots].nes 
    0xD5E8D258, //  Homebrew\Hacks\Arctic Tennis.nes 
    0xDCC2F863, //  Homebrew\Hacks\Balloon Fight꞉ Satoru Iwata Edition.nes 
    0xA8DA67B7, //  Homebrew\Hacks\Balloon Mario [Rinkaku].nes 
    0xB5C23B0C, //  Homebrew\Hacks\Battle City\Battle City 2.nes 
    0xBCF23D8D, //  Homebrew\Hacks\Battle City\Battle City 20 Maps.nes 
    0x1CA1108C, //  Homebrew\Hacks\Battle City\Battle City 8 Stages.nes 
    0x585ADD55, //  Homebrew\Hacks\Battle City\Battle City Extreme.nes 
    0x6C86E133, //  Homebrew\Hacks\Battle City\Battle City Stage 36-70 [Ghost].nes 
    0x118202C8, //  Homebrew\Hacks\Battle City\Battle Road [Tof Studio].nes 
    0x22751950, //  Homebrew\Hacks\Battle City\China 2008.nes 
    0xC69DF1D8, //  Homebrew\Hacks\Battle City\City 260808.nes 
    0x076F3439, //  Homebrew\Hacks\Battle City\Line Tank.nes 
    0x2A69DAE6, //  Homebrew\Hacks\Battle City\Mappy City [Mitz].nes 
    0x5E05065A, //  Homebrew\Hacks\Battle City\Marble City [Tof].nes 
    0x9DD812CF, //  Homebrew\Hacks\Battle City\Pac-Man Saw [Tof].nes 
    0xC265E746, //  Homebrew\Hacks\Battle City\Return BC [Romhacking.net.ru].nes 
    0x0D6002CB, //  Homebrew\Hacks\Battle City\Smile Rage [Mitz].nes 
    0xB737D9F4, //  Homebrew\Hacks\Battle City\Spider Wars [Romhacking.net.ru].nes 
    0xF4367666, //  Homebrew\Hacks\Battle City\Star Wars (v1.4)[Tof].nes 
    0xFA80AF91, //  Homebrew\Hacks\Battle City\Tank Duel [Kamming].nes 
    0xEAA7BE23, //  Homebrew\Hacks\Battle City\War in Hell [Googie].nes 
    0x71A48F27, //  Homebrew\Hacks\Battle City\Warsaw City Autumn [Berion].nes 
    0xA064F9E9, //  Homebrew\Hacks\Battle City\Warsaw City Summer [Berion].nes 
    0x5034B399, //  Homebrew\Hacks\Battle City\Warsaw City Winter [Berion].nes 
    0xE30B422C, //  Homebrew\Hacks\Battle City\Гитлер Капут.nes 
    0x251ECF12, //  Homebrew\Hacks\Battle City\Город Битв (v2.2)[Сприлл].nes 
    0x7BD0A6CD, //  Homebrew\Hacks\Battle City\坦克大战 2008.nes 
    0x801BD049, //  Homebrew\Hacks\Battle City\坦克风云 2 [金明改造].nes 
    0xB9FA279E, //  Homebrew\Hacks\Battle City\坦克风云 [金明改造].nes 
    0xEDFEA185, //  Homebrew\Hacks\Binary Land꞉ Penguins Family (v1.2)[Guyver].nes 
    0x41F72ABF, //  Homebrew\Hacks\Blac-Man [Blax].nes 
    0xF07C5B64, //  Homebrew\Hacks\Mario Bros. (play as Pit).nes 
    0x89E7E888, //  Homebrew\Hacks\Marioくん (忍者くん꞉ 魔城の冒険).nes 
    0x2CE182CB, //  Homebrew\Hacks\Pika Dug [pacnsacdave].nes 
    0x642F70D1, //  Homebrew\Hacks\Pinball (v2)[MacBee].nes 
    0x5D8CF8C2, //  Homebrew\Hacks\Pinball Castlevania [Redrum].nes 
    0xEBB1C045, //  Homebrew\Hacks\Pink Floyd꞉ The Wall [Grimlick].nes 
    0x5458BB02, //  Homebrew\Hacks\Poppy's Pinball.nes 
    0x65A8EC90, //  Homebrew\Hacks\東方乙橙々.nes 
    0x8608B73F, //  Homebrew\HexS.nes 
    0xD4E4B154, //  Homebrew\Light Shields.nes 
    0x09475BF9, //  Homebrew\Lj65 [Damian Yerrick].nes 
    0x629EA430, //  Homebrew\Nesglovphone - Power Glove.nes 
    0x0D77A2A4, //  Homebrew\Nintencat꞉ The Parody.nes 
    0xC1739923, //  Homebrew\Paint or Draw (v2).nes 
    0xCEC3E585, //  Homebrew\Ping Pong.nes 
    0xBF044D72, //  Homebrew\Pong+Head Bounce.nes 
    0x078B3E8E, //  Homebrew\Rock Paper Scissors Lizard Sbock (Build 9b).nes 
    0x019E6C7C, //  Homebrew\Squirrel Domino.nes 
    0x457CFF46, //  Homebrew\Tests\NES Paddle Controller Test (v0.02).nes 
    0x57342FED, //  Homebrew\Tests\Pulpers Bgscroll Test.nes 
    0xBCD77B6F, //  Homebrew\Tests\Turbo Controller Checker.nes 
    0x2D83C43C, //  Homebrew\Tests\Unlicensed HVC Controller Test.nes 
    0x482DA740, //  Homebrew\Tetramino.nes 
    0x6AF0708D, //  Homebrew\Tools\Color Bars 2 - Palette Viewing (v2)[Quietust].nes 
    0x36024195, //  Homebrew\Translations\English\Antarctic Adventure (rev0)[DvD Translations].nes 
    0xBC63D3FB, //  Homebrew\Translations\English\Antarctic Adventure (rev0)[Quest Games].nes 
    0x2CAD075E, //  Homebrew\Translations\English\Antarctic Adventure (rev1)[DvD Translations].nes 
    0xA6CC9530, //  Homebrew\Translations\English\Antarctic Adventure (rev1)[Quest Games].nes 
    0x7A4CB333, //  Homebrew\Translations\English\Gomoku Narabe Renju [Psyklax].nes 
    0x26CF0A58, //  Homebrew\Translations\English\Line [MrNorbert1994].nes 
    0x8E2AA0BB, //  Homebrew\Translations\English\Modern Shogi꞉ Naito 9-Dan Shogi Secret [Psyklax](Kanji).nes 
    0xAB3985A0, //  Homebrew\Translations\English\Modern Shogi꞉ Naito 9-Dan Shogi Secret [Psyklax](Roman).nes 
    0xAF73EEE4, //  Homebrew\Translations\English\Ninja Jajamaru in the Castle of Evil [Aishsha].nes 
    0x59446036, //  Homebrew\Translations\English\Ninja Kid's Demon Castle Adventure [Pacnsacdave].nes 
    0x836C4FA7, //  Licensed Japan\10-Yard Fight (rev0).nes 
    0x44D21F83, //  Licensed Japan\10-Yard Fight (rev1).nes 
    0x163ECCAE, //  Licensed Japan\Antarctic Adventure (rev0).nes 
    0x0C918A65, //  Licensed Japan\Antarctic Adventure (rev1).nes 
    0x2B462010, //  Licensed Japan\Balloon Fight.nes 
    0x78A48B23, //  Licensed Japan\Baseball.nes 
    0x7E053E64, //  Licensed Japan\Battle City.nes 
    0xF5F435B1, //  Licensed Japan\Binary Land.nes 
    0xCE67507A, //  Licensed Japan\BurgerTime.nes 
    0x622F059D, //  Licensed Japan\Chack 'n Pop.nes 
    0x5FAB6BCE, //  Licensed Japan\Devil World (rev0).nes 
    0x145A9A6C, //  Licensed Japan\Devil World (rev1).nes 
    0xB174B680, //  Licensed Japan\Dig Dug.nes 
    0xF863D5BB, //  Licensed Japan\Donkey Kong (rev0).nes 
    0x50A1B3FE, //  Licensed Japan\Donkey Kong Jr.+Jr. Lesson.nes 
    0x12E6CB79, //  Licensed Japan\Door Door.nes 
    0xBEB30478, //  Licensed Japan\Exerion.nes 
    0x1B45A73E, //  Licensed Japan\Field Combat.nes 
    0x63338C3C, //  Licensed Japan\Formation Z (rev0).nes 
    0xE9023072, //  Licensed Japan\Formation Z (rev1).nes 
    0xCF4DBDBE, //  Licensed Japan\Front Line.nes 
    0xE911BCC4, //  Licensed Japan\Galaga.nes 
    0x5C9063E0, //  Licensed Japan\Golf.nes 
    0x0B58880C, //  Licensed Japan\Ice Climber.nes 
    0x927C7A3A, //  Licensed Japan\Joust.nes 
    0xC4C3949A, //  Licensed Japan\Mario Bros..nes 
    0x77512388, //  Licensed Japan\Millipede.nes 
    0x739A1027, //  Licensed Japan\Nuts &amp; Milk.nes 
    0x3E95BA25, //  Licensed Japan\Pac-Man (rev0).nes 
    0x5CDB2823, //  Licensed Japan\Pac-Man (rev1).nes 
    0x2C7D68F3, //  Licensed Japan\Pac-Man (rev2).nes 
    0x035DC2E9, //  Licensed Japan\Pinball.nes 
    0x5581E835, //  Licensed Japan\Popeye (rev0).nes 
    0x27738241, //  Licensed Japan\Popeyeの英語遊び.nes 
    0x7EE625EB, //  Licensed Japan\Road Fighter.nes 
    0xE47E9FA7, //  Licensed Japan\Sky Destroyer.nes 
    0xD9C093B1, //  Licensed Japan\Space Invaders.nes 
    0x908505EE, //  Licensed Japan\Super Arabian.nes 
    0xD4D9E21A, //  Licensed Japan\Tennis.nes 
    0x64BBCB77, //  Licensed Japan\Warpman.nes 
    0x8BAA5FF2, //  Licensed Japan\Yie Ar Kung-Fu (v1.2).nes 
    0x86CEFC12, //  Licensed Japan\Yie Ar Kung-Fu (v1.4).nes 
    0xE492D45A, //  Licensed Japan\Zippy Race.nes 
    0x4A99B47E, //  Licensed Japan\五目ならべ.nes 
    0xA222F5A0, //  Licensed Japan\忍者くん꞉ 魔城の冒険 (rev0).nes 
    0xFC00A282, //  Licensed Japan\忍者くん꞉ 魔城の冒険 (rev1).nes 
    0xFFD9DB04, //  Licensed Japan\本将棋꞉ 内藤九段将棋秘伝.nes 
    0x401349A8, //  Licensed North America\Balloon Fight.nes 
    0xDAF9D7E3, //  Licensed North America\BurgerTime.nes 
    0xE7D2C49D, //  Licensed North America\Golf.nes 
    0x9E4E9CC2, //  Licensed North America\Pac-Man (Namco).nes 
    0xA9BBF44F, //  Licensed North America\Pac-Man (Tengen).nes 
    0x2DBB054D, //  Maintenance\HVC 検査 Cassette꞉ Controller Test.nes 
    0x2E0F51AF, //  Maintenance\NTF2 Test (rev0).nes 
    0x7BCC2E4E, //  Modern\AO (alt).nes 
    0x259D1C0E, //  Modern\AO.nes 
    0xDB8E76C8, //  Modern\Dragon Feet.nes 
    0x1FB5C75D, //  Modern\GameCube Edition\Baseball (Japan).nes 
    0x8ED5BC84, //  Modern\Hudson Best Collection\Binary Land.nes 
    0x47A633B4, //  Modern\Namco Museum Archives\Battle City.nes 
    0x0D27648F, //  Modern\Namco Museum Archives\Dig Dug.nes 
    0x053ACE3B, //  Modern\Namco Museum Archives\Galaga.nes 
    0x5B1E2065, //  Modern\Namco Museum Archives\Galaxian.nes 
    0x43F7CFD5, //  Modern\Namco Museum Archives\Pac-Man.nes 
    0x8457CD0C, //  Modern\Namco Museum Archives\Warpman.nes 
    0x06384200, //  Modern\Virtual Console\Balloon Fight.nes 
    0xE1D7B9A1, //  Modern\Virtual Console\Formation Z.nes 
    0x9B080B6E, //  Modern\Virtual Console\Pac-Man.nes 
    0x4CB2A105, //  Modern\じゃじゃ丸 Jr. 伝承記꞉ Jalecoもあり候う\Exerion.nes 
    0xD9436162, //  Modern\じゃじゃ丸 Jr. 伝承記꞉ Jalecoもあり候う\Formation Z.nes 
    0x6D51DE39, //  Multicarts\extracts\Double Crossing꞉ The Forbidden Four (menu).nes 
    0x3FD9420A, //  Playchoice\Balloon Fight.nes 
    0xBE191844, //  Playchoice\Baseball.nes 
    0xE8551F28, //  Playchoice\Golf.nes 
    0x04EF48F1, //  Playchoice\Tennis.nes 
    0x2E5768C9, //  Plug-and-Play\extracts\Airial Hero.nes 
    0x9D38F8F9, //  Unlicensed North America\Pac-Man.nes 
    0x30229527, //  Unlicensed Taiwan&amp;Hong Kong\Othello.nes 
    0x961ADCA7, //  Unlicensed Taiwan&amp;Hong Kong\暗棋 - Gloomy Chess.nes 
    0x24746B0E, //  Unreleased\Millipede.nes 
/**** 6 mapper=0 prgrom=16K chrrom=8K mirror=H system=SYSTEM_PAL */
    0x2A201CF4, //  Homebrew\Super Tilt Bro. (NESdev v4).nes 
    0x1BB0E3D5, //  Homebrew\Super Tilt Bro..nes 
    0x5F3B4C33, //  Homebrew\Tools\The Lunary Year Mascots of China.nes 
    0xE54138A9, //  Licensed PAL\Balloon Fight.nes 
    0x0C783F0C, //  Licensed PAL\Devil World.nes 
    0x565B1BDB, //  Licensed PAL\Golf.nes 
    0x2BF0F9C5, //  Licensed PAL\Mario Bros..nes 
    0x81210F63, //  Licensed PAL\Pac-Man.nes 
    0xD6AD4E9D, //  Licensed PAL\Pinball.nes 
    0x719571B3, //  Licensed PAL\Road Fighter.nes 
    0x304FA926, //  Licensed PAL\Tennis.nes 
    0xD4CEA9E3, //  Modern\GameCube Edition\Golf (PAL).nes 
/**** 7 mapper=0 prgrom=16K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x2E4CCF46, //  BIOS\Game Genie.nes 
    0xBA8BDB77, //  Bootleg Singles\Donkey Kong (LA03).nes 
    0x275E3CFB, //  Bootleg Singles\TouchGamePlayer\Baseball.nes 
    0xBCC3EE67, //  Bootleg Singles\TouchGamePlayer\Donkey Kong.nes 
    0x5AF637A4, //  Homebrew\Basic Championship Wrestling.nes 
    0x7616AD8B, //  Homebrew\Bsides.nes 
    0x43ACE5A4, //  Homebrew\Chase (rev0).nes 
    0x331BCC87, //  Homebrew\Chase (rev1).nes 
    0x1A757C16, //  Homebrew\Code Master.nes 
    0xBC7ECBB8, //  Homebrew\Falldown.nes 
    0x760DB6D9, //  Homebrew\Forehead Block Guy (2011-03-27) .nes 
    0xA84D71F2, //  Homebrew\Forehead Block Guy (2011-04-06) .nes 
    0x2A86004F, //  Homebrew\Forehead Block Guy (2011-05-26) .nes 
    0xBB8F6776, //  Homebrew\Hacks\Alice de Climber [DASTARD].nes 
    0x1A70B79E, //  Homebrew\Pong 198X.nes 
    0x2419483A, //  Homebrew\Ralph 4.nes 
    0xEE63ACCA, //  Homebrew\Sudoku (NESWORLD Edition).nes 
    0x39328BAA, //  Homebrew\You Are Insignificant.nes 
    0x48F68D40, //  Licensed Japan\Clu Clu Land.nes 
    0x6F97C721, //  Licensed Japan\Donkey Kong (rev1).nes 
    0x70860FCA, //  Licensed Japan\Popeye (rev1).nes 
    0xAFDCBD24, //  Licensed North America\Baseball.nes 
    0xFB98D46E, //  Licensed North America\Ice Climber.nes 
    0xA46ACAA6, //  Modern\GBA e-Reader\Ice Climber.nes 
    0x67861A27, //  Modern\GameCube Edition\Baseball (North America).nes 
    0xBC3EAD71, //  Modern\Virtual Console\Clu Clu Land.nes 
    0x9C4589E3, //  Unlicensed Elsewhere\Othello.nes 
/**** 8 mapper=0 prgrom=16K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x7CCB8D1E, //  Homebrew\Lines.nes 
/**** 9 mapper=0 prgrom=32K chrram=8K mirror=H */
    0x4D2C4C89, //  Educational Computers\extracts\Architect.nes 
    0x8CEFAF82, //  Educational Computers\extracts\Facemaker.nes 
    0xEB756DC7, //  Homebrew\Demos\Stage 7 [Modus Operandi].nes 
    0x8562DFC1, //  Homebrew\Double Action Blaster Guys (Cart Edition).nes 
    0x72F70C23, //  Homebrew\Double Action Blaster Guys (Compo Edition).nes 
    0x568999EA, //  Homebrew\NSF Players\Batman.nes 
    0x148F42C7, //  Homebrew\NSF Players\Bionic Commando.nes 
    0xC0F1B98A, //  Homebrew\NSF Players\Castlevania II Music.nes 
    0xB75E2A01, //  Homebrew\NSF Players\Castlevania II.nes 
    0x9A76C7B3, //  Homebrew\NSF Players\Codename Viper.nes 
    0x84A5555A, //  Homebrew\NSF Players\Crystalis.nes 
    0xEFCB70D7, //  Homebrew\NSF Players\Damian Yerrick Covers Vol 1.nes 
    0x9265F380, //  Homebrew\NSF Players\Duck Tales.nes 
    0x846876C7, //  Homebrew\NSF Players\Earthbound.nes 
    0xB2249A9A, //  Homebrew\NSF Players\Gunsmoke.nes 
    0xF00E9E91, //  Homebrew\NSF Players\Journey to Silius.nes 
    0xE4312D20, //  Homebrew\NSF Players\Linus Space Head.nes 
    0xE21A4FCB, //  Homebrew\NSF Players\Little Nemo.nes 
    0x8BB76C06, //  Homebrew\NSF Players\Mega Man 2.nes 
    0x1F674C1A, //  Homebrew\NSF Players\Mega Man 3.nes 
    0x60EE83DA, //  Homebrew\NSF Players\Mega Man 4.nes 
    0x4AF7183D, //  Homebrew\NSF Players\Mega Man 5.nes 
    0x157AAA5A, //  Homebrew\NSF Players\Mega Man 6.nes 
    0xD9420CCF, //  Homebrew\NSF Players\Micro Machines.nes 
    0xA9AC708B, //  Homebrew\NSF Players\Ninja Gaiden 2.nes 
    0x3B752B6C, //  Homebrew\NSF Players\Ninja Gaiden 3.nes 
    0xE9FB30EB, //  Homebrew\NSF Players\Ninja Gaiden.nes 
    0x1AB4BEF4, //  Homebrew\NSF Players\Pictionary.nes 
    0x18AA4541, //  Homebrew\NSF Players\Punch Out.nes 
    0xDE8A95DF, //  Homebrew\NSF Players\Snake's Revenge.nes 
    0x6EEC7836, //  Homebrew\NSF Players\Solstice.nes 
    0x58BBD04C, //  Homebrew\NSF Players\Somari.nes 
    0xC256D5D3, //  Homebrew\NSF Players\Star Trek - The Next Generation.nes 
    0x340341AE, //  Homebrew\NSF Players\Street Fighter 2010.nes 
    0x2D2A7B46, //  Homebrew\NSF Players\Super Mario Bros 2.nes 
    0x15CF5900, //  Homebrew\NSF Players\Tetris - Tengen.nes 
    0xAD2A324B, //  Homebrew\NSF Players\Tetris.nes 
    0xBA4CCAC0, //  Homebrew\NSF Players\Willow.nes 
    0x583970A2, //  Homebrew\NSF Players\Zelda 2.nes 
    0x9164FE2E, //  Homebrew\PCB Artist.nes 
    0x77274333, //  Homebrew\RHDE Furniture Fight.nes 
    0xAA2EEB7F, //  Homebrew\The Cowlitz Gamers' Adventure.nes 
    0x9CB104B0, //  Homebrew\Twinbee Yahho.nes 
    0xD0D0C932, //  Modern\8 Bit Xmas 2011꞉ Fireplace Bash (download).nes 
    0x50EBCEC5, //  Unlicensed South Korea\Beat 'n Box.nes 
/**** 10 mapper=0 prgrom=32K chrram=8K mirror=H system=SYSTEM_PAL */
    0x0021EBA8, //  Homebrew\Demos\Demo Pills for Your Brain 2015 [DSS].nes 
/**** 11 mapper=0 prgrom=32K prgram=8K chrram=8K mirror=H */
    0xF19D0BA8, //  Homebrew\Demos\Rnes Demo.nes 
    0x5A62AFC9, //  Homebrew\Demos\Speech [Memblers].nes 
    0x3271F470, //  Homebrew\NSF Players\Final Fantasy 2.nes 
    0x73484E83, //  Homebrew\NSF Players\Super Mario Bros 3.nes 
/**** 12 mapper=0 prgrom=32K chrrom=8K mirror=H */
    0x7BEC1745, //  Bad Dumps\Arkanoid [missing CHR banks][multicart mapper hack].nes 
    0x8BC2AC86, //  Bad Dumps\Balloon Mario [Rinkaku][PRG overdump].nes 
    0x329C0349, //  Bad Dumps\NTF2 Test [PRG overdump].nes 
    0x7F985452, //  Bootleg Hacks\Bomb.nes 
    0xB4BA8EE2, //  Bootleg Hacks\Gobang.nes 
    0xDF2B95BB, //  Bootleg Hacks\Inventor\Aether.nes 
    0xEF0FEFD1, //  Bootleg Hacks\Inventor\Chess.nes 
    0x7ABDA038, //  Bootleg Hacks\Inventor\Combata.nes 
    0x3F3A509E, //  Bootleg Hacks\Inventor\Dada.nes 
    0x9D0E6CD2, //  Bootleg Hacks\Inventor\Tennis.nes 
    0xEB38F8AE, //  Bootleg Hacks\Space War.nes 
    0x78048FC1, //  Bootleg Hacks\Tank 1990.nes 
    0x3126BCFA, //  Bootleg Hacks\X Racing.nes 
    0x78E43966, //  Bootleg Hacks\火龍.nes 
    0xD0502E2D, //  Bootleg Singles\Bung Game Doctor\(A001) Galaxian.nes 
    0xE5D2F889, //  Bootleg Singles\Bung Game Doctor\(B001) 10-Yard Fight.nes 
    0x234D8E79, //  Bootleg Singles\Bung Game Doctor\(B004) Antarctic Adventure.nes 
    0x580E350E, //  Bootleg Singles\Bung Game Doctor\(B006) Baseball.nes 
    0x57FA9CE3, //  Bootleg Singles\Bung Game Doctor\(B007) Battle City.nes 
    0x26624522, //  Bootleg Singles\Bung Game Doctor\(B008) Binary Land.nes 
    0xE6717DC8, //  Bootleg Singles\Bung Game Doctor\(B009) BurgerTime.nes 
    0xCEC1C64C, //  Bootleg Singles\Bung Game Doctor\(B011) Clu Clu Land.nes 
    0xA6C19E16, //  Bootleg Singles\Bung Game Doctor\(B014) Field Combat.nes 
    0x8D4E3ECA, //  Bootleg Singles\Bung Game Doctor\(B015) Devil World.nes 
    0xE7240BB2, //  Bootleg Singles\Bung Game Doctor\(B016) Dig Dug.nes 
    0x6C04B3D9, //  Bootleg Singles\Bung Game Doctor\(B020) Door Door.nes 
    0x60E5BC39, //  Bootleg Singles\Bung Game Doctor\(B024) Exerion.nes 
    0x1E1BAEB9, //  Bootleg Singles\Bung Game Doctor\(B026) Formation Z.nes 
    0xF8DFDC1E, //  Bootleg Singles\Bung Game Doctor\(B028) Galaga.nes 
    0x3BDE5ED4, //  Bootleg Singles\Bung Game Doctor\(B043) 忍者くん꞉ 魔城の冒険.nes 
    0x8602B762, //  Bootleg Singles\Bung Game Doctor\(B045) Pac-Man.nes 
    0xE4482F96, //  Bootleg Singles\Bung Game Doctor\(B046) Pinball.nes 
    0x7CFBEB70, //  Bootleg Singles\Bung Game Doctor\(B050) Road Fighter.nes 
    0x38CD0135, //  Bootleg Singles\Bung Game Doctor\(B051) Sky Destroyer.nes 
    0xB50782C4, //  Bootleg Singles\Bung Game Doctor\(B056) Warpman.nes 
    0x64A0E121, //  Bootleg Singles\Bung Game Doctor\(B061) Zippy Race.nes 
    0x4F691FAD, //  Bootleg Singles\Bung Game Doctor\(B067) 暗棋 - Gloomy Chess.nes 
    0x7A1281CF, //  Bootleg Singles\Bung Game Doctor\(C001) 1942.nes 
    0x6104C630, //  Bootleg Singles\Bung Game Doctor\(C006) Elevator Action.nes 
    0x22A711C6, //  Bootleg Singles\Bung Game Doctor\(C007) Exed Exes.nes 
    0x0E197A5E, //  Bootleg Singles\Bung Game Doctor\(C008) Flappy.nes 
    0x1347D6C9, //  Bootleg Singles\Bung Game Doctor\(C009) Galg.nes 
    0x2554F9FD, //  Bootleg Singles\Bung Game Doctor\(C018) Route-16 Turbo.nes 
    0x57A125B0, //  Bootleg Singles\Bung Game Doctor\(C023) Star Luster.nes 
    0x4BF72F9D, //  Bootleg Singles\Bung Game Doctor\(C025) Thexder.nes 
    0xAA2F7D91, //  Bootleg Singles\Bung Game Doctor\(C029) Wrecking Crew.nes 
    0x4706DB1D, //  Bootleg Singles\Bung Game Doctor\(C030) Xevious.nes 
    0x1117CE85, //  Bootleg Singles\Bung Game Doctor\(C033) MagMax.nes 
    0x64D56316, //  Bootleg Singles\Bung Game Doctor\(C034) Tag Team Pro-Wrestling.nes 
    0x59005CCE, //  Bootleg Singles\Bung Game Doctor\(C041) B-Wings.nes 
    0x7BF81EFC, //  Bootleg Singles\Bung Game Doctor\Ms. Pac-Man.nes 
    0xA66A2DCE, //  Bootleg Singles\Elevator Action (LB31).nes 
    0x3742B5B8, //  Bootleg Singles\Front Fareast Magic Card\(F1003) Battle City.nes 
    0x3A3CD775, //  Bootleg Singles\Front Fareast Magic Card\(F1012) Exerion.nes 
    0x917C55D8, //  Bootleg Singles\Front Fareast Magic Card\(F1018) 10-Yard Fight.nes 
    0x28D183AC, //  Bootleg Singles\Front Fareast Magic Card\(F1019) Antarctic Adventure.nes 
    0xD57EC983, //  Bootleg Singles\Front Fareast Magic Card\(F1020) BurgerTime.nes 
    0x037F3281, //  Bootleg Singles\Front Fareast Magic Card\(F1021) Galaxian.nes 
    0x03D656B0, //  Bootleg Singles\Front Fareast Magic Card\(F1024) Galaga.nes 
    0x749EE2AF, //  Bootleg Singles\Front Fareast Magic Card\(F1034) Zippy Race.nes 
    0xE8D57B47, //  Bootleg Singles\Front Fareast Magic Card\(F1039) Formation Z.nes 
    0xC8F6A21A, //  Bootleg Singles\Front Fareast Magic Card\(F1043) Ice Climber.nes 
    0xE47EED90, //  Bootleg Singles\Front Fareast Magic Card\(F1045) Pac-Man.nes 
    0x64A31331, //  Bootleg Singles\Front Fareast Magic Card\(F1065) Sky Destroyer.nes 
    0x0185F619, //  Bootleg Singles\Front Fareast Magic Card\(F1067) Warpman.nes 
    0x22F95FF1, //  Bootleg Singles\Front Fareast Magic Card\(F1071) Flappy.nes 
    0xF308E97A, //  Bootleg Singles\Front Fareast Magic Card\(F1072) 頭脳戦艦 Galg.nes 
    0xFDFA564D, //  Bootleg Singles\Front Fareast Magic Card\(F1073) Road Fighter.nes 
    0xE940D56F, //  Bootleg Singles\Front Fareast Magic Card\(F1103) Thexder.nes 
    0x936E229D, //  Bootleg Singles\Front Fareast Magic Card\(F1108) B-Wings.nes 
    0x0550D0B2, //  Bootleg Singles\Front Fareast Magic Card\(F1110) Pinball.nes 
    0x338B06E7, //  Bootleg Singles\Front Fareast Magic Card\(F1134) Field Combat.nes 
    0xD0FBE94A, //  Bootleg Singles\Front Fareast Magic Card\(F1135) Clu Clu Land.nes 
    0x49AF5921, //  Bootleg Singles\Front Fareast Magic Card\(F1140) Dig Dug.nes 
    0xA0ECA0F9, //  Bootleg Singles\Front Fareast Magic Card\(F1XX8) Pyramid.nes 
    0x1D3DBEB1, //  Bootleg Singles\TouchGamePlayer\B-Wings.nes 
    0xE1A9734F, //  Bootleg Singles\TouchGamePlayer\Elevator Action.nes 
    0x076EC229, //  Bootleg Singles\TouchGamePlayer\Exed Exes.nes 
    0x8A87FBCD, //  Bootleg Singles\TouchGamePlayer\Mario in Space.nes 
    0x0E86CDB9, //  Bootleg Singles\TwinBee (LB16).nes 
    0x22FAA285, //  Bootleg Singles\Wrecking Crew (LB30).nes 
    0x9CC8D8AE, //  Compatibility Hacks\Balloon Fight [conversion from Disk Writer version].nes 
    0x92DA79B6, //  Compatibility Hacks\炎のRangerman [m000 extract].nes 
    0x8F75F3B0, //  Homebrew\3-in-1 2P Pak (early build).nes 
    0xEAA51D05, //  Homebrew\Blockage (v0.3.1).nes 
    0xE2825E53, //  Homebrew\Blockage (v0.3.2).nes 
    0x2AED0656, //  Homebrew\Candelabra - Estoscerro (April Fool's Version).nes 
    0x0EA56EAD, //  Homebrew\Candelabra - Estoscerro (Demo 1).nes 
    0xC3652329, //  Homebrew\Cheats\Tank 1990 [invincibility].nes 
    0x30C7F729, //  Homebrew\Chicken of the Farm.nes 
    0x9DA01E43, //  Homebrew\Chuckout 2.nes 
    0xDFD1A7A0, //  Homebrew\Clarine Test.nes 
    0x5EAACE89, //  Homebrew\Demos\2009 Fireworks.nes 
    0xB352EC45, //  Homebrew\Demos\Candy Shop.nes 
    0xDD7BAD02, //  Homebrew\Demos\Emunespl Famicon Intro.nes 
    0xE854C385, //  Homebrew\Demos\Flowing Palette Demo [blargg].nes 
    0xC915A79F, //  Homebrew\Demos\Full Palette.nes 
    0xC0BACDD7, //  Homebrew\Demos\Gallerynes [8bitpeoples].nes 
    0x475C4E4B, //  Homebrew\Demos\GlitchNES [No Carrier].nes 
    0x651488BF, //  Homebrew\Demos\Happy New Year [NO CARRIER, Batsly Adams].nes 
    0xCB182F80, //  Homebrew\Demos\Libertango.nes 
    0xC9FB3EC4, //  Homebrew\Demos\Moonmagic.nes 
    0x489DB86E, //  Homebrew\Demos\Mr. Brownstone.nes 
    0xA8C659ED, //  Homebrew\Demos\Pretty Young Thing [snowbro].nes 
    0xA467AB46, //  Homebrew\Demos\Rapidemo.nes 
    0x923523AF, //  Homebrew\Demos\Smooth Criminal [snowbro].nes 
    0x12D1F888, //  Homebrew\Demos\Subor v3.0 Music Demo.nes 
    0xA9685216, //  Homebrew\Demos\Sweet Child o' Mine [snowbro].nes 
    0x215581C3, //  Homebrew\Demos\The Swing of Things [snowbro].nes 
    0xDC4DC667, //  Homebrew\Demos\Vegavox II Music Disc.nes 
    0x8A57208E, //  Homebrew\Demos\Wanna to Be Startin' Something [snowbro].nes 
    0xE87EA65C, //  Homebrew\Dig Deeper (Demo).nes 
    0x2F504958, //  Homebrew\Dragon Leap (Web Version 2 2012).nes 
    0xC3F7C60A, //  Homebrew\Dragon Leap.nes 
    0xEBFCC1F7, //  Homebrew\Falling.nes 
    0x5E38BBFB, //  Homebrew\Gold Guardian Gun Girl [Good Tune].nes 
    0x8A2BA796, //  Homebrew\Grave Digger.nes 
    0xC7086951, //  Homebrew\Hacks\1919 (Hard version).nes 
    0x90963850, //  Homebrew\Hacks\1919 (Normal version).nes 
    0x284BC8E5, //  Homebrew\Hacks\1942꞉ Cold Winter [Guyver].nes 
    0x64FDF7B5, //  Homebrew\Hacks\Ball Hack (v0.2) [Knoxburry-Aneurysm].nes 
    0xCBD805D3, //  Homebrew\Hacks\Battle City\Battle City꞉ Back From Source.nes 
    0x7A284F08, //  Homebrew\Hacks\Battle City\Binary City [Shedevr].nes 
    0x60CDD8A5, //  Homebrew\Hacks\Battle City\Warsaw City [Berion].nes 
    0x80A2EFBE, //  Homebrew\Hacks\Beer-aga.nes 
    0xCF102965, //  Homebrew\Hacks\Burning Spaceship꞉ Galg [Guyver].nes 
    0x70607BD8, //  Homebrew\Hacks\Mario in Space [Crush Productions].nes 
    0xF41092D8, //  Homebrew\Hacks\Pikachu Bros. [The Spoony Bard].nes 
    0xA4A23C86, //  Homebrew\Hacks\Pinball II [MAG].nes 
    0x7842B93D, //  Homebrew\Hacks\Pizza Time [dougeff].nes 
    0x79818266, //  Homebrew\Hangman.nes 
    0xB5D98039, //  Homebrew\Inversion (v1.4).nes 
    0x90D2E9F0, //  Homebrew\K.Y.F.F..nes 
    0x536E5200, //  Homebrew\Kitty's Catch.nes 
    0xD46B3C2D, //  Homebrew\Laser Puzzle Game.nes 
    0xA9AE836E, //  Homebrew\Lickshot!.nes 
    0x79B1EBC9, //  Homebrew\Malaria꞉ A Call to Action (Demo).nes 
    0x6E9A0BD0, //  Homebrew\Micro Knight (v1.3).nes 
    0xF9A53E72, //  Homebrew\Mini Brix Battle.nes 
    0xDBD6FBE1, //  Homebrew\NES Virus Cleaner.nes 
    0x9A31304C, //  Homebrew\Pegs (alt).nes 
    0x2D815920, //  Homebrew\Pegs.nes 
    0xC6F5D0C0, //  Homebrew\Shutshimi NES.nes 
    0x6CC687F0, //  Homebrew\Teletime - Music Cart.nes 
    0xD8ABD78E, //  Homebrew\Tests\NMI Music Test.nes 
    0xE0FF82B1, //  Homebrew\Tic-Tac XO.nes 
    0xB18CD78E, //  Homebrew\Tools\Palette-Generator.nes 
    0xB41D96FB, //  Homebrew\Translations\English\Penguin-kun Wars [Penguin Translations v1.012, Spinner 8 and friends].nes 
    0x147B5B05, //  Homebrew\Translations\English\Popeye Lingo Game [KingMike's Translations].nes 
    0xA6EF77E2, //  Homebrew\Translations\Russian\Arkanoid.nes 
    0xD8D96A47, //  Homebrew\Translations\Russian\Гонки Race.nes 
    0xDA6D73E1, //  Homebrew\Translations\Russian\бомба.nes 
    0x516ED116, //  Homebrew\UXO-Unexploded-Ordnance.nes 
    0x07DE2720, //  Homebrew\Upsad Down (v0.6)[Fadest].nes 
    0x171251E3, //  Licensed Japan\1942.nes 
    0xFCDACA80, //  Licensed Japan\Elevator Action (rev0).nes 
    0x7A3A49ED, //  Licensed Japan\Elevator Action (rev1).nes 
    0xC05A365B, //  Licensed Japan\Exed Exes.nes 
    0xCF701DA4, //  Licensed Japan\Flappy.nes 
    0x75A7E399, //  Licensed Japan\Lot Lot.nes 
    0x5E345B6D, //  Licensed Japan\MagMax.nes 
    0xE30B2BCF, //  Licensed Japan\Pachi Com.nes 
    0x02D7976B, //  Licensed Japan\Penguinくん Wars.nes 
    0x728BFA8D, //  Licensed Japan\Route 16 Turbo.nes 
    0x26BD6EC6, //  Licensed Japan\Star Luster.nes 
    0x08439D55, //  Licensed Japan\Tag Team Pro-Wrestling (Special Version).nes 
    0x32FA246F, //  Licensed Japan\Tag Team Pro-Wrestling.nes 
    0x06F15215, //  Licensed Japan\Thexder.nes 
    0xB3C30BEA, //  Licensed Japan\Xevious (rev0).nes 
    0xA2469526, //  Licensed Japan\Xevious (rev1).nes 
    0x455CA7DE, //  Licensed Japan\囲碁名鑑.nes 
    0xEC0517C4, //  Licensed Japan\頭脳戦艦 Galg.nes 
    0x2AC87283, //  Licensed North America\Elevator Action.nes 
    0x1DB07C0D, //  Licensed North America\Galaga꞉ Demons of Death.nes 
    0x77BF8B23, //  Licensed North America\Hydlide.nes 
    0x81389607, //  Licensed North America\MagMax.nes 
    0x699FA085, //  Licensed North America\Othello.nes 
    0xBF250AF2, //  Licensed North America\Tag Team Wrestling.nes 
    0xDFD70E27, //  Licensed North America\Xevious.nes 
    0xACADEA97, //  Modern\Enigmacore.nes 
    0x9D20AB45, //  Modern\GameCube Edition\Balloon Fight.nes 
    0x814F7E11, //  Modern\GameCube Edition\Golf (Japan).nes 
    0xDE01497A, //  Modern\GameCube Edition\Pinball (rev1).nes 
    0xD6BBD8BA, //  Modern\GameCube Edition\Tennis.nes 
    0x2FD91D7E, //  Modern\GameCube Edition\五目ならべ.nes 
    0x4B84EBA2, //  Modern\Get'em Gary.nes 
    0x4E6B9078, //  Modern\Micro Mages.nes 
    0xA1CAEE04, //  Modern\NES Virus Cleaner.nes 
    0x6790630B, //  Modern\Namco Museum Archives\Star Luster.nes 
    0xD7EDE073, //  Modern\Namco Museum Archives\Xevious.nes 
    0x52196024, //  Modern\Virtual Console\Elevator Action (Japan).nes 
    0x02EB5B4A, //  Modern\Virtual Console\Elevator Action (North America).nes 
    0x5CDD800F, //  Modern\Virtual Console\Star Luster.nes 
    0xA1C036FC, //  Multicarts\extracts\Sudo King.nes 
    0x47AB4D52, //  Playchoice\1942.nes 
    0x529CA3C4, //  Plug-and-Play\extracts\Eating.nes 
    0x0101D156, //  Plug-and-Play\extracts\Pikachu.nes 
    0xD903B9F2, //  Plug-and-Play\extracts\Pulveration.nes 
    0x5096513B, //  Samples\Blazing Rangers.nes 
    0xA1739321, //  Samples\NES Virus Cleaner (Demo).nes 
    0xD3E2C2A0, //  Samples\炎のRangerman.nes 
    0x43D30C2F, //  Unlicensed North America\Ms. Pac-Man.nes 
    0x52988835, //  Unreleased\Hoppin' Mad (4.0).nes 
    0xB9582F60, //  Unreleased\Mobile Suit Z Gundam꞉ Hot Scramble.nes 
    0xA9068D17, //  Unreleased\Virus (1989).nes 
    0x23F38647, //  Unreleased\頭脳戦艦 Galg.nes 
/**** 13 mapper=0 prgrom=32K chrrom=8K mirror=H system=SYSTEM_PAL */
    0x0C5F3973, //  Bad Dumps\Datel Pro Action Replay (v1.2)[wrong reset vector].nes 
    0x9EFD9332, //  Bootleg Hacks\South Park.nes 
    0x69C1AB7F, //  Homebrew\Demos\poNiES [Digital Sounds System].nes 
    0x46BE52E3, //  Homebrew\Mr. Splash (Omake Books).nes 
    0x999584A8, //  Licensed PAL\Galaga꞉ Demons of Death.nes 
    0xA1C0DA00, //  Licensed PAL\Mario Bros. Classic.nes 
    0xD745D7CB, //  Licensed PAL\Xevious.nes 
/**** 14 mapper=0 prgrom=32K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x4B143FB6, //  Bootleg Hacks\Shoot UFO.nes 
    0x543C6EDE, //  Bootleg Singles\Front Fareast Magic Card\(F1157) Duck - 醜小鴨.nes 
    0x39C26720, //  Homebrew\Bootèe.nes 
    0x9498ECF6, //  Homebrew\Cadaverion [Mojon Twins].nes 
    0x68AAED3D, //  Homebrew\Che-Man [Mojon Twins].nes 
    0x7FEF8A49, //  Homebrew\Cheril Perils Classic [Mojon Twins].nes 
    0x292BCE21, //  Homebrew\Demos\FamiTone Audio Library.nes 
    0xC0A78DB1, //  Homebrew\Draiocht.nes 
    0xF9BC6AED, //  Homebrew\Dúshlán.nes 
    0x7239BF47, //  Homebrew\From Below (v1.0).nes 
    0xB06A657F, //  Homebrew\GemVenture (Beta)[Tom Livak].nes 
    0xD83D8F31, //  Homebrew\Hacks\Apocalyptic Explosion.nes 
    0x4B2CA0E8, //  Homebrew\Jet Paco - Space Agent!.nes 
    0x15FB4D82, //  Homebrew\Lunar Limit (bug-fixed).nes 
    0xFAE31288, //  Homebrew\Lunar Limit (original).nes 
    0xD34D6E33, //  Homebrew\Machine Cave (v1.9).nes 
    0xEB97DCDC, //  Homebrew\Multidude.nes 
    0xFE731297, //  Homebrew\Robot Finds Kitten.nes 
    0xBA1BEFD1, //  Homebrew\Sgt. Helmet - Training Day.nes 
    0x3F6567E2, //  Homebrew\Snakky.nes 
    0x0E153C87, //  Homebrew\Sting.nes 
    0xD195DFDC, //  Homebrew\Yun.nes 
    0xDD3115DB, //  Homebrew\Zombie Calavera Prologue.nes 
    0x6AA4574C, //  Homebrew\Zooming Secretary (v1.02).nes 
    0xDF67DAA1, //  Licensed Japan\Block Set.nes 
    0x9B506A48, //  Licensed Japan\Wrecking Crew.nes 
    0x3D564757, //  Licensed North America\10-Yard Fight.nes 
    0x50D39ABE, //  Modern\Beer Slinger.nes 
    0x22C94D60, //  Modern\Blow'em Out.nes 
    0xDFEFE8CD, //  Modern\GameCube Edition\Clu Clu Land.nes 
    0x9D779B08, //  Modern\GameCube Edition\Donkey Kong.nes 
    0x8DB31730, //  Modern\GameCube Edition\Mario Bros..nes 
    0x1132AEA9, //  Modern\Perfect Pair.nes 
    0x6166BB81, //  Modern\Super UWOL!.nes 
    0x7E58DF89, //  Multicarts\extracts\Dream Fighter (Asder 20-in-1).nes 
    0xA9C69118, //  Multicarts\extracts\JP Ronny (Asder 20-in-1).nes 
    0xFF1CEFAA, //  Unlicensed Elsewhere\Duck - 醜小鴨.nes 
    0xD62BC37E, //  Unlicensed Taiwan&amp;Hong Kong\Dream Fighter.nes 
    0x0FAFC70B, //  Unlicensed Taiwan&amp;Hong Kong\Duck - 醜小鴨.nes 
    0x757EFB63, //  Unlicensed Taiwan&amp;Hong Kong\Skate Boy.nes 
/**** 15 mapper=0 prgrom=32K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x6200C69D, //  Unlicensed Elsewhere\Склад №18.nes 
    0xEAD40557, //  Unlicensed Elsewhere\Угадайка.nes 
    0x61721163, //  Unlicensed Elsewhere\балда - русская народная игра.nes 
/**** 16 mapper=0 prgrom=32K prgram=8K chrrom=8K mirror=H */
    0xF02CF867, //  Homebrew\Demos\Karmic's Music Demo.nes 
    0x68B7AAF2, //  Homebrew\Snake.nes 
/**** 17 mapper=0 prgrom=32K chrrom=32K mirror=H */
    0x7B9ED6DC, //  Homebrew\Mr. Splash (Columbia).nes 
    0xD498456A, //  Homebrew\Mr. Splash (Mondo21 Gg).nes 
    0x11ACC41A, //  Homebrew\Mr. Splash (Mondo21 Meteor).nes 
    0x682EB854, //  Homebrew\Mr. Splash (Mondo21).nes 
/**** 18 mapper=0 prgrom=16K prgnvram=8K chrrom=8K mirror=H battery=1 */
    0x47BCE4D9, //  Homebrew\Pac-Man [Kef Schecter](2016-xx-xx).nes 
    0x0B75F4FB, //  Homebrew\Pac-Man [Kef Schecter](2019-03-19).nes 
    0xD889FAA9, //  Homebrew\Pogo Cats.nes 
/**** 19 mapper=0 prgrom=32K prgnvram=8K chrrom=8K mirror=H battery=1 */
    0xE70C90F3, //  Homebrew\Donkey Kong (vertical orientation)[Sumez].nes 
    0xC6515C2E, //  Modern\Virtual Console\Wrecking Crew.nes 
/**** 20 mapper=0 prgrom=8K chrrom=8K mirror=V */
    0x752BA39A, //  BIOS\Sharp My Computer TV C1.nes 
    0x46A01115, //  Unlicensed Taiwan&amp;Hong Kong\Magic Jewelry.nes 
    0x71213AA1, //  Unlicensed Taiwan&amp;Hong Kong\小瑪琍 (C&amp;E).nes 
/**** 21 mapper=0 prgrom=16K chrram=8K mirror=V */
    0xC2B40597, //  Educational Computers\extracts\Painter.nes 
    0xEF6C5FAC, //  Homebrew\!Clik! (rev0).nes 
    0x3E595BD2, //  Homebrew\!Clik! (rev1).nes 
    0x85714D60, //  Homebrew\Ambushed (rev0).nes 
    0x75E7F405, //  Homebrew\Ambushed (rev1).nes 
    0x28FC0F77, //  Homebrew\Demos\Dma Delay.nes 
    0xE0AB06D8, //  Homebrew\Demos\NES 071910-1.nes 
    0x7904427A, //  Homebrew\Demos\NES 07210-1.nes 
    0x8B241428, //  Homebrew\Demos\Raycaster [Tokumaru].nes 
    0xEF5106EF, //  Homebrew\Demos\The Tarot Membler [Memblers].nes 
    0x456120AC, //  Homebrew\Flappy Block (version 2).nes 
    0x0851843D, //  Homebrew\GSM.nes 
    0x926848A1, //  Homebrew\Meteor Guard 2.nes 
    0xE6FADC3F, //  Homebrew\Meteor Guard.nes 
    0x20FCB4E4, //  Homebrew\Ninja Muncher.nes 
    0xFA78DBF7, //  Homebrew\No Points.nes 
    0x6707C67A, //  Homebrew\Obstacle Trek.nes 
    0x7F3E2D74, //  Homebrew\Pong 1k2p.nes 
    0x4ACDF560, //  Homebrew\That's Whack.nes 
    0x4477B906, //  Homebrew\The Invasion.nes 
    0x30F7ED38, //  Homebrew\Theremin.nes 
    0x17E013A2, //  Unlicensed Taiwan&amp;Hong Kong\野球拳 - Wild Ball.nes 
/**** 22 mapper=0 prgrom=16K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x6C80EE73, //  Homebrew\Bomber 4K.nes 
    0xFF033DC2, //  Homebrew\Fizz Buzz.nes 
    0x8F055EAB, //  Homebrew\Flappy Block (version 1).nes 
    0xEA7BA0A9, //  Homebrew\For Points (rev0).nes 
    0xB7BC47A3, //  Homebrew\For Points (rev1).nes 
    0x5D25774C, //  Homebrew\Froggy.nes 
    0x246C0CA6, //  Homebrew\Tools\Button Logger tool, Musical Controller Toy, and Stuff.nes 
/**** 23 mapper=0 prgrom=16K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x53A3F6F8, //  Educational Computers\extracts\Calculator 2.nes 
/**** 24 mapper=0 prgrom=16K chrrom=8K mirror=V */
    0xAE43E9BD, //  Bootleg Hacks\Boat Race.nes 
    0x8DDB4A27, //  Bootleg Hacks\Future Tank.nes 
    0x10522C7B, //  Bootleg Hacks\Inventor\Bitha.nes 
    0x39F6AE18, //  Bootleg Hacks\Inventor\Golgotha.nes 
    0xF80B49C7, //  Bootleg Hacks\Inventor\TNT.nes 
    0x5A59ACFF, //  Bootleg Hacks\Tank 1990 [vertical mirroring, disable menu].nes 
    0xE9530055, //  Bootleg Singles\Donkey Kong 3 (LA05).nes 
    0x11ED6E02, //  Bootleg Singles\Donkey Kong Jr. (LA04).nes 
    0xE46A05C3, //  Bootleg Singles\Excitebike (LA21).nes 
    0x66A71D09, //  Bootleg Singles\F-1 Race (LA16).nes 
    0x59081961, //  Bootleg Singles\Lode Runner (LA59).nes 
    0x656196A6, //  Bootleg Singles\Mappy (LA22).nes 
    0x673241B7, //  Bootleg Singles\Poker 集錦.nes 
    0x8F247BE4, //  Bootleg Singles\TouchGamePlayer\Bird Week.nes 
    0xA7493D4D, //  Bootleg Singles\TouchGamePlayer\Brush Roller.nes 
    0xADAE3063, //  Bootleg Singles\TouchGamePlayer\Championship Lode Runner.nes 
    0xD4AD48BF, //  Bootleg Singles\TouchGamePlayer\Defender II.nes 
    0xFFFD46C9, //  Bootleg Singles\TouchGamePlayer\Karateka.nes 
    0x24B81761, //  Bootleg Singles\TouchGamePlayer\Lunar Ball.nes 
    0x1B2395FA, //  Bootleg Singles\TouchGamePlayer\Mappy.nes 
    0xFDD1BB64, //  Bootleg Singles\TouchGamePlayer\Millipede.nes 
    0xBF384774, //  Bootleg Singles\TouchGamePlayer\Tag Team Match꞉ M.U.S.C.L.E..nes 
    0xF038FE34, //  Bootleg Singles\TouchGamePlayer\いっき.nes 
    0xCF8C0D13, //  Bootleg Singles\TouchGamePlayer\超時空要塞 Macross.nes 
    0x91987B41, //  Bootleg Singles\麻雀.nes 
    0xA189843D, //  Compatibility Hacks\Magic Jewelry [PRG doubled to 16 KiB].nes 
    0x834A117D, //  Compatibility Hacks\Magic Jewelry [PRG padded to 16 KiB].nes 
    0x7A3CA5B7, //  Homebrew\8-Bit Hero Trainer [Psych].nes 
    0x0E598956, //  Homebrew\BallBounce.nes 
    0xACECF26D, //  Homebrew\Battleball Udapong.nes 
    0x7967E98C, //  Homebrew\Beat'em &amp; Eat'em (beta without music).nes 
    0x3ED2DD12, //  Homebrew\Big City Sliding Blaster (NesDev 2014).nes 
    0xA91E5266, //  Homebrew\Big City Sliding Blaster.nes 
    0x6D6FF82E, //  Homebrew\Block Dude (NESdev 2018).nes 
    0x33839BC0, //  Homebrew\Block Dude.nes 
    0x4214AB84, //  Homebrew\Concentration Room꞉ Accident at Hombon Lab (v0.01)[Damian Yerrick].nes 
    0x356D5AB2, //  Homebrew\Concentration Room꞉ Accident at Hombon Lab (v0.02 alt)[Damian Yerrick].nes 
    0xD8EA7672, //  Homebrew\Concentration Room꞉ Accident at Hombon Lab (v0.02)[Damian Yerrick].nes 
    0x074306F5, //  Homebrew\Conway's Life Simulator (32x32 Grid).nes 
    0x3901D467, //  Homebrew\Conway's Life Simulator (64x32 Grid).nes 
    0xEC7C2070, //  Homebrew\Copyright Restorations\Bookyman [based on Gluk Video version].nes 
    0xF5A67117, //  Homebrew\Cornball Cocksuckers.nes 
    0x84FEF8F5, //  Homebrew\Demos\Boom!!! [Tokumaru].nes 
    0x1FDDFB4B, //  Homebrew\Demos\Copper Bars Demo (v1.1).nes 
    0xC93FEFF4, //  Homebrew\Demos\Displaywin.nes 
    0xEC93FE27, //  Homebrew\Demos\Generic Text Scroller Mod [CartCollector].nes 
    0xC26262F6, //  Homebrew\Demos\Generic Text Scroller [CartCollector].nes 
    0xBD4AE813, //  Homebrew\Demos\Hell.nes 
    0xF10BD835, //  Homebrew\Demos\NSF PowerPak (PowerPak Only).nes 
    0x01518223, //  Homebrew\Demos\Physics.nes 
    0x8DCC6F67, //  Homebrew\Demos\Splash Screen [Homebrew Games].nes 
    0x3FBF0480, //  Homebrew\Demos\Wall.nes 
    0x48721074, //  Homebrew\Demos\Wideboy 2 [Chris Covell].nes 
    0xC7DF2515, //  Homebrew\Demos\Xmas 2001 [Chris Covell].nes 
    0x544E4FB2, //  Homebrew\Flappy Bird (easy difficulty).nes 
    0xB3D917B2, //  Homebrew\Flappy Bird (normal difficulty).nes 
    0x924B1C86, //  Homebrew\Go [Zero Soul].nes 
    0x01D19D35, //  Homebrew\Hacks\Battle City\Penguin Shoot [Mitz].nes 
    0x5D6F1964, //  Homebrew\Hacks\Bomber Pacman.nes 
    0xF8080EA5, //  Homebrew\Hacks\Championship Lode Runner 改 [Shinya].nes 
    0xACA68AB8, //  Homebrew\Hacks\Chocobo's Dungeon World! [Celice].nes 
    0xBE9EE1EC, //  Homebrew\Hacks\Circus Mario [Akkii].nes 
    0xF866A1CA, //  Homebrew\Hacks\Deflower [pacnsacdave].nes 
    0x1917AE3B, //  Homebrew\Hacks\Lode Runner Rebuilt.nes 
    0xC0DE8B7F, //  Homebrew\Hacks\Lode Runner 改 [Shinya].nes 
    0xC0BA34EE, //  Homebrew\Hacks\Mario Runner꞉ Приключения в Подземелье [Guyver].nes 
    0x27744873, //  Homebrew\Hacks\Marippy [DASTARD].nes 
    0xCEB4E5B4, //  Homebrew\Hacks\Pooyan - Lomax Edition.nes 
    0x13E8F24A, //  Homebrew\Itscores.nes 
    0x6C9AD9E5, //  Homebrew\Kaboom 3.nes 
    0xC1925634, //  Homebrew\Kaboom Controller Pad.nes 
    0xCA6571EC, //  Homebrew\Manhole.nes 
    0x07C98AB5, //  Homebrew\MashyMashy.nes 
    0xB5165FE2, //  Homebrew\Mouser II.nes 
    0x0B0C4573, //  Homebrew\NES 15 (NTSC).nes 
    0x0F877A8C, //  Homebrew\Paddleboth.nes 
    0x254CC196, //  Homebrew\Paddleneat.nes 
    0x37313A4B, //  Homebrew\Particle Src.nes 
    0xD3FD2BC2, //  Homebrew\Pong (v1).nes 
    0x76FDF02B, //  Homebrew\Pong for the NES.nes 
    0x3690FEDC, //  Homebrew\Pong.nes 
    0x33536E85, //  Homebrew\Prezv2.nes 
    0xD2524629, //  Homebrew\Pung! Balls of Steel.nes 
    0x97D83366, //  Homebrew\Score.nes 
    0xC86FE729, //  Homebrew\Serve.nes 
    0x1542D79B, //  Homebrew\Shut Fly.nes 
    0x1FF449DB, //  Homebrew\Slappin' Bitches.nes 
    0x64C2913F, //  Homebrew\Snake Off.nes 
    0x4C2BA67C, //  Homebrew\Sound.nes 
    0xB331CF68, //  Homebrew\Space Fighter.nes 
    0x5EA4DFE9, //  Homebrew\Spacy Shooty.nes 
    0xC077E346, //  Homebrew\Split Second.nes 
    0x37FD1FD5, //  Homebrew\Tests\Colorwin Window.nes 
    0x3D0337BA, //  Homebrew\Tests\NES Overclock Test (r1a).nes 
    0x032AE256, //  Homebrew\Tests\Password Save Demo꞉ Generate.nes 
    0x844F915C, //  Homebrew\Tests\Password Save Demo꞉ Test.nes 
    0xF5E62944, //  Homebrew\Tests\Zap Ruder (v0.03).nes 
    0x0BF31DD8, //  Homebrew\Tetris Super Alpha.nes 
    0x4EF2AD9F, //  Homebrew\Thwaite (v0.01).nes 
    0xF7D8766D, //  Homebrew\Thwaite (v0.03).nes 
    0xA84CC326, //  Homebrew\Translations\English\Ikki [Psyklax].nes 
    0x105EF60D, //  Homebrew\Translations\English\Kinnikuman꞉ Muscle Tag Match [Stardust Crusaders].nes 
    0x7C113D74, //  Homebrew\Translations\English\Mahjong [Psyklax](Arabic numerals).nes 
    0x28219047, //  Homebrew\Translations\English\Mahjong [Psyklax].nes 
    0xA62E7660, //  Homebrew\Translations\English\Super Dimensional Fortress꞉ Macross [BlackPaladin].nes 
    0xBABA0D94, //  Homebrew\Vigilante Ninja꞉ Purple Cape Man.nes 
    0x169697FB, //  Homebrew\Vigilante SMS Port (Alpha 1).nes 
    0xD91104F1, //  Licensed Japan\4人打ち麻雀 (rev0).nes 
    0x84BE00E9, //  Licensed Japan\4人打ち麻雀 (rev1).nes 
    0xF2FC8212, //  Licensed Japan\Bomberman.nes 
    0xADB5D0B3, //  Licensed Japan\Championship Lode Runner.nes 
    0xB87AB35A, //  Licensed Japan\Circus Charlie.nes 
    0xE211B93A, //  Licensed Japan\Donkey Kong Jr. (rev0).nes 
    0x63AEA200, //  Licensed Japan\Donkey Kong Jr. の算数遊び.nes 
    0x49AEB3A6, //  Licensed Japan\Excitebike.nes 
    0xE0604F76, //  Licensed Japan\F-1 Race.nes 
    0x980BE936, //  Licensed Japan\Hyper Olympic.nes 
    0x915A53A7, //  Licensed Japan\Hyper Sports (rev0).nes 
    0x6AE762AE, //  Licensed Japan\Hyper Sports (rev1).nes 
    0x8BA75848, //  Licensed Japan\Karateka.nes 
    0x0A3FC393, //  Licensed Japan\Lode Runner.nes 
    0x2F55BE88, //  Licensed Japan\Lunar Ball.nes 
    0x5D1301C5, //  Licensed Japan\Mappy.nes 
    0x7F495283, //  Licensed Japan\Pooyan.nes 
    0x64C96F53, //  Licensed Japan\Raid on Bungeling Bay (rev0).nes 
    0xA547A6EC, //  Licensed Japan\Raid on Bungeling Bay (rev1).nes 
    0xC67865A2, //  Licensed Japan\Star Force.nes 
    0x493BD2FF, //  Licensed Japan\Star Gate.nes 
    0xB8B9ACA3, //  Licensed Japan\Wild Gunman (rev0).nes 
    0x821FEB7A, //  Licensed Japan\いっき.nes 
    0xA08B4701, //  Licensed Japan\キン肉マン꞉ Muscle Tag Match (rev0).nes 
    0xA07C1F81, //  Licensed Japan\キン肉マン꞉ Muscle Tag Match (rev1).nes 
    0xC1D7AB1D, //  Licensed Japan\超時空要塞 Macross.nes 
    0xB8535CA3, //  Licensed Japan\麻雀 (rev0).nes 
    0xA9842027, //  Licensed Japan\麻雀 (rev1).nes 
    0x23D91BC6, //  Licensed Japan\麻雀 (rev2).nes 
    0xDB9DCF89, //  Licensed North America\Bomberman.nes 
    0xA2AF25D0, //  Licensed North America\Defender II.nes 
    0xAF5676DE, //  Licensed North America\Lode Runner.nes 
    0x988B446D, //  Licensed North America\Lunar Pool.nes 
    0xAE52DECE, //  Licensed North America\Millipede.nes 
    0xD308D52C, //  Licensed North America\Raid on Bungeling Bay.nes 
    0x8FF31896, //  Licensed North America\Tag Team Match꞉ M.U.S.C.L.E..nes 
    0xCD883CDC, //  Maintenance\NTF2 Joystick Test.nes 
    0x77D59400, //  Maintenance\Power Pad Test.nes 
    0xD1FC0ED6, //  Modern\GBA e-Reader\Excitebike.nes 
    0x662B8C9C, //  Modern\GameCube Edition\Excitebike.nes 
    0xE8000BF7, //  Modern\GameCube Edition\Pinball (rev0).nes 
    0xE6007055, //  Modern\Hudson Best Collection\Bomberman.nes 
    0xA2926027, //  Modern\Hudson Best Collection\Championship Lode Runner.nes 
    0xE8676D77, //  Modern\Hudson Best Collection\Lode Runner.nes 
    0x7422EF17, //  Modern\Hudson Best Collection\Nuts &amp; Milk.nes 
    0x14AD8564, //  Modern\Hudson Best Collection\Star Force.nes 
    0x0808BAA6, //  Modern\Namco Museum Archives\Mappy.nes 
    0xFF33A325, //  Multicarts\extracts\350-in-1 (menu).nes 
    0xA379D7BC, //  Multicarts\extracts\64-in-1 (menu).nes 
    0x9E09371D, //  Playchoice\Duck Hunt.nes 
    0x2C25A6E0, //  Playchoice\Excitebike.nes 
    0x70E439DA, //  Playchoice\Hogan's Alley.nes 
    0x407DC088, //  Playchoice\Mario Bros..nes 
    0x32F7CC4E, //  Playchoice\Wild Gunman.nes 
    0xC7735D54, //  Plug-and-Play\extracts\Conqueror.nes 
    0x812BED77, //  Plug-and-Play\extracts\Magic Bubble.nes 
    0x994BF21E, //  Plug-and-Play\extracts\Orchard Kavass.nes 
    0x8A7D9467, //  Samples\Russian Roulette (v0.01)[Damian Yerrick].nes 
    0xF4E7A58C, //  Samples\Russian Roulette (v0.02)[Damian Yerrick].nes 
    0x9C4B07D9, //  Unlicensed China\中国麻將.nes 
    0x721961A4, //  Unlicensed South Korea\Block Puzzle.nes 
    0xF6B67BE1, //  Unlicensed Taiwan&amp;Hong Kong\Bookyman.nes 
    0xAC92E9E0, //  Unlicensed Taiwan&amp;Hong Kong\Brush Roller.nes 
    0x65B6AF68, //  Unlicensed Taiwan&amp;Hong Kong\Poker 集錦.nes 
    0x2969A5C1, //  Unlicensed Taiwan&amp;Hong Kong\Pyramid.nes 
    0x0F3B89E3, //  Unreleased\F-1 Race.nes 
    0xE9EDBA24, //  Unreleased\Karateka.nes 
    0xF5B9EE9D, //  Unreleased\Tetris꞉ The Soviet Mind Game (build #1).nes 
    0x51754C3C, //  Unreleased\Tetris꞉ The Soviet Mind Game (build #3).nes 
/**** 25 mapper=0 prgrom=16K chrrom=8K mirror=V system=SYSTEM_PAL */
    0x43CFE6AB, //  Homebrew\Fighter F-8000.nes 
    0x0C548E97, //  Homebrew\NES 15 (PAL).nes 
    0x8A0C7337, //  Licensed PAL\Excitebike.nes 
/**** 26 mapper=0 prgrom=16K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0x7BAF8142, //  Bootleg Hacks\Anon Gunman.nes 
    0xC267D861, //  Bootleg Hacks\Bestiarian.nes 
    0x283D7727, //  Bootleg Hacks\Cow Boy.nes 
    0x5E8C77DB, //  Bootleg Hacks\Fast Gunman.nes 
    0xA39A8063, //  Bootleg Hacks\Forest Guard.nes 
    0x327DDDEC, //  Bootleg Hacks\Gunfignt.nes 
    0x431A5F59, //  Bootleg Hacks\Inventor\Debar Bomb.nes 
    0xCFD8D4A5, //  Bootleg Hacks\Inventor\Dinosaur'Bale.nes 
    0xFA08CCBF, //  Bootleg Hacks\Inventor\Pop.nes 
    0xC616BAD5, //  Bootleg Hacks\Inventor\Snowfield Shoot.nes 
    0xA671DA25, //  Bootleg Hacks\Inventor\Space 2050.nes 
    0xA7C6C842, //  Bootleg Hacks\Rnpid Gunnery.nes 
    0x44340DA6, //  Bootleg Hacks\Sharp Shooter.nes 
    0x524BC479, //  Bootleg Hacks\Shooter (v1).nes 
    0x407449DA, //  Bootleg Hacks\Shooter (v2).nes 
    0x3488A174, //  Bootleg Singles\Duck Hunt (LA07).nes 
    0x5D4574E0, //  Bootleg Singles\Hogan's Alley (LA06).nes 
    0x473160CC, //  Bootleg Singles\TouchGamePlayer\Donkey Kong Jr..nes 
    0x18D1A0B8, //  Bootleg Singles\TouchGamePlayer\DonkeyKongJrMath.nes 
    0xAF950ABC, //  Homebrew\Astroid.nes 
    0xEC45682C, //  Homebrew\Chopper.nes 
    0x3D2E6A7B, //  Homebrew\Eat Fish.nes 
    0x5A2248D0, //  Homebrew\Game of Life.nes 
    0xDD518261, //  Homebrew\Jupiter Scope 2.nes 
    0x3CC4AB3C, //  Homebrew\Lawn Mower (NesDev 2011).nes 
    0x4C1E44B0, //  Homebrew\Lawn Mower.nes 
    0xE0BA29E8, //  Homebrew\Mineshaft.nes 
    0x1F3C61BA, //  Homebrew\Simplistic.nes 
    0xE334716F, //  Homebrew\Super Floofy Sheepie.nes 
    0xB3D74C0D, //  Licensed Japan\Donkey Kong 3.nes 
    0x4864C304, //  Licensed Japan\Donkey Kong Jr. (rev1).nes 
    0x24598791, //  Licensed Japan\Duck Hunt.nes 
    0xFF24D794, //  Licensed Japan\Hogan's Alley.nes 
    0x656D4265, //  Licensed Japan\Urban Champion.nes 
    0x5112DC21, //  Licensed Japan\Wild Gunman (rev1).nes 
    0x0504B007, //  Licensed North America\Donkey Kong Jr. Math.nes 
    0xFCEBCC5F, //  Licensed PAL\M82 Game Selectable Working Product Display.nes 
    0x3F23C194, //  Modern\GBA e-Reader\Donkey Kong Jr..nes 
    0xEFB2B7E8, //  Unlicensed Elsewhere\Arctic Adventure꞉ The Penguin &amp; Seal.nes 
    0xBC06543C, //  Unlicensed Elsewhere\Bookyman.nes 
    0x2472C3EB, //  Unlicensed North America\Pyramid (rev1).nes 
/**** 27 mapper=0 prgrom=16K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0xB4767838, //  Bootleg Singles\Urban Champion (LA17).nes 
    0xDC621DD1, //  Homebrew\Bugfixes, Improvements\Склад №18 [remove unused PRG space].nes 
/**** 28 mapper=0 prgrom=16K chrrom=8K chrram=8K mirror=V */
    0x4233B70E, //  Bad Dumps\野球拳 - Wild Ball [CHR-ROM on CHR-RAM game].nes 
/**** 29 mapper=0 prgrom=16K prgram=8K chrrom=8K mirror=V */
    0x15DE054B, //  Homebrew\Demos\NES BG Scaling-Rotation Demo [Bregalad].nes 
/**** 30 mapper=0 prgrom=32K chrram=8K mirror=V */
    0x35063511, //  Homebrew\Blob Quest [Tom Livak].nes 
    0xEC50DA84, //  Homebrew\Demos\Music.nes 
    0x58994EEB, //  Homebrew\Love Story.nes 
    0x3F7977D5, //  Homebrew\NSF Players\Super Shinobi Music.nes 
    0xB2E67A9F, //  Homebrew\Soko Man.nes 
    0x32E02CB8, //  Maintenance\Port Test Cartridge.nes 
    0x851CEA10, //  Modern\8 Bit Xmas 2009꞉ Snowball Fight (download).nes 
    0xA6E02975, //  Modern\8 Bit Xmas 2010꞉ Jolly Joyriding (download).nes 
    0xB655C53A, //  Multicarts\extracts\Block Force (Tetris Family 9-in-1).nes 
    0x71CAF097, //  Unlicensed Taiwan&amp;Hong Kong\3D Block (RCM).nes 
/**** 31 mapper=0 prgrom=32K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x06F3CA14, //  Homebrew\I Wanna Flip the Sky.nes 
    0xBEDECC17, //  Homebrew\Memory (Squirrel's Kitchen).nes 
    0x2EAADD18, //  Homebrew\NESert Golfing (alt).nes 
    0x3EC19565, //  Homebrew\NESert Golfing - Tournament Edition.nes 
    0x7B26BEA4, //  Homebrew\NESert Golfing.nes 
    0x6339ABE6, //  Homebrew\Tools\Allpads Controller Test (v9)[Damian Yerrick].nes 
/**** 32 mapper=0 prgrom=32K chrram=8K mirror=V system=SYSTEM_NTSC */
    0xD308E347, //  Educational Computers\extracts\Solitaire.nes 
/**** 33 mapper=0 prgrom=32K prgram=8K chrram=8K mirror=V */
    0xA15609EB, //  Homebrew\Demos\Nestronome.nes 
    0xA1F87DC0, //  Homebrew\Invaders Must Die!.nes 
/**** 34 mapper=0 prgrom=32K chrrom=8K mirror=V */
    0x40498C2C, //  Bad Dumps\Fancy Bros [reset vector returns to multicart menu].nes 
    0xC1DF7A99, //  Bad Dumps\Magic Jewelry [PRG overdump].nes 
    0x950A326F, //  Bootleg Hacks\Air Umbrella.nes 
    0xC88B205B, //  Bootleg Hacks\Bicycle Race.nes 
    0x43F730E3, //  Bootleg Hacks\Goodhand.nes 
    0x1BB9F39E, //  Bootleg Hacks\Inventor\Billiard 2008.nes 
    0x9A135E82, //  Bootleg Hacks\Inventor\Bounce.nes 
    0x8CC9CB95, //  Bootleg Hacks\Inventor\Cobra of the Sky.nes 
    0xBA97EC84, //  Bootleg Hacks\Inventor\Conte Enegy.nes 
    0xF6D845BB, //  Bootleg Hacks\Inventor\Football.nes 
    0xAE498559, //  Bootleg Hacks\Inventor\Ladangel.nes 
    0xBA8C3B55, //  Bootleg Hacks\Inventor\Penguin.nes 
    0xA69A0B02, //  Bootleg Hacks\Inventor\Tiny Toon.nes 
    0x1C201099, //  Bootleg Hacks\Mushroom.nes 
    0xFEA85A4A, //  Bootleg Hacks\Risker.nes 
    0x3BF08F1B, //  Bootleg Hacks\Shift.nes 
    0x1185215B, //  Bootleg Hacks\Super Tetris II.nes 
    0x40C0815B, //  Bootleg Hacks\方塊.nes 
    0x1BA44397, //  Bootleg Singles\Bung Game Doctor\(B002) Super Arabian.nes 
    0x8FA53D20, //  Bootleg Singles\Bung Game Doctor\(B003) Donkey Kong Jr. Math.nes 
    0x2721E7B3, //  Bootleg Singles\Bung Game Doctor\(B005) Balloon Fight.nes 
    0x27F29611, //  Bootleg Singles\Bung Game Doctor\(B010) Chack'n Pop.nes 
    0x3784AFA0, //  Bootleg Singles\Bung Game Doctor\(B012) Championship Lode Runner.nes 
    0x0BAA5AB6, //  Bootleg Singles\Bung Game Doctor\(B013) 五目ならべ.nes 
    0xB0E19418, //  Bootleg Singles\Bung Game Doctor\(B017) Donkey Kong.nes 
    0x6BF61CB5, //  Bootleg Singles\Bung Game Doctor\(B018) Donkey Kong Jr..nes 
    0xFE62E29C, //  Bootleg Singles\Bung Game Doctor\(B019) Donkey Kong 3.nes 
    0x0F263E59, //  Bootleg Singles\Bung Game Doctor\(B021) Duck Hunt.nes 
    0x9DA2A676, //  Bootleg Singles\Bung Game Doctor\(B023) Excitebike.nes 
    0xF6C7BE70, //  Bootleg Singles\Bung Game Doctor\(B025) F-1 Race.nes 
    0xBD4BA343, //  Bootleg Singles\Bung Game Doctor\(B029) Golf.nes 
    0x82908FF7, //  Bootleg Singles\Bung Game Doctor\(B030) Hogan's Alley.nes 
    0x094E153D, //  Bootleg Singles\Bung Game Doctor\(B031) Hyper Olympic.nes 
    0xB157C806, //  Bootleg Singles\Bung Game Doctor\(B032) Hyper Sports.nes 
    0x87883E16, //  Bootleg Singles\Bung Game Doctor\(B035) Karateka.nes 
    0x863EE598, //  Bootleg Singles\Bung Game Doctor\(B036) Lode Runner.nes 
    0x7BF1957F, //  Bootleg Singles\Bung Game Doctor\(B038) 超時空要塞 Macross.nes 
    0xD85BA5D2, //  Bootleg Singles\Bung Game Doctor\(B039) 麻雀.nes 
    0x61BD418F, //  Bootleg Singles\Bung Game Doctor\(B040) 4人打ち麻雀.nes 
    0x161FEE5D, //  Bootleg Singles\Bung Game Doctor\(B041) Mappy.nes 
    0x952F4A23, //  Bootleg Singles\Bung Game Doctor\(B042) Mario Bros..nes 
    0xE2C3F416, //  Bootleg Singles\Bung Game Doctor\(B044) Nuts &amp; Milk.nes 
    0xB9E2AA77, //  Bootleg Singles\Bung Game Doctor\(B047) Pooyan.nes 
    0xFA825D38, //  Bootleg Singles\Bung Game Doctor\(B048) Popeye.nes 
    0x90DC763E, //  Bootleg Singles\Bung Game Doctor\(B049) Raid on Bungeling Bay.nes 
    0xB03C08A9, //  Bootleg Singles\Bung Game Doctor\(B052) Space Invaders.nes 
    0x9BF47143, //  Bootleg Singles\Bung Game Doctor\(B053) Star Force.nes 
    0x4638FE34, //  Bootleg Singles\Bung Game Doctor\(B054) Tennis.nes 
    0xC96CAF4D, //  Bootleg Singles\Bung Game Doctor\(B055) Urban Champion.nes 
    0x090568DF, //  Bootleg Singles\Bung Game Doctor\(B057) Wild Gunman.nes 
    0xB397EDBC, //  Bootleg Singles\Bung Game Doctor\(B058) キン肉マン꞉ Muscle Tag Match.nes 
    0x9C07FFBE, //  Bootleg Singles\Bung Game Doctor\(B059) いっき.nes 
    0x55E17B4E, //  Bootleg Singles\Bung Game Doctor\(B060) Yie Ar Kung-Fu.nes 
    0x357B18FD, //  Bootleg Singles\Bung Game Doctor\(B062) Circus Charlie.nes 
    0x5578B0C6, //  Bootleg Singles\Bung Game Doctor\(B063) Bird Week.nes 
    0x43287E79, //  Bootleg Singles\Bung Game Doctor\(C002) Bokosuka Wars.nes 
    0x85D79AB2, //  Bootleg Singles\Bung Game Doctor\(C003) Bomberman.nes 
    0x2C163614, //  Bootleg Singles\Bung Game Doctor\(C004) Challenger.nes 
    0x484B03DB, //  Bootleg Singles\Bung Game Doctor\(C005) Dough Boy.nes 
    0xCE94F92F, //  Bootleg Singles\Bung Game Doctor\(C010) Geimos.nes 
    0x7D718B44, //  Bootleg Singles\Bung Game Doctor\(C012) Lot Lot.nes 
    0xA8E39EC6, //  Bootleg Singles\Bung Game Doctor\(C013) Mach Rider.nes 
    0x0C7A7B3E, //  Bootleg Singles\Bung Game Doctor\(C014) オバケのＱ太郎꞉ ワンワン Panic.nes 
    0xF0E89EFA, //  Bootleg Singles\Bung Game Doctor\(C015) Pac-Land.nes 
    0x84D0E294, //  Bootleg Singles\Bung Game Doctor\(C016) Pachi Com.nes 
    0xDAE467A8, //  Bootleg Singles\Bung Game Doctor\(C017) Penguinくん Wars.nes 
    0x41496EBD, //  Bootleg Singles\Bung Game Doctor\(C019) Astro Robo Sasa.nes 
    0xA08D7CA9, //  Bootleg Singles\Bung Game Doctor\(C020) Soccer.nes 
    0x23B87A6D, //  Bootleg Singles\Bung Game Doctor\(C022) Spelunker.nes 
    0xD7458274, //  Bootleg Singles\Bung Game Doctor\(C026) The Tower of Druaga.nes 
    0xFEBF5DC1, //  Bootleg Singles\Bung Game Doctor\(C027) おにゃんこ Town.nes 
    0x4D825E11, //  Bootleg Singles\Bung Game Doctor\(C028) Volguard II.nes 
    0x9ADB2AF7, //  Bootleg Singles\Bung Game Doctor\(C031) Son Son.nes 
    0x208DF309, //  Bootleg Singles\Bung Game Doctor\(C032) 忍者ハットリ君꞉ 忍者は修行でござるの巻.nes 
    0xBAAAE9E8, //  Bootleg Singles\Bung Game Doctor\(C035) Hydlide Special.nes 
    0xCBAD8B09, //  Bootleg Singles\Bung Game Doctor\(C036) Baltron.nes 
    0xBD1A6751, //  Bootleg Singles\Bung Game Doctor\(C037) Dig Dug II.nes 
    0x2C4E59FD, //  Bootleg Singles\Bung Game Doctor\(C038) Spy vs. Spy.nes 
    0x833458EF, //  Bootleg Singles\Bung Game Doctor\(C039) Gyrodine.nes 
    0x780F9F28, //  Bootleg Singles\Bung Game Doctor\(C040) Sqoon.nes 
    0x744C93AE, //  Bootleg Singles\Bung Game Doctor\(C042) Family Computer Othello.nes 
    0xB18FFD6C, //  Bootleg Singles\Bung Game Doctor\Poker 集錦.nes 
    0xABB61F8C, //  Bootleg Singles\Bung Game Doctor\Pyramid.nes 
    0xB9F85B1B, //  Bootleg Singles\Front Fareast Magic Card\(F1006) Donkey Kong 3.nes 
    0x4A3D4790, //  Bootleg Singles\Front Fareast Magic Card\(F1007) Hogan's Alley.nes 
    0x04A6B46D, //  Bootleg Singles\Front Fareast Magic Card\(F1008) Duck Hunt.nes 
    0x7C899CFA, //  Bootleg Singles\Front Fareast Magic Card\(F1009) Wild Gunman.nes 
    0x5B938AFF, //  Bootleg Singles\Front Fareast Magic Card\(F1011) Bird Week.nes 
    0xF92546F8, //  Bootleg Singles\Front Fareast Magic Card\(F1013) Raid on Bungeling Bay.nes 
    0xD9D2F56B, //  Bootleg Singles\Front Fareast Magic Card\(F1014) Lunar Ball.nes 
    0x869CE2A8, //  Bootleg Singles\Front Fareast Magic Card\(F1015) 4人打ち麻雀.nes 
    0xF20AE892, //  Bootleg Singles\Front Fareast Magic Card\(F1016) Hyper Olympic.nes 
    0x7D8F6A29, //  Bootleg Singles\Front Fareast Magic Card\(F1017) Yie Ar Kung-Fu.nes 
    0x25E378A6, //  Bootleg Singles\Front Fareast Magic Card\(F1022) Circus Charlie.nes 
    0x51EF1F93, //  Bootleg Singles\Front Fareast Magic Card\(F1023) Excitebike.nes 
    0xE28F2596, //  Bootleg Singles\Front Fareast Magic Card\(F1025) Pac-Land.nes 
    0x34EAD263, //  Bootleg Singles\Front Fareast Magic Card\(F1026) F-1 Race.nes 
    0xD11A5119, //  Bootleg Singles\Front Fareast Magic Card\(F1027) いっき.nes 
    0xE9248433, //  Bootleg Singles\Front Fareast Magic Card\(F1029) Nuts &amp; Milk.nes 
    0xA9896D5F, //  Bootleg Singles\Front Fareast Magic Card\(F1031) Donkey Kong Jr..nes 
    0x17982DED, //  Bootleg Singles\Front Fareast Magic Card\(F1032) Mario Bros..nes 
    0x7099EAC7, //  Bootleg Singles\Front Fareast Magic Card\(F1038) Pooyan.nes 
    0xAE9ED60E, //  Bootleg Singles\Front Fareast Magic Card\(F1040) Super Arabian.nes 
    0x0F3592D2, //  Bootleg Singles\Front Fareast Magic Card\(F1041) Bomberman.nes 
    0xA2F3A3B5, //  Bootleg Singles\Front Fareast Magic Card\(F1042) Golf.nes 
    0xED6EB6BC, //  Bootleg Singles\Front Fareast Magic Card\(F1044) Balloon Fight.nes 
    0x796A55D8, //  Bootleg Singles\Front Fareast Magic Card\(F1046) Popeye.nes 
    0xE8F4C651, //  Bootleg Singles\Front Fareast Magic Card\(F1047) Lode Runner.nes 
    0x8B1C251B, //  Bootleg Singles\Front Fareast Magic Card\(F1048) Karateka.nes 
    0xAE9208E9, //  Bootleg Singles\Front Fareast Magic Card\(F1049) Mappy.nes 
    0x2C3FDFA8, //  Bootleg Singles\Front Fareast Magic Card\(F1050) キン肉マン꞉ Muscle Tag Match.nes 
    0x9CDCC73A, //  Bootleg Singles\Front Fareast Magic Card\(F1052) 超時空要塞 Macross.nes 
    0x34A516FD, //  Bootleg Singles\Front Fareast Magic Card\(F1053) Challenger.nes 
    0xD2094946, //  Bootleg Singles\Front Fareast Magic Card\(F1054) Donkey Kong.nes 
    0xF8F494A6, //  Bootleg Singles\Front Fareast Magic Card\(F1055) 麻雀.nes 
    0xE7AD92E2, //  Bootleg Singles\Front Fareast Magic Card\(F1059) Championship Lode Runner.nes 
    0xE8EDCB61, //  Bootleg Singles\Front Fareast Magic Card\(F1060) Taito Front Line.nes 
    0xCF98EB4A, //  Bootleg Singles\Front Fareast Magic Card\(F1063) 五目ならべ.nes 
    0x52A7CC3A, //  Bootleg Singles\Front Fareast Magic Card\(F1064) Urban Champion.nes 
    0x79C91906, //  Bootleg Singles\Front Fareast Magic Card\(F1066) Seicross.nes 
    0x91B5B6A1, //  Bootleg Singles\Front Fareast Magic Card\(F1074) Star Force.nes 
    0xFD214C58, //  Bootleg Singles\Front Fareast Magic Card\(F1077) The Tower of Druaga.nes 
    0x1EEC890F, //  Bootleg Singles\Front Fareast Magic Card\(F1100) Popeyeの英語遊び.nes 
    0xB4481476, //  Bootleg Singles\Front Fareast Magic Card\(F1122) Arkanoid.nes 
    0x8436EB5E, //  Bootleg Singles\Front Fareast Magic Card\(F1124) Joust.nes 
    0x0A2259FE, //  Bootleg Singles\Front Fareast Magic Card\(F1125) Star Gate.nes 
    0x7E252E2C, //  Bootleg Singles\Front Fareast Magic Card\(F1126) Millipede.nes 
    0x322F723A, //  Bootleg Singles\Front Fareast Magic Card\(F1129) Baltron.nes 
    0xB648DA74, //  Bootleg Singles\Front Fareast Magic Card\(F1133) Door Door.nes 
    0x589B2EC8, //  Bootleg Singles\Front Fareast Magic Card\(F1136) Spelunker.nes 
    0x8F8175AE, //  Bootleg Singles\Front Fareast Magic Card\(F1137) Space Invaders.nes 
    0x4E741906, //  Bootleg Singles\Front Fareast Magic Card\(F1138) 本将棋꞉ 内藤九段将棋秘伝.nes 
    0xF2C4B221, //  Bootleg Singles\Front Fareast Magic Card\(F1139) Astro Robo Sasa.nes 
    0x5C1B1466, //  Bootleg Singles\Front Fareast Magic Card\(F1143) Hydlide Special.nes 
    0x1D62F13F, //  Bootleg Singles\Front Fareast Magic Card\(F1146) Othello.nes 
    0x716C3FCE, //  Bootleg Singles\Front Fareast Magic Card\(F1152) Lot Lot.nes 
    0x2E6B1432, //  Bootleg Singles\Front Fareast Magic Card\(SM1001) Magic Jewelry.nes 
    0x739BA800, //  Bootleg Singles\Soccer (LB33).nes 
    0xCAA17AB5, //  Bootleg Singles\Super Mario Bros. (LB35).nes 
    0x4CBE4A2C, //  Bootleg Singles\TouchGamePlayer\オバケのＱ太郎꞉ ワンワン Panic.nes 
    0x6F011C95, //  Compatibility Hacks\VT01\Bandits (VT01 to 2C02).nes 
    0x40AAE7E2, //  Compatibility Hacks\VT01\Mushroom (VT01 restoration #1).nes 
    0x9ADDC156, //  Compatibility Hacks\VT01\Mushroom (VT01 restoration #2).nes 
    0x61309551, //  Compatibility Hacks\VT01\Porter (VT01).nes 
    0x966730CF, //  Compatibility Hacks\動動腦 1 [m000].nes 
    0xCF3C1F9D, //  Educational Computers\extracts\Mine.nes 
    0x56BC90E7, //  Homebrew\8bitpeoples After Dark.nes 
    0xB84035A7, //  Homebrew\Alter Ego (NROM-256).nes 
    0xC05A972B, //  Homebrew\Altercation (v0.1)[Scott Lowe].nes 
    0xE7467FA7, //  Homebrew\Auge.nes 
    0x1687EB87, //  Homebrew\Banana Nana.nes 
    0x1261E5A4, //  Homebrew\Battleball.nes 
    0x54C02733, //  Homebrew\Beat'em &amp; Eat'em (beta with music).nes 
    0xF66542BA, //  Homebrew\Beat'em &amp; Eat'em (v1.0).nes 
    0xDB275231, //  Homebrew\Bloodfall.nes 
    0xFE8CFC22, //  Homebrew\Copyright Restorations\Porter.nes 
    0x82FD0204, //  Homebrew\Decursus.nes 
    0x3C0C7FFD, //  Homebrew\Demos\Chip Addiction.nes 
    0x2A5312D6, //  Homebrew\Demos\Eternal GladNES [Psych].nes 
    0x99432855, //  Homebrew\Demos\Happy Birthday Famicom - GreyBox (alt).nes 
    0x0ED27E38, //  Homebrew\Demos\Happy Birthday Famicom - GreyBox.nes 
    0xE317B30A, //  Homebrew\Demos\Mandelbrot Set 2008 [Psych].nes 
    0x7A0E9034, //  Homebrew\Demos\Mandelbrot [Psych].nes 
    0x6AB38DFB, //  Homebrew\Demos\Maze.nes 
    0x5A5DAC15, //  Homebrew\Demos\New Year 2011 [Shiru].nes 
    0x167BBF41, //  Homebrew\Demos\Pulse Wave [8bitpeoples].nes 
    0x520631C5, //  Homebrew\Demos\Pulsewave Invite.nes 
    0x02C2220C, //  Homebrew\Demos\Sayoonara! [Chris Covell].nes 
    0xE1789032, //  Homebrew\Demos\Stars SE [Chris Covell].nes 
    0x4BAB6CB4, //  Homebrew\Demos\TB5.nes 
    0xDADB8A8F, //  Homebrew\Demos\Turquoise Palace [MorphCat].nes 
    0x33878F0B, //  Homebrew\Demos\Wavetable.nes 
    0xF905F8CC, //  Homebrew\Eskimo Bob, Starring Alfonzo (Demo)[Spoony Bard].nes 
    0x938533F4, //  Homebrew\Flappy Jack.nes 
    0xD3CDD97A, //  Homebrew\Geminim (v0.990)[Sivak].nes 
    0xC9D47DBF, //  Homebrew\Gruniozerca [Lukasz Kur].nes 
    0x4F42DB87, //  Homebrew\Hacks\Blob Buster [RuSteD Hacks].nes 
    0x8FE8ADC6, //  Homebrew\Hacks\Bokosuka Quest III [Kasion].nes 
    0x7F5BDDAB, //  Homebrew\Hacks\Bokosuka Wars II [Gil-Galad].nes 
    0xF81F5F09, //  Homebrew\Hacks\Challenger DX [Osman Color].nes 
    0xE7D6B718, //  Homebrew\Hacks\Challenger Mario [Anton].nes 
    0xE67E5A01, //  Homebrew\Hacks\Mario Bros.꞉ The Revenge [Morgan Johansson].nes 
    0x90E477DD, //  Homebrew\Hacks\Mario Spy vs. Spy.nes 
    0x09172016, //  Homebrew\Hacks\Pac-Man Xtreme [KaBooM!].nes 
    0x91F7582A, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (LB35)[Water Level].nes 
    0x22D8129D, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Instant Death Spike (v0.95)[79].nes 
    0x49023CEA, //  Homebrew\Hacks\Super Star Wars 2 [Romhackers Inc.].nes 
    0x8E4695B5, //  Homebrew\Hacks\ドラえBomb [Rinkaku].nes 
    0xA0025AA1, //  Homebrew\Karate Kick.nes 
    0x5F559737, //  Homebrew\LAN Master.nes 
    0xC388DDD4, //  Homebrew\Lake Fever.nes 
    0x3A0031A1, //  Homebrew\Melo Jellos 2.nes 
    0xDEB21C9E, //  Homebrew\Mouser.nes 
    0xFC2FB9D0, //  Homebrew\NES Speech Synthesizer.nes 
    0x96314622, //  Homebrew\NESnake 2 (v1.2 NTSC).nes 
    0x0E88C118, //  Homebrew\NESnake 2 (v1.3).nes 
    0xDB22DCD8, //  Homebrew\Ninja Slapper.nes 
    0x416EEAB6, //  Homebrew\Nintencat꞉ The Parody (2017 version).nes 
    0x5F3E5011, //  Homebrew\NintendoAge Halloween.nes 
    0x550DE399, //  Homebrew\Rock Paper Scissors Lizard Sbock.nes 
    0xEF318802, //  Homebrew\Rock Paper Scissors.nes 
    0x5A2A1A0D, //  Homebrew\Save the Kuin.nes 
    0xABDE0FC7, //  Homebrew\Siamond.nes 
    0x86A8DA8B, //  Homebrew\Snail Maze.nes 
    0x659BAC67, //  Homebrew\Sokoban.nes 
    0x9AEA064B, //  Homebrew\Spacey McRacey.nes 
    0x48F2DCF5, //  Homebrew\Star Evil (NESdev 2017).nes 
    0x1CA6D4C2, //  Homebrew\Star Evil.nes 
    0x65622692, //  Homebrew\Sweetie and the Carnage.nes 
    0x6D4D0DDD, //  Homebrew\TGUN (Web Version).nes 
    0x9A167849, //  Homebrew\Tesla vs. Edison.nes 
    0xD9B885FC, //  Homebrew\Tests\Controller Queue.nes 
    0x8539196C, //  Homebrew\Tests\Ui Test.nes 
    0xD78C3026, //  Homebrew\The Mad Wizard꞉ A Candelabra Chronicle (download).nes 
    0x7E750B3A, //  Homebrew\The Paths of Bridewell.nes 
    0xE4C37A04, //  Homebrew\The Rise of Amondus (Demo).nes 
    0x730E70AC, //  Homebrew\The Wit.nes.nes 
    0x03E9C033, //  Homebrew\Translations\English\4 Player Strike Mahjong [GAFF Translations].nes 
    0xE1DB5CB1, //  Homebrew\Translations\English\BugTris [Green Jerry v1.0].nes 
    0x69F389DD, //  Homebrew\Translations\English\Dog Town [Pacnsacdave].nes 
    0x604E6EA6, //  Homebrew\Translations\English\Little Ninja Hattori [Pacnsacdave].nes 
    0xA25E634B, //  Homebrew\Translations\English\Q-Tarō the Ghost [Pacnsacdave].nes 
    0x5FB1ECF5, //  Homebrew\Translations\English\Sqoon [Stardust Crusaders].nes 
    0xAB7DD6E8, //  Homebrew\Turquoise Palace.nes 
    0x2DC05A6F, //  Licensed Japan\Astro Robo Sasa.nes 
    0x1F2D9DB7, //  Licensed Japan\Baltron.nes 
    0xFE18E6B6, //  Licensed Japan\Bokosuka Wars.nes 
    0x73418721, //  Licensed Japan\Challenger.nes 
    0x2BE254E9, //  Licensed Japan\Dig Dug II.nes 
    0xF760F1CB, //  Licensed Japan\Dough Boy.nes 
    0x1DBD1D2B, //  Licensed Japan\Geimos.nes 
    0x29E173FF, //  Licensed Japan\Gyrodine.nes 
    0x8575A0CB, //  Licensed Japan\Hydlide Special.nes 
    0x59977A46, //  Licensed Japan\Mach Rider (rev0).nes 
    0x38810A91, //  Licensed Japan\Mach Rider (rev1).nes 
    0x89D42098, //  Licensed Japan\Pac-Land.nes 
    0x7C596E45, //  Licensed Japan\Portopia 連続殺人事件.nes 
    0x657F7875, //  Licensed Japan\Soccer.nes 
    0x10BAEEF3, //  Licensed Japan\Son Son.nes 
    0x6058C65D, //  Licensed Japan\Spartan X.nes 
    0x636923BB, //  Licensed Japan\Spelunker.nes 
    0x4DFD949E, //  Licensed Japan\Sqoon (rev0).nes 
    0x73D5F7D3, //  Licensed Japan\Sqoon (rev1).nes 
    0xEB764567, //  Licensed Japan\The Tower of Druaga.nes 
    0xFDE14CCE, //  Licensed Japan\Volguard II.nes 
    0x270EAED5, //  Licensed Japan\おにゃんこ Town.nes 
    0x330DE468, //  Licensed Japan\オバケのＱ太郎꞉ ワンワン Panic.nes 
    0x89550500, //  Licensed Japan\囲碁指南.nes 
    0x55761931, //  Licensed Japan\忍者ハットリ君꞉ 忍者は修行でござるの巻.nes 
    0x77833016, //  Licensed North America\Chubby Cherub.nes 
    0xDBB06A25, //  Licensed North America\Dig Dug II꞉ Trouble in Paradise.nes 
    0xAE64CA77, //  Licensed North America\Ice Hockey.nes 
    0xD5C64257, //  Licensed North America\Kung Fu.nes 
    0xB6D2D300, //  Licensed North America\Ms. Pac-Man.nes 
    0x27AA3933, //  Licensed North America\Seicross.nes 
    0x86670C93, //  Licensed North America\Slalom.nes 
    0x99D15A91, //  Licensed North America\Spelunker.nes 
    0xC4A02712, //  Licensed North America\Spy vs. Spy.nes 
    0x44F34172, //  Licensed North America\Sqoon.nes 
    0x3747CD0B, //  Maintenance\NES Test.nes 
    0xEE7E61DE, //  Maintenance\U-Force Test.nes 
    0xF3670FE3, //  Modern\Eyra, The Crow Maiden (alt).nes 
    0x074CB327, //  Modern\Eyra, The Crow Maiden.nes 
    0x5A378EDC, //  Modern\Family Picross.nes 
    0x022032B3, //  Modern\GameCube Edition\Donkey Kong Jr. の算数遊び.nes 
    0x781901F4, //  Modern\GameCube Edition\麻雀.nes 
    0x73275797, //  Modern\Hackmatch (NTSC).nes 
    0x53C19218, //  Modern\Hudson Best Collection\Challenger.nes 
    0xF7466450, //  Modern\Namco Museum Archives\Dig Dug II.nes 
    0x70CCA102, //  Modern\Namco Museum Archives\Pac-Land.nes 
    0xC8DF8CBE, //  Modern\Namco Museum Archives\The Tower of Druaga.nes 
    0xC8EB5A7D, //  Modern\Rollie.nes 
    0xA80290A6, //  Modern\Super Mario Bros. (25th Anniversary NTSC).nes 
    0x77EAD911, //  Modern\Virtual Console\Dig Dug II꞉ Trouble in Paradise.nes 
    0x5B1EB1F7, //  Modern\Virtual Console\Lode Runner (Japan).nes 
    0x48591193, //  Modern\Virtual Console\Spelunker (Japan).nes 
    0xFABA94E6, //  Modern\Virtual Console\Spelunker (World).nes 
    0x70A192C5, //  Modern\Virtual Console\Volleyball.nes 
    0x8D9F4989, //  Playchoice\Kung Fu.nes 
    0xB99F6CD0, //  Playchoice\Super Mario Bros..nes 
    0x6A519A5E, //  Playchoice\Volleyball.nes 
    0xAF69375E, //  Plug-and-Play\extracts\100m Hurdles.nes 
    0x4F33DF4D, //  Plug-and-Play\extracts\Boxworld.nes 
    0x8A38AE20, //  Plug-and-Play\extracts\Caper Alone.nes 
    0x6DC4121F, //  Plug-and-Play\extracts\Diamond (Nice Code).nes 
    0xE2B19CCB, //  Plug-and-Play\extracts\Dringle.nes 
    0xC1711414, //  Plug-and-Play\extracts\Hit-Mousee.nes 
    0x19158CDD, //  Plug-and-Play\extracts\Hurry Burry.nes 
    0x1088D198, //  Plug-and-Play\extracts\Twin Cards.nes 
    0x21684965, //  Samples\Nebs 'n Debs.nes 
    0x1918F689, //  Unlicensed Japan\Super Maruo.nes 
    0x2E6F1A75, //  Unlicensed South Korea\BugTris.nes 
    0xB69F7C0F, //  Unlicensed Taiwan&amp;Hong Kong\Fire Dragon.nes 
    0xE8A11BD7, //  Unlicensed Taiwan&amp;Hong Kong\Porter.nes 
    0x207DD1C4, //  Unlicensed Taiwan&amp;Hong Kong\Tank.nes 
    0x0C1FE23D, //  Unlicensed Taiwan&amp;Hong Kong\小紅帽 - Little Red Hood.nes 
    0x1E407387, //  Unreleased\Baltron.nes 
    0x90600B85, //  Unreleased\Seicross.nes 
    0x0D9AE579, //  Unreleased\Tetris꞉ The Soviet Mind Game (build #2).nes 
    0xFA12003D, //  Unreleased\忍者ハットリ君꞉ 忍者は修行でござるの巻.nes 
/**** 35 mapper=0 prgrom=32K chrrom=8K mirror=V system=SYSTEM_PAL */
    0x178DBA78, //  BIOS\Datel Pro Action Replay (v1.0).nes 
    0x4D7D896C, //  Bad Dumps\Datel Pro Action Replay (v1.2)[no reset vector].nes 
    0x940C4B07, //  Bootleg Hacks\Super Shrek Brothers.nes 
    0x7F8C7E03, //  Homebrew\Hacks\Super Mario Bros. series\Super Bernie Bros. (v04)[Trumptendo].nes 
    0x4D4AA705, //  Homebrew\Hacks\Super Mario Bros. series\Super Mikey Erg [Punktendo].nes 
    0xF49A1856, //  Homebrew\NESnake 2 (v1.2 PAL).nes 
    0x92924548, //  Licensed PAL\Ice Hockey.nes 
    0x464A67AB, //  Licensed PAL\Kung Fu.nes 
    0xC3A0A3E0, //  Licensed PAL\Lunar Pool.nes 
    0xE57E5384, //  Licensed PAL\Mach Rider.nes 
    0xC30848D3, //  Licensed PAL\Slalom.nes 
    0x972D2784, //  Licensed PAL\Soccer.nes 
    0xC1E91D3F, //  Licensed PAL\Spy vs. Spy.nes 
    0x9A2DB086, //  Licensed PAL\Super Mario Bros. (rev1).nes 
    0xFB2594DC, //  Modern\Hackmatch (PAL).nes 
    0xE66AD6B8, //  Modern\Super Mario Bros. (25th Anniversary PAL).nes 
    0x19CE7F12, //  Unlicensed Elsewhere\Magic Mathematic.nes 
/**** 36 mapper=0 prgrom=32K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0xFCA9BA6D, //  Bootleg Hacks\Frog Prince.nes 
    0x7A018E1F, //  Bootleg Hacks\Hunt.nes 
    0x6519CB3B, //  Bootleg Hacks\Invader.nes 
    0xE18CD9AA, //  Bootleg Hacks\Inventor\Shoot Out.nes 
    0x7CDF51D5, //  Bootleg Hacks\Inventor\Super Shoot.nes 
    0x4D3982BC, //  Bootleg Hacks\Inventor\Surprise.nes 
    0x69D88990, //  Bootleg Hacks\Pandamar.nes 
    0xE953A676, //  Bootleg Hacks\Pocket Maero.nes 
    0x7A1C19FF, //  Bootleg Hacks\Super Boy.nes 
    0x5AF2334D, //  Bootleg Hacks\Super Kid.nes 
    0x906C65B8, //  Bootleg Hacks\Super Simpsons.nes 
    0x8922F79F, //  Bootleg Hacks\van der Merwe en Boetie.nes 
    0x728E7F04, //  Bootleg Hacks\수퍼마리오.nes 
    0x46E9CACD, //  Homebrew\!.nes 
    0xA2680634, //  Homebrew\Bugfixes, Improvements\Super Mario Bros. [Spiny Egg trajectory].nes 
    0xFFC055A7, //  Homebrew\Cat Killer.nes 
    0x484D149F, //  Homebrew\Columns.nes 
    0xE97F2F5D, //  Homebrew\Demos\Battle of the Bits Winter Chip 4.nes 
    0xB4EFC8CC, //  Homebrew\Demos\Battle of the Bits Winter Chip V.nes 
    0xE2F5FBFA, //  Homebrew\Dick Drawing Simulator.nes 
    0x48B8E5CC, //  Homebrew\Euchre.nes 
    0xD0FBE052, //  Homebrew\Falling Tiles (revA).nes 
    0xBCFDD7DE, //  Homebrew\Falling Tiles (revB).nes 
    0xDD512607, //  Homebrew\Fire and Rescue (v1.1)[Skyboy Games].nes 
    0x65C2E22A, //  Homebrew\Frenzy A7800 [Bob Decrescenzo].nes 
    0x2F06FDD7, //  Homebrew\Hacks\Super Mario Bros. series\Adventures of Ice Mario [pacnsacdave].nes 
    0x891B3FFC, //  Homebrew\Hacks\Super Mario Bros. series\After World 8 (v1.1)[Insectduel's Domain].nes 
    0xE52C9CD2, //  Homebrew\Hacks\Super Mario Bros. series\All Night Nippon 2 Super Mario Bros. [teeporage777].nes 
    0xAD9DDCEA, //  Homebrew\Hacks\Super Mario Bros. series\Autobot Bros. [Grimlick].nes 
    0xF58E6A16, //  Homebrew\Hacks\Super Mario Bros. series\Blocker [Wes].nes 
    0xF0D3F511, //  Homebrew\Hacks\Super Mario Bros. series\Boring Bros (v1.5)[Pikamon101].nes 
    0xF37EABA7, //  Homebrew\Hacks\Super Mario Bros. series\Bowser vs. Wario [Icegoom].nes 
    0x8DD0A0C4, //  Homebrew\Hacks\Super Mario Bros. series\Carnage.nes 
    0x8D013C94, //  Homebrew\Hacks\Super Mario Bros. series\Cebolinha [Jet Chan].nes 
    0xA2131D17, //  Homebrew\Hacks\Super Mario Bros. series\Chapolim Adventure [NRHW].nes 
    0x9B4EAB9A, //  Homebrew\Hacks\Super Mario Bros. series\Dismembered Mario.nes 
    0x86AFBF17, //  Homebrew\Hacks\Super Mario Bros. series\Down Mario.nes 
    0x9B1D443D, //  Homebrew\Hacks\Super Mario Bros. series\Dr. Eggman vs. Bowser [Dom Jurumela].nes 
    0xA00687CF, //  Homebrew\Hacks\Super Mario Bros. series\EDS Mario Bros. [EDS Hacks].nes 
    0xB1ADEBBB, //  Homebrew\Hacks\Super Mario Bros. series\Enigmario [DrFloppy].nes 
    0xE77E063B, //  Homebrew\Hacks\Super Mario Bros. series\Evil Mario.nes 
    0x0E87B8D7, //  Homebrew\Hacks\Super Mario Bros. series\Gothic Mario Bros.nes 
    0x5477E8D4, //  Homebrew\Hacks\Super Mario Bros. series\Happy Birthday! BMF x 28.nes 
    0x30008503, //  Homebrew\Hacks\Super Mario Bros. series\Hänsel &amp; Gretel.nes 
    0xA60D8CC8, //  Homebrew\Hacks\Super Mario Bros. series\James Bond 007꞉ You Only Live Twice (v6.0)[emask].nes 
    0x8AEF5B7B, //  Homebrew\Hacks\Super Mario Bros. series\Jedi Mario Bros [The Theologian].nes 
    0x20912907, //  Homebrew\Hacks\Super Mario Bros. series\Jugem.nes 
    0xA40A06B6, //  Homebrew\Hacks\Super Mario Bros. series\Kaizo Bros. (v1.3)[IzzyBear].nes 
    0x08D53FEC, //  Homebrew\Hacks\Super Mario Bros. series\Lady Opera in Swf Panic! (alt)[Darkdata].nes 
    0xDECCAA54, //  Homebrew\Hacks\Super Mario Bros. series\Legend of Zelda꞉ Curse from the Outskirts Redux [Dr. Mario].nes 
    0x7F8B4B6D, //  Homebrew\Hacks\Super Mario Bros. series\Letty9 [DASTARD].nes 
    0xDFB8B983, //  Homebrew\Hacks\Super Mario Bros. series\Luigi and the Christmas Quest [DarkNight13].nes 
    0x3CBE8BDD, //  Homebrew\Hacks\Super Mario Bros. series\Luigi and the New Quest (Spiffy Version)[DarkNight13].nes 
    0x9505C2C4, //  Homebrew\Hacks\Super Mario Bros. series\Luigi's First Quest꞉ The Search For Mario!!! [teeporage777].nes 
    0x0CF47926, //  Homebrew\Hacks\Super Mario Bros. series\Luigi's Summer Adventure [teeporage777].nes 
    0x52AF5F2F, //  Homebrew\Hacks\Super Mario Bros. series\Mario '98.nes 
    0xE9A13751, //  Homebrew\Hacks\Super Mario Bros. series\Mario is Back! [RetroReality].nes 
    0xE2302407, //  Homebrew\Hacks\Super Mario Bros. series\Mario's Moon Adventure [ROMulus].nes 
    0xD1653FC7, //  Homebrew\Hacks\Super Mario Bros. series\Mario's Stone Age Adventure.nes 
    0xE8AC8A3A, //  Homebrew\Hacks\Super Mario Bros. series\Mark McGwire Bros. [Googie].nes 
    0x2D855C33, //  Homebrew\Hacks\Super Mario Bros. series\Metomario.nes 
    0x2A059EEB, //  Homebrew\Hacks\Super Mario Bros. series\Miracle.nes 
    0xEDA38049, //  Homebrew\Hacks\Super Mario Bros. series\Mortal Kombat Bros..nes 
    0x772A3892, //  Homebrew\Hacks\Super Mario Bros. series\Mr. Sandman! [The Eadernator].nes 
    0xFEBA99D4, //  Homebrew\Hacks\Super Mario Bros. series\Mushroom Nightmare (v2.0)[Shadic].nes 
    0xE73AE060, //  Homebrew\Hacks\Super Mario Bros. series\New Mario Bros. [Stiviboy].nes 
    0xAB2FA8BC, //  Homebrew\Hacks\Super Mario Bros. series\Paper Mario Bros. [Fai Soft].nes 
    0xC24F5FE4, //  Homebrew\Hacks\Super Mario Bros. series\Peach and Daisy and the Royal Games [DarkData].nes 
    0x82D7D62B, //  Homebrew\Hacks\Super Mario Bros. series\Peach's Winter Adventure [teeporage777].nes 
    0x5CF09A36, //  Homebrew\Hacks\Super Mario Bros. series\Peppy Mario [Mikachu].nes 
    0xE7C9D022, //  Homebrew\Hacks\Super Mario Bros. series\Pepsi Man (v1.1.1) [Tizcommand].nes 
    0xE257A25B, //  Homebrew\Hacks\Super Mario Bros. series\River City Mario (v1.2)[Danoz].nes 
    0x6FD1BE10, //  Homebrew\Hacks\Super Mario Bros. series\SMB1 Mario's Moonlight Adventure [teeporage777].nes 
    0xC1F41743, //  Homebrew\Hacks\Super Mario Bros. series\Skeens Mario Bros. [Mike Skeens].nes 
    0x34C22E01, //  Homebrew\Hacks\Super Mario Bros. series\Sonic Bros..nes 
    0xC47A42AE, //  Homebrew\Hacks\Super Mario Bros. series\Sonic the Hedgehog in Super Mario Bros. [Kengen].nes 
    0x32DAA9BB, //  Homebrew\Hacks\Super Mario Bros. series\Suicide.nes 
    0x23D60F64, //  Homebrew\Hacks\Super Mario Bros. series\Supa Mari 2 [Maikami].nes 
    0x23AD6818, //  Homebrew\Hacks\Super Mario Bros. series\Supa Mari. [Maikami].nes 
    0xF54482B8, //  Homebrew\Hacks\Super Mario Bros. series\Supah Pantufa [VAdaPEGA].nes 
    0xC5E352DE, //  Homebrew\Hacks\Super Mario Bros. series\Super 2600 Bros. [Grimlick].nes 
    0x376605B6, //  Homebrew\Hacks\Super Mario Bros. series\Super ASCII Brothers [sl1me].nes 
    0xE3620259, //  Homebrew\Hacks\Super Mario Bros. series\Super Acid Bros..nes 
    0x1E307415, //  Homebrew\Hacks\Super Mario Bros. series\Super Flappy Bird Nightmare [Guyver].nes 
    0xC3D4BF28, //  Homebrew\Hacks\Super Mario Bros. series\Super Koopa Bros. [Hejmstel].nes 
    0x722FEB8E, //  Homebrew\Hacks\Super Mario Bros. series\Super Luigi Bros. [TheNintendude64].nes 
    0x4E60CC36, //  Homebrew\Hacks\Super Mario Bros. series\Super Luigi Bros. [zemigi].nes 
    0x4DD23C0E, //  Homebrew\Hacks\Super Mario Bros. series\Super Mairo Bros. (vertical balls enable)[AtanoK Si].nes 
    0xC390E3C8, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bash. [BashSoftware.net].nes 
    0x30CDE8EB, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (2x Fireballs) [teeporage777].nes 
    0x9C596C2F, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Bud Bros.).nes 
    0x28C79009, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Death Moon) [OnyxFusionHacks].nes 
    0xF149BD71, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Fire Mario to Big Mario when hit)[w7n].nes 
    0xF2F66DFE, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Frustration).nes 
    0x482D45CD, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Gameboy Bros.)[teeporage777].nes 
    0x5CCA5D96, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Goombas don't fall off edges)[teeporage777].nes 
    0xD141DE0D, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Impossibros.).nes 
    0x1EE57B0E, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Luigi Game v0.8)[YY].nes 
    0x01A2A609, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Mushroom Kingdom Chaos)[L. Soft].nes 
    0x13E2DA4F, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Super Link Bros.).nes 
    0x3D57CB40, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Underwater Mario).nes 
    0xE019B4D8, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (always big)[AP].nes 
    0xB5162486, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (play as Pac-Man).nes 
    0x2E8B0E0E, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (play as Wario).nes 
    0x28B2F467, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (play as a Goomba).nes 
    0x20D98817, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (poison mushroom)[YY].nes 
    0x8B605124, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (strange unnamed).nes 
    0x268D6D7C, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (unnamed).nes 
    0x4B34E777, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (v2.55)[YY].nes 
    0xFAF45918, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3꞉ The Forgotten Worlds [szemigi].nes 
    0x2EBB1C40, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 4 [Harmony7].nes 
    0x6A10C17D, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 4꞉ The Undiscovered Zones [szemigi].nes 
    0xA1FC238B, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Enhanced.nes 
    0xE3AC75B8, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. New (v1.2)[ByVegeta].nes 
    0xC2525C98, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Popoyan [Mikachu].nes 
    0x096FAFDA, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Rebirth [Remz].nes 
    0xD42A338E, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Remix 2꞉ The New Adventures [Rayside].nes 
    0x45714979, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Remix 3꞉ The Link Adventures [Rayside].nes 
    0x80ED536D, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Remix꞉ The New Adventures [Rayside].nes 
    0xC376EAC6, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. TMNT Bros..nes 
    0xB81E574D, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. [EraserHead].nes 
    0x389F40BD, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. [TKB].nes 
    0xD08AEC51, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. and the 32 Lost Levels.nes 
    0x386FEA13, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ 2005 Special.nes 
    0xC1324BF4, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ 2005 [Mikachu].nes 
    0xC2595759, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ 3 Prototype.nes 
    0xAAF9AAD2, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Bomber Mario [M.K.S.].nes 
    0x54E1DF34, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Bowser's Castle.nes 
    0x6B94FF3B, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Cozy with Death [EBS].nes 
    0xA8DBB450, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Final Quest.nes 
    0x5F51FCBF, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Gold Edition [szemigi].nes 
    0x5B13E270, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Luigi [Mikachu].nes 
    0xEFB4344B, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Mario Misadventures [Guzmeme].nes 
    0x97B66BC0, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Mario in Zebes.nes 
    0xD9240981, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Master Quest.nes 
    0x7C26958E, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Revisited (v4.3).nes 
    0x187E9D07, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ The Lost Levels 2.nes 
    0xB4FD9489, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Toad Bros..nes 
    0xB73E0914, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ With Demo and Iris [RosieDilly].nes 
    0xB2B87669, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ feat. Mega Man [WillJ168].nes 
    0x089E97B2, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Brothers DX '05 [FlamePanther].nes 
    0xCF6CD965, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Brothers Desert Land [Luan Silva].nes 
    0x04B7CAAF, //  Homebrew\Hacks\Super Mario Bros. series\Super One Up! [Salmonhead].nes 
    0x850B64B0, //  Homebrew\Hacks\Super Mario Bros. series\Super Royal Pals. [QUXNLADY].nes 
    0x91322665, //  Homebrew\Hacks\Super Mario Bros. series\Super Smashing Pumpkins Bros (v2)[Steven &amp; David Pukin].nes 
    0x7F7FC751, //  Homebrew\Hacks\Super Mario Bros. series\Super Spartan Bros. [Belial].nes 
    0x71AD96BE, //  Homebrew\Hacks\Super Mario Bros. series\Super Wario Bros. [Googie].nes 
    0xA6C4F97D, //  Homebrew\Hacks\Super Mario Bros. series\TDGL2 and SGL! ACFun [Zyh].nes 
    0x50529768, //  Homebrew\Hacks\Super Mario Bros. series\Tetris Mario Bros. [Garuna].nes 
    0xEC57ED46, //  Homebrew\Hacks\Super Mario Bros. series\The Adventures of Jerry Mouse꞉ The Rescue of Nibbles [Green Jerry].nes 
    0x35EFF4ED, //  Homebrew\Hacks\Super Mario Bros. series\The Legend of Zelda in Super Mario Brothers World [Proveaux].nes 
    0x4DA380E5, //  Homebrew\Hacks\Super Mario Bros. series\The New Strange Mario Bros! [ACMLM].nes 
    0x011537CA, //  Homebrew\Hacks\Super Mario Bros. series\The Pixel Kingdom [Tyrukia].nes 
    0xC29558FE, //  Homebrew\Hacks\Super Mario Bros. series\Toad's Adventure꞉ Where's Mario？ (v2.0)[Omniverse].nes 
    0x026DBF97, //  Homebrew\Hacks\Super Mario Bros. series\Toad's Autumn Adventure [teeporage777].nes 
    0xBED50E23, //  Homebrew\Hacks\Super Mario Bros. series\Uncle Mario Bros (v1.1)[zacmario].nes 
    0x6F58B836, //  Homebrew\Hacks\Super Mario Bros. series\Virtual Boy Mario (v1.1)[Green Jerry].nes 
    0x62E5E029, //  Homebrew\Hacks\Super Mario Bros. series\Waluigi's Adventure [Googie].nes 
    0xFCF19A0C, //  Homebrew\Hacks\Super Mario Bros. series\Yoda Bros..nes 
    0xBE1A8FCB, //  Homebrew\Hacks\Super Mario Bros. series\YoonA꞉ The Raid of the Black Girls' Generation (Hard)[w7n].nes 
    0x28B6A043, //  Homebrew\Hacks\Super Mario Bros. series\YoonA꞉ The Raid of the Black Girls' Generation (Normal)[w7n].nes 
    0x802D0D61, //  Homebrew\Hacks\Super Mario Bros. series\Yoshi Islands [Ku-Pi-Mintendo].nes 
    0x51975D5B, //  Homebrew\Hacks\Super Mario Bros. series\Yoshi Mario.nes 
    0x780BC432, //  Homebrew\Hacks\Super Mario Bros. series\Yoshi's Spring Adventure [teeporage777].nes 
    0x137EB720, //  Homebrew\Hacks\Super Mario Bros. series\ßex.nes 
    0x8FDE3846, //  Homebrew\Hacks\Super Mario Bros. series\β Mario Final.nes 
    0x077B7DE8, //  Homebrew\Nim &amp; Nom (v1.2)[metakrill].nes 
    0xD1AAD0F5, //  Homebrew\Sir Ababol (Remastered Edition).nes 
    0xB90C56BE, //  Homebrew\Sir Ababol.nes 
    0xBA3A28AC, //  Homebrew\Super Painter.nes 
    0x83C8EDF7, //  Homebrew\Tiger Jenny.nes 
    0x1F589DF1, //  Homebrew\我想尿尿 (NESdev 2016).nes 
    0x97116D42, //  Homebrew\我想尿尿 (v1.3e).nes 
    0xDF8441F3, //  Homebrew\硝煙の騎士 Durandal [Karu Gamo].nes 
    0x023A5A32, //  Licensed Japan\Gyro Set.nes 
    0xD445F698, //  Licensed Japan\Super Mario Bros..nes 
    0x27777635, //  Licensed North America\Volleyball.nes 
    0x2F4FBBB3, //  Maintenance\NTF2 Test (rev1).nes 
    0xC3F3A4A0, //  Modern\City Trouble.nes 
    0xAAC10C16, //  Modern\GameCube Edition\Donkey Kong 3.nes 
    0xA69A1F2A, //  Modern\GameCube Edition\Donkey Kong Jr..nes 
    0xB0719977, //  Modern\Nebs 'n Debs.nes 
    0x0F6EA0FF, //  Modern\Quest Forge.nes 
    0xFE7704BA, //  Modern\Virtual Console\Lode Runner (World).nes 
    0xF0666B3C, //  Multicarts\extracts\Tank (Asder 20-in-1).nes 
    0x27ACE333, //  Plug-and-Play\extracts\Anti-Terror Action.nes 
    0xB5D28D3F, //  Samples\Eskimo Bob Starring Alfonzo.nes 
    0x905B93F6, //  Unlicensed Elsewhere\Balloon Monster (rev1).nes 
    0x71C01B19, //  Unlicensed Elsewhere\Little Red Hood.nes 
    0xB4BADF56, //  Unlicensed Elsewhere\Tank.nes 
    0x0B3513A0, //  Unlicensed Taiwan&amp;Hong Kong\Balloon Monster.nes 
    0x7CF6B30A, //  Unlicensed Taiwan&amp;Hong Kong\Locksmith.nes 
/**** 37 mapper=0 prgrom=32K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0xF7C88C35, //  Homebrew\Demos\DCEvo Dream Emulation Project.nes 
    0x592329EA, //  Homebrew\Hot Dance 2020.nes 
    0xFAAD108A, //  Homebrew\Morskoy Boy.nes 
    0xADB9C470, //  Homebrew\Translations\English\Storehouse No. 18 [Gil Galad].nes 
    0x8C36176E, //  Modern\Virtual Console\Ice Hockey.nes 
/**** 38 mapper=0 prgrom=32K prgram=8K chrrom=8K mirror=V */
    0xE87E182E, //  Bootleg Singles\Front Fareast Magic Card\(F1087) Super Mario Bros..nes 
    0x69759626, //  Bootleg Singles\Front Fareast Magic Card\(F1154) Family BASIC.nes 
/**** 39 mapper=0 prgrom=32K prgram=8K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0xFCD7A58F, //  Homebrew\Hacks\Super Mario Bros. series\Peach's Nightmare [Elishasoft].nes 
    0xC60E44B5, //  Homebrew\Hacks\Super Mario Bros. series\Snipe &amp; Dash Hammer Bros. [BlckMage86].nes 
    0x460710FA, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Mushroom Dreams [Rage Games].nes 
    0x72751BA0, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Made with MI41.nes 
    0xD3D34C85, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ TKB Vol. 2 [TKB].nes 
    0x13AAD777, //  Homebrew\Hacks\Super Mario Bros. series\Yoshi's Quest.nes 
/**** 40 mapper=0 prgrom=32K prgram=8K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0x84D59A58, //  Educational Computers\Education Computer 7-in-1.nes 
    0x8F7B1669, //  Educational Computers\FP-BASIC (v3.3).nes 
/**** 41 mapper=0 prgrom=32K chrrom=16K mirror=V */
    0x02BC030E, //  Bad Dumps\Police vs. Thief꞉ Violent Chasing [CHR overdump].nes 
    0xAD1381DF, //  Plug-and-Play\extracts\Bomberman 2002.nes 
    0x9334DE5B, //  Plug-and-Play\extracts\Fish Story.nes 
    0xD1B91F24, //  Plug-and-Play\extracts\Goblet Tower.nes 
/**** 42 mapper=0 prgrom=32K chrrom=32K mirror=V */
    0x18922271, //  Bad Dumps\Magic Bubble [CHR overdump].nes 
/**** 43 mapper=0 prgrom=48K chrrom=8K mirror=V */
    0x83428A58, //  Homebrew\Alter Ego (NROM-368).nes 
/**** 44 mapper=0 prgrom=32K prgnvram=2K chrrom=8K mirror=V battery=1 */
    0xF8BBC22C, //  Educational Computers\家庭电脑教学键盘 - Family Competer Keyboard.nes 
    0xF7DB8B5C, //  Licensed Japan\Family BASIC (v1.0).nes 
    0xF7606810, //  Licensed Japan\Family BASIC (v2.0).nes 
    0x895037BC, //  Licensed Japan\Family BASIC (v2.1).nes 
    0xACB8FE86, //  Licensed Japan\Playbox BASIC.nes 
    0x912989DC, //  Unreleased\Playbox BASIC.nes 
/**** 45 mapper=0 prgrom=32K prgnvram=4K chrrom=8K mirror=V battery=1 */
    0xB2530AFC, //  Licensed Japan\Family BASIC (v3.0).nes 
/**** 46 mapper=0 prgrom=32K prgnvram=8K chrrom=8K mirror=V battery=1 */
    0x3B833CCA, //  Modern\Classic Edition\Excitebike.nes 
    0xB7CB1734, //  Modern\Virtual Console\Excitebike (NTSC).nes 
    0xB08D0374, //  Modern\Virtual Console\Mach Rider (PAL).nes 
    0xF8D564F9, //  Modern\Virtual Console\Mach Rider (World rev0).nes 
    0x99C3142E, //  Modern\Virtual Console\Mach Rider (World rev1).nes 
/**** 47 mapper=0 prgrom=32K prgnvram=8K chrrom=8K mirror=V battery=1 system=SYSTEM_PAL */
    0x3BC0ADE6, //  Modern\Virtual Console\Excitebike (PAL).nes 
/**** 48 mapper=1 prgrom=32K chrrom=16K mirror=4 */
    0x7D01D4E0, //  Compatibility Hacks\Vs. Hogan's Alley [home console v1.0b1].nes 
/**** 49 mapper=1 prgrom=64K prgram=2K chrrom=32K mirror=4 */
    0x2B85420E, //  Vs. System\Dr. Mario.nes 
/**** 50 mapper=1 prgrom=16K chrram=8K mirror=H */
    0xC596AD7B, //  BIOS\WideBoy.nes 
/**** 51 mapper=1 prgrom=16K chrrom=8K mirror=H */
    0x4CAF31FA, //  Homebrew\Move 31.nes 
/**** 52 mapper=1 prgrom=16K chrrom=32K mirror=H */
    0x988AF9F1, //  Homebrew\Demos\Splash Rightnut.nes 
/**** 53 mapper=1 prgrom=32K chrram=8K mirror=H */
    0x7CA7586A, //  Homebrew\Demos\Deadline Console Invitro [8bitpeoples].nes 
/**** 54 mapper=1 prgrom=32K prgram=8K chrram=8K mirror=H */
    0x2C19B3C1, //  Homebrew\Tests\MW4 Test.nes 
/**** 55 mapper=1 prgrom=32K prgram=8K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x61286D57, //  Homebrew\Ball 'n Flag (Neo Coding Compo).nes 
    0x4032FA0C, //  Homebrew\Ball 'n Flag.nes 
/**** 56 mapper=1 prgrom=32K chrrom=16K mirror=H */
    0xAEC82F8A, //  Homebrew\Betelgeuse.nes 
/**** 57 mapper=1 prgrom=32K chrrom=16K mirror=H system=SYSTEM_NTSC */
    0xBF4277F4, //  Homebrew\Nalle Land (v0.4.0).nes 
/**** 58 mapper=1 prgrom=32K chrrom=32K mirror=H */
    0x21763B95, //  Bad Dumps\Virus (1990-03)[missing PROM].nes 
    0x131E67E7, //  Homebrew\Hacks\Adventures of Lala [Zynk Oxhyde].nes 
    0x7BFCC141, //  Homebrew\Hacks\Adventures of Lolo꞉ Mystical Forest.nes 
    0xE147F61C, //  Homebrew\Hacks\R.C. Pro-Am (Mario Kart Edition).nes 
    0xA8787D92, //  Homebrew\Translations\English\Hirake! Ponkikki [Gil Galad v1.01].nes 
    0x8EB8CE9B, //  Homebrew\Translations\English\Open! Ponkikki [Open! Ponkikki].nes 
    0xF9BACC95, //  Unreleased\Virus (1990-03).nes 
/**** 59 mapper=1 prgrom=32K chrrom=32K mirror=H system=SYSTEM_PAL */
    0x8416A0D7, //  Modern\Virtual Console\Adventures of Lolo (PAL).nes 
/**** 60 mapper=1 prgrom=64K prgram=8K chrram=8K mirror=H system=SYSTEM_PAL */
    0x573DFDFA, //  Homebrew\Demos\Years Behind [Retrocoders].nes 
/**** 61 mapper=1 prgrom=64K prgram=8K chrrom=8K mirror=H */
    0xB25C00AB, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Special (v1.0rc1)[Karatorian].nes 
/**** 62 mapper=1 prgrom=64K prgram=8K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x8BB93AF8, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Special (unknown version)[Karatorian].nes 
    0x1D6805CC, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Remix꞉ The Toad Bros Tale.nes 
/**** 63 mapper=1 prgrom=64K chrrom=16K mirror=H */
    0x2C40B09B, //  Homebrew\Fluffy Space Escape [Ludum Dare 40].nes 
/**** 64 mapper=1 prgrom=64K chrrom=16K mirror=H system=SYSTEM_NTSC */
    0xEBE3DCB4, //  Homebrew\Dizzy Sheep Disaster (BitBitJam #4).nes 
/**** 65 mapper=1 prgrom=64K prgram=8K chrrom=16K mirror=H */
    0x26A2D515, //  Compatibility Hacks\Vs. Ice Climber [home console v2.0b].nes 
/**** 66 mapper=1 prgrom=64K chrrom=32K mirror=H */
    0x9E382EBF, //  Licensed North America\Dance Aerobics.nes 
    0x6435C095, //  Licensed North America\Short Order+Eggsplode.nes 
    0xE50A9130, //  Licensed North America\The Bugs Bunny Crazy Castle.nes 
/**** 67 mapper=1 prgrom=64K prgram=8K chrrom=32K mirror=H */
    0xBB0F2A16, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Special X1 [Messatu].nes 
/**** 68 mapper=1 prgrom=64K chrrom=128K mirror=H */
    0x66F4D9F5, //  Licensed Japan\Knight Rider.nes 
    0x4318A2F8, //  Licensed North America\Barker Bill's Trick Shooting.nes 
    0xEBCFE7C5, //  Licensed North America\Knight Rider.nes 
    0xC6C2EDB5, //  Licensed North America\Magic Johnson's Fast Break.nes 
    0x5EE6008E, //  Licensed North America\Mechanized Attack.nes 
    0x958E4BAE, //  Licensed North America\Orb 3-D.nes 
    0x9FFE2F55, //  Licensed North America\Sky Shark.nes 
/**** 69 mapper=1 prgrom=64K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x1F6660E6, //  Licensed PAL\Barker Bill's Trick Shooting.nes 
    0x280AD3C5, //  Licensed PAL\Knight Rider.nes 
/**** 70 mapper=1 prgrom=128K chrram=8K mirror=H */
    0x12B8CA7D, //  Homebrew\Translations\English\Dragon Slayer Jr.꞉ Romancia (revA)[DvD Translations].nes 
    0xDAE24036, //  Homebrew\Translations\English\紫禁城 [Suicidal Translations].nes 
    0x74EE0FFC, //  Licensed Japan\Bridgestone Cycle꞉ Radac Tailor-Made (rev0).nes 
    0xBB98CD38, //  Licensed Japan\Bridgestone Cycle꞉ Radac Tailor-Made (rev1).nes 
    0xE44001D8, //  Licensed Japan\Casino Derby.nes 
    0xCF9CF7A2, //  Licensed Japan\Dragon Slayer Jr.꞉ Romancia (rev0).nes 
    0xEBB84D81, //  Licensed Japan\Dragon Slayer Jr.꞉ Romancia (rev1).nes 
    0xD9F45BE9, //  Licensed Japan\Gimmi a Break꞉ 史上最強のQuiz 王決定戦.nes 
    0xBCF68611, //  Licensed Japan\Great Deal.nes 
    0x02589598, //  Licensed Japan\Highway Star.nes 
    0x03D56CF7, //  Licensed Japan\Olympusの戦い.nes 
    0xA38857EB, //  Licensed Japan\Ripple Island.nes 
    0x43539A3C, //  Licensed Japan\Space Harrier.nes 
    0x1A2EA6B9, //  Licensed Japan\Super Pinball.nes 
    0xE74AA15A, //  Licensed Japan\帰って来た! 軍人将棋꞉ なんやそれ!？.nes 
    0x6CD9CC23, //  Licensed Japan\爆笑! Star ものまね四天王.nes 
    0x9C3E8FC0, //  Licensed Japan\目指せパチPro꞉ パチ夫くん.nes 
    0x9B53F848, //  Licensed Japan\紫禁城.nes 
    0x49F745E0, //  Licensed North America\720 Degrees.nes 
    0x1AE7B933, //  Licensed North America\Bad Street Brawler.nes 
    0x7156CB4D, //  Licensed North America\Jim Henson's Muppet Adventure꞉ Chaos at the Carnival.nes 
    0x8B9D3E9C, //  Licensed North America\Rad Racer.nes 
    0xAD12A34F, //  Licensed North America\Tecmo Baseball.nes 
    0x6B53006A, //  Licensed North America\The Battle of Olympus.nes 
    0xF74DFC91, //  Licensed North America\Win, Lose or Draw.nes 
    0x52880295, //  Licensed North America\Winter Games (rev0).nes 
    0x44B05F06, //  Playchoice\Rad Racer.nes 
    0x3F57E040, //  Samples\Square Deal.nes 
    0xE14F0A3F, //  Unreleased\Super Pinball.nes 
    0x9568EB74, //  Unreleased\目指せパチPro꞉ パチ夫くん.nes 
/**** 71 mapper=1 prgrom=128K chrram=8K mirror=H system=SYSTEM_PAL */
    0x9AC644F4, //  Homebrew\Driar.nes 
    0x654F4E90, //  Licensed PAL\Rad Racer.nes 
    0xA97567A4, //  Licensed PAL\The Battle of Olympus.nes 
/**** 72 mapper=1 prgrom=128K prgram=8K chrram=8K mirror=H */
    0x773D24F9, //  Homebrew\Bugfixes, Improvements\Metroid (NTSC)[corrected RNG][drk421].nes 
    0xCF26363D, //  Homebrew\Demos\Specus.nes 
    0x96B726FC, //  Homebrew\Hacks\Metroid 99꞉ Maximumpotion Kzk.nes 
    0x9E6DA943, //  Homebrew\Hacks\Metroid꞉ Samus' Dream (bad CHR)[Ryu654].nes 
    0x75547617, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 Beta Into Doki Doki Panic.nes 
    0x9A4BD541, //  Homebrew\Polynes.nes 
    0x13C2ABF3, //  Homebrew\ShootNES.nes 
    0xF27D2FB5, //  Homebrew\Super Bat Puncher (Demo).nes 
    0x0C401790, //  Licensed Japan\Bomberman II.nes 
    0xB4113F3C, //  Licensed Japan\Hatris.nes 
    0x69BCDB8B, //  Licensed Japan\Navy Blue.nes 
    0x0ABDD5CA, //  Licensed Japan\Spot.nes 
    0x75901B18, //  Licensed Japan\Titan.nes 
    0x1EBB5B42, //  Licensed North America\Bomberman II.nes 
    0x841B69B6, //  Licensed North America\Hatris.nes 
    0x0939852F, //  Licensed North America\M.U.L.E..nes 
    0x70080810, //  Licensed North America\Metroid.nes 
    0x95E4E594, //  Licensed North America\Qix.nes 
    0xCFAE9DFA, //  Licensed North America\Spot.nes 
    0x08598049, //  Modern\Hudson Best Collection\Bomberman II.nes 
    0xD59ADB3F, //  Modern\Virtual Console\Metroid (NTSC).nes 
    0x6CF3116A, //  Playchoice\Metroid.nes 
    0x6ED31CCD, //  Unreleased\Chip's Challenge.nes 
    0x52387646, //  Unreleased\Super Mario Bros. 2.nes 
/**** 73 mapper=1 prgrom=128K prgram=8K chrram=8K mirror=H system=SYSTEM_PAL */
    0x70647464, //  Homebrew\Bugfixes, Improvements\Metroid (PAL)[corrected RNG][drk421].nes 
    0x34BB757B, //  Licensed PAL\Dynablaster.nes 
    0x7751588D, //  Licensed PAL\Metroid.nes 
    0x07E8D0CF, //  Modern\Virtual Console\Metroid (PAL).nes 
/**** 74 mapper=1 prgrom=128K prgram=8K chrram=8K mirror=H system=SYSTEM_NTSC */
    0xE917380B, //  Homebrew\Hacks\Kid Icarus Remix [Clomax Dominion].nes 
    0xD29EAF12, //  Homebrew\Hacks\Kid Icarus Zz.nes 
    0xD9F0749F, //  Licensed North America\Kid Icarus (rev0).nes 
    0xD67FD6A6, //  Licensed North America\Kid Icarus (rev1).nes 
    0x654EC7C0, //  Modern\Virtual Console\Kid Icarus.nes 
/**** 75 mapper=1 prgrom=128K chrrom=32K mirror=H */
    0x265B230E, //  Homebrew\Hacks\Hyper Bobble [Cool Nestea].nes 
    0x9B7BD576, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Unlimited (v0.997)[Frantik].nes 
    0x17348D62, //  Homebrew\Translations\English\Eggerland꞉ Revival of the Labyrinth (v1.0)[Kapow](English password).nes 
    0x00CD6327, //  Homebrew\Translations\English\Eggerland꞉ Revival of the Labyrinth (v1.0)[Kapow].nes 
    0x5A3A8843, //  Homebrew\Translations\English\Keroppi &amp; Keroleen's Splash Bomb! [Suicidal Translations].nes 
    0xCDACE4D9, //  Homebrew\Translations\English\Zombie Hunter [KingMike's Translations v1.01].nes 
    0x77DCBBA3, //  Licensed Japan\Eggerland꞉ 迷宮の復活.nes 
    0xB4FF91E7, //  Licensed Japan\Garfield꞉ A Week of Garfield.nes 
    0x4156A3CD, //  Licensed Japan\KeroppiとKerolineのSplash Bomb!.nes 
    0x9ECB9DCD, //  Licensed Japan\Perfect Bowling.nes 
    0xE2A79A57, //  Licensed Japan\Rollerball.nes 
    0x47232739, //  Licensed Japan\Top Rider.nes 
    0x67A3C362, //  Licensed Japan\Touchdown Fever꞉ American Football.nes 
    0xAFC32114, //  Licensed Japan\Yasuda Fire &amp; Marine Safety Rally - 救火拉力赛.nes 
    0x1E4D3831, //  Licensed Japan\Yoshiのたまご.nes 
    0x41632CB6, //  Licensed Japan\Zombie Hunter.nes 
    0xE333FFA1, //  Licensed Japan\囲碁指南 '91.nes 
    0x7A11D2C9, //  Licensed Japan\囲碁指南 '92.nes 
    0x9C053F24, //  Licensed Japan\囲碁指南 '93.nes 
    0xA7E784ED, //  Licensed Japan\囲碁指南 '94.nes 
    0x5F0BCE2A, //  Licensed North America\Break Time꞉ The National Pool Tour.nes 
    0x5E900522, //  Licensed North America\Bubble Bobble.nes 
    0x4D3FBA78, //  Licensed North America\Dr. Jekyll and Mr. Hyde.nes 
    0x69635A6E, //  Licensed North America\Rollerball.nes 
    0xEA4EB69E, //  Licensed North America\Touchdown Fever.nes 
    0x711896B8, //  Licensed North America\Xenophobe.nes 
    0xE7EAD93B, //  Licensed North America\Yoshi.nes 
    0x5723D797, //  Modern\Virtual Console\Bubble Bobble (NTSC).nes 
    0xFB77099E, //  Samples\Garfield꞉ A Week of Garfield.nes 
    0xF110BE46, //  Unreleased\Garfield꞉ A Week of Garfield (Prototype).nes 
/**** 76 mapper=1 prgrom=128K chrrom=32K mirror=H system=SYSTEM_PAL */
    0xC99B690A, //  Licensed PAL\Bubble Bobble.nes 
    0x836685C4, //  Licensed PAL\Mario &amp; Yoshi.nes 
    0xA99016C6, //  Licensed PAL\Rollerball.nes 
    0x101A76A0, //  Modern\Virtual Console\Bubble Bobble (PAL).nes 
/**** 77 mapper=1 prgrom=128K prgram=8K chrrom=32K mirror=H */
    0x630FDB8F, //  Homebrew\Hacks\Phantasy Star꞉ The Hopeless.nes 
    0x35C41CD4, //  Licensed North America\Air Fortress.nes 
    0xD7F6320C, //  Licensed North America\The Chessmaster (rev0).nes 
    0x19F4CA6B, //  Licensed North America\The Chessmaster (rev1).nes 
/**** 78 mapper=1 prgrom=128K prgram=8K chrrom=32K mirror=H system=SYSTEM_PAL */
    0xA1F90826, //  Licensed PAL\Air Fortress.nes 
    0xE3027EBE, //  Licensed PAL\The Chessmaster.nes 
/**** 79 mapper=1 prgrom=128K chrrom=64K mirror=H */
    0xDE17A5CF, //  Homebrew\Hacks\Castlevania series\Super! Castlevania II꞉ Simon's Quest (v0.9)[JimW].nes 
    0x529B621F, //  Licensed North America\NES Power Set (rev0).nes 
    0x60E63537, //  Licensed North America\NES Power Set (rev1).nes 
    0x20353E63, //  Licensed North America\Peter Pan &amp; The Pirates.nes 
/**** 80 mapper=1 prgrom=128K chrrom=72K mirror=H */
    0x3FF04490, //  Homebrew\PlayPower Games.nes 
/**** 81 mapper=1 prgrom=128K chrrom=128K mirror=H */
    0x4AF742FA, //  Bad Dumps\Sky Shark [overdump].nes 
    0x6E4697BF, //  Bad Dumps\The Untouchables.nes 
    0x4989D551, //  Bootleg Hacks\Back Go to Future 4.nes 
    0x12B57E82, //  Bootleg Hacks\Double Dragon IV.nes 
    0x1DBAFE5D, //  Bootleg Hacks\Ghostbusters III.nes 
    0x09663DC5, //  Bootleg Hacks\Grand Combat.nes 
    0x6B9C68B3, //  Bootleg Hacks\Rockman 5꞉ 新たなる野望!!.nes 
    0x90B17835, //  Bootleg Hacks\Urban Strike.nes 
    0x27DB76BC, //  Bootleg Hacks\忍者龍劍傳 - Ninja Ryuken 4.nes 
    0xBC5DFDE8, //  Bootleg Hacks\金剛戰士 - Power Rangers.nes 
    0xB4E44ADB, //  Bootleg Singles\ChipとDaleの大作戦 2 (C-D2).nes 
    0x04C37212, //  Bootleg Singles\Double Dragon V꞉ The Shadow Falls (D-N3).nes 
    0x8C97E96B, //  Bootleg Singles\Kero Kero Keroppiの大冒険 2꞉ Donuts 池はおおさわぎ!.nes 
    0xEF8B2822, //  Bootleg Singles\RoadBlasters (R-B1).nes 
    0x1BC756BF, //  Compatibility Hacks\Bill &amp; Ted's Excellent Video Game Adventure [MMC1 initialization].nes 
    0x2AB7EF95, //  Compatibility Hacks\Holy Diver [m001].nes 
    0xFB20A47D, //  Homebrew\Assimilate (2012).nes 
    0x04B7CC0D, //  Homebrew\Bugfixes, Improvements\A Boy and His Blob꞉ Trouble on Blobolonia [DPCM bit order].nes 
    0xA7F8BBC8, //  Homebrew\Bugfixes, Improvements\Mad City [DPCM bit order, DPCM clicking].nes 
    0x85CA1447, //  Homebrew\Copyright Restorations\Castlevania II꞉ Simon's Quest (rev1)[NewRisingSun].nes 
    0x428155DB, //  Homebrew\Hacks\2DDK Blaster Master.nes 
    0xFCF7AB2E, //  Homebrew\Hacks\Adoventoro Tcheco [tcheco].nes 
    0x860DD08F, //  Homebrew\Hacks\Adventures in the Magic Kingdom꞉ Sora Edition [pacnsacdave].nes 
    0x41A3576F, //  Homebrew\Hacks\Adventures of Lolo III [Chris].nes 
    0x6EB0A2CF, //  Homebrew\Hacks\Air Contra [Cx4].nes 
    0x2764A647, //  Homebrew\Hacks\As Tartarugas Ninja [Macbee].nes 
    0x2B498E35, //  Homebrew\Hacks\Assassins Creed Gaiden [Dom Jurumela].nes 
    0xB4F68A71, //  Homebrew\Hacks\Back to the Future 5 &amp; 6 [RyanVG].nes 
    0x625DA1B3, //  Homebrew\Hacks\Basterd Master.nes 
    0x846C612D, //  Homebrew\Hacks\Captain Silver - Enhanced Version.nes 
    0x1B02A011, //  Homebrew\Hacks\Castlevania series\Castlevania II꞉ Simon's Quest - Lomax Edition.nes 
    0x36424B35, //  Homebrew\Hacks\Castlevania series\Castlevania II꞉ Trevor's Quest.nes 
    0xBFADCA20, //  Homebrew\Hacks\Castlevania series\Castlevania II꞉ Vengence of Hell (v1.0)[Ballz].nes 
    0xE0727E4B, //  Homebrew\Hacks\Castlevania series\Castlevania II꞉ Vengence of Hell (v1.0d)[Ballz].nes 
    0xE5FBC8F9, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Opposing Bloodlines [Redrum&amp;Morgoth].nes 
    0xD8A00CCE, //  Homebrew\Hacks\Castlevania series\Simon's Redaction [TheAlmightyGuru].nes 
    0x083AEA82, //  Homebrew\Hacks\Propeller Master.nes 
    0x06FBE365, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Bowl - 2007-2008 [kefkaroth].nes 
    0x4EC87E00, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Bowl NFL 2007 Playoffs.nes 
    0x2BF07493, //  Homebrew\Original Mapper Restorations\Bolide.nes 
    0xF1E4CEAF, //  Homebrew\Translations\English\A Taxing Woman [M-Tee].nes 
    0xB30BCE64, //  Homebrew\Translations\English\Airwolf [Vice Translations].nes 
    0xBF94797A, //  Homebrew\Translations\English\American Dream [Pale Dim].nes 
    0xD1E7FF2C, //  Homebrew\Translations\English\Big Strategy Pachinko 2 [MrRichard999].nes 
    0xF8C3A3A9, //  Homebrew\Translations\English\Big Strategy Pachinko [MrRichard999 v0.97b].nes 
    0x1BFAE7A6, //  Homebrew\Translations\English\Bird Squadron Jetman [Chronix].nes 
    0x7DAAFBCB, //  Homebrew\Translations\English\Captain Silver [Stardust Crusaders].nes 
    0x99B3881F, //  Homebrew\Translations\English\Captain Tsubasa [Lestat v1.0, knighTeen87 v1.5].nes 
    0xCB89A0E9, //  Homebrew\Translations\English\Captain Tsubasa [Lestat v1.0].nes 
    0x418B1090, //  Homebrew\Translations\English\Choujin Sentai Jetman [Magimoogle].nes 
    0xE168E5A1, //  Homebrew\Translations\English\Chuka Taisen - Cloud Master [Stardust Crusaders].nes 
    0xC8337611, //  Homebrew\Translations\English\Chuka Taisen [Ice Translations].nes 
    0x3312156B, //  Homebrew\Translations\English\Donald Duck [Chronix].nes 
    0x6AB6ED56, //  Homebrew\Translations\English\Elysion [Tenshigami v0.92].nes 
    0x85AD014C, //  Homebrew\Translations\English\Game Party [Proveaux].nes 
    0xB0D6F726, //  Homebrew\Translations\English\Godzilla [Stardust Crusaders v1.0].nes 
    0xEBFC2FFD, //  Homebrew\Translations\English\Golf Club꞉ Birdy Rush [MrRichard999].nes 
    0xA21D75D8, //  Homebrew\Translations\English\Gourmet [Snark v1.0].nes 
    0xD35D2149, //  Homebrew\Translations\English\Holy Diver [Monaco, MottZilla R1].nes 
    0xE68F5C57, //  Homebrew\Translations\English\Kero Kero Keroppi's Great Big Adventure 2꞉ Trouble in Donut Pond [The Spoony Bard].nes 
    0xC129B2AF, //  Homebrew\Translations\English\Kiteretsu Encyclopedia [Zynk Oxhyde].nes 
    0x2B6E1DA8, //  Homebrew\Translations\English\Little Ghosts [KingMike].nes 
    0x3F8BB92D, //  Homebrew\Translations\English\Mad City [Stardust Crusaders].nes 
    0x5150BC1D, //  Homebrew\Translations\English\Masked Ninja Hanamaru [Pacnsacdave v1.1].nes 
    0xE49F4090, //  Homebrew\Translations\English\Mini Putt [Klarth Aileron].nes 
    0xAFE1C114, //  Homebrew\Translations\English\Motocross Champion [Suicidal Translations v1.00].nes 
    0x72EEAE2D, //  Homebrew\Translations\English\Parody World꞉ Monster Party [Infinest].nes 
    0xDF664080, //  Homebrew\Translations\English\Parody World꞉ Monster Party [Stardust Crusaders].nes 
    0xC9E6F127, //  Homebrew\Translations\English\Pop Star Debut [Woolsey Fan Company v1.1].nes 
    0xB5EF2DEF, //  Homebrew\Translations\English\Princesstual Orgy [Suicidal Translations].nes 
    0xFC98C203, //  Homebrew\Translations\English\Saint Seiya - Knights of the Zodiac꞉ The Golden Legend - Final Chapter [Aishsha &amp; Djinn v1.01].nes 
    0xB6F80655, //  Homebrew\Translations\English\Softball Heaven [Stardust Crusaders].nes 
    0x7B40D235, //  Homebrew\Translations\English\Softball Heaven [The Spoony Bard v1.1].nes 
    0xA7FD5AB0, //  Homebrew\Translations\English\Super Real Baseball '88 [MrRichard999, Proveaux, TheMajinZenki, Jink640, FlashPV].nes 
    0x6B0EA620, //  Homebrew\Translations\English\Tao [Snark].nes 
    0x7E36C98B, //  Homebrew\Translations\English\The 100 World Story꞉ The Tales on a Watery Wilderness [AlanMidas BS.VQt].nes 
    0x40DF6A65, //  Homebrew\Translations\English\The Golf '92 [Zynk Oxhyde].nes 
    0x02AA1E43, //  Homebrew\Translations\English\The Great Maze꞉ Master of the Maze [Filler, KingMike's Translations].nes 
    0x89F8EBDA, //  Homebrew\Translations\English\The Venus Wars꞉ Back to the City ''Io'' [J2e Translations v2.0].nes 
    0xB1A84413, //  Homebrew\Translations\English\WRC FIA World Rally꞉ World Rally Championship [Klepto Software].nes 
    0x0F08DF85, //  Homebrew\Translations\English\熱血高校 Dodgeball 部 [Generica Online v0.9].nes 
    0x01B87025, //  Homebrew\Translations\French\The Adventures of Bayou Billy [GenerationIX].nes 
    0x14CD576E, //  Licensed Japan\Abadox.nes 
    0xEAB93CFB, //  Licensed Japan\Adventures of Lolo II.nes 
    0xFB8A9B80, //  Licensed Japan\Airwolf.nes 
    0x5B837E8D, //  Licensed Japan\Alien Syndrome.nes 
    0x3FFA5762, //  Licensed Japan\American Dream.nes 
    0x074EC424, //  Licensed Japan\Arabian Dream Scheherazade.nes 
    0x008E2D30, //  Licensed Japan\Blodia Land꞉ Puzzle Quest.nes 
    0xCB0A3AF4, //  Licensed Japan\Captain Silver.nes 
    0xF3F1269D, //  Licensed Japan\Captain 翼.nes 
    0x99686DAD, //  Licensed Japan\ChipとDaleの大作戦 2.nes 
    0xD923EB5B, //  Licensed Japan\ChipとDaleの大作戦.nes 
    0x24BA90CA, //  Licensed Japan\Cobra Command.nes 
    0xE429F0D3, //  Licensed Japan\Cycle Race Roadman꞉ 激走!! 日本一周 4000km.nes 
    0x28C2DFCE, //  Licensed Japan\Die Hard.nes 
    0xE66BDDCF, //  Licensed Japan\Donald Duck.nes 
    0xD17B76DA, //  Licensed Japan\Donald Land.nes 
    0x6DCE4B23, //  Licensed Japan\Double Dragon.nes 
    0x4B0DACCE, //  Licensed Japan\Dragon Fighter.nes 
    0x240C6DE8, //  Licensed Japan\Elysion.nes 
    0xE15C973D, //  Licensed Japan\Exciting Rally꞉ World Rally Championship.nes 
    0xA485ABED, //  Licensed Japan\Ferrari Grand Prix Challenge.nes 
    0x8308FED7, //  Licensed Japan\Fighting Golf.nes 
    0xD5941AA9, //  Licensed Japan\Final Mission.nes 
    0xF1C76AED, //  Licensed Japan\Game Party.nes 
    0xFCB13110, //  Licensed Japan\Golf Club꞉ Birdy Rush.nes 
    0x3F0C8136, //  Licensed Japan\Golf Grand Slam.nes 
    0xA0006B26, //  Licensed Japan\Golgo 13 第一章꞉ 神々の黄昏.nes 
    0xD09B74DC, //  Licensed Japan\Great Tank.nes 
    0x63C4E122, //  Licensed Japan\Guevara.nes 
    0x2DB7C31E, //  Licensed Japan\Hook.nes 
    0x85C5953F, //  Licensed Japan\Hostages꞉ The Embassy Mission.nes 
    0x2FC1ABAE, //  Licensed Japan\Hudson Hawk.nes 
    0xAA4318AE, //  Licensed Japan\I Love Softball.nes 
    0x8DA4E539, //  Licensed Japan\Idol 八犬伝.nes 
    0x41F5D38D, //  Licensed Japan\Kero Kero Keroppiの大冒険 2꞉ Donuts 池はおおさわぎ!.nes 
    0xC7642467, //  Licensed Japan\Konamic Sports in Seoul.nes 
    0xDF3E45D2, //  Licensed Japan\Mad City.nes 
    0xE02133AC, //  Licensed Japan\Magic Darts.nes 
    0x099B8CAA, //  Licensed Japan\Mahjong RPG ドラ! ドラ! ドラ!.nes 
    0x8531C166, //  Licensed Japan\Majaventure 麻雀戦記.nes 
    0xBF93112A, //  Licensed Japan\Metal Flame Psybuster.nes 
    0x6439F53A, //  Licensed Japan\Mini-Putt.nes 
    0xE6C9029E, //  Licensed Japan\Motocross Champion.nes 
    0x5931BE01, //  Licensed Japan\New Ghostbusters II.nes 
    0x4ED3C6F1, //  Licensed Japan\Predator.nes 
    0xF4DD5BA5, //  Licensed Japan\Puzslot.nes 
    0x3FEA656A, //  Licensed Japan\Quarterback Scramble.nes 
    0xC22FF1D8, //  Licensed Japan\RoboCop 2.nes 
    0x9EFF96D2, //  Licensed Japan\Rough World.nes 
    0xAAF49344, //  Licensed Japan\Snow Bros..nes 
    0x8D3C33B3, //  Licensed Japan\Soccer League꞉ Winner's Cup.nes 
    0x822F17EB, //  Licensed Japan\Softball 天国.nes 
    0xF17486DF, //  Licensed Japan\Super Chinese 3.nes 
    0xD175B0CB, //  Licensed Japan\Super Real Baseball '88.nes 
    0x9F5138CB, //  Licensed Japan\Super Rugby.nes 
    0x856E7600, //  Licensed Japan\Superman.nes 
    0xDAD34EE6, //  Licensed Japan\Tao꞉ 究極の世紀末 Role Play.nes 
    0x588E7492, //  Licensed Japan\Tecmo Bowl.nes 
    0x139B15BA, //  Licensed Japan\The Golf '92.nes 
    0xB9AB06AA, //  Licensed Japan\The Money Game 2꞉ 兜町の奇跡.nes 
    0xCC3544B0, //  Licensed Japan\The Triathlon.nes 
    0xDA8F65AE, //  Licensed Japan\The Untouchables.nes 
    0x7B72FBA4, //  Licensed Japan\Thunderbirds.nes 
    0x1D8BF724, //  Licensed Japan\Venus 戦記.nes 
    0x752743EC, //  Licensed Japan\Willow.nes 
    0x4DCD15EE, //  Licensed Japan\World Boxing.nes 
    0x41CC30A7, //  Licensed Japan\World Super Tennis.nes 
    0x4E5257D7, //  Licensed Japan\おそ松くん꞉ Back to the Meの出っ歯の巻.nes 
    0xF8C1A690, //  Licensed Japan\かってに白熊.nes 
    0x5DEC84F8, //  Licensed Japan\ちいさな おばけ アッチ コッチ ソッチ.nes 
    0x0B13658B, //  Licensed Japan\ひょっこりひょうたん島꞉ なぞのかいぞくせん.nes 
    0xDB479677, //  Licensed Japan\キテレツ大百科.nes 
    0x8A36D2B7, //  Licensed Japan\ゴジラ.nes 
    0x44F92026, //  Licensed Japan\パチンコ大作戦 2.nes 
    0xC22C23AB, //  Licensed Japan\パチンコ大作戦.nes 
    0xE2281986, //  Licensed Japan\マルサの女.nes 
    0x1B7BD879, //  Licensed Japan\不思議な Blobby꞉ Blobaniaの危機.nes 
    0x5AB54795, //  Licensed Japan\中国雀士 Story꞉ 東風.nes 
    0x8E0D9179, //  Licensed Japan\中華大仙.nes 
    0x489D19AB, //  Licensed Japan\亀の恩返し꞉ ウラシマ伝説.nes 
    0xE6B30BB3, //  Licensed Japan\仮面の忍者꞉ 花丸.nes 
    0x0F5F1F86, //  Licensed Japan\全米!! Pro Basket.nes 
    0x415E5109, //  Licensed Japan\大迷路꞉ 迷宮の達人.nes 
    0x9E777EA5, //  Licensed Japan\忍者 Cop サイゾウ.nes 
    0xD7CB398F, //  Licensed Japan\忍者龍剣伝.nes 
    0xF7D20181, //  Licensed Japan\怒 III - Ikari Three.nes 
    0x268E39D0, //  Licensed Japan\探偵神宮寺三郎꞉ 横浜港連続殺人事件.nes 
    0x52AB2D17, //  Licensed Japan\東京パチ Slot Adventure (rev0).nes 
    0x162CCBD0, //  Licensed Japan\東京パチ Slot Adventure (rev1).nes 
    0x97BC4585, //  Licensed Japan\東方見文録.nes 
    0x64A02715, //  Licensed Japan\激亀忍者伝.nes 
    0x50893B58, //  Licensed Japan\激闘 Pro Wres!! 闘魂伝説.nes 
    0x481519B1, //  Licensed Japan\激闘 Stadium!!.nes 
    0x62C67984, //  Licensed Japan\熱血高校 Dodgeball 部.nes 
    0xF96D07C8, //  Licensed Japan\燃える! お兄さん.nes 
    0x16E93F39, //  Licensed Japan\田代まさしのPrincessがいっぱい.nes 
    0xE95E51E0, //  Licensed Japan\百の世界の物語꞉ The Tales on a Watery Wilderness.nes 
    0x264F26B1, //  Licensed Japan\美味しんぼ꞉ 究極のMenu 三本勝負.nes 
    0x9561798D, //  Licensed Japan\聖闘士星矢꞉ 黄金伝説 完結編.nes 
    0x360AA8B4, //  Licensed Japan\脱獄.nes 
    0x25EDAF5C, //  Licensed Japan\落っことし Puzzle とんじゃん!.nes 
    0x21DD2174, //  Licensed Japan\覇邪の封印.nes 
    0x396F0D59, //  Licensed Japan\赤龍王.nes 
    0x039B4A9C, //  Licensed Japan\超惑星戦記 MetaFight.nes 
    0x23BF0507, //  Licensed Japan\電撃 Big Bang!.nes 
    0x74663267, //  Licensed Japan\飛龍の拳 II꞉ Dragonの翼.nes 
    0x07977186, //  Licensed Japan\飛龍の拳 Special꞉ Fighting Wars.nes 
    0x2D2F91B8, //  Licensed Japan\魔法のPrincess Minky Momo꞉ Remember Dream.nes 
    0x952A9E77, //  Licensed Japan\鳥人戦隊 Jetman.nes 
    0x4D1AC58C, //  Licensed North America\A Boy and His Blob꞉ Trouble on Blobolonia.nes 
    0xB134D713, //  Licensed North America\Abadox꞉ The Deadly Inner War.nes 
    0x5DBD6099, //  Licensed North America\Adventures in the Magic Kingdom.nes 
    0x8DD92725, //  Licensed North America\Adventures of Lolo III.nes 
    0x6E85D8DD, //  Licensed North America\Adventures of Tom Sawyer.nes 
    0xA8B0DA56, //  Licensed North America\Alex DeMeo's Race America.nes 
    0x934DB14A, //  Licensed North America\All-Pro Basketball.nes 
    0x1973AEA8, //  Licensed North America\American Gladiators.nes 
    0x7C6F615F, //  Licensed North America\Attack of the Killer Tomatoes.nes 
    0x37BA3261, //  Licensed North America\Back to the Future Part II &amp; III.nes 
    0x40DAFCBA, //  Licensed North America\Bad News Baseball.nes 
    0x5B6CA654, //  Licensed North America\Barbie (rev0).nes 
    0x18B249E5, //  Licensed North America\Barbie (rev1).nes 
    0xC42E648A, //  Licensed North America\Bigfoot.nes 
    0xC4B6ED3C, //  Licensed North America\Bill &amp; Ted's Excellent Video Game Adventure.nes 
    0x3F0FD764, //  Licensed North America\Blaster Master.nes 
    0xA5E8D2CD, //  Licensed North America\BreakThru.nes 
    0xC471E42D, //  Licensed North America\Castlevania II꞉ Simon's Quest.nes 
    0xD1EA84C3, //  Licensed North America\Caveman Games.nes 
    0xFC5783A7, //  Licensed North America\Chip 'n Dale꞉ Rescue Rangers 2.nes 
    0x8BF29CB6, //  Licensed North America\Chip 'n Dale꞉ Rescue Rangers.nes 
    0xD152FB02, //  Licensed North America\Circus Caper.nes 
    0x82AFA828, //  Licensed North America\Clash at Demonhead.nes 
    0x2D75C7A9, //  Licensed North America\Cobra Command.nes 
    0xD73AA04C, //  Licensed North America\Cool World.nes 
    0x398B8182, //  Licensed North America\Darkman.nes 
    0x5DCE2EEA, //  Licensed North America\Darkwing Duck.nes 
    0x4681691A, //  Licensed North America\Demon Sword.nes 
    0x085DE7C9, //  Licensed North America\Die Hard.nes 
    0x0F1CC048, //  Licensed North America\Double Dragon.nes 
    0xA166548F, //  Licensed North America\Dragon Fighter.nes 
    0x38B590E4, //  Licensed North America\Dusty Diamond's All-Star Softball.nes 
    0xBDE93999, //  Licensed North America\Dynowarz꞉ The Destruction of Spondylus.nes 
    0x059E0CDF, //  Licensed North America\Eliminator Boat Duel.nes 
    0xCE77B4BE, //  Licensed North America\Ferrari Grand Prix Challenge.nes 
    0x10180072, //  Licensed North America\Flying Warriors.nes 
    0xE943EC4D, //  Licensed North America\Frankenstein꞉ The Monster Returns.nes 
    0x3E58A87E, //  Licensed North America\Freedom Force.nes 
    0x2AE97660, //  Licensed North America\Ghostbusters II.nes 
    0x2BC25D5A, //  Licensed North America\Ghoul School.nes 
    0x9FB32923, //  Licensed North America\Godzilla 2꞉ War of the Monsters.nes 
    0x74386F15, //  Licensed North America\Godzilla꞉ Monster of Monsters!.nes 
    0xCF5F8AF0, //  Licensed North America\Golf Grand Slam.nes 
    0xF532F09A, //  Licensed North America\Golgo 13꞉ Top Secret Episode.nes 
    0x6DECD886, //  Licensed North America\Guerrilla War.nes 
    0x2E6EE98D, //  Licensed North America\Harlem Globetrotters.nes 
    0xEB15169E, //  Licensed North America\Heavy Shreddin'.nes 
    0xD8230D0E, //  Licensed North America\Hook.nes 
    0xF4615036, //  Licensed North America\Hoops.nes 
    0x5A4F156D, //  Licensed North America\Hudson Hawk.nes 
    0x567E1620, //  Licensed North America\Ikari III꞉ The Rescue.nes 
    0xB14EA4D2, //  Licensed North America\Iron Tank꞉ The Invasion of Normandy.nes 
    0xE2C4EDCE, //  Licensed North America\Journey to Silius.nes 
    0x88E1A5F4, //  Licensed North America\Lee Trevino's Fighting Golf.nes 
    0xA69F29FA, //  Licensed North America\Lemmings.nes 
    0x7077B075, //  Licensed North America\Lethal Weapon.nes 
    0x859C65E1, //  Licensed North America\Little League Baseball Championship Series.nes 
    0x9F6C119C, //  Licensed North America\MTV's Remote Control.nes 
    0x5EB8E707, //  Licensed North America\Magic Darts.nes 
    0x05378607, //  Licensed North America\Metal Mech꞉ Man &amp; Machine.nes 
    0x9747AC09, //  Licensed North America\Monopoly.nes 
    0x02B9E7C2, //  Licensed North America\Monster Party.nes 
    0x0A0926BD, //  Licensed North America\Motor City Patrol.nes 
    0x4751A751, //  Licensed North America\Nigel Mansell's World Championship Challenge.nes 
    0x7C4A72D8, //  Licensed North America\Ninja Gaiden.nes 
    0xEDC3662B, //  Licensed North America\Operation Wolf꞉ Take no Prisoners.nes 
    0x75255F88, //  Licensed North America\P.O.W. - Prisoners of War.nes 
    0xDC02F095, //  Licensed North America\Pictionary.nes 
    0x2538D860, //  Licensed North America\Pinball Quest.nes 
    0xB3769A51, //  Licensed North America\Platoon (rev0).nes 
    0x695515A2, //  Licensed North America\Platoon (rev1).nes 
    0xA7DE65E4, //  Licensed North America\Predator.nes 
    0x63FCC0DD, //  Licensed North America\Rescue꞉ The Embassy Mission.nes 
    0x8ADA3497, //  Licensed North America\RoadBlasters.nes 
    0xB4CDF95F, //  Licensed North America\RoboCop 2 (rev0).nes 
    0x990985C0, //  Licensed North America\RoboCop 2 (rev1).nes 
    0x96087988, //  Licensed North America\RoboCop 3.nes 
    0x74189E12, //  Licensed North America\S.C.A.T. - Special Cybernetic Attack Team.nes 
    0x24EECC15, //  Licensed North America\Sesame Street꞉ 123 &amp; ABC.nes 
    0x5CF6A82E, //  Licensed North America\Sesame Street꞉ Countdown.nes 
    0xE74A91BB, //  Licensed North America\Silkworm.nes 
    0xE9A6C211, //  Licensed North America\Ski or Die.nes 
    0x48E904D0, //  Licensed North America\Snake's Revenge.nes 
    0xA8923256, //  Licensed North America\Snoopy's Silly Sports Spectacular.nes 
    0x1DAC6208, //  Licensed North America\Snow Bros..nes 
    0xC6DD7E69, //  Licensed North America\Stealth ATF.nes 
    0x61D86167, //  Licensed North America\Street Cop.nes 
    0x689971F9, //  Licensed North America\Super Dodge Ball.nes 
    0xCF4487A2, //  Licensed North America\Super Jeopardy!.nes 
    0x1FA8C4A4, //  Licensed North America\Superman.nes 
    0xA1FF4E1D, //  Licensed North America\Swamp Thing.nes 
    0x798EEB98, //  Licensed North America\TaleSpin.nes 
    0x93B49582, //  Licensed North America\Target꞉ Renegade.nes 
    0xCE00022D, //  Licensed North America\Tecmo Bowl (rev0).nes 
    0x85A6C0D5, //  Licensed North America\Tecmo Bowl (rev1).nes 
    0x696D7839, //  Licensed North America\Tecmo Cup꞉ Soccer Game.nes 
    0x7FF76219, //  Licensed North America\Tecmo World Wrestling.nes 
    0xEE921D8E, //  Licensed North America\Teenage Mutant Ninja Turtles.nes 
    0x65518EAE, //  Licensed North America\The Addams Family.nes 
    0xB6BF5137, //  Licensed North America\The Addams Family꞉ Fester's Quest.nes 
    0x063E5653, //  Licensed North America\The Addams Family꞉ Pugsley's Scavenger Hunt.nes 
    0x67751094, //  Licensed North America\The Adventures of Bayou Billy.nes 
    0xD8EE7669, //  Licensed North America\The Adventures of Rad Gravity.nes 
    0x2DFF7FDC, //  Licensed North America\The Great Waldo Search.nes 
    0x92197173, //  Licensed North America\The Magic of Scheherazade.nes 
    0x7DCB4C18, //  Licensed North America\The Mutant Virus꞉ Crisis in a Computer World.nes 
    0x6F10097D, //  Licensed North America\The Simpsons꞉ Bart vs. the Space Mutants (rev0).nes 
    0x5248CAF3, //  Licensed North America\The Simpsons꞉ Bart vs. the Space Mutants (rev1).nes 
    0x538CD2EA, //  Licensed North America\The Three Stooges.nes 
    0x209F3587, //  Licensed North America\The Untouchables (rev0).nes 
    0x588A31FE, //  Licensed North America\The Untouchables (rev1).nes 
    0x20C795EB, //  Licensed North America\The Untouchables (rev2).nes 
    0x2DDC2DC3, //  Licensed North America\Thunderbirds.nes 
    0xF99E37EB, //  Licensed North America\Top Players' Tennis, Featuring Chris Evert &amp; Ivan Lendl.nes 
    0x09C083B7, //  Licensed North America\Track &amp; Field II (rev0).nes 
    0x7FA191E7, //  Licensed North America\Track &amp; Field II (rev1).nes 
    0xB918580C, //  Licensed North America\Treasure Master.nes 
    0x103E7E7F, //  Licensed North America\Willow.nes 
    0xF651398D, //  Licensed North America\Wrath of the Black Manta (rev0).nes 
    0x3A8723B9, //  Licensed North America\Wrath of the Black Manta (rev1).nes 
    0xB1612FE6, //  Licensed North America\Xexyz.nes 
    0x50D141FC, //  Licensed North America\Yo! Noid.nes 
    0x3D16979B, //  Modern\Castlevania II꞉ Simon's Quest (Castlevania Anniversary Collection).nes 
    0x709B54B2, //  Modern\Castlevania II꞉ Simon's Quest (Konami Collector's Series).nes 
    0x9E8A45B7, //  Modern\Chip 'n Dale꞉ Rescue Rangers (The Disney Afternoon Collection).nes 
    0xC56084D7, //  Modern\Chip 'n Dale꞉ Rescue Rangers 2 (The Disney Afternoon Collection).nes 
    0xA3759CB3, //  Modern\Classic Edition\Tecmo Bowl.nes 
    0xEDC7E8C6, //  Modern\Darkwing Duck (The Disney Afternoon Collection).nes 
    0x1CB7CE42, //  Modern\Great Tank (SNK 40th Anniversary Collection).nes 
    0x08529D4A, //  Modern\Little Lancelot.nes 
    0xA97ECE5B, //  Modern\TaleSpin (The Disney Afternoon Collection).nes 
    0xE59C4A99, //  Modern\Virtual Console\Blaster Master.nes 
    0x7F6D08A8, //  Modern\Virtual Console\Flying Warriors.nes 
    0x1BFE42AB, //  Modern\Virtual Console\Mad City.nes 
    0x2FE4B645, //  Modern\Virtual Console\Ninja Gaiden.nes 
    0x2790ED68, //  Modern\Virtual Console\S.C.A.T. - Special Cybernetic Attack Team.nes 
    0xBEBB4E01, //  Modern\Virtual Console\Super Dodge Ball.nes 
    0xDC4DB09A, //  Modern\Virtual Console\Teenage Mutant Ninja Turtles.nes 
    0x8373021E, //  Modern\Virtual Console\The Adventures of Bayou Billy.nes 
    0x00EBEB79, //  Modern\Virtual Console\忍者龍剣伝 (alt).nes 
    0x7FBFD559, //  Modern\Virtual Console\忍者龍剣伝.nes 
    0xCF78BA06, //  Modern\Virtual Console\探偵神宮寺三郎꞉ 横浜港連続殺人事件.nes 
    0xAAE9E72F, //  Modern\Virtual Console\激亀忍者伝.nes 
    0xAED38D5A, //  Modern\Virtual Console\熱血高校 Dodgeball 部.nes 
    0x4676CED3, //  Modern\Virtual Console\超惑星戦記 MetaFight.nes 
    0xC3A8B1ED, //  Modern\Virtual Console\飛龍の拳 II꞉ Dragonの翼.nes 
    0x88C369B6, //  Modern\くにおくん熱血 Collection\熱血高校 Dodgeball 部.nes 
    0x5C16DC2F, //  Playchoice\Chip 'n Dale꞉ Rescue Rangers.nes 
    0x0E004847, //  Playchoice\Double Dragon.nes 
    0x806E5CD7, //  Playchoice\Fester's Quest.nes 
    0xA2E64E5E, //  Playchoice\Ninja Gaiden.nes 
    0x802671E0, //  Playchoice\Tecmo Bowl.nes 
    0xB04C53F1, //  Playchoice\Teenage Mutant Ninja Turtles.nes 
    0x17EA92A8, //  Playchoice\Yo! Noid.nes 
    0x4FBBE319, //  Unreleased\Adventures in the Magic Kingdom (Beta 1).nes 
    0x3CF749DE, //  Unreleased\Adventures in the Magic Kingdom (Beta 2).nes 
    0xADA40FB2, //  Unreleased\Arcadia VI.nes 
    0x7C7A0A73, //  Unreleased\Bashi Bazook꞉ Morphoid Masher.nes 
    0xB683A856, //  Unreleased\Blaster Master.nes 
    0xCC6CA4DC, //  Unreleased\Chip 'n Dale꞉ Rescue Rangers 2.nes 
    0x24C5EF30, //  Unreleased\Chip 'n Dale꞉ Rescue Rangers.nes 
    0x8DB43824, //  Unreleased\Darkwing Duck.nes 
    0x6EB336F9, //  Unreleased\Days of Thunder (Compiled PRG+CHR).nes 
    0x064505AE, //  Unreleased\Days of Thunder (Prebuilt PRG+CHR Patched).nes 
    0x7BDA85C2, //  Unreleased\Days of Thunder (Prebuilt PRG+CHR).nes 
    0x41462D21, //  Unreleased\Double Dragon.nes 
    0x4232C609, //  Unreleased\Drac's Night Out.nes 
    0x92DD67EA, //  Unreleased\Flying Warriors.nes 
    0x73CCDAE0, //  Unreleased\Freedom Force.nes 
    0x0E0060C8, //  Unreleased\Happily Ever After.nes 
    0x9D9A89C4, //  Unreleased\Hook.nes 
    0x9768AE75, //  Unreleased\Mike Ditka's Big Play Football.nes 
    0x5ABBF861, //  Unreleased\New Ghostbusters II.nes 
    0x6EEA1B10, //  Unreleased\Ninja Gaiden.nes 
    0x21E28F50, //  Unreleased\Parody World꞉ Monster Party.nes 
    0xF6CC7A99, //  Unreleased\Quarter Back Scramble.nes 
    0xF6AB12A2, //  Unreleased\Rescue꞉ The Embassy Mission.nes 
    0x78C72C75, //  Unreleased\S.C.A.T. - Special Cybernetic Attack Team.nes 
    0xD31EB7BB, //  Unreleased\Star Trek V꞉ The Final Frontier.nes 
    0xDE84354A, //  Unreleased\Superman (Kemco).nes 
    0xE5EA0EBE, //  Unreleased\Tecmo Bowl.nes 
    0x1FF251AE, //  Unreleased\Teenage Mutant Ninja Turtles.nes 
    0xF471827D, //  Unreleased\The Addams Family꞉ Fester's Quest.nes 
    0xAE97627C, //  Unreleased\The Bugs Bunny Fun House.nes 
    0x1C9EA55C, //  Unreleased\The Three Stooges.nes 
    0xB0874760, //  Unreleased\USA Bowling.nes 
    0x6A6FC14C, //  Unreleased\Yo! Noid.nes 
    0x7C108923, //  Unreleased\聖闘士星矢꞉ 黄金伝説 完結編.nes 
    0x0C462638, //  Unreleased\電撃 Big Bang!.nes 
    0x14374128, //  Unreleased\飛龍の拳 Special꞉ Fighting Wars.nes 
/**** 82 mapper=1 prgrom=128K chrrom=128K mirror=H system=SYSTEM_PAL */
    0xC5CEF2C4, //  Homebrew\Translations\English\Knights of the Zodiac꞉ Legend of the Golden Armor [The Rosetta Group].nes 
    0x64A7FC0E, //  Homebrew\Translations\English\The Knights of the Zodiac꞉ The Legend of the Gold (France) [KingMike's Translations v1.0].nes 
    0xAB2AC325, //  Licensed PAL\A Boy and His Blob꞉ Trouble on Blobolonia (rev0).nes 
    0x8ECBC577, //  Licensed PAL\A Boy and His Blob꞉ Trouble on Blobolonia (rev1).nes 
    0x3AC0830A, //  Licensed PAL\Action in New York.nes 
    0x6B761858, //  Licensed PAL\Adventures in the Magic Kingdom.nes 
    0x37397194, //  Licensed PAL\Adventures of Lolo 3.nes 
    0xA6153536, //  Licensed PAL\Attack of the Killer Tomatoes.nes 
    0xD630EE8F, //  Licensed PAL\Aussie Rules Footy.nes 
    0xD364F816, //  Licensed PAL\Barbie.nes 
    0x34C1E893, //  Licensed PAL\Bigfoot.nes 
    0x8106E694, //  Licensed PAL\Blaster Master.nes 
    0xD6F7383E, //  Licensed PAL\Castlevania II꞉ Simon's Quest.nes 
    0xC076D66F, //  Licensed PAL\Championship Rally.nes 
    0xF83E0D2D, //  Licensed PAL\Chip 'n Dale꞉ Rescue Rangers 2.nes 
    0x84F7FC31, //  Licensed PAL\Chip 'n Dale꞉ Rescue Rangers.nes 
    0x14255C57, //  Licensed PAL\Corvette ZR-1 Challenge.nes 
    0xCFE02ADA, //  Licensed PAL\Darkman.nes 
    0x66F6A39E, //  Licensed PAL\Darkwing Duck (English).nes 
    0x443FC6CD, //  Licensed PAL\Darkwing Duck (German).nes 
    0x607BD020, //  Licensed PAL\Die Hard.nes 
    0xA1A0C13F, //  Licensed PAL\Double Dragon.nes 
    0x924CDE0B, //  Licensed PAL\Eliminator Boat Duel.nes 
    0x53A9B53A, //  Licensed PAL\Ferrari Grand Prix Challenge.nes 
    0x48B8EE58, //  Licensed PAL\Four Players' Tennis.nes 
    0x7B55D481, //  Licensed PAL\Ghostbusters II.nes 
    0x46480432, //  Licensed PAL\Godzilla꞉ Monster of Monsters!.nes 
    0x622E054A, //  Licensed PAL\Guerrilla War.nes 
    0xBF888B75, //  Licensed PAL\Hook.nes 
    0x3DBD6DAF, //  Licensed PAL\Hoops.nes 
    0xAC3E5677, //  Licensed PAL\Hudson Hawk.nes 
    0xA66D5150, //  Licensed PAL\International Cricket.nes 
    0xDF6D0CE8, //  Licensed PAL\Iron Tank꞉ The Invasion of Normandy.nes 
    0xAB671224, //  Licensed PAL\Journey to Silius.nes 
    0x9630A7E5, //  Licensed PAL\Lee Trevino's Fighting Golf.nes 
    0x9B568CC4, //  Licensed PAL\Lemmings.nes 
    0x358E29DD, //  Licensed PAL\Les Chevaliers du Zodiaque꞉ La Legende d'Or.nes 
    0xE043C6A5, //  Licensed PAL\Lethal Weapon.nes 
    0x03B8DEFA, //  Licensed PAL\Monopoly (French).nes 
    0x39BB6616, //  Licensed PAL\Monopoly (German).nes 
    0xF0C198FF, //  Licensed PAL\New Ghostbusters II.nes 
    0x1677D21D, //  Licensed PAL\Nigel Mansell's World Championship Challenge.nes 
    0x5529431F, //  Licensed PAL\Operation Wolf꞉ Take no Prisoners.nes 
    0x073A0EBE, //  Licensed PAL\P.O.W. - Prisoners of War.nes 
    0xB1C937C8, //  Licensed PAL\Parasol Stars꞉ The Story of Bubble Bobble 3.nes 
    0x9F119033, //  Licensed PAL\Pinball Quest.nes 
    0x978E19FC, //  Licensed PAL\Predator.nes 
    0xBE0E93C3, //  Licensed PAL\Rainbow Islands꞉ The Story of Bubble Bobble 2.nes 
    0x8B4D2443, //  Licensed PAL\Rescue꞉ The Embassy Mission.nes 
    0x17389E3D, //  Licensed PAL\RoadBlasters.nes 
    0x89821E2B, //  Licensed PAL\RoboCop 2.nes 
    0xA8D93537, //  Licensed PAL\RoboCop 3.nes 
    0x709C9399, //  Licensed PAL\Shadow Warriors.nes 
    0x0DA0E723, //  Licensed PAL\Ski or Die.nes 
    0xE71D034E, //  Licensed PAL\Snake's Revenge.nes 
    0xA9660690, //  Licensed PAL\Snow Bros..nes 
    0xD5C588DF, //  Licensed PAL\Snowboard Challenge.nes 
    0xE5A8401B, //  Licensed PAL\Stealth ATF.nes 
    0xAAA985D7, //  Licensed PAL\Swamp Thing.nes 
    0xFE08D602, //  Licensed PAL\TaleSpin.nes 
    0x60925D08, //  Licensed PAL\Tecmo Cup꞉ Football Game (English).nes 
    0x7F801368, //  Licensed PAL\Tecmo Cup꞉ Football Game (Spanish).nes 
    0xCF849F72, //  Licensed PAL\Tecmo World Wrestling.nes 
    0x00AD1189, //  Licensed PAL\Teenage Mutant Hero Turtles.nes 
    0x6F27300B, //  Licensed PAL\Teenage Mutant Ninja Turtles.nes 
    0xBF700470, //  Licensed PAL\The Addams Family.nes 
    0x68CF9B78, //  Licensed PAL\The Addams Family꞉ Fester's Quest (rev0).nes 
    0x8B2694BD, //  Licensed PAL\The Addams Family꞉ Fester's Quest (rev1).nes 
    0x9632C470, //  Licensed PAL\The Addams Family꞉ Pugsley's Scavenger Hunt.nes 
    0xBC9BFFCB, //  Licensed PAL\The Adventures of Bayou Billy.nes 
    0xA31142FF, //  Licensed PAL\The Adventures of Rad Gravity.nes 
    0xFD7E9A7E, //  Licensed PAL\The Legend of Prince Valiant.nes 
    0xDE7E4629, //  Licensed PAL\The Simpsons꞉ Bart vs. the Space Mutants.nes 
    0xC53CF1D0, //  Licensed PAL\Track &amp; Field II.nes 
    0xD9323EE6, //  Licensed PAL\Willow.nes 
    0x06689AA4, //  Licensed PAL\Wrath of the Black Manta.nes 
    0x80107F84, //  Modern\Virtual Console\Teenage Mutant Hero Turtles.nes 
    0x99C88648, //  Unreleased\International Cricket.nes 
    0x23BEFF5E, //  Unreleased\Parasol Stars꞉ The Story of Bubble Bobble 3.nes 
    0xC8F203F9, //  Unreleased\The Addams Family꞉ Pugsley's Scavenger Hunt.nes 
/**** 83 mapper=1 prgrom=128K chrrom=128K mirror=H system=SYSTEM_NTSC */
    0xE7882A07, //  Homebrew\Journey.nes 
    0x6F7649BD, //  Modern\Virtual Console\Double Dragon.nes 
/**** 84 mapper=1 prgrom=128K prgram=8K chrrom=128K mirror=H */
    0x491EE6E7, //  Compatibility Hacks\闘将!! 拉麺男꞉ 炸裂超人102芸 [m001].nes 
    0x14A8E405, //  Homebrew\Translations\English\Battle Storm [MrRichard999].nes 
    0x8E373118, //  Licensed Japan\Battle Storm.nes 
    0xA6648353, //  Licensed Japan\Conflict.nes 
    0x149C0EC3, //  Licensed Japan\Famicom 将棋꞉ 竜王戦.nes 
    0xD6FE9826, //  Licensed Japan\Viva! Las Vegas.nes 
    0x828F8F1F, //  Licensed Japan\戦車戦略꞉ 砂漠の狐.nes 
    0x3E785DC3, //  Licensed Japan\空中要塞 Air Fortress.nes 
    0x8D5B77C0, //  Licensed Japan\銀河英雄伝説꞉ わが征くは星の大海.nes 
    0x32CF4307, //  Licensed North America\Conflict.nes 
    0x68EC97CB, //  Licensed North America\Desert Commander.nes 
    0xC973699D, //  Licensed North America\Vegas Dream.nes 
    0xD624E908, //  Playchoice\Baseball Stars.nes 
    0x6866A989, //  Samples\Scarabeus.nes 
    0x2C5FAC1C, //  Unreleased\Famicom 将棋꞉ 竜王戦.nes 
/**** 85 mapper=1 prgrom=256K chrram=8K mirror=H */
    0xBACE1014, //  Bad Dumps\Rockman Peercast 2.nes 
    0x9CCBEC9F, //  Homebrew\Hacks\Bionic Commando - Winter Edition (v0.1)[Matrixz].nes 
    0x6D10930F, //  Homebrew\Hacks\Bionic Commando 99 [Chris Covell].nes 
    0x068A987E, //  Homebrew\Hacks\Faxanadu Facelift [Tek Hacks].nes 
    0x33DF4C6C, //  Homebrew\Hacks\Faxanadu Uncensored (v1.0b)[Safaribans].nes 
    0x94FF5564, //  Homebrew\Hacks\Mega Man series\Cutman's Bad Scizzors Day [Jason Godman Hayes].nes 
    0x5D099A2E, //  Homebrew\Hacks\Mega Man series\Mega Man 2꞉ The Revenge of the Eight Robot Masters [Durfarc].nes 
    0x80A34090, //  Homebrew\Hacks\Mega Man series\Mega Man II - Speed Patches [StalkerMaestro].nes 
    0xA9C4F91C, //  Homebrew\Hacks\Mega Man series\Mega Man II Remix [Kenclops, はか世].nes 
    0xAF74456E, //  Homebrew\Hacks\Mega Man series\Mega Man II Turbo꞉ Wily's Conquest - Hyper Edition (v1.12) [8-bit fan].nes 
    0xCC26DB05, //  Homebrew\Hacks\Mega Man series\Mega Man NEO.nes 
    0x9CE644D4, //  Homebrew\Hacks\Mega Man series\Mega Man Ultra [Infidelity].nes 
    0x01EC1E1F, //  Homebrew\Hacks\Mega Man series\Rockman 2 - Another [AYCNN].nes 
    0x73792404, //  Homebrew\Hacks\Mega Man series\Rockman 2 Dash+꞉ Dr. Wilyの謎 [Small Apple].nes 
    0x369322E4, //  Homebrew\Hacks\Mega Man series\Rockman 2 Dash꞉ Dr. Wilyの謎 [Small Apple].nes 
    0xCA644C8D, //  Homebrew\Hacks\Mega Man series\Rockman 2 Function 9 ꞉-Gray Zone [Wined].nes 
    0x7286F404, //  Homebrew\Hacks\Mega Man series\Rockman 2 Plus! (alt)[AYCNN].nes 
    0x02CE5DBF, //  Homebrew\Hacks\Mega Man series\Rockman 2 Plus! [AYCNN].nes 
    0xEB74365E, //  Homebrew\Hacks\Mega Man series\Rockman 2nd꞉ Dr. Wily 再び [K-Game].nes 
    0x5C48A631, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Deus Ex Machina (easy)[Misty].nes 
    0x4A0BF881, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Deus Ex Machina (hard)[Misty].nes 
    0x70D5D8AA, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Deus Ex Machina (normal)[Misty].nes 
    0xEBF957E5, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Dr. Wilyの謎 (v1.1b)[Krizal].nes 
    0x08FA5A4E, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Dr. Wilyの謎 - Easy.nes 
    0x58F3D578, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Dr. Wilyの謎 - Endless.nes 
    0x0271A341, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Dr. Wilyの謎 - Old.nes 
    0x9D284DB6, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Min [Tar].nes 
    0xE1FB5E95, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Wed of Slasher.nes 
    0x5FE24EE3, //  Homebrew\Hacks\Mega Man series\Rockman 2꞉ Yendor Code (v1.02).nes 
    0x7FAA440F, //  Homebrew\Hacks\Mega Man series\Rockman Exhaust [DHI].nes 
    0x94B126DA, //  Homebrew\Hacks\Mega Man series\Rockman II Deluxe Ver [Tarte].nes 
    0x9EB6C790, //  Homebrew\Hacks\Mega Man series\Rockman L꞉ Dr. Wilyの悪夢 (easy)[Myrtos].nes 
    0xF3BE760F, //  Homebrew\Hacks\Mega Man series\Rockman L꞉ Dr. Wilyの悪夢 [Myrtos].nes 
    0x1F117D2E, //  Homebrew\Hacks\Mega Man series\Rockman No Constancy [Ika].nes 
    0x07F76EB8, //  Homebrew\Hacks\Mega Man series\Rockman P꞉ Peercast (v1.2)[Nekoronda].nes 
    0xE5261260, //  Homebrew\Hacks\Mega Man series\Rockman Unprecedented [DHI].nes 
    0x249E64D0, //  Homebrew\Hacks\Mega Man series\Rockman Z꞉ Dr. Wilyの謎 [Kuwata].nes 
    0xDB233C11, //  Homebrew\Translations\English\Ankoku Shinwa꞉ The Dark Myth [Pacnsacdave].nes 
    0x426B8C70, //  Homebrew\Translations\English\Ankoku Shinwa꞉ The Dark Myth [Proveaux].nes 
    0x5307CF09, //  Homebrew\Translations\English\Ankoku Shinwa꞉ The Dark Myth [snark].nes 
    0xF7A632C3, //  Homebrew\Translations\English\Mega Man 2 [Aeon Genesis, DarthVaderX v1.0a].nes 
    0x75B2DE15, //  Homebrew\Translations\English\Mega Man 2꞉ Dr. Wily's Riddle [Aeon Genesis, DarthVaderX v1.0a].nes 
    0x1A71FD06, //  Homebrew\Translations\English\Peacock King [Snark v1.1].nes 
    0x001CA5A5, //  Homebrew\Translations\English\Ripple Island [Supper v1.00].nes 
    0xF52C67D7, //  Homebrew\Translations\English\Rockman 2꞉ Dr. Wily's Riddle [Aeon Genesis].nes 
    0x73E41AC7, //  Licensed Japan\100万 Dollar Kid꞉ 幻の帝王編.nes 
    0x3F56A392, //  Licensed Japan\Captain Ed.nes 
    0xA80FA181, //  Licensed Japan\Faxanadu.nes 
    0x1545BD13, //  Licensed Japan\Gimmi a Break꞉ 史上最強のQuiz 王決定戦 2.nes 
    0x0DC53188, //  Licensed Japan\Jesus꞉ 恐怖のBio Monster.nes 
    0x86759C0F, //  Licensed Japan\Monopoly.nes 
    0x6150517C, //  Licensed Japan\Rockman 2꞉ Dr. Wilyの謎.nes 
    0x016C93D8, //  Licensed Japan\Saladの国のTomato姫.nes 
    0x291BCD7D, //  Licensed Japan\パチ夫くん 2.nes 
    0x992AF039, //  Licensed Japan\北海道連鎖殺人꞉ Okhotskに消ゆ.nes 
    0x0A73A792, //  Licensed Japan\名探偵 Holmes꞉ M からの挑戦状.nes 
    0x1A7E97ED, //  Licensed Japan\名探偵 Holmes꞉ 霧のLondon 殺人事件.nes 
    0x50CCDA33, //  Licensed Japan\太陽の神殿꞉ Aztec 2.nes 
    0x71C9ED1E, //  Licensed Japan\孔雀王.nes 
    0xD2BC86F3, //  Licensed Japan\怒 II꞉ 怒号層圏.nes 
    0x644E312B, //  Licensed Japan\暗黒神話꞉ ヤマトタケル伝説.nes 
    0xB5576820, //  Licensed Japan\桃太郎伝説 - Peach Boy Legend.nes 
    0x09C1FC7D, //  Licensed Japan\水島新司の大甲子園.nes 
    0xBA58ED29, //  Licensed Japan\神宮館 '89 神宮館 '89 電脳九星占い.nes 
    0xF568A7A4, //  Licensed Japan\第一生命 Famicom Land꞉ Family School.nes 
    0x0021ED29, //  Licensed Japan\霊幻道士.nes 
    0x27C16011, //  Licensed Japan\馬券必勝学꞉ Gate In.nes 
    0x2C2DDFB4, //  Licensed North America\Battle Chess.nes 
    0xD2574720, //  Licensed North America\Bionic Commando.nes 
    0x28FB71AE, //  Licensed North America\Defender of the Crown.nes 
    0x57DD23D1, //  Licensed North America\Faxanadu (rev0).nes 
    0xE71DB268, //  Licensed North America\Faxanadu (rev1).nes 
    0x4F467410, //  Licensed North America\Ikari Warriors II꞉ Victory Road.nes 
    0x8BCA5146, //  Licensed North America\Indiana Jones and the Last Crusade.nes 
    0x8192D2E7, //  Licensed North America\Kid Niki꞉ Radical Ninja (rev0).nes 
    0xA9415562, //  Licensed North America\Kid Niki꞉ Radical Ninja (rev1).nes 
    0x0FCFC04D, //  Licensed North America\Mega Man II.nes 
    0xCC37094C, //  Licensed North America\Phantom Fighter.nes 
    0x56756615, //  Licensed North America\Princess Tomato in Salad Kingdom.nes 
    0x86B0D1CF, //  Licensed North America\Robin Hood꞉ Prince of Thieves (rev0).nes 
    0xC3CCC493, //  Licensed North America\Robin Hood꞉ Prince of Thieves (rev1).nes 
    0x67F77118, //  Licensed North America\Rocket Ranger.nes 
    0x2220E14A, //  Licensed North America\Space Shuttle Project.nes 
    0x02EE3706, //  Licensed North America\Strider Hiryu.nes 
    0x1D6DECCC, //  Licensed North America\The Rocketeer.nes 
    0x50D296B3, //  Licensed North America\Tombs and Treasure.nes 
    0x5EC886A4, //  Modern\Capcom Classics꞉ Mini Mix\Strider 飛竜.nes 
    0xE91ECC29, //  Modern\Hudson Best Collection\Saladの国のTomato姫.nes 
    0xCF5DE2BC, //  Modern\Mega Man II (Mega Man Legacy Collection).nes 
    0xE2287991, //  Modern\Virtual Console\Mega Man II (NTSC).nes 
    0x8C3022E4, //  Modern\Virtual Console\Princess Tomato in Salad Kingdom.nes 
    0xFBCF0690, //  Modern\Virtual Console\Rockman 2꞉ Dr. Wilyの謎.nes 
    0x663476B9, //  Modern\Virtual Console\Saladの国のTomato姫.nes 
    0x11C9AC37, //  Unreleased\Princess Tomato in Salad Kingdom.nes 
    0x153BA1BC, //  Unreleased\Robin Hood꞉ Prince of Thieves.nes 
    0x367566CE, //  Unreleased\Strider 飛竜.nes 
/**** 86 mapper=1 prgrom=256K chrram=8K mirror=H system=SYSTEM_PAL */
    0xFA7EE642, //  Licensed PAL\Bionic Commando.nes 
    0x68F9B5F5, //  Licensed PAL\Defender of the Crown (English).nes 
    0x2FD2E632, //  Licensed PAL\Defender of the Crown (French).nes 
    0x76C161E3, //  Licensed PAL\Faxanadu.nes 
    0xA6638CBA, //  Licensed PAL\Mega Man II.nes 
    0xB67D16F6, //  Licensed PAL\Robin Hood꞉ Prince of Thieves (English).nes 
    0xD49DCA84, //  Licensed PAL\Robin Hood꞉ Prince of Thieves (German).nes 
    0xFD21F54D, //  Licensed PAL\Robin Hood꞉ Prince of Thieves (Spanish).nes 
    0x0150E2EF, //  Modern\Virtual Console\Mega Man II (PAL).nes 
/**** 87 mapper=1 prgrom=256K prgram=8K chrram=8K mirror=H */
    0x091A6A4C, //  Bad Dumps\Mario's Open Golf [missing PROM].nes 
    0xD769973B, //  Homebrew\Demos\Shortnes(e) [Edison].nes 
    0xFD60875D, //  Homebrew\Translations\English\Bionic Commando꞉ Return of Hitler [Stardust Crusaders v1.03].nes 
    0x00737729, //  Homebrew\Translations\English\Bionic Commando꞉ Return of Hitler [Stardust Crusaders v1.04].nes 
    0x2E2ACAE9, //  Licensed Japan\Gambler 自己中心派꞉ 麻雀 Game.nes 
    0x16A0A3A3, //  Licensed Japan\Top Secret꞉ Hitlerの復活.nes 
    0x7B44FB2A, //  Licensed Japan\井出洋介名人の実戦麻雀 II.nes 
    0xF540474E, //  Modern\Capcom Classics꞉ Mini Mix\Bionic Commando.nes 
    0x3125DB22, //  Playchoice\Mario's Open Golf.nes 
    0x10C06E27, //  Unreleased\Chuck Yeager's Fighter Combat.nes 
    0x01F63D1A, //  Unreleased\Top Secret꞉ Hitlerの復活.nes 
/**** 88 mapper=1 prgrom=256K chrrom=8K mirror=H */
    0xD73F6974, //  Homebrew\Nm vs. Ji (Demo).nes 
    0x63EE59F8, //  Homebrew\Nm vs. Ji (WIP).nes 
/**** 89 mapper=1 prgrom=256K chrrom=8K mirror=H system=SYSTEM_PAL */
    0x860D29DF, //  Homebrew\Demos\Mom, My Ears are Bleeding!.nes 
/**** 90 mapper=1 prgrom=256K chrrom=16K mirror=H system=SYSTEM_NTSC */
    0xC1EEA751, //  Homebrew\Dizzy Sheep Disaster.nes 
    0x33F84EB9, //  Homebrew\Missing Lands.nes 
/**** 91 mapper=1 prgrom=256K chrrom=64K mirror=H */
    0xD28BEE7F, //  Homebrew\Hacks\Bases Loaded꞉ Nippon Pro Baseball 2007.nes 
    0xB4241FCC, //  Licensed North America\Bases Loaded (rev0).nes 
    0xDAEE19F2, //  Licensed North America\Bases Loaded (rev1).nes 
    0x6ABAD366, //  Licensed North America\Bases Loaded (rev2).nes 
    0xF6A889D6, //  Playchoice\Bases Loaded.nes 
/**** 92 mapper=1 prgrom=256K prgram=8K chrrom=64K mirror=H */
    0x92C138E4, //  Licensed North America\The Miracle Piano Teaching System.nes 
/**** 93 mapper=1 prgrom=256K prgram=8K chrrom=64K mirror=H system=SYSTEM_PAL */
    0x9DF58E80, //  Licensed PAL\The Miracle Piano Teaching System (French).nes 
    0xB3974D6C, //  Licensed PAL\The Miracle Piano Teaching System (German).nes 
/**** 94 mapper=1 prgrom=256K chrrom=128K mirror=H */
    0x2E50EE0E, //  Homebrew\Translations\English\Final Mission [Stardust Crusaders v1.01].nes 
    0x7E59DC0E, //  Homebrew\Translations\English\Grand Master [Aishsha, Stardust Crusaders v1.01].nes 
    0xF3ED81EB, //  Homebrew\Translations\English\Idol Hakkenden [Ccmar].nes 
    0xC060ED0A, //  Licensed Japan\Grand Master.nes 
    0x0DA00298, //  Licensed Japan\御存知꞉ 弥次喜多珍道中.nes 
    0xB9CF171F, //  Licensed North America\Bases Loaded II꞉ Second Season.nes 
    0xE145B441, //  Licensed North America\Day Dreamin' Davey.nes 
    0x84148F73, //  Licensed North America\Goal!.nes 
    0xECCD4089, //  Licensed North America\Racket Attack.nes 
    0xFDE1C7ED, //  Licensed North America\Sesame Street꞉ Big Bird's Hide &amp; Speak.nes 
    0x06961BE4, //  Licensed North America\Skate or Die 2꞉ The Search for Double Trouble.nes 
/**** 95 mapper=1 prgrom=256K chrrom=128K mirror=H system=SYSTEM_PAL */
    0xC0103592, //  Licensed PAL\Goal!.nes 
    0xD72560E1, //  Licensed PAL\Racket Attack.nes 
/**** 96 mapper=1 prgrom=256K prgram=8K chrrom=128K mirror=H */
    0x34B5723A, //  Homebrew\Translations\English\Tank Commander꞉ Desert Fox [Stardust Crusaders].nes 
    0xDF1A5080, //  Modern\Pyramids of Ra.nes 
/**** 97 mapper=1 prgrom=512K chrram=8K mirror=H */
    0x821A98F4, //  Homebrew\Multicarts\MottZilla 11-in-1 Version 1.nes 
    0x00763EF6, //  Homebrew\Multicarts\MottZilla 11-in-1 Version 2.nes 
    0x27F01BF2, //  Homebrew\Tools\MottZilla 11-in-1.nes 
    0xC89258B0, //  Homebrew\Translations\English\Jesus꞉ Tale of the Dreadful Bio-Monster [Stardust Crusaders 3 Days Later Edition Rev 2].nes 
    0xD379050A, //  Homebrew\Translations\English\Princess Tomato in the Salad Kingdom [Stardust Crusaders v1.02].nes 
    0xFD7BB2FF, //  Homebrew\Translations\English\The Legend of Peach Boy [KingMike's Translations].nes 
/**** 98 mapper=1 prgrom=512K chrram=8K mirror=H system=SYSTEM_NTSC */
    0x8C0B332C, //  Homebrew\What Remains.nes 
    0xC19DA17F, //  Modern\What Remains.nes 
/**** 99 mapper=1 prgrom=512K prgram=8K chrram=8K mirror=H */
    0xD40CC6F5, //  Homebrew\Multicarts\MottZilla 11-in-1 with Guns.nes 
/**** 100 mapper=1 prgrom=512K prgram=32K chrram=8K mirror=H */
    0x696712F9, //  BIOS\Famicom Titler.nes 
/**** 101 mapper=1 prgrom=32K prgnvram=8K chrram=8K mirror=H battery=1 */
    0x0D5CCE53, //  Homebrew\Tools\Graphics Editor for NES.nes 
/**** 102 mapper=1 prgrom=32K prgnvram=8K chrrom=8K mirror=H battery=1 */
    0xB4E6DD24, //  Homebrew\Tools\Drum Edit.nes 
/**** 103 mapper=1 prgrom=32K prgnvram=8K chrrom=16K mirror=H battery=1 */
    0x8B7DA8B8, //  Compatibility Hacks\Vs. Duck Hunt [home console v2.1].nes 
    0x8BBDFB16, //  Homebrew\Pitfall! The Unofficial Adventure (v0.83.0).nes 
    0x9B208AB1, //  Licensed Japan\囲碁꞉ 九路盤対局.nes 
/**** 104 mapper=1 prgrom=32K prgnvram=8K chrrom=32K mirror=H battery=1 */
    0x7F8C8232, //  Compatibility Hacks\Vs. Super Mario Bros. [home console v1.0].nes 
    0x1D664A65, //  Compatibility Hacks\Vs. Super Mario Bros. [home console v1.1].nes 
    0xB3EEC419, //  Homebrew\Translations\English\Go 9 Row Boardgame [Helly, MrRichard999, Proveaux v1.0].nes 
/**** 105 mapper=1 prgrom=32K prgnvram=8K chrrom=64K mirror=H battery=1 */
    0xE149E0B2, //  Maintenance\NTF2 System.nes 
/**** 106 mapper=1 prgrom=64K prgnvram=8K chrrom=8K mirror=H battery=1 */
    0x5DF5AB57, //  Homebrew\Pitfall! The Unofficial Adventure (v0.92.3).nes 
/**** 107 mapper=1 prgrom=64K prgnvram=8K chrrom=16K mirror=H battery=1 */
    0xD6FF1625, //  Homebrew\Hacks\Mario Warrior.nes 
    0xFB3C0671, //  Homebrew\Hacks\S.M. Remix II꞉ Bowser Has Kinopio!! [Omniverse].nes 
    0x5184A320, //  Homebrew\Pitfall! The Unofficial Adventure (v0.93.0).nes 
    0x3B3F88F0, //  Licensed North America\Dragon Warrior (rev0).nes 
    0x2545214C, //  Licensed North America\Dragon Warrior (rev1).nes 
/**** 108 mapper=1 prgrom=64K prgnvram=8K chrrom=32K mirror=H battery=1 */
    0x8828B2E0, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Special (New Color Palette+Highscore Save).nes 
    0xA471C585, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Special (Old Color Palette+Highscore Save).nes 
/**** 109 mapper=1 prgrom=128K prgnvram=8K chrram=8K mirror=H battery=1 */
    0xB53EB795, //  Homebrew\Hacks\The Battle of Olympus (SRAM+Rebalanced) [8-bit fan].nes 
    0x8FDA652A, //  Homebrew\Hacks\Zelda series\Blink 4꞉ Escape from Nintendoland [ROMs and Robots].nes 
    0x5A6E8F21, //  Homebrew\Hacks\Zelda series\Fall of the Moon [Imperial].nes 
    0x668855D6, //  Homebrew\Tools\NTRQ - NES Music Tracker.nes 
    0xD68A6F33, //  Licensed Japan\Dungeon Kid.nes 
    0x174F860A, //  Licensed Japan\Indoraの光.nes 
    0x3FF44F87, //  Licensed Japan\Tetris 2+Bombliss (rev0).nes 
    0xFD45E9C1, //  Licensed Japan\Tetris 2+Bombliss (rev1).nes 
    0x7AE0BF3C, //  Licensed Japan\Zeldaの伝説 1꞉ The Hyrule Fantasy.nes 
    0xCD7A2FD7, //  Licensed Japan\半熟英雄.nes 
    0x44B060DA, //  Licensed Japan\将軍.nes 
    0x34DEBDFD, //  Licensed Japan\田村光昭の麻雀 Seminar.nes 
    0xE63D9193, //  Licensed Japan\谷川浩司の将棋指南 III.nes 
    0x3836EEAC, //  Licensed Japan\谷川浩司の将棋指南 II꞉ 名人への道.nes 
    0xF714FAE3, //  Licensed Japan\麻雀大会.nes 
    0x1352F1B9, //  Licensed North America\Greg Norman's Golf Power.nes 
    0x9B821A83, //  Licensed North America\The Bard's Tale꞉ Tales of the Unknown.nes 
    0x3FE272FB, //  Licensed North America\The Legend of Zelda (rev0).nes 
    0xEAF7ED72, //  Licensed North America\The Legend of Zelda (rev1).nes 
    0x46E0D37D, //  Modern\GameCube Edition\The Legend of Zelda (rev0).nes 
    0x34540318, //  Modern\GameCube Edition\The Legend of Zelda (rev1).nes 
    0xE50CC16A, //  Modern\Virtual Console\The Legend of Zelda (NTSC).nes 
    0xCEB4D8FF, //  Modern\Virtual Console\半熟英雄.nes 
    0x4ED5AA56, //  Unreleased\The Bard's Tale꞉ Tales of the Unknown (North America Beta 1).nes 
    0xFA434E09, //  Unreleased\The Bard's Tale꞉ Tales of the Unknown (North America Beta 2).nes 
/**** 110 mapper=1 prgrom=128K prgnvram=8K chrram=8K mirror=H battery=1 system=SYSTEM_PAL */
    0xBC7485B5, //  Compatibility Hacks\Elite [iNES emulator].nes 
    0xA4BDCC1D, //  Licensed PAL\Elite.nes 
    0xED7F5555, //  Licensed PAL\The Legend of Zelda (rev0).nes 
    0xD44B412E, //  Licensed PAL\The Legend of Zelda (rev1).nes 
    0x6A74FF62, //  Modern\Virtual Console\The Legend of Zelda (PAL).nes 
/**** 111 mapper=1 prgrom=128K prgnvram=32K chrram=8K mirror=H battery=1 */
    0x8185FFD0, //  Homebrew\Demos\Pulsar.nes 
    0x7CF5EFA9, //  Homebrew\Tools\Pr8 Nes Drum Synth.nes 
    0x70636601, //  Homebrew\Translations\English\Dezaemon [Aeon Genesis v1.1].nes 
    0xD1E50064, //  Licensed Japan\Game Designer 養成 Soft꞉ 絵描衛門.nes 
/**** 112 mapper=1 prgrom=128K prgnvram=8K chrrom=8K mirror=H battery=1 */
    0x9E356267, //  Licensed Japan\森田和郎の将棋.nes 
/**** 113 mapper=1 prgrom=128K prgnvram=8K chrrom=16K mirror=H battery=1 */
    0x77917127, //  Compatibility Hacks\Rainbow Brite꞉ Journey to Rainbow Land (cartridge)[m001 extract].nes 
    0xC88CDDCD, //  Compatibility Hacks\Rainbow Brite꞉ Journey to Rainbow Land (mini-arcade)[m001 extract].nes 
/**** 114 mapper=1 prgrom=128K prgram=8K prgnvram=8K chrrom=16K mirror=H battery=1 */
    0xC6F25A20, //  Homebrew\Translations\English\A-Train [MrRichard999, Proveaux, AgentOrange, Jink640, Rotwang v0.99].nes 
    0x6377CB75, //  Licensed Japan\A列車で行こう.nes 
/**** 115 mapper=1 prgrom=128K prgnvram=8K chrrom=32K mirror=H battery=1 */
    0x6E6993BF, //  Compatibility Hacks\The Money Game [m001].nes 
    0xF5C90AAC, //  Homebrew\Brony Blaster (2015).nes 
    0x5EB8029C, //  Homebrew\Translations\English\Artelius [MageCraft Translations].nes 
    0xE784989E, //  Homebrew\Translations\English\大戦略 [Aeon Genesis].nes 
    0x093E845F, //  Licensed Japan\Artelius.nes 
    0xE19293A2, //  Licensed Japan\Best Play Pro 野球 (新 Data).nes 
    0x3BBFF3A6, //  Licensed Japan\Best Play Pro 野球.nes 
    0x2470402B, //  Licensed Japan\Famicom 囲碁入門꞉ 日本棋院推篇.nes 
    0x8DCD9486, //  Licensed Japan\Jumbo 尾崎のHole-in-One Professional.nes 
    0x5A5A0CD9, //  Licensed Japan\大戦略.nes 
/**** 116 mapper=1 prgrom=128K prgnvram=8K chrrom=112K mirror=H battery=1 */
    0x5EEA106C, //  Homebrew\Hacks\Zelda series\Evil Dead [K. Voorhees].nes 
    0x895821C5, //  Homebrew\Hacks\Zelda series\Zelda III꞉ The Adventure of Link [Nulear Sonar].nes 
    0xE074FB3E, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link 1999 [Imperial].nes 
/**** 117 mapper=1 prgrom=128K chrrom=128K mirror=H battery=1 system=SYSTEM_NTSC */
    0x94DF7AA3, //  Modern\Alfonzo's Arctic Adventure.nes 
    0x15DBF7C8, //  Samples\Alfonzo's Arctic Adventure.nes 
/**** 118 mapper=1 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xE0D26D8C, //  Homebrew\Hacks\Baseball Stars 2002.nes 
    0xE56693DA, //  Homebrew\Hacks\Zelda series\The Legend of Zelda꞉ Leaf of Inertia (easy with jump)[Ice Penguin].nes 
    0x238C5D7A, //  Homebrew\Hacks\Zelda series\The Legend of Zelda꞉ Leaf of Inertia (easy)[Ice Penguin].nes 
    0x0373AA21, //  Homebrew\Hacks\Zelda series\The Legend of Zelda꞉ Leaf of Inertia (hard)[Ice Penguin].nes 
    0x5F31C2BF, //  Homebrew\Hacks\Zelda series\Zelda 2.5 [Hax0r Kyo].nes 
    0x023F36C6, //  Homebrew\Hacks\Zelda series\Zelda II꞉ New Adventure of Link (noob mode)(v4.2)[HollowShadow].nes 
    0x84C61649, //  Homebrew\Hacks\Zelda series\Zelda II꞉ New Adventure of Link (one-hit death)(v4.2)[HollowShadow].nes 
    0xBFAD7E0F, //  Homebrew\Hacks\Zelda series\Zelda II꞉ New Adventure of Link (recommended mode)(v4.2)[HollowShadow].nes 
    0x000B7B60, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Error [DT].nes 
    0x7827B77F, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Lady Link [ArchAngelleofJustice].nes 
    0x4B039FD0, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link (boss endurance)[njosro].nes 
    0xF3821B6C, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link (new Link sprites)[Falchion22].nes 
    0x78E8CDE1, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link (restart from palace at Game Over) [njosro].nes 
    0x15DDAD01, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link - Refresh [LastduaL].nes 
    0x8DA0256F, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link - Rev Edition [Revility].nes 
    0x63667A83, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link Part 2 (easy) [Doug C.].nes 
    0xBC53885D, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link Part 2 (hard) [Doug C.].nes 
    0x9F7047D5, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link Part 2 (medium) [Doug C.].nes 
    0xB28D8BE3, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link Part 3 (easy)[Doug C.].nes 
    0xBBA56C07, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Adventure of Link Part 3 (hard)[Doug C.].nes 
    0x14DCA057, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Nightmare of Ganon (v1.5)[HollowShadow].nes 
    0x9FE165F2, //  Homebrew\Hacks\Zelda series\Zelda II꞉ The Triforce of Courage [ultimaweapon].nes 
    0xEC259856, //  Homebrew\Translations\English\Advanced Dungeons &amp; Dragons꞉ Dragons of Flame [DvD Translations].nes 
    0xADA1F73C, //  Homebrew\Translations\English\Adventures of Musashi [Gaijin Productions &amp; MadHacker, Pacnsacdave].nes 
    0x11C1F686, //  Homebrew\Translations\English\America 大統領選挙 [Hanage v0.8].nes 
    0x5CB2C610, //  Homebrew\Translations\English\Castlevania II꞉ Simon's Quest [Bisqwit v2.9.8.19].nes 
    0x605F7CDD, //  Homebrew\Translations\English\Cosmic Wars [TransBRC v0.993].nes 
    0x16F6BC65, //  Homebrew\Translations\English\Cosmo Police Galivan [Jair].nes 
    0x3C934B25, //  Homebrew\Translations\English\Elysion [Tenshigami v0.92](SRAM).nes 
    0x4E8D76D2, //  Homebrew\Translations\English\Emoto's Pro Baseball꞉ 10x More Fun! [MrRichard999 v0.95].nes 
    0x012E12E3, //  Homebrew\Translations\English\Fortune Street [RType88 v0.1].nes 
    0x392E268C, //  Homebrew\Translations\English\Fortune Street꞉ Stop at My Shop [Eric Engel, MrRichard999 v0.10b].nes 
    0xE902746D, //  Homebrew\Translations\English\Gunhed꞉ The New Battle [Filler].nes 
    0xD9CBC985, //  Homebrew\Translations\English\Peacock King II [Snark].nes 
    0x0C504EF5, //  Homebrew\Translations\English\Sted꞉ Starfield of Memorable Relics [T-En J2e Translations, KingMike's Translations].nes 
    0x07964C82, //  Homebrew\Translations\English\Sted꞉ Starfield of Memorable Relics [T-En J2e Translations].nes 
    0x7851B794, //  Homebrew\Translations\English\ムサシの冒険 [Gaijin Productions &amp; MadHacker v0.98].nes 
    0xE616FF0A, //  Licensed Japan\Advanced Dungeons &amp; Dragons꞉ Dragons of Flame.nes 
    0x1E472E7A, //  Licensed Japan\Advanced Dungeons &amp; Dragons꞉ Heroes of the Lance.nes 
    0x9273F18E, //  Licensed Japan\America 大統領選挙.nes 
    0xAE5C3D94, //  Licensed Japan\Baseball Star꞉ めざせ三冠王!!.nes 
    0x61A852EA, //  Licensed Japan\Battle Stadium꞉ 選抜 Pro 野球.nes 
    0x2BFB1186, //  Licensed Japan\Be-Bop-Highschool꞉ 高校生極楽伝説.nes 
    0xA1A33B85, //  Licensed Japan\Best 競馬꞉ Derby Stallion (rev0).nes 
    0x83EAF3B1, //  Licensed Japan\Best 競馬꞉ Derby Stallion (rev1).nes 
    0x9F2712DF, //  Licensed Japan\Cosmic Wars.nes 
    0xE53F7A55, //  Licensed Japan\Cosmo Police Galivan.nes 
    0x8C4D59D6, //  Licensed Japan\Derby Stallion꞉ 全国版.nes 
    0x459D0C2A, //  Licensed Japan\Dungeon &amp; Magic꞉ Swords of Element.nes 
    0x9D45D8EC, //  Licensed Japan\Emoやんの10倍 Pro 野球꞉ The League 編.nes 
    0x9CFA55E7, //  Licensed Japan\Famicom 名人戦 (rev0).nes 
    0x821F2F9F, //  Licensed Japan\Famicom 名人戦 (rev1).nes 
    0xD0A9F4E1, //  Licensed Japan\Famicom 野球盤.nes 
    0xF885D931, //  Licensed Japan\Faria꞉ 封印の剣.nes 
    0xE8BAA782, //  Licensed Japan\Gun Hed꞉ 新たなる戦い.nes 
    0x3057B904, //  Licensed Japan\Home Run Nighter꞉ Pennant League!!.nes 
    0xF7E07B83, //  Licensed Japan\Moulin Rouge 戦記꞉ Melvilleの炎.nes 
    0xF19A11AF, //  Licensed Japan\STED꞉ 遺跡惑星の野望.nes 
    0x850090BC, //  Licensed Japan\White Lion 伝説꞉ Pyramidの彼方に.nes 
    0x498187B6, //  Licensed Japan\Wizardry꞉ Proving Grounds of the Mad Overlord.nes 
    0xB5F7E661, //  Licensed Japan\World Grand-Prix꞉ Pole to Finish.nes 
    0xCA503F32, //  Licensed Japan\いただき Street꞉ 私のお店によってって.nes 
    0xE24483B1, //  Licensed Japan\おぼっちゃまくん.nes 
    0x972D08C5, //  Licensed Japan\つっぱり Wars.nes 
    0x70F67AB7, //  Licensed Japan\ムサシの冒険.nes 
    0xA4DCDF28, //  Licensed Japan\孔雀王 II.nes 
    0x83CB743F, //  Licensed Japan\甲子園.nes 
    0xDC1E07D2, //  Licensed Japan\百鬼夜行.nes 
    0x0902C8F0, //  Licensed Japan\真田十勇士.nes 
    0x078CED30, //  Licensed Japan\超人꞉ Ultra Baseball.nes 
    0x4C0E8BBB, //  Licensed Japan\里見八犬伝.nes 
    0xB17574F3, //  Licensed North America\Advanced Dungeons &amp; Dragons꞉ Heroes of the Lance.nes 
    0x5BC9D7A1, //  Licensed North America\Al Unser Jr. Turbo Racing.nes 
    0x1F6EA423, //  Licensed North America\Baseball Simulator 1.000.nes 
    0x40D159B6, //  Licensed North America\Baseball Stars.nes 
    0x23C3FB2D, //  Licensed North America\Dungeon Magic꞉ Sword of the Elements.nes 
    0x45F03D2E, //  Licensed North America\Faria꞉ A World of Mystery &amp; Danger!.nes 
    0x5A8B4DA8, //  Licensed North America\Formula One꞉ Built To Win.nes 
    0x04766130, //  Licensed North America\Legend of the Ghost Lion.nes 
    0x3D0996B2, //  Licensed North America\Sid Meier's Pirates!.nes 
    0xD9BB572C, //  Licensed North America\Wizardry꞉ Proving Grounds of the Mad Overlord.nes 
    0xBA322865, //  Licensed North America\Zelda II꞉ The Adventure of Link.nes 
    0x262F31AC, //  Modern\GameCube Edition\Zelda II꞉ The Adventure of Link.nes 
    0x93468390, //  Modern\Virtual Console\Baseball Simulator 1.000 (USA) (Virtual Console).nes 
    0xD25FC226, //  Modern\Virtual Console\Zelda II꞉ The Adventure of Link (NTSC).nes 
    0x607FD2AE, //  Modern\Virtual Console\超人꞉ Ultra Baseball.nes 
    0xC22F3E9F, //  Unreleased\Advanced Dungeons &amp; Dragons꞉ Heroes of the Lance.nes 
    0xA058219D, //  Unreleased\Taro's Quest.nes 
/**** 119 mapper=1 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 system=SYSTEM_PAL */
    0x441DE6D8, //  Licensed PAL\Sid Meier's Pirates! (English).nes 
    0x574E5F8B, //  Licensed PAL\Sid Meier's Pirates! (German).nes 
    0x4D1DF589, //  Licensed PAL\Turbo Racing.nes 
    0x47FD88CF, //  Licensed PAL\Zelda II꞉ The Adventure of Link (rev0).nes 
    0x97D52C06, //  Licensed PAL\Zelda II꞉ The Adventure of Link (rev1).nes 
    0x47B6A39F, //  Licensed PAL\Zelda II꞉ The Adventure of Link (rev2).nes 
    0x40AA120B, //  Modern\Virtual Console\Zelda II꞉ The Adventure of Link (PAL).nes 
/**** 120 mapper=1 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 system=SYSTEM_NTSC */
    0xB1384E6D, //  Modern\Witch n' Wiz.nes 
/**** 121 mapper=1 prgrom=256K prgnvram=8K chrram=8K mirror=H battery=1 */
    0x9BD01DD7, //  Homebrew\Bugfixes, Improvements\Ys [MakoKnight][screen Split].nes 
    0x30B827C1, //  Homebrew\Demos\Hellraiser.nes 
    0x46C47C6B, //  Homebrew\Hacks\Super Mario Quest [PR Translations].nes 
    0xCC3D6457, //  Homebrew\Multicarts\Double Crossing꞉ The Forbidden Four.nes 
    0xE6F7D3FB, //  Homebrew\Multicarts\Team Site.nes 
    0x54FB2AE9, //  Homebrew\Nova the Squirrel.nes 
    0xA5CA4232, //  Homebrew\Translations\English\Bloody Warriors꞉ Shan-Go Strikes Back [ded302, snark, BlackPaladin v0.99.9].nes 
    0xD3A10B38, //  Homebrew\Translations\English\Bloody Warriors꞉ シャンゴーの逆襲 [Dodgy Translations].nes 
    0xEEE7584A, //  Homebrew\Translations\English\Bloody Warriors꞉ シャンゴーの逆襲 [ded302, snark].nes 
    0x81293104, //  Homebrew\Translations\English\Cocoron [Akujin, Dynamic-Designs].nes 
    0x039426A8, //  Homebrew\Translations\English\Deep Dungeon IV꞉ The Black Sorcerer [KingMike].nes 
    0xCC6AD67B, //  Homebrew\Translations\English\Doraemon꞉ The Revenge of Giga Zombie [WakdHacks].nes 
    0x9F830358, //  Homebrew\Translations\English\Dragon Quest III [Spinner 8 and friends v0.0111].nes 
    0xEFA0A4E9, //  Homebrew\Translations\English\Final Fantasy (rev1)[Chaos Rush v1.3](Chaos Edition)(Chicago Font).nes 
    0x0E3A8F05, //  Homebrew\Translations\English\Final Fantasy (rev1)[Chaos Rush v1.3](Chaos Edition)(U.S. Font).nes 
    0xE15FF3BF, //  Homebrew\Translations\English\Final Fantasy (rev1)[Chaos Rush v1.3](Chaos Edition).nes 
    0xE7C29858, //  Homebrew\Translations\English\Final Fantasy (rev1)[Chaos Rush v1.3](Chicago Font).nes 
    0x0658B3B4, //  Homebrew\Translations\English\Final Fantasy (rev1)[Chaos Rush v1.3](U.S. Font).nes 
    0xE93DCF0E, //  Homebrew\Translations\English\Final Fantasy (rev1)[Chaos Rush v1.3].nes 
    0x3A34900D, //  Homebrew\Translations\English\Final Fantasy II [Chaos Rush].nes 
    0x54A01FA0, //  Homebrew\Translations\English\Final Fantasy II [Demiforce v1.03, Chaos Rush v2.2b](B Button Dash).nes 
    0x501AD3A8, //  Homebrew\Translations\English\Final Fantasy II [Demiforce v1.03, Chaos Rush v2.2b].nes 
    0x196BCE46, //  Homebrew\Translations\English\Final Fantasy II [Demiforce v1.03, Parasyte v1.0].nes 
    0xF683E0EC, //  Homebrew\Translations\English\Future Soldier Lios [Aeon Genesis].nes 
    0xE5C534B6, //  Homebrew\Translations\English\Mashin Hero Wataru Gaiden [Hubz].nes 
    0x860E3828, //  Homebrew\Translations\English\Moon Princess [Snark v0.99].nes 
    0xB3E2A73C, //  Homebrew\Translations\English\Square no Tom Sawyer [Dushbadge v2.0].nes 
    0x05A9D7A5, //  Homebrew\Translations\English\Square's Tom Sawyer.nes 
    0x6E781580, //  Homebrew\Translations\English\Sweet Home [Gaijin Productions &amp; Suicidal Translations].nes 
    0x8A20EC21, //  Homebrew\Translations\English\Sweet Home [TheSiege].nes 
    0xAB8DE091, //  Homebrew\Translations\English\The Glory of Heracles II꞉ Titans' Downfall [The Spoony Bard v1.31].nes 
    0x5EF30DD2, //  Homebrew\Translations\English\The Light of Indra [KingMike's Translations v1.0 Beta].nes 
    0xF4DE2E65, //  Homebrew\Translations\English\Ys [MakoKnight].nes 
    0xF32FAE0C, //  Homebrew\Translations\English\ビックリマン World꞉ 激闘聖戦士 [Tenshigami].nes 
    0x2C33161D, //  Licensed Japan\Advanced Dungeons &amp; Dragons꞉ Hillsfar.nes 
    0x974E8840, //  Licensed Japan\Best Play Pro 野球 '90.nes 
    0xC2EF3422, //  Licensed Japan\Best Play Pro 野球 II.nes 
    0x391AA1B8, //  Licensed Japan\Bloody Warriors꞉ シャンゴーの逆襲.nes 
    0x78C4460D, //  Licensed Japan\Cocoron.nes 
    0x73F7E5D8, //  Licensed Japan\Deep Dungeon III꞉ 勇士への旅.nes 
    0xFE364BE5, //  Licensed Japan\Deep Dungeon IV꞉ 黒の妖術師.nes 
    0xA49B48B8, //  Licensed Japan\Dragon Quest III꞉ そして伝説へ... (rev0).nes 
    0x869501CA, //  Licensed Japan\Dragon Quest III꞉ そして伝説へ... (rev1).nes 
    0x58507BC9, //  Licensed Japan\Famicom Top Management.nes 
    0x466EFDC2, //  Licensed Japan\Final Fantasy (rev0).nes 
    0xF71E7EDD, //  Licensed Japan\Final Fantasy (rev1).nes 
    0xD29DB3C7, //  Licensed Japan\Final Fantasy II.nes 
    0x85F12D37, //  Licensed Japan\Gambler 自己中心派 2.nes 
    0xBA766EC6, //  Licensed Japan\Heraklesの栄光 II꞉ Titanの滅亡.nes 
    0x46F30F2D, //  Licensed Japan\Maison 一刻.nes 
    0xBAEBA201, //  Licensed Japan\Mario Open Golf (rev0).nes 
    0xDB564628, //  Licensed Japan\Mario Open Golf (rev1).nes 
    0xFB1C0551, //  Licensed Japan\Monster Maker꞉ 7つの秘宝.nes 
    0x4640EBE0, //  Licensed Japan\Presidentの選択.nes 
    0xC9484BB3, //  Licensed Japan\Pro 野球 殺人事件!.nes 
    0xCB0A76B1, //  Licensed Japan\SquareのTom Sawyer.nes 
    0xDFC0CE21, //  Licensed Japan\Super Black Onyx.nes 
    0x09FFDF45, //  Licensed Japan\Super 桃太郎電鉄.nes 
    0x252FFD12, //  Licensed Japan\Sweet Home.nes 
    0xC4E1886F, //  Licensed Japan\The Bard's Tale II꞉ The Destiny Knight.nes 
    0x7EE02CA2, //  Licensed Japan\The Bard's Tale꞉ Tales of the Unknown.nes 
    0x250F7913, //  Licensed Japan\Ultima III꞉ 恐怖のExodus.nes 
    0x71D8C6E9, //  Licensed Japan\Ultima IV꞉ 聖者への道.nes 
    0x57E9B21C, //  Licensed Japan\Vegas Connection꞉ Casinoから愛をこめて.nes 
    0x92547F1C, //  Licensed Japan\Ys.nes 
    0x89E085FE, //  Licensed Japan\Zoids 2꞉ Zenebasの逆襲.nes 
    0x26CEC726, //  Licensed Japan\かぐや姫伝説.nes 
    0xD7215873, //  Licensed Japan\ドラえもん꞉ Gigazombieの逆襲.nes 
    0xC6224026, //  Licensed Japan\ビックリマン World꞉ 激闘聖戦士.nes 
    0x9EBDC94E, //  Licensed Japan\不如帰.nes 
    0x18D44BBA, //  Licensed Japan\井崎脩五郎の競馬必勝学.nes 
    0x771CE357, //  Licensed Japan\北斗の拳 3꞉ 新世紀創造 - 凄拳列伝.nes 
    0x63469396, //  Licensed Japan\北斗の拳 4꞉ 七星覇拳伝 - 北斗神拳の彼方へ.nes 
    0x1AC701B5, //  Licensed Japan\天地を喰らう (rev0).nes 
    0x637A7ACB, //  Licensed Japan\天地を喰らう (rev1).nes 
    0xC30C9EC9, //  Licensed Japan\小公子 Ceddie.nes 
    0x565A4681, //  Licensed Japan\必殺道場破り.nes 
    0x23E9C736, //  Licensed Japan\新 里見八犬伝꞉ 光と闇の戦い.nes 
    0xDA690D17, //  Licensed Japan\日本一の名監督.nes 
    0x66DD04E1, //  Licensed Japan\明治維新.nes 
    0xE94D5181, //  Licensed Japan\未来戦史 Lios.nes 
    0x1208E754, //  Licensed Japan\松本亨の株式必勝学 II.nes 
    0xABAA6F78, //  Licensed Japan\松本亨の株式必勝学.nes 
    0x7172F3D4, //  Licensed Japan\株式道場.nes 
    0x0E997CF6, //  Licensed Japan\武田信玄 2.nes 
    0x2858933B, //  Licensed Japan\殺意の階層꞉ Power Soft 連続殺人事件.nes 
    0xBB435255, //  Licensed Japan\神仙伝.nes 
    0x9D976153, //  Licensed Japan\赤川次郎の幽霊列車.nes 
    0x0973F714, //  Licensed Japan\雀豪.nes 
    0xD0CC5EC8, //  Licensed Japan\魔神英雄伝꞉ ワタル外伝.nes 
    0x5DE61639, //  Licensed North America\Advanced Dungeons &amp; Dragons꞉ Hillsfar.nes 
    0xA558FB52, //  Licensed North America\Destiny of an Emperor.nes 
    0x8C5A784E, //  Licensed North America\Dragon Warrior II.nes 
    0xCEBD2A31, //  Licensed North America\Final Fantasy.nes 
    0x0D9F5BD1, //  Licensed North America\Maniac Mansion.nes 
    0xF6B9799C, //  Licensed North America\NES Open Tournament Golf.nes 
    0x2856111F, //  Licensed North America\Overlord.nes 
    0xBE3BF3B3, //  Licensed North America\Shingen The Ruler.nes 
    0xA4062017, //  Licensed North America\Ultima III꞉ Exodus.nes 
    0xA25A750F, //  Licensed North America\Ultima IV꞉ Quest of the Avatar.nes 
    0x4823EEFE, //  Licensed North America\Ultima V꞉ Warriors of Destiny.nes 
    0xC42D6424, //  Modern\Virtual Console\Final Fantasy (Japan rev0).nes 
    0x85359519, //  Modern\Virtual Console\Final Fantasy (Japan rev1).nes 
    0x3552BE7B, //  Modern\Virtual Console\Final Fantasy (North America).nes 
    0x2C72FCB0, //  Modern\Virtual Console\Final Fantasy II (rev0).nes 
    0x9202A8AF, //  Modern\Virtual Console\Final Fantasy II (rev1).nes 
    0xED749E50, //  Modern\Virtual Console\Mario Open Golf.nes 
    0x9953D34A, //  Modern\Virtual Console\NES Open Tournament Golf (NTSC).nes 
    0xAF6B5B85, //  Samples\Sweet Home.nes 
    0x988C290E, //  Samples\The Bard's Tale꞉ Tales of the Unknown.nes 
    0x93A2EEFB, //  Unreleased\Final Fantasy II.nes 
    0x1F53E946, //  Unreleased\Final Fantasy.nes 
    0xDE0C29A9, //  Unreleased\Maniac Mansion.nes 
    0x7D6C2065, //  Unreleased\The Legend of Robin Hood.nes 
/**** 122 mapper=1 prgrom=256K prgnvram=8K chrram=8K mirror=H battery=1 system=SYSTEM_PAL */
    0xF59CFC3D, //  Licensed PAL\Maniac Mansion (English).nes 
    0xF4B70BFE, //  Licensed PAL\Maniac Mansion (French).nes 
    0x60EA98A0, //  Licensed PAL\Maniac Mansion (German).nes 
    0xDC529482, //  Licensed PAL\Maniac Mansion (Italian).nes 
    0xF5B2AFCA, //  Licensed PAL\Maniac Mansion (Spanish).nes 
    0x3F2BDA65, //  Licensed PAL\Maniac Mansion (Swedish).nes 
    0x2D020965, //  Licensed PAL\NES Open Tournament Golf.nes 
    0x401B6AD0, //  Modern\Virtual Console\NES Open Tournament Golf (PAL).nes 
/**** 123 mapper=1 prgrom=256K prgnvram=32K chrram=8K mirror=H battery=1 */
    0xB1AF38AA, //  Homebrew\Nova the Squirrel (alt).nes 
    0xB8747ABF, //  Licensed Japan\Best Play Pro 野球 Special (rev0).nes 
    0xC3DE7C69, //  Licensed Japan\Best Play Pro 野球 Special (rev1).nes 
/**** 124 mapper=1 prgrom=256K prgram=8K prgnvram=8K chrram=8K mirror=H battery=1 */
    0xABBF7217, //  Licensed Japan\三國志 (rev0).nes 
    0xCCF35C02, //  Licensed Japan\三國志 (rev1).nes 
    0x3F7AD415, //  Licensed Japan\信長の野望꞉ 全国版 (rev0).nes 
    0x2B11E0B0, //  Licensed Japan\信長の野望꞉ 全国版 (rev1).nes 
    0xFB69743A, //  Licensed Japan\蒼き狼と白き牝鹿꞉ Genghis Khan.nes 
    0x2225C20F, //  Licensed North America\Genghis Khan.nes 
    0x4642DDA6, //  Licensed North America\Nobunaga's Ambition.nes 
    0xC6182024, //  Licensed North America\Romance of the Three Kingdoms.nes 
/**** 125 mapper=1 prgrom=256K prgnvram=8K chrrom=48K mirror=H battery=1 */
    0x06086781, //  Homebrew\Demon District.nes 
/**** 126 mapper=1 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x10E4AF0C, //  Bad Dumps\Jay and Silent Bob Mall Brawl [Steam file extract #1, no sound].nes 
    0xF4643DB5, //  Bad Dumps\Jay and Silent Bob Mall Brawl [Steam file extract #2, no sound].nes 
    0xCDDB85C9, //  Bad Dumps\Jay and Silent Bob Mall Brawl [Steam memory extract, no sound].nes 
    0x226D2EA9, //  Bad Dumps\Jay and Silent Bob Mall Brawl [Switch file extract, no sound].nes 
    0x99CF4DE0, //  Compatibility Hacks\Jay and Silent Bob Mall Brawl [m001 extract].nes 
    0x4EEBE327, //  Homebrew\Translations\English\Chaos World [Aeon Genesis].nes 
    0xF9B75C39, //  Homebrew\Translations\English\Chaos World [Chronix].nes 
    0x0F82F97F, //  Homebrew\Translations\English\The Adventures of Musashi Jr. [Aishsha &amp; Stardust Crusaders v1.00].nes 
    0x59CF2856, //  Homebrew\Translations\English\The Adventures of Musashi Jr. [Aishsha &amp; Stardust Crusaders v1.01].nes 
    0xE0CBC2BA, //  Licensed Japan\Chaos World.nes 
/**** 127 mapper=1 prgrom=512K prgnvram=8K chrram=8K mirror=H battery=1 */
    0xB6A3A8E4, //  Compatibility Hacks\Faxanadu [Unsavory Maggot v1.04][FCE Ultra].nes 
    0x61C28C5E, //  Homebrew\Timec.nes 
    0xDDA62852, //  Homebrew\Translations\English\Deep Dungeon III꞉ The Journey to Become a Hero [KingMike].nes 
    0x4BD605AD, //  Homebrew\Translations\English\Faxanadu [Unsavory Maggot v1.04].nes 
    0x1A5CE587, //  Homebrew\Translations\English\Ninja Rahoi! [Aishsha &amp; Stardust Crusaders, Me_Dave].nes 
    0x8507A4F9, //  Homebrew\Translations\English\Ninja Rahoi! [Aishsha &amp; Stardust Crusaders].nes 
    0x88900F7E, //  Homebrew\Translations\English\Shinsenden꞉ The Legend of Immortals [aishsha v1.00].nes 
    0x2DD71ACB, //  Licensed Japan\Dragon Quest IV꞉ 導かれし者たち (rev0).nes 
    0x0794F2A5, //  Licensed Japan\Dragon Quest IV꞉ 導かれし者たち (rev1).nes 
    0xCEE5857B, //  Licensed Japan\忍者らホイ!.nes 
    0xA86A5318, //  Licensed North America\Dragon Warrior III.nes 
    0x506E259D, //  Licensed North America\Dragon Warrior IV.nes 
/**** 128 mapper=1 prgrom=512K prgnvram=32K chrram=8K mirror=H battery=1 */
    0x629ACE81, //  Homebrew\Translations\English\Final Fantasy I &amp; II [Demiforce v1.03, Grond v1.1].nes 
    0xC9556B36, //  Licensed Japan\Final Fantasy I &amp; II.nes 
/**** 129 mapper=1 submapper=5 prgrom=32K chrrom=16K mirror=H */
    0x2578AFDF, //  Homebrew\Bugfixes, Improvements\Tetris (title screen music).nes 
    0x1394F57E, //  Licensed North America\Tetris.nes 
    0x5EDEC8CD, //  Unreleased\Virus (1990-02).nes 
/**** 130 mapper=1 submapper=5 prgrom=32K chrrom=16K mirror=H system=SYSTEM_PAL */
    0xFDFF80D5, //  Licensed PAL\Tetris.nes 
/**** 131 mapper=1 submapper=5 prgrom=32K chrrom=32K mirror=H */
    0xAE868E88, //  Bootleg Singles\TouchGamePlayer\Boulder Dash.nes 
    0xF00726DE, //  Homebrew\Hacks\Anti-Hippie Dr. Mario [Blooshed&amp;Capt. Blade].nes 
    0x5004378F, //  Homebrew\Hacks\Challenging Lolo [Sivak].nes 
    0x14CB601E, //  Homebrew\Hacks\Dr. Dadio.nes 
    0xD046F734, //  Homebrew\Hacks\Dr. Lip [Midna].nes 
    0xE847BF74, //  Homebrew\Hacks\Dr. Luigi Lite [Shugo].nes 
    0x14F2F140, //  Homebrew\Hacks\Dr. Luigi [Shugo].nes 
    0x4798F1A0, //  Homebrew\Hacks\Dr. Luigi꞉ The Year of Luigi [YOLE, Shugo].nes 
    0x3F33D24C, //  Homebrew\Hacks\Dr. Wario [DiamondPhoenix].nes 
    0xB1263D1B, //  Homebrew\Hacks\Nurseのお仕事 [Rinkaku].nes 
    0xEA026E48, //  Homebrew\Hacks\Pill Head [FuctCamel].nes 
    0xCD82A6DF, //  Homebrew\Translations\English\Palamedes II꞉ Star Twinkles [KingMike's Translations].nes 
    0x89567668, //  Licensed Japan\Boulder Dash.nes 
    0x198C2F41, //  Licensed Japan\Dr. Mario (rev0).nes 
    0xDE581355, //  Licensed Japan\Dr. Mario (rev1).nes 
    0x31957AE4, //  Licensed Japan\Palamedes II꞉ Star Twinkles.nes 
    0xC6B5D7E0, //  Licensed Japan\Palamedes.nes 
    0x64BD6CDB, //  Licensed Japan\ひらけ! ポンキッキ.nes 
    0x71BF075F, //  Licensed North America\Adventures of Lolo.nes 
    0x99A9F57E, //  Licensed North America\Anticipation.nes 
    0xA8F4D99E, //  Licensed North America\Boulder Dash.nes 
    0x6BB6A0CE, //  Licensed North America\Palamedes.nes 
    0xAAED295C, //  Licensed North America\R.C. Pro-Am (rev1).nes 
    0x339437F6, //  Licensed North America\Sesame Street꞉ 123.nes 
    0xEC0FC2DE, //  Licensed North America\Sesame Street꞉ ABC.nes 
    0xFDF4569B, //  Licensed North America\Snake Rattle 'n Roll.nes 
    0x40ED2A9D, //  Licensed North America\Taboo꞉ The Sixth Sense (rev0).nes 
    0x482C79AF, //  Licensed North America\Taboo꞉ The Sixth Sense (rev1).nes 
    0x2AE77B9B, //  Modern\Virtual Console\Adventures of Lolo (NTSC).nes 
    0x141FF4EE, //  Playchoice\Dr. Mario.nes 
    0xE841518E, //  Playchoice\R.C. Pro-Am.nes 
    0x6FD69F34, //  Unreleased\Dr. Mario (1990-04-27).nes 
    0xC49FCAB4, //  Unreleased\Dr. Mario (1990-04-27+).nes 
/**** 132 mapper=1 submapper=5 prgrom=32K chrrom=32K mirror=H system=SYSTEM_PAL */
    0xDF4EDC13, //  Licensed PAL\Adventures of Lolo.nes 
    0x8B7D3C75, //  Licensed PAL\Anticipation.nes 
    0xE5FCC4C1, //  Licensed PAL\Boulder Dash.nes 
    0x9735D267, //  Licensed PAL\Dr. Mario.nes 
    0x46931EA0, //  Licensed PAL\R.C. Pro-Am (rev1).nes 
    0x3824F7A5, //  Licensed PAL\Snake Rattle 'n Roll.nes 
/**** 133 mapper=1 submapper=5 prgrom=32K chrrom=40K mirror=H */
    0x6197EC16, //  Homebrew\Hacks\Asian Dr. Mario.nes 
/**** 134 mapper=1 submapper=5 prgrom=32K chrrom=128K mirror=H */
    0x489EF6A2, //  Licensed North America\Airwolf.nes 
    0x3322105A, //  Licensed North America\Sky Kid.nes 
    0x51BEE3EA, //  Licensed North America\The All-New Family Feud.nes 
/**** 135 mapper=1 submapper=5 prgrom=32K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x4D345422, //  Licensed PAL\Airwolf.nes 
/**** 136 mapper=1 submapper=5 prgrom=128K chrrom=128K mirror=H */
    0x1FE4E64A, //  Homebrew\Hacks\Dr. Blario.nes 
    0x4A4B8F1F, //  Homebrew\Hacks\Dr. Fred [Glutate].nes 
    0xB03087A2, //  Homebrew\Hacks\Dr. Manson [Kreegs].nes 
    0x88926A1F, //  Homebrew\Hacks\Dr. Maria [TeamX].nes 
    0x5250C15F, //  Homebrew\Hacks\Dr. Mario Crystal Clear [Tempes].nes 
/**** 137 mapper=2 prgrom=64K chrram=8K mirror=H */
    0x6EAFF3DD, //  Homebrew\F-FF.nes 
/**** 138 mapper=2 prgrom=48K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0x1774EBB8, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. View-On Edition.nes 
/**** 139 mapper=2 prgrom=128K chrram=8K mirror=V */
    0x1060311F, //  Homebrew\Translations\English\Dash Rascal [Pacnsacdave].nes 
    0x218AB967, //  Homebrew\Translations\English\The Black Bass [GAFF Translations].nes 
/**** 140 mapper=2 submapper=1 prgrom=128K chrram=8K mirror=H */
    0x3DD3BA1E, //  Bootleg Singles\Front Fareast Magic Card\(F2001) 戦場の狼.nes 
    0xB42A57C7, //  Bootleg Singles\Front Fareast Magic Card\(F2020) Booby Kids.nes 
    0x6F365E7B, //  Bootleg Singles\Front Fareast Magic Card\(F2023) Fantasy Zone.nes 
    0x957F3D28, //  Bootleg Singles\Front Fareast Magic Card\(F2032) Top Gun.nes 
    0xB8F3781D, //  Bootleg Singles\Front Fareast Magic Card\(F2033) 光の戦士 Photon꞉ The Ultimate Game on Planet Earth.nes 
    0x8C3D54E8, //  Bootleg Singles\Front Fareast Magic Card\(F2045) 怒 - Ikari.nes 
/**** 141 mapper=2 submapper=1 prgrom=1024K chrram=8K mirror=H */
    0xAC7B0742, //  Unlicensed Taiwan&amp;Hong Kong\金曲 KTV.nes 
/**** 142 mapper=2 submapper=1 prgrom=64K chrrom=8K mirror=V */
    0x12C096C0, //  Compatibility Hacks\Super Mario Bros. View-On Edition [PRG size].nes 
/**** 143 mapper=2 submapper=1 prgrom=128K chrram=8K mirror=V */
    0x09AD3773, //  Bad Dumps\Dan's Stake Demo [corrupt].nes 
    0xC86E8963, //  Bootleg Singles\Bung Game Doctor\(G080) Space Harrier.nes 
    0x3D623C6E, //  Bootleg Singles\Bung Game Doctor\たけしの挑戦状.nes 
    0xDA122635, //  Bootleg Singles\Front Fareast Magic Card\(F2008) 雀棒.nes 
    0x97886454, //  Bootleg Singles\Front Fareast Magic Card\(F2034) Argosの戦士꞉ はちゃめちゃ大進撃.nes 
    0x322C9B09, //  Bootleg Singles\Front Fareast Magic Card\(F2038) Metal Gear.nes 
    0xB301B0DC, //  Bootleg Singles\Front Fareast Magic Card\(F2039) Higemaru 魔界島꞉ 七つの島大冒険.nes 
    0x48C8DE53, //  Bootleg Singles\Front Fareast Magic Card\(F2040) Heraklesの栄光꞉ 闘人魔境伝.nes 
    0x63AF202F, //  Bootleg Singles\Front Fareast Magic Card\(F2041) JJ - とびだせ大作戦 Part 2.nes 
    0x78B09986, //  Bootleg Singles\Front Fareast Magic Card\(F2046) 魔城伝説 II꞉ 大魔司教 Galious.nes 
    0xFCFA4C1E, //  Bootleg Singles\Front Fareast Magic Card\(F2058) Woody Poko.nes 
    0xBB4EE993, //  Bootleg Singles\Front Fareast Magic Card\(F2082) たけしの挑戦状.nes 
    0x5012D5D0, //  Bootleg Singles\Front Fareast Magic Card\(F2085) Ripple Island.nes 
    0xEE6EF957, //  Bootleg Singles\Front Fareast Magic Card\(F2086) Space Harrier.nes 
/**** 144 mapper=2 submapper=1 prgrom=128K prgram=8K chrram=8K mirror=V */
    0xB382AEA4, //  Bootleg Singles\Bung Game Doctor\Bomberman II.nes 
/**** 145 mapper=2 submapper=1 prgrom=256K chrram=8K mirror=V */
    0xA92C8FEB, //  Bootleg Singles\Bung Game Doctor\(L125) Jesus꞉ 恐怖のBio Monster.nes 
    0x5E5723F4, //  Bootleg Singles\Front Fareast Magic Card\(F4001) Faxanadu.nes 
    0x02B0B405, //  Bootleg Singles\Front Fareast Magic Card\(F4003) Esper 冒険隊꞉ 魔王の砦.nes 
    0xB4362294, //  Bootleg Singles\Front Fareast Magic Card\(F4111) Jesus꞉ 恐怖のBio Monster.nes 
    0xCA8204C4, //  Homebrew\Demos\Dynamite Dudes.nes 
    0x86BE4746, //  Unlicensed South Korea\Dooly Bravo Land.nes 
/**** 146 mapper=2 submapper=1 prgrom=512K chrram=8K mirror=V */
    0x387D3408, //  Homebrew\Proto Dere.nes 
/**** 147 mapper=2 submapper=2 prgrom=128K prgram=2K chrram=8K mirror=4 */
    0xFFBEF374, //  Vs. System\Vs. Castlevania.nes 
    0x8C0C2DF5, //  Vs. System\Vs. Top Gun.nes 
/**** 148 mapper=2 submapper=2 prgrom=16K chrram=8K mirror=H */
    0x9CBDE609, //  Homebrew\Critical Match.nes 
/**** 149 mapper=2 submapper=2 prgrom=32K chrram=8K mirror=H */
    0xCC3148F6, //  Homebrew\MilioNESy.nes 
/**** 150 mapper=2 submapper=2 prgrom=64K chrram=8K mirror=H */
    0x0AA209C0, //  Homebrew\Arlington Apple in an Interplanetary Pickle (2017-04-22@16-22-26.12).nes 
    0x68D97836, //  Homebrew\Arlington Apple in an Interplanetary Pickle (2017-04-22@17-20-44.87).nes 
    0x280689D6, //  Homebrew\Arlington Apple in an Interplanetary Pickle (2017-04-23).nes 
    0x338B6133, //  Homebrew\Arlington Apple in an Interplanetary Pickle (2017-04-24).nes 
    0x1FEF81D9, //  Homebrew\Arlington Apple in an Interplanetary Pickle (Post-Ludum Dare).nes 
    0x2059B2A3, //  Homebrew\Arlington Apple in an Interplanetary Pickle (R.C.#6).nes 
    0x0CADB202, //  Homebrew\Arlington Apple in an Interplanetary Pickle (R.C.#7).nes 
    0x01461C7F, //  Homebrew\Arlington Apple in an Interplanetary Pickle (R.C.#7a).nes 
    0x472B9734, //  Homebrew\Dizzy꞉ Melanchony of Existance Chapter 0.nes 
    0x94766E4C, //  Homebrew\Nothing Good Can Come of This.nes 
    0x37141810, //  Homebrew\Squish꞉ The Bouncing Cat Ball.nes 
    0xF665FE1F, //  Samples\Lucky Penguin [Lukasz Kur].nes 
    0xFB253226, //  Samples\Tapeworm Disco Puzzle.nes 
/**** 151 mapper=2 submapper=2 prgrom=64K chrram=8K mirror=H system=SYSTEM_NTSC */
    0x02D41AEA, //  Modern\Lucky Penguin.nes 
/**** 152 mapper=2 submapper=2 prgrom=128K chrram=8K mirror=H */
    0xD4C7F0C3, //  Bootleg Singles\Bung Game Doctor\(G003) Terra Cresta.nes 
    0x24826927, //  Bootleg Singles\Bung Game Doctor\(G004) 怒.nes 
    0x67EDE374, //  Bootleg Singles\Bung Game Doctor\(G043) Fantasy Zone.nes 
    0x364C49C9, //  Bootleg Singles\Bung Game Doctor\(G046) 戦場の狼.nes 
    0x19A7F794, //  Bootleg Singles\Front Fareast Magic Card\(F2072) Rainbow Islands꞉ The Story of Bubble Bobble 2.nes 
    0x85D3F785, //  Bootleg Singles\Front Fareast Magic Card\(F2089) 1944.nes 
    0x6E9E5890, //  Bootleg Singles\Gun.Smoke (NTDEC).nes 
    0x3C7A1329, //  Bootleg Singles\TouchGamePlayer\FlyingHero.nes 
    0x4C24C375, //  Homebrew\Black Hole - Czarna Dziura (v0.20)[SIUDYM].nes 
    0x40CCAE9B, //  Homebrew\Hacks\1493꞉ The Year After [S. K. Dumbbus].nes 
    0xF2830B14, //  Homebrew\Hacks\1943꞉ The Battle of Midway - 2011 Update [DamienC].nes 
    0xC687EC8E, //  Homebrew\Kosmity Atakujo (10.07.2020).nes 
    0x244DCB2D, //  Homebrew\Micro Knight 3 (Demo)[sdm].nes 
    0x6C430897, //  Homebrew\Micro Knight II.nes 
    0xDAC46396, //  Homebrew\Micro Knight IV (v1.02).nes 
    0x750DC088, //  Homebrew\Micro Knight꞉ Revenge of the Four Skulls (Contrabanda Party).nes 
    0x7FB3C5E0, //  Homebrew\Micro Knight꞉ Revenge of the Four Skulls (v1.0.0).nes 
    0x9776BAF0, //  Homebrew\The Mad Wizard꞉ A Candelabra Chronicle (cartridge).nes 
    0xACA3C0CB, //  Homebrew\Translations\English\4 Wheel Drive Battle [Pacnsacdave].nes 
    0xA5B0F9E3, //  Homebrew\Translations\English\Pescatore [KingMike's Translations].nes 
    0xF6ED5121, //  Homebrew\Translations\English\Photon, Soldier of Light꞉ The Ultimate Game on Planet Earth [KingMike's Translations v1.00b].nes 
    0x238F2146, //  Homebrew\Translations\English\Puyo Puyo [Aeon Genesis.nes 
    0x616A3E90, //  Homebrew\Translations\English\Rampart [Magnus Nilsson &amp; MrRichard999].nes 
    0x0B870325, //  Homebrew\Translations\English\The Most Dangerous Detective [Gil Galad v1.00].nes 
    0x4E42F13A, //  Licensed Japan\1943꞉ The Battle of Valhalla.nes 
    0x1F74EA6C, //  Licensed Japan\Arctic.nes 
    0x4178497A, //  Licensed Japan\Booby Kids.nes 
    0xF6751D3D, //  Licensed Japan\Flying Hero.nes 
    0xFA704C86, //  Licensed Japan\Rainbow Islands꞉ The Story of Bubble Bobble 2.nes 
    0x3ECDB1F7, //  Licensed Japan\Rampart.nes 
    0x38BFC03C, //  Licensed Japan\Shanghai II.nes 
    0x6D65CAC6, //  Licensed Japan\Terra Cresta.nes 
    0x37CB1801, //  Licensed Japan\Top Gun.nes 
    0xFCB5CB1E, //  Licensed Japan\ぷよぷよ.nes 
    0xF89300FB, //  Licensed Japan\もっともあぶない刑事.nes 
    0x51BD8336, //  Licensed Japan\光の戦士 Photon꞉ The Ultimate Game on Planet Earth.nes 
    0xAB47A50E, //  Licensed Japan\怒 - Ikari.nes 
    0x04109355, //  Licensed Japan\激突四駆 Battle.nes 
    0xA56208A0, //  Licensed Japan\雀棒.nes 
    0x12C6D5C7, //  Licensed North America\1943꞉ The Battle of Midway.nes 
    0x63E992AC, //  Licensed North America\Alfred Chicken.nes 
    0x7329118D, //  Licensed North America\Casino Kid 2.nes 
    0x05A688C8, //  Licensed North America\Casino Kid.nes 
    0x82BE4724, //  Licensed North America\Commando.nes 
    0xC6000085, //  Licensed North America\Conan꞉ The Mysteries of Time.nes 
    0xA8784932, //  Licensed North America\Gun.Smoke.nes 
    0x655EFEED, //  Licensed North America\Ikari Warriors (rev0).nes 
    0x2D273AA4, //  Licensed North America\Ikari Warriors (rev1).nes 
    0x2EA8CC16, //  Licensed North America\Indiana Jones and the Last Crusade꞉ The Action Game.nes 
    0xF4DFDB14, //  Licensed North America\KlashBall.nes 
    0x9EA1DC76, //  Licensed North America\Rainbow Islands꞉ The Story of Bubble Bobble 2.nes 
    0xDC4DA5D4, //  Licensed North America\Side Pocket.nes 
    0xE575687C, //  Licensed North America\Star Trek꞉ The Next Generation.nes 
    0x2A46B57F, //  Licensed North America\Terra Cresta.nes 
    0xAFB46DD6, //  Licensed North America\Thundercade.nes 
    0x4E22368D, //  Licensed North America\Top Gun (rev0).nes 
    0xCF6D0D7A, //  Licensed North America\Top Gun (rev1).nes 
    0xB6661BDA, //  Licensed North America\Wall Street Kid.nes 
    0x969EF9E4, //  Licensed North America\Winter Games (rev1).nes 
    0xE292AA10, //  Licensed North America\Zanac A.I..nes 
    0xFEAB08F1, //  Modern\8 Bit Xmas 2012꞉ Santa's Biplane (download).nes 
    0xF8D04AA0, //  Modern\8 Bit Xmas 2013꞉ Santa vs. the Aliens (download).nes 
    0x900CA825, //  Modern\8 Bit Xmas 2014꞉ Squish Everyone! (download).nes 
    0x376C9FCC, //  Modern\8 Bit Xmas 2015꞉ Twelve Seconds 'til Xmas (download).nes 
    0x3E03466B, //  Modern\8 Bit Xmas 2019.nes 
    0xB4A7B014, //  Modern\The Mad Wizard꞉ A Candelabra Chronicle.nes 
    0xC117A9AB, //  Samples\Mayan's Revenge (2014-05-23).nes 
    0x6F790F9B, //  Samples\Rainbow Islands - The Story of Bubble Bobble 2.nes 
    0x13F8C418, //  Samples\Rampart.nes 
    0x7600520D, //  Samples\Study Hall.nes 
    0x3FF10E3D, //  Unreleased\1943꞉ The Battle of Midway.nes 
    0x7E9BCA05, //  Unreleased\Crackout.nes 
    0xA3BF2ADA, //  Unreleased\HeroQuest (NTSC).nes 
    0x5D40C08A, //  Unreleased\Pescatore.nes 
/**** 153 mapper=2 submapper=2 prgrom=128K chrram=8K mirror=H system=SYSTEM_PAL */
    0x3B317F9B, //  Bootleg Singles\TouchGamePlayer\Gun.Smoke.nes 
    0x759418D2, //  Licensed PAL\Alfred Chicken.nes 
    0x81AF4AF9, //  Licensed PAL\Crackout.nes 
    0xB79C320D, //  Licensed PAL\Gun.Smoke.nes 
    0x538218B2, //  Licensed PAL\Ikari Warriors.nes 
    0x8D9AD3BF, //  Licensed PAL\Indiana Jones and the Last Crusade꞉ The Action Game.nes 
    0xD7B35F7D, //  Licensed PAL\Konami Hyper Soccer.nes 
    0x83000991, //  Licensed PAL\Side Pocket.nes 
    0xD78BFB28, //  Licensed PAL\Top Gun.nes 
    0xF9FC0700, //  Unreleased\HeroQuest (PAL English).nes 
    0xD91CF3DC, //  Unreleased\HeroQuest (PAL German).nes 
/**** 154 mapper=2 submapper=2 prgrom=128K prgram=8K chrram=8K mirror=H */
    0x7D868C39, //  Bootleg Singles\Front Fareast Magic Card\(F2092) 激突四駆 Battle.nes 
/**** 155 mapper=2 submapper=2 prgrom=256K chrram=8K mirror=H */
    0xA0AE2A2D, //  Homebrew\JAMG - The Lost Coins.nes 
    0x5BCC67D0, //  Homebrew\Super Roman.nes 
    0x3A0965B1, //  Licensed North America\Paperboy 2.nes 
    0x48BB5A71, //  Modern\Battle Kid꞉ Fortress of Peril (v1.000).nes 
    0xAEF464CE, //  Modern\Battle Kid꞉ Fortress of Peril (v1.100).nes 
    0x18E4BCFD, //  Modern\ぽるんちゃんのおにぎり大好き (Amabie).nes 
    0x4A20A465, //  Modern\ぽるんちゃんのおにぎり大好き (Trial).nes 
    0x3B767B4A, //  Modern\ぽるんちゃんのおにぎり大好き (v1.1).nes 
    0xAAEA1D27, //  Samples\Battle Kid 2꞉ Mountain of Torment.nes 
    0xF7A14E41, //  Samples\Battle Kid꞉ Fortress of Peril.nes 
/**** 156 mapper=2 submapper=2 prgrom=256K chrram=8K mirror=H system=SYSTEM_PAL */
    0x509E6032, //  Licensed PAL\Paperboy 2.nes 
/**** 157 mapper=2 submapper=2 prgrom=256K chrram=8K mirror=H system=SYSTEM_NTSC */
    0x3C5BE639, //  Homebrew\Demos\PCM Demo Wgraphics.nes 
    0x2CDC974C, //  Samples\Twin Dragons.nes 
/**** 158 mapper=2 submapper=2 prgrom=16K chrram=8K mirror=V */
    0x2CA86503, //  Homebrew\Hot Seat Harry.nes 
/**** 159 mapper=2 submapper=2 prgrom=32K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x2AB114D5, //  Homebrew\Tests\Rainwarrior - Microphone Input Test.nes 
/**** 160 mapper=2 submapper=2 prgrom=64K chrram=8K mirror=V */
    0xE8C53885, //  Homebrew\Böbl (v1.0).nes 
    0xDEE8631E, //  Homebrew\Böbl (v1.1).nes 
    0xFFDDEA7E, //  Homebrew\Filthy Kitchen [Dustmop].nes 
    0xEBCF45BE, //  Homebrew\Super City Mayor.nes 
    0x1394DED0, //  Homebrew\Super PakPak.nes 
    0x6599113A, //  Homebrew\Tools\240p Test Suite (v0.15)[Artemio].nes 
    0x3F87F554, //  Homebrew\Veggie Invasion.nes 
    0xBB4569FA, //  Homebrew\Waddles the Duck.nes 
    0xACD15774, //  Homebrew\we are hejikle.nes 
    0x5581D82D, //  Samples\The Cowlitz Gamers' 2nd Adventure (v1.1)[Cowlitz Gamers].nes 
    0x29A67DFD, //  Samples\Trials of the Wolf Clan #1꞉ The Wolf Spirit [Valdir Salgueiro].nes 
/**** 161 mapper=2 submapper=2 prgrom=64K chrram=8K mirror=V system=SYSTEM_NTSC */
    0xF8FA4EF2, //  Homebrew\Cheril the Goddess (Alternate Jump).nes 
    0xBA392DA2, //  Homebrew\Cheril the Goddess.nes 
    0x4CAC8341, //  Homebrew\Gruniozerca 2 (NESdev rev0).nes 
    0x9FB8F842, //  Homebrew\Gruniozerca 2 (NESdev rev1).nes 
    0xCCEB1CE3, //  Homebrew\Gruniozerca 2꞉ The Great Cavy Rescue!.nes 
    0xF4DE6CC4, //  Homebrew\Gruniozerca 3꞉ The Great Cavy Clean-Up! (alt).nes 
    0x4CC4E7DE, //  Homebrew\Gruniozerca 3꞉ The Great Cavy Clean-Up!.nes 
    0x1833B431, //  Homebrew\Lala the Magical (NESdev 2016).nes 
    0xA418DE9D, //  Homebrew\The Banketh (Demo).nes 
    0x301CA5BB, //  Homebrew\The Cowlitz Gamers' Lost Adventure.nes 
    0x6D618DDD, //  Homebrew\Wolfling (v1.1).nes 
    0xAF28C35C, //  Homebrew\Wolfling (v1.2).nes 
    0x4C86AD14, //  Homebrew\Wolfling (v1.4).nes 
/**** 162 mapper=2 submapper=2 prgrom=64K chrram=16K mirror=V */
    0x0E6EB220, //  Homebrew\Spacegulls (v1.1)[Morphcat Games].nes 
/**** 163 mapper=2 submapper=2 prgrom=128K chrram=8K mirror=V */
    0xACC8154A, //  Bad Dumps\Athena [broken music].nes 
    0x1959CB18, //  Bad Dumps\Contra [missing PROM].nes 
    0x6A91F769, //  Bootleg Hacks\'93 超级魂.nes 
    0x810D0132, //  Bootleg Hacks\8-in-1 The Little Mermaid.nes 
    0x35DA0A07, //  Bootleg Hacks\Contra (Super Technos).nes 
    0x8A94B57F, //  Bootleg Hacks\Forcity.nes 
    0xD4EF08A8, //  Bootleg Hacks\Top Gun III.nes 
    0xA7C6BD80, //  Bootleg Hacks\超级魂 Super Contra II.nes 
    0x3E9D567B, //  Bootleg Singles\Airball (Retrozone).nes 
    0x80796845, //  Bootleg Singles\Bung Game Doctor\(G001) 魔界村.nes 
    0x9960A919, //  Bootleg Singles\Bung Game Doctor\(G002) Super Pitfall.nes 
    0x0809E07E, //  Bootleg Singles\Bung Game Doctor\(G005) Daiva꞉ Nirsartiaの玉座.nes 
    0x562D3A94, //  Bootleg Singles\Bung Game Doctor\(G006) Layla.nes 
    0x3DE9BB55, //  Bootleg Singles\Bung Game Doctor\(G007) 闘いの挽歌.nes 
    0x5674D1F3, //  Bootleg Singles\Bung Game Doctor\(G008) 飛龍の拳꞉ 奥義の書.nes 
    0x3DC64CC0, //  Bootleg Singles\Bung Game Doctor\(G009) Labyrinth.nes 
    0x7D48D1EB, //  Bootleg Singles\Bung Game Doctor\(G010) Super Star Force.nes 
    0x1A1D3CA2, //  Bootleg Singles\Bung Game Doctor\(G011) Aighinaの予言꞉ Balubaloukの伝説より.nes 
    0x3877CE9E, //  Bootleg Singles\Bung Game Doctor\(G012) 熱血硬派くにおくん.nes 
    0x138F0BB0, //  Bootleg Singles\Bung Game Doctor\(G013) Argosの戦士꞉ はちゃめちゃ大進撃.nes 
    0x869BCAF2, //  Bootleg Singles\Bung Game Doctor\(G014) City Adventure Touch꞉ Mystery of Triangle.nes 
    0x3F42293B, //  Bootleg Singles\Bung Game Doctor\(G015) 花のStar 街道.nes 
    0xB0D6590F, //  Bootleg Singles\Bung Game Doctor\(G016) 北斗の拳 2꞉ 世紀末救世主伝説.nes 
    0xA7D227EF, //  Bootleg Singles\Bung Game Doctor\(G017) Law of the West.nes 
    0x4C7FDD56, //  Bootleg Singles\Bung Game Doctor\(G027) 火の鳥鳳凰編꞉ 我王の冒険.nes 
    0x1E8C32EF, //  Bootleg Singles\Bung Game Doctor\(G035) Castlevania.nes 
    0x6DC690D9, //  Bootleg Singles\Bung Game Doctor\(G051) JJ - とびだせ大作戦 Part 2.nes 
    0x96CADADB, //  Bootleg Singles\Bung Game Doctor\(G078) The Black Bass II.nes 
    0xBC44544E, //  Bootleg Singles\Bung Game Doctor\(G082) California Games.nes 
    0xB4C09A4D, //  Bootleg Singles\Bung Game Doctor\(G083) Skate or Die!.nes 
    0xDA731F14, //  Bootleg Singles\Bung Game Doctor\Arctic.nes 
    0x299F355B, //  Bootleg Singles\Bung Game Doctor\Dick Tracy.nes 
    0xAB58D65A, //  Bootleg Singles\Bung Game Doctor\Jackal.nes 
    0x89AB27D8, //  Bootleg Singles\Bung Game Doctor\Shanghai II.nes 
    0x73F84410, //  Bootleg Singles\Contra (LH57).nes 
    0xB624C4D8, //  Bootleg Singles\Duck Tales 2 (D-T2).nes 
    0x2980B656, //  Bootleg Singles\Front Fareast Magic Card\(F2013) 魔界村.nes 
    0x5E98BA4A, //  Bootleg Singles\Front Fareast Magic Card\(F2031) Spelunker 2꞉ 勇者への挑戦.nes 
    0x15662081, //  Bootleg Singles\Front Fareast Magic Card\(F2035) Hector '87.nes 
    0x4471D531, //  Bootleg Singles\Front Fareast Magic Card\(F2042) 星をみるひと.nes 
    0x131A18CB, //  Bootleg Singles\Front Fareast Magic Card\(F2043) Layla.nes 
    0xB59A7A29, //  Bootleg Singles\Front Fareast Magic Card\(F2049) Guardic 外伝.nes 
    0xFC201C46, //  Bootleg Singles\Front Fareast Magic Card\(F2052) 南国指令!! Spy vs. Spy.nes 
    0x605F5D51, //  Bootleg Singles\Front Fareast Magic Card\(F2053) Elnarkの財宝.nes 
    0x3F9970F9, //  Bootleg Singles\Front Fareast Magic Card\(F2054) Athena.nes 
    0x838BF76F, //  Bootleg Singles\Front Fareast Magic Card\(F2056) Outlanders.nes 
    0x9622FBD9, //  Bootleg Singles\Front Fareast Magic Card\(F2059) Ballblazer.nes 
    0xA16AB939, //  Bootleg Singles\Front Fareast Magic Card\(F2067) 釣りキチ三平꞉ Blue Marlin 編.nes 
    0xE61E89EE, //  Bootleg Singles\Front Fareast Magic Card\(F2074) Shanghai.nes 
    0x203D32AB, //  Bootleg Singles\Front Fareast Magic Card\(F2081) 西遊記 World.nes 
    0x2F462388, //  Bootleg Singles\Front Fareast Magic Card\(F2087) 中国占星術.nes 
    0x83431081, //  Bootleg Singles\Front Fareast Magic Card\(F2088) Flying Hero.nes 
    0xC0FB91AC, //  Bootleg Singles\Front Fareast Magic Card\(F2106) Shanghai II.nes 
    0xA88D8E19, //  Bootleg Singles\Front Fareast Magic Card\(SM2002) Twin Eagle꞉ Revenge Joe's Brother.nes 
    0xD2121F97, //  Bootleg Singles\Front Fareast Magic Card\(SM2003) Blades of Steel.nes 
    0x823AFD95, //  Bootleg Singles\Front Fareast Magic Card\(SM2005) Magic Block.nes 
    0xD4F7AAE9, //  Bootleg Singles\Super NHL '97 (NT-881).nes 
    0xF2603454, //  Bootleg Singles\The Little Mermaid (NT-6031).nes 
    0xE4216F78, //  Bootleg Singles\TouchGamePlayer\Jackal.nes 
    0xF0929533, //  Bootleg Singles\TouchGamePlayer\Rush'n Attack.nes 
    0x55CAE33E, //  Compatibility Hacks\TV System Conversions\Astérix (NTSC)[NewRisingSun].nes 
    0x01E62AE2, //  Compatibility Hacks\Vs. Castlevania [home console].nes 
    0xD7CB97C7, //  Homebrew\Bugfixes, Improvements\Double Dribble (rev1)[DPCM bit order].nes 
    0x3FC98C7F, //  Homebrew\Bugfixes, Improvements\Jackal [DPCM clicking].nes 
    0x3A024B2A, //  Homebrew\Cheats\'93 超级魂 [always Spread Gun].nes 
    0xDF01DA1E, //  Homebrew\Godzilla꞉ King of Monsters! [FG].nes 
    0x22CB5355, //  Homebrew\Hacks\ALF Escapes! [pacnsacdave].nes 
    0x637533A0, //  Homebrew\Hacks\Adventures in Vancouver, BC [RyanVG].nes 
    0xA6E68C88, //  Homebrew\Hacks\Castlevania series\Castle VI.nes 
    0xAEA58E8E, //  Homebrew\Hacks\Castlevania series\CastleVania 5 [xXbelmontXx].nes 
    0x41E396DA, //  Homebrew\Hacks\Castlevania series\Castlevania (Gun)[klaykree].nes 
    0xE69E57BD, //  Homebrew\Hacks\Castlevania series\Castlevania (Improved Controls)[NaOH].nes 
    0x1827F2D1, //  Homebrew\Hacks\Castlevania series\Castlevania (Localization Fix) [spiffy].nes 
    0xC1C877FD, //  Homebrew\Hacks\Castlevania series\Castlevania (Vertical Knockback)[NaOH].nes 
    0xABB2A9D2, //  Homebrew\Hacks\Castlevania series\Castlevania - Dracula Edition (v1.1)[Dom Jurumela].nes 
    0x0AF29397, //  Homebrew\Hacks\Castlevania series\Castlevania Extreme Dracula (v0.42b)[Inccubus].nes 
    0xAE67EFCF, //  Homebrew\Hacks\Castlevania series\Castlevania Extreme Dracula (v1.0)[Inccubus].nes 
    0x61C98A20, //  Homebrew\Hacks\Castlevania series\Castlevania Reborn [XXBelmontXX].nes 
    0x620A1AF1, //  Homebrew\Hacks\Castlevania series\Castlevania Remix.nes 
    0x57921F2C, //  Homebrew\Hacks\Castlevania series\Castlevania Retold (v2.0)(Castlevania 2 sprites)[Dragonsbrethren].nes 
    0xCF38242F, //  Homebrew\Hacks\Castlevania series\Castlevania Retold (v2.0)[Dragonsbrethren].nes 
    0x2BC72705, //  Homebrew\Hacks\Castlevania series\Castlevania Shadows [Rayside Productions].nes 
    0xCDE9EA78, //  Homebrew\Hacks\Castlevania series\Castlevania [SlainDragon].nes 
    0x99C2F526, //  Homebrew\Hacks\Castlevania series\Castlevania [Thaddeus].nes 
    0x3D322063, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Blood Moon [Redrum].nes 
    0xD0F9050C, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Bloody Adventures [Slaindragon].nes 
    0x07C2D171, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Boss Rush [Justice Colde].nes 
    0x89D98500, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Chorus of Mysteries (v2)[Optomon].nes 
    0xA062FDA9, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Dracula's Revenge (Alt)[Ryus].nes 
    0xC443EC2E, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Dracula's Revenge [Ryus].nes 
    0x355C3CD8, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Eternal Confrontation [Guardian].nes 
    0xC2DD992D, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Fan Edition (Alt)[Thaddeus].nes 
    0x8B58DE20, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Fan Edition [Thaddeus].nes 
    0x199ADF3A, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Halloween 98.nes 
    0x335FDBEE, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Hell's Fury (Easy Edition)[Sonikku7].nes 
    0x47B561B9, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Hell's Fury [Sonikku7].nes 
    0x1D77246A, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ High Budget Remake [Dr. Mario].nes 
    0x00836FE4, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Horror of Dracula (v2.0 easy)[Frederic Bouix].nes 
    0xD164C618, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Horror of Dracula (v2.0 hard)[Frederic Bouix].nes 
    0x272DDC4E, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Horror of Dracula (v2.0)[Frederic Bouix].nes 
    0x9162C689, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Opposing Bloodlines [Redrum].nes 
    0x76CBBAC6, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Orchestra of Despair (v2).nes 
    0x384A373E, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Overflow Darkness (English)[Luto Akino].nes 
    0x1AB7DE4C, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Overflow Darkness (Latino)[Luto Akino].nes 
    0x16B3D102, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Poisonous Offering (Hack by Morgoth G).nes 
    0x10373894, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Poisonous Offering [Morgoth].nes 
    0x5883333D, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Prelude of Darkness (Easy).nes 
    0x2F5E25F6, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Prelude of Darkness (Hard).nes 
    0xAF012699, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Prelude of Darkness (Regular).nes 
    0xC1A9B11D, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Prince of Darkness [Frederic Bouix].nes 
    0xB874A564, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Red Scale.nes 
    0x5488BE8B, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Skel's Revenge [Redrum&amp;Slogra].nes 
    0x5516C576, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ Stairs of Doom (v1.1)[Alucard].nes 
    0xB50A188F, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ The Holy Relics [Optomon].nes 
    0xA51633D7, //  Homebrew\Hacks\Castlevania series\Castlevania꞉ The Legend of Trevor Belmont [Dom Jurumela].nes 
    0x652A873B, //  Homebrew\Hacks\Castlevania series\Dan's Stake Demo.nes 
    0x5B60C2DA, //  Homebrew\Hacks\Castlevania series\Pencilvania.nes 
    0x13733936, //  Homebrew\Hacks\Castlevania series\Schoolvania [Chipgnn].nes 
    0xB3198D6E, //  Homebrew\Hacks\Castlevania series\Ultimate Castlevania - Skin A (v1.1)[Khefz].nes 
    0x0574100B, //  Homebrew\Hacks\Castlevania series\Ultimate Castlevania - Skin B (v1.1)[Khefz].nes 
    0xEF52F391, //  Homebrew\Hacks\Castlevania series\Ultimate Castlevania - Skin C (v1.1)[Khefz].nes 
    0xDB7CFFCC, //  Homebrew\Hacks\Castlevania series\Unknown Castlevania Hack.nes 
    0xBE5FCF97, //  Homebrew\Hacks\Castlevania series\紅魔館 Remilia (alt)[DASTARD].nes 
    0x016FADF0, //  Homebrew\Hacks\Castlevania series\紅魔館 Remilia [DASTARD].nes 
    0x82EEAE47, //  Homebrew\Hacks\Ghosts'n Goblins - Lomax Edition.nes 
    0xCED05FC0, //  Homebrew\Hacks\Ghosts'n Goblins꞉ Arcade Restore.nes 
    0x0C928C0E, //  Homebrew\Hacks\Ghosts'n Goblins꞉ Hard Type.nes 
    0xE2F7968B, //  Homebrew\Hacks\God of War [pacnsacdave].nes 
    0x3EFA7E44, //  Homebrew\Hacks\Mega Man series\Mega Girl [Clomax Dominion].nes 
    0xF7145160, //  Homebrew\Hacks\Mega Man series\Mega Man 1 Speed Patches [StalkerMaestro].nes 
    0x9206137B, //  Homebrew\Hacks\Mega Man series\Mega Man Alpha.nes 
    0x562E6354, //  Homebrew\Hacks\Mega Man series\Mega Man Metal Army.nes 
    0xA475AEDD, //  Homebrew\Hacks\Mega Man series\Mega Man Redux [Amaweks].nes 
    0xA2A196DD, //  Homebrew\Hacks\Mega Man series\Mega Man Reloaded - Bass Edition (v1.4b)[Zieldak].nes 
    0x95DE270E, //  Homebrew\Hacks\Mega Man series\Mega Man Reloaded - Mega Man Edition (v1.4m)[Zieldak].nes 
    0x41D77076, //  Homebrew\Hacks\Mega Man series\Mega Man Reved Up!!.nes 
    0x12C16793, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown I [Dragon Eye Studios].nes 
    0x4FD994C2, //  Homebrew\Hacks\Mega Man series\Mega Man Simplified (v2.0)[MG64].nes 
    0x1F2FFF17, //  Homebrew\Hacks\Mega Man series\Mega Man [Googie].nes 
    0x7655D26B, //  Homebrew\Hacks\Mega Man series\Mega Man in Icarus World.nes 
    0xD202ABA7, //  Homebrew\Hacks\Mega Man series\Mega Man in the Mushroom Kingdom.nes 
    0x6E1B9BBB, //  Homebrew\Hacks\Mega Man series\Mega Man꞉ Return of Wily [Jimmy52905].nes 
    0xEDD8889A, //  Homebrew\Hacks\Mega Man series\Mega Mari.nes 
    0x19D3FD42, //  Homebrew\Hacks\Mega Man series\Rockman 2000 [Kuwata].nes 
    0xE801BAFA, //  Homebrew\Hacks\Mega Man series\Rockman EX [AYCNN].nes 
    0xF555C780, //  Homebrew\Hacks\Mega Man series\Rockman Kai.nes 
    0xC3D66255, //  Homebrew\Hacks\Mega Man series\Rockman Kenk.nes 
    0x331B4C61, //  Homebrew\Hacks\Mega Man series\Rockman NEO.nes 
    0x39CBA47E, //  Homebrew\Hacks\Mega Man series\Rockman RX.nes 
    0xFEADE8CB, //  Homebrew\Hacks\Mega Man series\Rockman Zero [MB Hacks].nes 
    0xD7CD7E8E, //  Homebrew\Panesian Power.nes 
    0x951AF0C1, //  Homebrew\Super Marxsky Comrade.nes 
    0x9280687F, //  Homebrew\The Incident (Demo).nes 
    0xF500383F, //  Homebrew\The Rise of Amondus.nes 
    0x78A40BD5, //  Homebrew\Tools\Tommy T's Sound Editor.nes 
    0xF7594BB0, //  Homebrew\Translations\Chinese (Simplified)\假面的忍者赤影 [ms].nes 
    0x5515D92F, //  Homebrew\Translations\English\Aighina's Prophecy, from ''The Legend of Balubalouk'' [Psyklax].nes 
    0x0ACC007E, //  Homebrew\Translations\English\Aiginaの予言꞉ Balubaloukの伝説より [KP Hacks &amp; Translations].nes 
    0xAF31A310, //  Homebrew\Translations\English\Argosの戦士꞉ はちゃめちゃ大進撃 [Toma].nes 
    0xDCB6B0DA, //  Homebrew\Translations\English\Attack Animal Academy [PentarouZero].nes 
    0x5D901BFE, //  Homebrew\Translations\English\Batsu&amp;Terry [Zynk Oxhyde].nes 
    0x69E4F953, //  Homebrew\Translations\English\Burnin' TwinBee꞉ The Rescue of Dr. Cinnamon! [DvD Translations revA].nes 
    0x42BFB569, //  Homebrew\Translations\English\Burnin' TwinBee꞉ The Rescue of Dr. Cinnamon! [MrRichard999 v0.90b].nes 
    0xF57DEAD7, //  Homebrew\Translations\English\Chester Field [Aeon Genesis].nes 
    0x3D5FBD64, //  Homebrew\Translations\English\City Adventure Touch꞉ Mystery of Triangle [filler].nes 
    0xB8A6AB09, //  Homebrew\Translations\English\Daiva Story 6꞉ Imperial of Nirsartia [MrRichard999].nes 
    0xCDCB918D, //  Homebrew\Translations\English\Delinquent Girl Detective III [MrRichard999 v0.95].nes 
    0x5523A727, //  Homebrew\Translations\English\Demon World Village [BlackPaladin].nes 
    0x4E6F2567, //  Homebrew\Translations\English\Dr. Jekyll and Mr. Hyde [Yauch].nes 
    0xCEA04EE7, //  Homebrew\Translations\English\Empire City 1931 [Psyklax].nes 
    0x60B0FEBB, //  Homebrew\Translations\English\Fishing Maniac Sanpei꞉ Blue Marlin Chapter [MrRichard999 v0.94].nes 
    0x4226F280, //  Homebrew\Translations\English\Flowering Star Highway [GAFF Translations](Engrish Edition).nes 
    0xF50A92B3, //  Homebrew\Translations\English\Flowering Star Highway [GAFF Translations].nes 
    0x8E5D7C79, //  Homebrew\Translations\English\Hello Kitty World [Flake].nes 
    0xE214B790, //  Homebrew\Translations\English\Hello Kitty World [Hello Kitty Kicks Ass Translations].nes 
    0x2A3D3E21, //  Homebrew\Translations\English\Higemaru꞉ Seven Islands Adventure [Snark].nes 
    0x4D92A725, //  Homebrew\Translations\English\JJ - Jump Out! The Epic War Part II [DvD Translations].nes 
    0xA2549FC6, //  Homebrew\Translations\English\JJ - The 3-D Battles of WorldRunner Part II [DvD Translations].nes 
    0xAFEC6D5F, //  Homebrew\Translations\English\Knightmare II꞉ The Maze of Galious [Stardust Crusaders].nes 
    0xF41D5373, //  Homebrew\Translations\English\Labyrinth [Suicidal Translations Beta].nes 
    0x647DF687, //  Homebrew\Translations\English\Lost Word of Jenny [Zynk Oxhyde].nes 
    0xC5A4350E, //  Homebrew\Translations\English\Ninja Jajamaru in Asura's Wrath [Aishsha].nes 
    0x9046B3CD, //  Homebrew\Translations\English\River City Ransom Zero [Vice Translations v1.01].nes 
    0xA3D25DA6, //  Homebrew\Translations\English\Shufflepuck Café [MadHacker].nes 
    0x32031918, //  Homebrew\Translations\English\Spelunker II꞉ 勇者への挑戦 [Sarysa v0.9].nes 
    0xCAECCFA7, //  Homebrew\Translations\English\Spy vs. Spy꞉ The Island Caper!! [Pacnsacdave].nes 
    0x0B01C96D, //  Homebrew\Translations\English\Suddenly! Machoman [Pacnsacdave].nes 
    0x9707861A, //  Homebrew\Translations\English\Super Star Force [GAFF Translations].nes 
    0x4454B8E8, //  Homebrew\Translations\English\The Masked Ninja Akakage [Zynk Oxhyde].nes 
    0x4D0185F1, //  Homebrew\Translations\English\Treasures of Elnark [MrRichard999].nes 
    0x65A891B6, //  Homebrew\Translations\English\Valis꞉ The Fantasm Soldier [Satsu &amp; Sliver X v1.0].nes 
    0x4A797501, //  Homebrew\Translations\English\Valis꞉ The Fantasm Soldier [Satsu &amp; Sliver X v1.1].nes 
    0xDBDBBEA7, //  Homebrew\Translations\English\Wit's [Suicidal Translations].nes 
    0x389235E4, //  Homebrew\Translations\English\Woody Poko [Sqpat Final].nes 
    0xD33B0B5B, //  Homebrew\Translations\English\北斗の拳 2꞉ 世紀末救世主伝説 [Extreme Fun Translations v0.50].nes 
    0x8C46A487, //  Homebrew\Translations\English\忍者くん꞉ 阿修羅の章 [The Spoony Bard v1.0].nes 
    0xE290EE90, //  Homebrew\Translations\English\魔城伝説 II꞉ 大魔司教 Galious [Manipulate v0.31b].nes 
    0x9D56BCAC, //  Homebrew\Translations\Russian\Bat &amp; Terry.nes 
    0x43FB36BD, //  Homebrew\Translations\Russian\Contra.nes 
    0x15B1CBBD, //  Homebrew\Translations\Russian\DuckTales III.nes 
    0xAF2A79DC, //  Homebrew\Translations\Russian\Jimmy Connors Tennis.nes 
    0x3A47BA55, //  Homebrew\Translations\Russian\Prince of Persia.nes 
    0x04CCE62C, //  Homebrew\Translations\Russian\The Little Mermaid.nes 
    0x5871D9B8, //  Homebrew\Translations\Russian\超级魂 Super Contra II.nes 
    0x3C04E8EF, //  Homebrew\Zombie Zap.nes 
    0x37F59450, //  Licensed Japan\Aiginaの予言꞉ Balubaloukの伝説より.nes 
    0xE1B260DA, //  Licensed Japan\Argosの戦士꞉ はちゃめちゃ大進撃.nes 
    0x8BCDE59A, //  Licensed Japan\Athena.nes 
    0x7D55CF29, //  Licensed Japan\Attack Animal 学園.nes 
    0x484A60DB, //  Licensed Japan\Ballblazer.nes 
    0xF989296C, //  Licensed Japan\Bat &amp; Terry꞉ 魔境の鉄人Race.nes 
    0xBD9D0E85, //  Licensed Japan\Bomber King.nes 
    0x6C70A17B, //  Licensed Japan\Chester Field꞉ 暗黒神への挑戦.nes 
    0xB834EB30, //  Licensed Japan\City Adventure 'Touch'꞉ Mystery of Triangle.nes 
    0x721B5217, //  Licensed Japan\Daiva꞉ Nirsartiaの玉座.nes 
    0x1ED48C5C, //  Licensed Japan\Dash 野郎.nes 
    0x48349B0B, //  Licensed Japan\Dragon Quest II꞉ 悪霊の神々.nes 
    0x804F898A, //  Licensed Japan\Dragon Unit.nes 
    0xEDDCC468, //  Licensed Japan\DuckTales 2.nes 
    0xF450DB3A, //  Licensed Japan\Elnarkの財宝.nes 
    0x07910BF9, //  Licensed Japan\Family Quiz꞉ 4人は Rival.nes 
    0xDAD88CC5, //  Licensed Japan\Guardic 外伝.nes 
    0x784272F2, //  Licensed Japan\Hector '87.nes 
    0x67D5C3F9, //  Licensed Japan\Hello Kitty World.nes 
    0xB15653BD, //  Licensed Japan\Heraklesの栄光꞉ 闘人魔境伝.nes 
    0xF7893859, //  Licensed Japan\Higemaru 魔界島꞉ 七つの島大冒険.nes 
    0xB39A3F5B, //  Licensed Japan\JJ - とびだせ大作戦 Part 2.nes 
    0x96BA90B0, //  Licensed Japan\Jekyll 博士の彷魔が刻.nes 
    0xF6139EE9, //  Licensed Japan\Labyrinth.nes 
    0xAE128FAC, //  Licensed Japan\Law of the West.nes 
    0xEA31CCD3, //  Licensed Japan\Layla.nes 
    0x3CD6BB0E, //  Licensed Japan\Lost Word of Jenny.nes 
    0x8E7ABDFC, //  Licensed Japan\Magnum 危機一髪 - Empire City꞉ 1931.nes 
    0x805F81BC, //  Licensed Japan\Metal Gear.nes 
    0x9F8336DB, //  Licensed Japan\Miracle Ropit's Adventure in 2100.nes 
    0x7AA02377, //  Licensed Japan\Outlanders.nes 
    0x01609F57, //  Licensed Japan\Pro Wres - Famicom Wrestling Association.nes 
    0x35EFFD0E, //  Licensed Japan\Rambo꞉ First Blood Part II.nes 
    0xD31DC910, //  Licensed Japan\Rockman.nes 
    0x7C3D2EA3, //  Licensed Japan\SWAT꞉ Special Weapons and Tactics.nes 
    0x2BB6A0F8, //  Licensed Japan\Sherlock Holmes꞉ 伯爵令嬢誘拐事件.nes 
    0x917D9262, //  Licensed Japan\Shufflepuck Café.nes 
    0xC37F225C, //  Licensed Japan\Spelunker 2꞉ 勇者への挑戦.nes 
    0x5440811C, //  Licensed Japan\Stick Hunter꞉ Exciting Ice Hockey.nes 
    0x1C66BAF6, //  Licensed Japan\Super Pitfall.nes 
    0x3FA96277, //  Licensed Japan\Super Star Force꞉ 時空暦の秘密.nes 
    0x99A62E47, //  Licensed Japan\The Black Bass II.nes 
    0xB04BA659, //  Licensed Japan\The Black Bass.nes 
    0x92F04530, //  Licensed Japan\The Goonies II꞉ Fratelli 最後の挑戦.nes 
    0x2FFDE228, //  Licensed Japan\Times of Lore.nes 
    0x3CF67AEC, //  Licensed Japan\Twin Eagle꞉ Revenge Joe's Brother.nes 
    0xF3808245, //  Licensed Japan\WWF WrestleMania Challenge.nes 
    0xB1B16B8A, //  Licensed Japan\Wit's.nes 
    0xADB810F8, //  Licensed Japan\Woody Poko.nes 
    0xD0DF726E, //  Licensed Japan\Zoids꞉ 中央大陸の戦い (rev0).nes 
    0xE0B6B7BB, //  Licensed Japan\Zoids꞉ 中央大陸の戦い (rev1).nes 
    0xDB196068, //  Licensed Japan\たけしの挑戦状.nes 
    0x9F03B11F, //  Licensed Japan\もえろ TwinBee꞉ Cinnamon 博士を救え!.nes 
    0xDCB7C0A1, //  Licensed Japan\わんぱく Duck 夢冒険.nes 
    0x8324A464, //  Licensed Japan\カケフくんのJump天国꞉ Speed地獄.nes 
    0x5229FCDD, //  Licensed Japan\キョロちゃん Land.nes 
    0x02863604, //  Licensed Japan\スケバン刑事 III.nes 
    0xF64CB545, //  Licensed Japan\中国占星術.nes 
    0x8E066CCB, //  Licensed Japan\井出洋介名人の実戦麻雀 (rev0).nes 
    0x9FAE4D46, //  Licensed Japan\井出洋介名人の実戦麻雀 (rev1).nes 
    0x64C0FA3B, //  Licensed Japan\人魚姫.nes 
    0x28C1D3D5, //  Licensed Japan\仮面の忍者 赤影.nes 
    0xD1F7DF3A, //  Licensed Japan\北斗の拳 2꞉ 世紀末救世主伝説.nes 
    0x10D62149, //  Licensed Japan\南国指令!! Spy vs. Spy.nes 
    0xC76AADF4, //  Licensed Japan\夢幻戦士 Valis.nes 
    0x219DFABF, //  Licensed Japan\忍者くん꞉ 阿修羅の章.nes 
    0xA0F99BB8, //  Licensed Japan\悪魔城 Dracula (Re-release).nes 
    0x2061772A, //  Licensed Japan\探偵神宮寺三郎꞉ 時の過ぎゆくままに.nes 
    0x3DCADA42, //  Licensed Japan\星をみるひと.nes 
    0xC5CFE54E, //  Licensed Japan\時空の旅人 - Time Stranger.nes 
    0x0A42D84F, //  Licensed Japan\武田信玄.nes 
    0x2AC5233C, //  Licensed Japan\火の鳥 鳳凰編꞉ 我王の冒険.nes 
    0xA7D3635E, //  Licensed Japan\熱血硬派 くにおくん.nes 
    0xE24DF353, //  Licensed Japan\突然! Machoman.nes 
    0x6A6B7239, //  Licensed Japan\花のStar 街道.nes 
    0x69D07DDB, //  Licensed Japan\西遊記 World.nes 
    0x0EAA7515, //  Licensed Japan\要請物語 Rod Land.nes 
    0x5A18F611, //  Licensed Japan\釣りキチ三平꞉ Blue Marlin 編.nes 
    0x8A368744, //  Licensed Japan\銀河の三人.nes 
    0xBE95B219, //  Licensed Japan\闘いの挽歌 (rev0).nes 
    0x60E563F1, //  Licensed Japan\闘いの挽歌 (rev1).nes 
    0x0B8E8649, //  Licensed Japan\飛龍の拳꞉ 奥義の書.nes 
    0x4536FE1C, //  Licensed Japan\魔城伝説 II꞉ 大魔司教 Galious.nes 
    0xBF3635CF, //  Licensed Japan\魔界村.nes 
    0xE2B43A68, //  Licensed North America\Amagon.nes 
    0xF304F1B9, //  Licensed North America\Archon꞉ The Light and the Dark.nes 
    0x27DDF227, //  Licensed North America\Athena.nes 
    0x8AB52A24, //  Licensed North America\Blades of Steel.nes 
    0xDB1D03E5, //  Licensed North America\Caesars Palace.nes 
    0x2F66E302, //  Licensed North America\California Games.nes 
    0x0AE6C9E2, //  Licensed North America\Castelian.nes 
    0x2F2D1FA9, //  Licensed North America\Castle of Dragon.nes 
    0x0AC1AA8F, //  Licensed North America\Castlevania (rev0).nes 
    0xB668C7FC, //  Licensed North America\Castlevania (rev1).nes 
    0xCDC641FC, //  Licensed North America\Championship Pool.nes 
    0xA3C0D49F, //  Licensed North America\Classic Concentration.nes 
    0x0123BFFE, //  Licensed North America\Color a Dinosaur.nes 
    0xF6035030, //  Licensed North America\Contra.nes 
    0xD738C059, //  Licensed North America\Dick Tracy.nes 
    0x437E7B69, //  Licensed North America\Double Dribble (rev0).nes 
    0xD0E96F6B, //  Licensed North America\Double Dribble (rev1).nes 
    0x73620901, //  Licensed North America\Dr. Chaos.nes 
    0xCA033B3A, //  Licensed North America\Dragon's Lair.nes 
    0x73C7FCF4, //  Licensed North America\DuckTales 2.nes 
    0xEFB09075, //  Licensed North America\DuckTales.nes 
    0x06D72C83, //  Licensed North America\Fist of the North Star.nes 
    0xF92BE7F2, //  Licensed North America\Flight of the Intruder.nes 
    0xF1FED9B8, //  Licensed North America\Flying Dragon꞉ The Secret Scroll.nes 
    0x576A0DE8, //  Licensed North America\Fun House.nes 
    0xB4E4879E, //  Licensed North America\Ghosts'n Goblins.nes 
    0x3869E598, //  Licensed North America\Hollywood Squares.nes 
    0xC313EF54, //  Licensed North America\Jack Nicklaus' Greatest 18 Holes of Major Championship Golf.nes 
    0x1D5B03A5, //  Licensed North America\Jackal.nes 
    0x00E95D86, //  Licensed North America\Jimmy Connors Tennis.nes 
    0x303D4371, //  Licensed North America\Jordan vs. Bird꞉ One on One.nes 
    0xAA6BB985, //  Licensed North America\Kid Kool and the Quest for the Seven Wonder Herbs.nes 
    0xA2194CAD, //  Licensed North America\Legendary Wings.nes 
    0xC4BC85A2, //  Licensed North America\Life Force (rev0).nes 
    0x4061D767, //  Licensed North America\Life Force (rev1).nes 
    0xFD8D6C75, //  Licensed North America\Loopz.nes 
    0x6EE4BB0A, //  Licensed North America\Mega Man.nes 
    0x817431EC, //  Licensed North America\Metal Gear.nes 
    0x35B6FEBF, //  Licensed North America\NFL Football.nes 
    0x70CE3771, //  Licensed North America\Prince of Persia.nes 
    0xE6F08E93, //  Licensed North America\Pro Wrestling (rev0).nes 
    0x64B710D2, //  Licensed North America\Pro Wrestling (rev1).nes 
    0x6E0EB43E, //  Licensed North America\Puss 'n Boots꞉ Pero's Great Adventure.nes 
    0xE1C41D7C, //  Licensed North America\Rally Bike.nes 
    0xA342A5FD, //  Licensed North America\Rambo (rev0).nes 
    0x4F9DBBE5, //  Licensed North America\Rambo (rev1).nes 
    0xA0568E1D, //  Licensed North America\Renegade.nes 
    0x810B7AB9, //  Licensed North America\Robo Warrior.nes 
    0xDE25B90F, //  Licensed North America\Rush'n Attack.nes 
    0xB3783F2A, //  Licensed North America\Rygar (rev0).nes 
    0x37C474D5, //  Licensed North America\Rygar (rev1).nes 
    0x0FEC90D2, //  Licensed North America\Section-Z.nes 
    0x5E767671, //  Licensed North America\Silent Service (rev0).nes 
    0xB843EB84, //  Licensed North America\Silent Service (rev1).nes 
    0x423ADA8E, //  Licensed North America\Skate or Die!.nes 
    0x9F432594, //  Licensed North America\Starship Hector.nes 
    0xC5B0B1AB, //  Licensed North America\Stinger.nes 
    0x419461D0, //  Licensed North America\Super Cars.nes 
    0xEFCF375D, //  Licensed North America\Super Glove Ball.nes 
    0x979C5314, //  Licensed North America\Super Pitfall.nes 
    0x3417EC46, //  Licensed North America\Swords and Serpents.nes 
    0xE6A477B2, //  Licensed North America\The 3-D Battles of World Runner.nes 
    0x55773880, //  Licensed North America\The Adventures of Gilligan's Island.nes 
    0x1D0F4D6B, //  Licensed North America\The Black Bass USA.nes 
    0x9F2EEF20, //  Licensed North America\The Blues Brothers.nes 
    0x999577B6, //  Licensed North America\The Goonies II.nes 
    0xFA43146B, //  Licensed North America\The Guardian Legend.nes 
    0x3BE244EF, //  Licensed North America\The Little Mermaid.nes 
    0x2D41EF92, //  Licensed North America\The Uncanny X-Men.nes 
    0x0F86FEB4, //  Licensed North America\Times of Lore.nes 
    0x248566A7, //  Licensed North America\Total Recall.nes 
    0xEE6892EB, //  Licensed North America\Trog.nes 
    0xFC3E5C86, //  Licensed North America\Trojan.nes 
    0xCF26A149, //  Licensed North America\Twin Eagle꞉ Revenge Joe's Brother.nes 
    0xA0230D75, //  Licensed North America\WWF WrestleMania Challenge.nes 
    0x15F0D3F1, //  Licensed North America\Wayne Gretzky Hockey.nes 
    0x38431BF6, //  Modern\8 Bit Xmas 2018.nes 
    0xD8AB6288, //  Modern\8 Bit Xmas 2021꞉ Exciteduck (download).nes 
    0x7D563942, //  Modern\Castlevania (Castlevania Anniversary Collection).nes 
    0xACEC652E, //  Modern\Castlevania (Konami Collector's Series).nes 
    0x755B7D59, //  Modern\Contra (Contra Anniversary Collection).nes 
    0x7708B286, //  Modern\Contra (Konami Collector's Series).nes 
    0x45C33A1B, //  Modern\DuckTales (The Disney Afternoon Collection).nes 
    0x57085979, //  Modern\DuckTales 2 (The Disney Afternoon Collection).nes 
    0x5F0DEC80, //  Modern\Hudson Best Collection\Hector '87.nes 
    0x718EB40A, //  Modern\Jackal (Konami Collector's Series).nes 
    0x1C47D202, //  Modern\Mega Man (Mega Man Legacy Collection).nes 
    0x825313CA, //  Modern\The Rise of Amondus.nes 
    0x1394B713, //  Modern\Virtual Console\Blades of Steel (NTSC).nes 
    0xBD5932EC, //  Modern\Virtual Console\Castlevania (NTSC).nes 
    0xAEF5B452, //  Modern\Virtual Console\Mega Man.nes 
    0x1CA40FAF, //  Modern\Virtual Console\Renegade.nes 
    0xA7A82B60, //  Modern\Virtual Console\Rockman.nes 
    0x87E8CA9C, //  Modern\Virtual Console\Stinger.nes 
    0xCBD7D955, //  Modern\Virtual Console\たけしの挑戦状.nes 
    0x72EEED23, //  Modern\Virtual Console\もえろ TwinBee꞉ Cinnamon博士を救え!.nes 
    0x4BAE96C2, //  Modern\Virtual Console\熱血硬派くにおくん.nes 
    0x87CF8EF6, //  Modern\悪魔城 Dracula (Castlevania Anniversary Collection).nes 
    0x7F8779F4, //  Playchoice\Castlevania.nes 
    0xD431825B, //  Playchoice\Contra.nes 
    0x107405C7, //  Playchoice\Double Dribble.nes 
    0x29B2F488, //  Playchoice\Pro Wrestling.nes 
    0xEFE2D423, //  Playchoice\Rush'n Attack.nes 
    0xC88337AE, //  Playchoice\Rygar.nes 
    0x989BAF84, //  Playchoice\Trojan.nes 
    0xAE5B4B2D, //  Samples\Armed for Battle.nes 
    0xDAAB873B, //  Samples\Spook-o'-tron (v1.1).nes 
    0x85BFFFEF, //  Samples\The Goonies II꞉ Fratelli 最後の挑戦.nes 
    0xE85B4D3D, //  Unlicensed Taiwan&amp;Hong Kong\Hit Marmot.nes 
    0xED4D696F, //  Unlicensed Taiwan&amp;Hong Kong\Magic Block.nes 
    0x239971D1, //  Unlicensed Taiwan&amp;Hong Kong\撞球傳說 花撞 II - Ball Story.nes 
    0x0143EEB4, //  Unreleased\Airball (Prototype 1).nes 
    0x240863B9, //  Unreleased\Airball (Prototype 2).nes 
    0xD02BB85C, //  Unreleased\Astérix (NTSC)[Acmlm, WIP].nes 
    0xA48D26C1, //  Unreleased\Caesars Palace.nes 
    0x8D97155C, //  Unreleased\California Raisins꞉ The Grape Escape (Beta).nes 
    0xF2096D9C, //  Unreleased\California Raisins꞉ The Grape Escape (Final).nes 
    0x53328FC4, //  Unreleased\Chester Field꞉ 暗黒神への挑戦.nes 
    0xCB17D41E, //  Unreleased\Color a Dinosaur.nes 
    0x2C26736B, //  Unreleased\DuckTales (1989-02-05).nes 
    0xFF53D73E, //  Unreleased\DuckTales (1989-05-25).nes 
    0x55B4052B, //  Unreleased\Makai Island.nes 
    0x3B0F4DB2, //  Unreleased\The Adventures of Dr. Franken.nes 
    0x8366CF72, //  Unreleased\Titan Warriors.nes 
    0x55568F0D, //  Unreleased\UWC.nes 
    0x98C7B4DA, //  Unreleased\わんぱくDuck 夢冒険.nes 
    0xEBB5E666, //  Unreleased\人魚姫.nes 
    0xFBDD0F1B, //  Unreleased\突然! Machoman.nes 
    0xD44FBB05, //  Unreleased\西遊記 World.nes 
/**** 164 mapper=2 submapper=2 prgrom=128K chrram=8K mirror=V system=SYSTEM_PAL */
    0x7548616F, //  Homebrew\Hacks\All Hallow's Eve [Plimoth League Studios].nes 
    0xB8141DA3, //  Homebrew\Hacks\Mega Man series\Danzig [Punktendo].nes 
    0xED77B453, //  Licensed PAL\Astérix.nes 
    0xC0EDEDD0, //  Licensed PAL\Blades of Steel.nes 
    0xB400172A, //  Licensed PAL\California Games.nes 
    0xCF7CA9BD, //  Licensed PAL\Castelian.nes 
    0xA93527E2, //  Licensed PAL\Castlevania.nes 
    0x7F4CB1B4, //  Licensed PAL\Double Dribble.nes 
    0x096D8364, //  Licensed PAL\DuckTales 2 (English).nes 
    0x30C5E6CF, //  Licensed PAL\DuckTales 2 (French).nes 
    0x0D14285A, //  Licensed PAL\DuckTales 2 (German).nes 
    0xD029F841, //  Licensed PAL\DuckTales.nes 
    0x9369A2F8, //  Licensed PAL\Ghosts'n Goblins.nes 
    0x836FE2C2, //  Licensed PAL\Jack Nicklaus' Greatest 18 Holes of Major Championship Golf.nes 
    0xB80192B7, //  Licensed PAL\Jimmy Connors Tennis.nes 
    0xD161888B, //  Licensed PAL\Kick Off.nes 
    0x7002FE8D, //  Licensed PAL\Life Force - Salamander.nes 
    0x94476A70, //  Licensed PAL\Mega Man.nes 
    0x84C4A12E, //  Licensed PAL\Metal Gear.nes 
    0x18027A1F, //  Licensed PAL\Phantom Air Mission.nes 
    0xB7F39933, //  Licensed PAL\Prince of Persia (English).nes 
    0x8293803A, //  Licensed PAL\Prince of Persia (French).nes 
    0x60AA9AE0, //  Licensed PAL\Prince of Persia (German).nes 
    0x4B1ED62E, //  Licensed PAL\Prince of Persia (Swedish).nes 
    0x38DE7053, //  Licensed PAL\Pro Wrestling.nes 
    0xB13F00D4, //  Licensed PAL\Probotector.nes 
    0x99A28276, //  Licensed PAL\Robo Warrior.nes 
    0x22AB9694, //  Licensed PAL\Rod Land, Featuring Rit and Tam.nes 
    0xE0AC6242, //  Licensed PAL\Rush'n Attack.nes 
    0x8F197B0A, //  Licensed PAL\Rygar.nes 
    0x45A9DB6F, //  Licensed PAL\Section-Z.nes 
    0xFA014BA1, //  Licensed PAL\Silent Service.nes 
    0x66EBDB64, //  Licensed PAL\Skate or Die!.nes 
    0xD153CAF6, //  Licensed PAL\Swords and Serpents (English).nes 
    0x46135141, //  Licensed PAL\Swords and Serpents (French).nes 
    0xE4776A2B, //  Licensed PAL\The Blues Brothers.nes 
    0x8897A8F1, //  Licensed PAL\The Goonies II.nes 
    0xFE907015, //  Licensed PAL\The Guardian Legend.nes 
    0x4022C94E, //  Licensed PAL\The Smurfs.nes 
    0xFC3236D1, //  Licensed PAL\Total Recall.nes 
    0xB6B5C372, //  Licensed PAL\Trog.nes 
    0x4F48B240, //  Licensed PAL\Trojan.nes 
    0x138862C5, //  Licensed PAL\WWF WrestleMania Challenge.nes 
    0x811029EE, //  Modern\Virtual Console\Blades of Steel (PAL).nes 
    0xD1DF88C6, //  Modern\Virtual Console\Life Force - Salamander.nes 
    0x89EC53C8, //  Unreleased\DuckTales 2.nes 
    0x36C3B13A, //  Unreleased\Rod Land, Featuring Rit and Tam.nes 
/**** 165 mapper=2 submapper=2 prgrom=128K chrram=8K mirror=V system=SYSTEM_NTSC */
    0xF968E392, //  Homebrew\Scramble.nes 
    0x3EBB0E6D, //  Homebrew\The Cowlitz Gamers' 2nd Adventure.nes 
    0x3C882B42, //  Modern\Chumlee Adventure꞉ The Quest for Pinky.nes 
    0xF8D71993, //  Modern\L'Abbaye des Morts.nes 
    0x894FA1A1, //  Modern\Streemerz (v01 English).nes 
    0xE8F57BCF, //  Modern\Streemerz (v02 English).nes 
    0x2D80C6DE, //  Modern\Streemerz (v02 French).nes 
    0x534F11AE, //  Modern\Virtual Console\Castlevania (PAL).nes 
/**** 166 mapper=2 submapper=2 prgrom=128K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x68A6D938, //  Compatibility Hacks\TV System Conversions\Astérix (Dendy)[NewRisingSun].nes 
    0xED4984E0, //  Unlicensed China\跳舞与卡拉OK.nes 
/**** 167 mapper=2 submapper=2 prgrom=256K chrram=8K mirror=V */
    0x92042049, //  Bootleg Singles\Bung Game Doctor\The Rocketeer.nes 
    0x6ABE6C87, //  Homebrew\Translations\English\Fiery Renegade Kunio [Stardust Crusaders v1.00].nes 
    0x1760CF9D, //  Homebrew\Translations\English\Ghost Hunter [Snark, Proveaux].nes 
    0x19FEEA1F, //  Homebrew\Translations\English\Law of the West [GAFF Translations].nes 
    0xDD0D56A4, //  Homebrew\Translations\English\Outlanders [GAFF Translations v1.01].nes 
    0x729F18CB, //  Homebrew\Translations\English\Phoenix Karma꞉ Gao's Adventure [Stardust Crusaders v1.01].nes 
    0xD0419F4C, //  Homebrew\Translations\English\SWAT꞉ Special Weapons and Tactics [Psyklax].nes 
    0xB79A0CEE, //  Homebrew\Translations\English\Saiyuuki World [Nebulous Translations v0.95].nes 
    0x4F123CC4, //  Homebrew\Translations\English\Sherlock Holmes꞉ The Kidnapped Countess [Psyklax v1.1].nes 
    0x5F1F191E, //  Homebrew\Translations\English\Spelunker II꞉ A Hero's Challenge [Stardust Crusaders].nes 
    0x357426F7, //  Homebrew\Translations\English\Takeshi's Challenge [KingMike's Translations].nes 
    0x5FF91792, //  Homebrew\Translations\English\The Glory of Heracles ꞉ Labors of the Divine Hero [DvD Translations revB].nes 
    0x7B0A41B9, //  Licensed Japan\Esper 冒険隊꞉ 魔王の砦.nes 
    0x3DA2085E, //  Licensed Japan\Maniac Mansion.nes 
    0xD8F651E2, //  Licensed Japan\星霊狩り.nes 
    0x1027C432, //  Licensed Japan\桃太郎電鉄.nes 
    0xA6A725B8, //  Licensed North America\Best of the Best꞉ Championship Karate.nes 
    0x566F1F66, //  Modern\F-Theta [Little Sound].nes 
/**** 168 mapper=2 submapper=2 prgrom=256K chrram=8K mirror=V system=SYSTEM_PAL */
    0xBD339E75, //  Licensed PAL\Best of the Best꞉ Championship Karate.nes 
/**** 169 mapper=2 submapper=2 prgrom=256K chrram=8K mirror=V system=SYSTEM_NTSC */
    0xA0B8CB59, //  Samples\Roniu's Tale.nes 
    0xCAB97150, //  Samples\The Meating (Cafeparty Demo).nes 
/**** 170 mapper=2 submapper=2 prgrom=256K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x18832471, //  Educational Computers\小学语文数学智力游戏 趣味教学卡 - 小状元.nes 
/**** 171 mapper=2 submapper=2 prgrom=512K chrram=8K mirror=V */
    0x6DC55447, //  Homebrew\Translations\English\Esper Corps [Aishsha v1.01].nes 
/**** 172 mapper=2 submapper=2 prgrom=256K prgnvram=8K chrram=8K mirror=V battery=1 */
    0xC86DD855, //  Homebrew\Translations\English\Stargazers [KingMike's Translations v0.80](SRAM Hack).nes 
    0x9CEB6D70, //  Homebrew\Translations\English\Stargazers [KingMike's Translations v0.80].nes 
/**** 173 mapper=2 submapper=2 prgrom=512K prgnvram=8K chrram=8K mirror=V battery=1 system=SYSTEM_NTSC */
    0x10066391, //  Modern\Alwa's Awakening.nes 
/**** 174 mapper=3 prgrom=32K chrrom=32K mirror=H */
    0xE01389E9, //  Homebrew\Translations\English\Cosmo Genesis [Aeon Genesis v1.1].nes 
/**** 175 mapper=3 prgrom=32K chrrom=32K mirror=V */
    0xFC8EB63A, //  Modern\Virtual Console\迷宮組曲꞉ Milonの大冒険.nes 
/**** 176 mapper=3 submapper=1 prgrom=32K chrrom=16K mirror=H */
    0x5E1ADD91, //  Bootleg Singles\Bung Game Doctor\(E016) 響尾蛇 - Sidewinder.nes 
/**** 177 mapper=3 submapper=1 prgrom=32K chrrom=32K mirror=H */
    0x21178D4D, //  Bootleg Hacks\FIFA 99.nes 
    0xD04A40E6, //  Compatibility Hacks\75 Bingo [m003].nes 
    0x2915FAF0, //  Compatibility Hacks\蝶變 - Incantation [m003].nes 
/**** 178 mapper=3 submapper=1 prgrom=32K prgram=2K chrrom=32K mirror=H */
    0xB3BE2F71, //  Multicarts\extracts\超一流 烟山杯 围棋.nes 
/**** 179 mapper=3 submapper=1 prgrom=16K chrrom=32K mirror=V */
    0x189ACA12, //  Homebrew\ファミ姦.nes 
/**** 180 mapper=3 submapper=1 prgrom=32K chrrom=16K mirror=V */
    0xC7D0AF96, //  Bootleg Singles\Bung Game Doctor\(D002) 忍者じゃじゃ丸くん.nes 
/**** 181 mapper=3 submapper=1 prgrom=32K chrrom=32K mirror=V */
    0x1D31AED1, //  Bootleg Singles\Bung Game Doctor\(F002) Atlantisの謎.nes 
    0x45A9C9A7, //  Bootleg Singles\Bung Game Doctor\(F005) かんしゃく玉投げカン太郎の東海道五十三次.nes 
    0xF283CF58, //  Bootleg Singles\Bung Game Doctor\Colorful Dragon.nes 
    0xB9FD16E1, //  Bootleg Singles\Bung Game Doctor\Poker 精靈.nes 
    0x2DEB12B8, //  Compatibility Hacks\Venice Beach Volley (Super Mega)[m003].nes 
    0xE8BF92F7, //  Homebrew\Hacks\爆チュー問題.nes 
    0xB1857F3B, //  Homebrew\Tetris.nes 
    0x70852046, //  Modern\Hudson Best Collection\高橋名人の冒険島.nes 
/**** 182 mapper=3 submapper=2 prgrom=16K chrrom=16K mirror=H */
    0xBE387AF0, //  Licensed North America\Joust.nes 
/**** 183 mapper=3 submapper=2 prgrom=16K chrrom=32K mirror=H system=SYSTEM_NTSC */
    0x973DFED9, //  Homebrew\Drakaina.nes 
/**** 184 mapper=3 submapper=2 prgrom=32K chrrom=16K mirror=H */
    0xF9443A53, //  Bootleg Singles\Bung Game Doctor\(E001) TwinBee.nes 
    0x6ED1EC0B, //  Bootleg Singles\Bung Game Doctor\(E004) 影の伝説.nes 
    0x60CCE95D, //  Bootleg Singles\Bung Game Doctor\(E005) Mighty Bomb Jack.nes 
    0xDE134F07, //  Bootleg Singles\Bung Game Doctor\(E011) Banana.nes 
    0x0E30EAC6, //  Bootleg Singles\Bung Game Doctor\(E012) Arkanoid.nes 
    0x19E69767, //  Bootleg Singles\Bung Game Doctor\Poker II.nes 
    0x0B3D7B44, //  Bootleg Singles\Front Fareast Magic Card\(F1115) Mighty Bomb Jack.nes 
    0xED3402D2, //  Compatibility Hacks\Vs. Pinball [home console v1.3].nes 
    0x0F92E654, //  Homebrew\Hacks\Arkanoid 11 [Bloodstar].nes 
    0x07CC0E99, //  Homebrew\Hacks\Arkanoid Revised (v1.03)[Nesrocks].nes 
    0x9F91F9D0, //  Homebrew\Hacks\Arkanoid꞉ Enemies Hack [Anton].nes 
    0x5FE82456, //  Homebrew\Ultimate Frogger Champion (Demo).nes 
    0xD89E5A67, //  Licensed Japan\Arkanoid.nes 
    0x8F4497EE, //  Licensed Japan\Peepar Time.nes 
    0x32FB0583, //  Licensed North America\Arkanoid.nes 
    0xBBED6E6E, //  Licensed North America\The Legend of Kage.nes 
    0x03BDC96F, //  Modern\Virtual Console\Mighty Bomb Jack.nes 
    0x925C63C3, //  Modern\Virtual Console\The Legend of Kage.nes 
/**** 185 mapper=3 submapper=2 prgrom=32K chrrom=16K mirror=H system=SYSTEM_NTSC */
    0x64044F64, //  Homebrew\Cave [Mojon Twins].nes 
    0x10F25E07, //  Homebrew\The Wizard.nes 
    0x77EE877C, //  Modern\Ultimate Frogger Champion.nes 
/**** 186 mapper=3 submapper=2 prgrom=32K chrrom=24K mirror=H */
    0x38B9DC71, //  Bad Dumps\マリ・アヤミ・ルカのA.V. Poker (Taiwan).nes 
/**** 187 mapper=3 submapper=2 prgrom=32K chrrom=32K mirror=H */
    0xB23508FF, //  Bootleg Singles\Bung Game Doctor\(F006) Solomonの鍵.nes 
    0xF99525B7, //  Bootleg Singles\Bung Game Doctor\(F010) ASO - Armored Scrum Object.nes 
    0x078864F2, //  Bootleg Singles\Bung Game Doctor\(F012) King's Knight.nes 
    0xC5FB1766, //  Bootleg Singles\Bung Game Doctor\(F013) Buggy Popper.nes 
    0x22A228BD, //  Bootleg Singles\Bung Game Doctor\(F017) Tiger-Heli.nes 
    0x470A5F3B, //  Bootleg Singles\Bung Game Doctor\(U007) Puzzle.nes 
    0xE116447F, //  Bootleg Singles\Bung Game Doctor\75 賓果.nes 
    0x8D872DC9, //  Bootleg Singles\Bung Game Doctor\Back to the Future.nes 
    0x0BED1622, //  Bootleg Singles\Bung Game Doctor\Garry Kitchen's Battle Tank.nes 
    0xA23F0A27, //  Bootleg Singles\Exerion (NintendoAge).nes 
    0xD858033D, //  Bootleg Singles\Front Fareast Magic Card\(F1033) A.S.O. - Armored Scrum Object.nes 
    0xE46C156D, //  Bootleg Singles\Front Fareast Magic Card\(F1095) Tiger-Heli.nes 
    0xF5D061CA, //  Bootleg Singles\Front Fareast Magic Card\(F1096) Spy Hunter.nes 
    0x304827EC, //  Bootleg Singles\Front Fareast Magic Card\(F1149) Jaws.nes 
    0xE3F3F6AE, //  Bootleg Singles\Front Fareast Magic Card\(F1162) Paperboy.nes 
    0x9347F682, //  Compatibility Hacks\Exploding Fist (Piko Interactive NES cartridge)[m003 extract].nes 
    0x7EE60C14, //  Homebrew\Copyright Restorations\Destroyer [based on Gluk Video version].nes 
    0x32D11749, //  Homebrew\Demos\NES Fireplace.nes 
    0x38336451, //  Homebrew\Elusion of the Dead (Early Concept 1).nes 
    0xCDD03185, //  Homebrew\Hacks\Back to the Future 4 [RyanVG].nes 
    0x2D8196A1, //  Homebrew\Hacks\Back to the Future꞉ Enhanced Edition [pacnsacdave].nes 
    0x9CFB9199, //  Homebrew\Hacks\Popeye vs. Bluto DK Edition [Riggs].nes 
    0xFA0A70C5, //  Homebrew\Jammin' Honey (rev0).nes 
    0x29003686, //  Homebrew\Jammin' Honey (rev1).nes 
    0x62606957, //  Homebrew\Translations\English\Egypt [Dodgy Translations v1.01].nes 
    0xF4B71793, //  Homebrew\Translations\English\Egypt [Magic Translations v0.90].nes 
    0x304DA68E, //  Homebrew\Translations\English\Family Trainer꞉ Baby Kyonshi's Big Adventure [Pacnsacdave].nes 
    0xDD6423A9, //  Homebrew\Translations\English\Family Trainer꞉ Showdown! Takeshi's Castle [AgentOrange, MrRichard999 v1.01].nes 
    0x6AF3DEF8, //  Homebrew\Translations\English\Fleet Commander [MrRichard999 v1.01].nes 
    0xE3E2C3BF, //  Licensed Japan\A.S.O. - Armored Scrum Object.nes 
    0x9FD35802, //  Licensed Japan\Buggy Popper.nes 
    0x9992F445, //  Licensed Japan\Championship Bowling.nes 
    0x0CD79B71, //  Licensed Japan\Cosmo Genesis.nes 
    0xB786AB95, //  Licensed Japan\Dynamite Bowl (rev0).nes 
    0x9E66A66B, //  Licensed Japan\Dynamite Bowl (rev1).nes 
    0xE1C03EB6, //  Licensed Japan\Egypt.nes 
    0x29DE87AF, //  Licensed Japan\Family Trainer꞉ Aerobics Studio.nes 
    0xEA90F3E2, //  Licensed Japan\Family Trainer꞉ Running Stadium.nes 
    0x9044550E, //  Licensed Japan\Family Trainer꞉ 来! 来! キョンシーズ - Baby キョンシーのあみだ大冒険.nes 
    0xB811C054, //  Licensed Japan\Fleet Commander.nes 
    0x1773F76D, //  Licensed Japan\Flipull꞉ An Exciting Cube Game (rev0).nes 
    0x69FEECB2, //  Licensed Japan\Flipull꞉ An Exciting Cube Game (rev1).nes 
    0x4057C51B, //  Licensed Japan\Ghostbusters.nes 
    0xAC136F2D, //  Licensed Japan\King's Knight.nes 
    0x2C043781, //  Licensed Japan\Paperboy.nes 
    0x7080D1F8, //  Licensed Japan\Power Soccer.nes 
    0x8752DCCB, //  Licensed Japan\Puzznic.nes 
    0x9A808C3B, //  Licensed Japan\Quarth.nes 
    0xD821A1C6, //  Licensed Japan\Solomonの鍵.nes 
    0x2E68ACFC, //  Licensed Japan\Tiger-Heli.nes 
    0xDBF90772, //  Licensed North America\Alpha Mission.nes 
    0x1425D7F4, //  Licensed North America\Arkista's Ring.nes 
    0xA55FA397, //  Licensed North America\Back to the Future.nes 
    0x50CCC8ED, //  Licensed North America\Battleship.nes 
    0xA0A095C4, //  Licensed North America\Bump 'n' Jump.nes 
    0x90D68A43, //  Licensed North America\Garry Kitchen's Battle Tank.nes 
    0x6A154B68, //  Licensed North America\Ghostbusters.nes 
    0x27D14A54, //  Licensed North America\Jaws.nes 
    0xACA145D8, //  Licensed North America\Karate Champ (rev0).nes 
    0xF42B0DBD, //  Licensed North America\Karate Champ (rev1).nes 
    0x01B4CA89, //  Licensed North America\King's Knight.nes 
    0xAA74A4D8, //  Licensed North America\Kung-Fu Heroes.nes 
    0xAAC2E75E, //  Licensed North America\Mighty Bomb Jack.nes 
    0x32086826, //  Licensed North America\Paperboy.nes 
    0xEB61133B, //  Licensed North America\Puzznic.nes 
    0x40684E95, //  Licensed North America\Solomon's Key.nes 
    0xC7197FB1, //  Licensed North America\Spy Hunter.nes 
    0xFCE71311, //  Licensed North America\Stadium Events.nes 
    0xB1723338, //  Licensed North America\Star Voyager.nes 
    0xC3C7A568, //  Licensed North America\Tiger-Heli.nes 
    0x5734EB9E, //  Licensed North America\World Class Track Meet (rev0).nes 
    0xAF4010EA, //  Licensed North America\World Class Track Meet (rev1).nes 
    0xAE90380E, //  Modern\Virtual Console\King's Knight (Japan).nes 
    0x8B62807B, //  Modern\Virtual Console\King's Knight (North America).nes 
    0xE326E0F5, //  Samples\ASO - Armored Scrum Object.nes 
    0x026C1E1A, //  Unlicensed Taiwan&amp;Hong Kong\Cosmos Cop.nes 
    0x2EFE622F, //  Unlicensed Taiwan&amp;Hong Kong\Destroyer.nes 
    0x2B20ED9B, //  Unlicensed Taiwan&amp;Hong Kong\水果狸.nes 
    0x1DA98519, //  Unlicensed Taiwan&amp;Hong Kong\電視瑪琍.nes 
    0x809067D7, //  Unreleased\Exerion.nes 
    0xB97BFDD7, //  Unreleased\Exploding Fist (Alpha).nes 
    0x1C31DD60, //  Unreleased\Exploding Fist (Beta).nes 
    0xE4A6E151, //  Unreleased\Family Trainer꞉ 来! 来! キョンシーズ - Baby キョンシーのあみだ大冒険.nes 
    0xC8BD1908, //  Unreleased\Ghostbusters.nes 
/**** 188 mapper=3 submapper=2 prgrom=32K chrrom=32K mirror=H system=SYSTEM_PAL */
    0x976893D2, //  Licensed PAL\Alpha Mission.nes 
    0xFC5026EE, //  Licensed PAL\Battleship.nes 
    0x842A3FD9, //  Licensed PAL\Mighty Bomb Jack.nes 
    0xEE219A49, //  Licensed PAL\Paperboy.nes 
    0xFC2DA286, //  Licensed PAL\Puzznic.nes 
    0x75C3E7D4, //  Licensed PAL\Solomon's Key.nes 
    0x0DA28A50, //  Licensed PAL\Stadium Events.nes 
    0x40BFA660, //  Licensed PAL\Tiger-Heli (rev0).nes 
    0x95D3BFFF, //  Licensed PAL\Tiger-Heli (rev1).nes 
/**** 189 mapper=3 submapper=2 prgrom=32K chrrom=32K mirror=H system=SYSTEM_NTSC */
    0x9DB91BDB, //  Bad Dumps\Dream Fighter (Asder 20-in-1 extract)[CHR overdump].nes 
    0xC69FBC1F, //  Homebrew\Espitene.nes 
    0xBA96C270, //  Homebrew\MiedoW꞉ Cheril's Nightmares.nes 
    0x5519A326, //  Modern\Donkey Kong - Original Edition (NTSC).nes 
    0x90B1B309, //  Modern\Donkey Kong - Original Edition (PAL).nes 
    0x9908C6C9, //  Modern\Exploding Fist (Piko Interactive Evercade cartridge).nes 
    0xE9AD2163, //  Unlicensed Elsewhere\Cosmos Cop.nes 
    0x322C9F6A, //  Unlicensed Elsewhere\Destroyer.nes 
    0xBF0C485D, //  Unlicensed Elsewhere\Jackpot.nes 
/**** 190 mapper=3 submapper=2 prgrom=32K prgram=2K chrrom=32K mirror=H */
    0x642855AB, //  Bootleg Hacks\超一流 烟山杯 围棋.nes 
    0xFA7E02FA, //  Licensed Japan\早打ち Super 囲碁 (rev0).nes 
    0x74F19C76, //  Licensed Japan\早打ち Super 囲碁 (rev1).nes 
/**** 191 mapper=3 submapper=2 prgrom=32K chrrom=128K mirror=H */
    0x2B286601, //  Compatibility Hacks\美女拳 - Honey Peach [m003].nes 
/**** 192 mapper=3 submapper=2 prgrom=16K chrrom=16K mirror=V */
    0xCDF27B7C, //  Homebrew\Demos\Super Fighter Demo [Chris Covell].nes 
/**** 193 mapper=3 submapper=2 prgrom=16K chrrom=32K mirror=V */
    0xF278E465, //  Compatibility Hacks\Bird Week [m003].nes 
/**** 194 mapper=3 submapper=2 prgrom=32K chrrom=8K mirror=V */
    0x08E11357, //  Bad Dumps\Super Dyna'mix Badminton [missing CHR bank].nes 
/**** 195 mapper=3 submapper=2 prgrom=32K chrrom=16K mirror=V */
    0x395BE496, //  Bad Dumps\Super Maruo [CHR overdump].nes 
    0x14B3D977, //  Bootleg Singles\Bung Game Doctor\(D001) City Connection.nes 
    0x007BB58A, //  Bootleg Singles\Bung Game Doctor\(E002) The Goonies.nes 
    0x7E024394, //  Bootleg Singles\Bung Game Doctor\(E003) Argus.nes 
    0x313615FB, //  Bootleg Singles\Bung Game Doctor\(E006) Seicross.nes 
    0x27251D6C, //  Bootleg Singles\Bung Game Doctor\(E007) Choplifter.nes 
    0x78EB1CFF, //  Bootleg Singles\Bung Game Doctor\(E013) いきなり Musician.nes 
    0x373A9566, //  Bootleg Singles\Bung Game Doctor\777 幸運輪盤.nes 
    0xCD52B771, //  Bootleg Singles\Bung Game Doctor\Chinese Checkers.nes 
    0x0AE3476B, //  Bootleg Singles\Bung Game Doctor\Pipe 5.nes 
    0xBFE8D39E, //  Bootleg Singles\Bung Game Doctor\Star Soldier.nes 
    0x28C4B064, //  Bootleg Singles\Bung Game Doctor\動動腦 II꞉ 國中英文(一).nes 
    0xE3B98143, //  Bootleg Singles\Front Fareast Magic Card\(F1051) The Goonies.nes 
    0x2C777D45, //  Bootleg Singles\Front Fareast Magic Card\(F1082) Spy vs. Spy.nes 
    0x72E32402, //  Bootleg Singles\Front Fareast Magic Card\(F1101) 算数 3年 けいさん Game.nes 
    0x3C000EEA, //  Bootleg Singles\Front Fareast Magic Card\(F1107) Gyrodine.nes 
    0x12278820, //  Bootleg Singles\Front Fareast Magic Card\(F1145) Super Dyna'mix Badminton.nes 
    0xA41919EE, //  Compatibility Hacks\Elementary Math Grade 3꞉ Calculation Game [Pikachumanson][m003].nes 
    0x5FB2062A, //  Homebrew\Bare Metal 2.0 [scarduadev].nes 
    0xA464CB89, //  Homebrew\Bugfixes, Improvements\Tetris (rev2)[triangle mute bug fixed].nes 
    0xBDF9BA9B, //  Homebrew\Demos\CMC 80's [Chris Covell].nes 
    0xD11A3391, //  Homebrew\Hacks\Ballnana.nes 
    0xB379F50D, //  Homebrew\Robo-Ninja Climb (NESdev 2017).nes 
    0x57B2EA2F, //  Homebrew\Robo-Ninja Climb (alt).nes 
    0xA8ED1000, //  Homebrew\Robo-Ninja Climb.nes 
    0x66EF7B24, //  Homebrew\Translations\English\Banana [KingMike].nes 
    0x76272E67, //  Homebrew\Translations\English\Spontaneous Musician [Psyklax].nes 
    0x2E22071C, //  Homebrew\Translations\English\Super Maruo.nes 
    0x86ACB36B, //  Licensed Japan\Banana.nes 
    0x8434B263, //  Licensed Japan\Super Dyna'mix Badminton.nes 
    0x0115EAF0, //  Licensed Japan\Tetris (rev0).nes 
    0xD074653D, //  Licensed Japan\Tetris (rev1).nes 
    0x5C5A1AB8, //  Licensed Japan\Tetris (rev2).nes 
    0x092EC15C, //  Licensed Japan\いきなり Musician.nes 
    0x3413E33B, //  Modern\Virtual Console\Seicross.nes 
    0xF0F14C4C, //  Samples\Star Soldier.nes 
    0x89E6C1F1, //  Unlicensed Taiwan&amp;Hong Kong\Adam &amp; Eve.nes 
    0xCF4483AF, //  Unreleased\Banana (1986-00-00).nes 
    0x7C42CB7B, //  Unreleased\Banana (1986-06-30).nes 
/**** 196 mapper=3 submapper=2 prgrom=32K chrrom=16K mirror=V system=SYSTEM_NTSC */
    0x655BC8FC, //  Bad Dumps\Balloon Monster [CHR overdump].nes 
    0x0E3B9491, //  Homebrew\Sinking Feeling.nes 
    0xAB2006B4, //  Licensed North America\Donkey Kong Classics.nes 
    0x09CA75FB, //  Samples\Project Blue [Toggle Switch &amp; Frankengraphics].nes 
    0xBF6418AA, //  Unlicensed Elsewhere\Adam &amp; Eve.nes 
/**** 197 mapper=3 submapper=2 prgrom=32K chrrom=32K mirror=V */
    0xB66A1BAE, //  Bad Dumps\Ballnana [CHR overdump].nes 
    0x019F531B, //  Bad Dumps\Banana [KingMike][overdumped CHR].nes 
    0xC9EE15A7, //  Bootleg Hacks\Aladdin III (A-N2).nes 
    0x4C455DF7, //  Bootleg Hacks\Super Barbie.nes 
    0x623020FB, //  Bootleg Hacks\孫小毛 奇幻島 - Kiddy Sun in Fantasia.nes 
    0x886B5BD3, //  Bootleg Singles\Bung Game Doctor\(F001) ゲゲゲの鬼太郎꞉ 妖怪大魔境.nes 
    0x04D800DA, //  Bootleg Singles\Bung Game Doctor\(F003) Gradius.nes 
    0x8C23DBC4, //  Bootleg Singles\Bung Game Doctor\(F007) 六三四の剣꞉ ただいま修行中.nes 
    0xBB473A41, //  Bootleg Singles\Bung Game Doctor\(F008) 北斗の拳.nes 
    0x15C43F78, //  Bootleg Singles\Bung Game Doctor\(F009) じゃじゃ丸の大冒険.nes 
    0x8B8E405A, //  Bootleg Singles\Bung Game Doctor\(F011) 高橋名人の冒険島.nes 
    0xF6E2BD6E, //  Bootleg Singles\Bung Game Doctor\(F014) 迷宮組曲꞉ Milonの大冒険.nes 
    0x42EDE8CB, //  Bootleg Singles\Bung Game Doctor\(F015) 元祖西遊記꞉ Super Monkey 大冒険.nes 
    0xA74325EF, //  Bootleg Singles\Bung Game Doctor\(F016) 長靴をはいた猫 世界一周 80日大冒険.nes 
    0x22EEEC6D, //  Bootleg Singles\Bung Game Doctor\(F018) 戦え! 超 Robot 生命体 Transformers꞉ Convoyの謎.nes 
    0xB79F5883, //  Bootleg Singles\Bung Game Doctor\(F019) Hottermanの地底探検.nes 
    0x4EF429CC, //  Bootleg Singles\Bung Game Doctor\(F020) 新人類꞉ The New Type.nes 
    0xF1C3ECE6, //  Bootleg Singles\Bung Game Doctor\(F024) うる星やつら꞉ LumのWedding Bell.nes 
    0x9EC00946, //  Bootleg Singles\Bung Game Doctor\(F036) 鉄道王꞉ Famicom Boardgame.nes 
    0x4E7A6181, //  Bootleg Singles\Bung Game Doctor\(F040) 水果狸.nes 
    0x83F47FB3, //  Bootleg Singles\Bung Game Doctor\(F042) 臺灣 16 麻雀.nes 
    0x6FD21582, //  Bootleg Singles\Bung Game Doctor\(F043) Tetris꞉ The Soviet Mind Game.nes 
    0xC58EEA57, //  Bootleg Singles\Bung Game Doctor\(F044) 四川麻將 - Mahjong Trap.nes 
    0x7A02A281, //  Bootleg Singles\Bung Game Doctor\(U005) 未来小子 - Joy Van Kid.nes 
    0x6AA0D156, //  Bootleg Singles\Bung Game Doctor\(U008) Friday the 13th.nes 
    0x4FD5FB0B, //  Bootleg Singles\Bung Game Doctor\Brush Roller.nes 
    0x755E87B4, //  Bootleg Singles\Bung Game Doctor\Dash Galaxy in the Alien Asylum.nes 
    0xE29131F5, //  Bootleg Singles\Bung Game Doctor\Flipull꞉ An Exciting Cube Game.nes 
    0x84C2F5C1, //  Bootleg Singles\Bung Game Doctor\Magic Carpet 1001.nes 
    0xCF74BCA2, //  Bootleg Singles\Bung Game Doctor\Q-Bert.nes 
    0xE8607D32, //  Bootleg Singles\Bung Game Doctor\Strategist.nes 
    0x7F54FFC1, //  Bootleg Singles\Bung Game Doctor\双鷹 - Twin Eagle.nes 
    0xEBD0644D, //  Bootleg Singles\Bung Game Doctor\盜帥 - Master Chu.nes 
    0xDFEB7E98, //  Bootleg Singles\Bung Game Doctor\電視瑪琍.nes 
    0x60F1FD6F, //  Bootleg Singles\Front Fareast Magic Card\(F1037) うる星やつら꞉ LumのWedding Bell.nes 
    0x577482BB, //  Bootleg Singles\Front Fareast Magic Card\(F1089) 六三四の剣꞉ ただいま修行中.nes 
    0x5DC0BEB8, //  Bootleg Singles\Front Fareast Magic Card\(F1097) Dragon Quest.nes 
    0xD60E10AA, //  Bootleg Singles\Front Fareast Magic Card\(F1113) ゲゲゲの鬼太郎꞉ 妖怪大魔境.nes 
    0x942F6BB0, //  Bootleg Singles\Front Fareast Magic Card\(F1116) 鉄道王꞉ Famicom Boardgame.nes 
    0xE1DC9B54, //  Bootleg Singles\Front Fareast Magic Card\(F1119) Banana.nes 
    0xDC33AED8, //  Bootleg Singles\Front Fareast Magic Card\(F1131) Castle Excellent.nes 
    0x4FBBFA74, //  Bootleg Singles\Front Fareast Magic Card\(F1150) Gotcha! The Sport!.nes 
    0xB807446E, //  Bootleg Singles\Front Fareast Magic Card\(F1153) Tetris.nes 
    0x8DEDEA07, //  Bootleg Singles\Front Fareast Magic Card\(F1155) 水果狸.nes 
    0xADCE5A3B, //  Bootleg Singles\Front Fareast Magic Card\(F1158) Tetris꞉ The Soviet Mind Game.nes 
    0x67755041, //  Bootleg Singles\Front Fareast Magic Card\(F1160) Flipull꞉ An Exciting Cube Game.nes 
    0x04F3354D, //  Bootleg Singles\Front Fareast Magic Card\(F1161) Friday the 13th.nes 
    0xB14E668E, //  Bootleg Singles\Front Fareast Magic Card\(SM2001) Sanrio Carnival.nes 
    0xF18C1CAF, //  Bootleg Singles\Mickey Mouse (LD28).nes 
    0xE8483C36, //  Bootleg Singles\TouchGamePlayer\Gyruss.nes 
    0xFB270B96, //  Bootleg Singles\闘智拼盤 - Wisdom Boy.nes 
    0xAA588D49, //  Bootleg Singles\高橋名人の冒険島 (LD12).nes 
    0xAA8A09D5, //  Compatibility Hacks\Elementary Math Grade 1꞉ Improvement Game [Pikachumanson][m003].nes 
    0x5390EF61, //  Compatibility Hacks\Everdrive N8\Cybernoid꞉ The Fighting Machine [SmokeMonster].nes 
    0x8B9CB8F2, //  Compatibility Hacks\動動腦 II꞉ 國中英文(一) [m003].nes 
    0x407B4AFA, //  Homebrew\Antenna Antics [FG Software].nes 
    0x9447458A, //  Homebrew\Bugfixes, Improvements\Elfland [restored CHR data][NewRisingSun].nes 
    0x6A6A6100, //  Homebrew\Bugfixes, Improvements\Star Soldier (Special Version)[homebrew restoration].nes 
    0x2A0C9105, //  Homebrew\Demos\Tanespot [Dreamhack].nes 
    0x8AFA82C8, //  Homebrew\Elusion of the Dead (Early Concept 2).nes 
    0xA574B2B4, //  Homebrew\Espitenen und Herr Pimponen.nes 
    0xB68BBEE0, //  Homebrew\Galf.nes 
    0xC7673DDA, //  Homebrew\Hacks\After Lolo 2 [Googie].nes 
    0x02951227, //  Homebrew\Hacks\Ben Roethlisberger's Big Adventure.nes 
    0xA0540F23, //  Homebrew\Hacks\Chodius.nes 
    0x76E69AD8, //  Homebrew\Hacks\Marioくん (Moaiくん).nes 
    0x0E891078, //  Homebrew\Hacks\Nesrocks' Adventure Island Speedrun (v1.2)[Nesrocks].nes 
    0x9B405349, //  Homebrew\Nin Nin.nes 
    0xE4E874C7, //  Homebrew\Saturn Smash.nes 
    0x2EC1E92E, //  Homebrew\Solar Wars (v1.5).nes 
    0x4F42D622, //  Homebrew\Translations\English\Cadillac [Gil Galad].nes 
    0x11F238D0, //  Homebrew\Translations\English\Dragon Quest [Polynim v1.3, Bregalad v1.0](Quick EXP).nes 
    0x610FA718, //  Homebrew\Translations\English\Dragon Quest [Polynim v1.3, Bregalad v1.0].nes 
    0x8CBDFC48, //  Homebrew\Translations\English\Dragon Quest [Polynim v1.3](Quick EXP).nes 
    0xFC406380, //  Homebrew\Translations\English\Dragon Quest [Polynim v1.3].nes 
    0x9637D78F, //  Homebrew\Translations\English\Elementary Math Grade 4꞉ Improvement Game [Pikachumanson].nes 
    0xDA148C59, //  Homebrew\Translations\English\Elementary Math Grades 5-6꞉ Calculation Game [Pikachumanson].nes 
    0xB7F57950, //  Homebrew\Translations\English\Fist of the North Star - Hokuto no Ken [Stardust Crusaders v1.01].nes 
    0x37C2B9DC, //  Homebrew\Translations\English\George Tokoro’s Offense and Defense [Psyklax].nes 
    0x0E8E732A, //  Homebrew\Translations\English\Gorby’s Pipeline꞉ Great Military Operation [DvD Translations revA].nes 
    0xA2A76290, //  Homebrew\Translations\English\Hello Kitty's Flower Shop [Suicidal Translations].nes 
    0x0DBC1BFE, //  Homebrew\Translations\English\Hotman [Six Feet Under].nes 
    0x87DE0FC8, //  Homebrew\Translations\English\Journey to the West꞉ Super Monkey Adventures [GAFF Translations, Pacnsacdave].nes 
    0xD4E4AFCD, //  Homebrew\Translations\English\Kero Kero Keroppi's Big Adventure 1 [Gaijin Productions].nes 
    0xAF4A95BF, //  Homebrew\Translations\English\Moai-kun [Suicidal Translations].nes 
    0xDAD3AE2A, //  Homebrew\Translations\English\Mr. Moai [Stardust Crusaders v1.0].nes 
    0x69352B64, //  Homebrew\Translations\English\Puss in Boots꞉ Around the World in 80 Days [Psyklax].nes 
    0xC6952FC1, //  Homebrew\Translations\English\Railroad Baron [Jink640, MrRichard999, Proveaux v0.98].nes 
    0x845EDFCD, //  Homebrew\Translations\English\Sanrio Carnival 2 [Suicidal Translations].nes 
    0xB172A6D2, //  Homebrew\Translations\English\Sanrio Carnival [Zynk Oxhyde v1.1].nes 
    0x1523A369, //  Homebrew\Translations\English\Sanrio Cup꞉ Pon Pon Volley [Gaijin Productions].nes 
    0x92B4C7D5, //  Homebrew\Translations\English\Space Hunter [Zatos Hacks].nes 
    0x055183D8, //  Homebrew\Translations\English\Spooky Kitarо̄ in the Yо̄kai World [aishsha].nes 
    0x4301B96C, //  Homebrew\Translations\English\Sumo Wrestling! [The Spoony Bard v1.01].nes 
    0x305BCD61, //  Homebrew\Translations\English\Tabo's Tactics [Suicidal Translations v1.01, Hap v1.01].nes 
    0x322F14C5, //  Homebrew\Translations\English\The Transformers꞉ Mystery of Convoy [Stardust Crusaders v1.01].nes 
    0x9D2EAC31, //  Homebrew\Translations\English\The Universe Soldiers [Pacnsacdave].nes 
    0xB9978378, //  Homebrew\Translations\English\元祖西遊記꞉ Super Monkey Adventure [GAFF Translations].nes 
    0xBEDB6612, //  Homebrew\Translations\English\六三四の剣꞉ Training Days [GAFF Translations v1.00].nes 
    0x0719260C, //  Licensed Japan\Cadillac.nes 
    0x7FA2CC55, //  Licensed Japan\Castle Excellent.nes 
    0x3FC1DC19, //  Licensed Japan\Deblock.nes 
    0x88053D25, //  Licensed Japan\Dragon Quest.nes 
    0x0354868A, //  Licensed Japan\Family Trainer꞉ Athletic World (rev0).nes 
    0x60AD090A, //  Licensed Japan\Family Trainer꞉ Athletic World (rev1).nes 
    0x6CCA1C1F, //  Licensed Japan\Family Trainer꞉ 大運動会.nes 
    0x8A12A7D9, //  Licensed Japan\Family Trainer꞉ 突撃! 風雲たけし城.nes 
    0x1829616A, //  Licensed Japan\Gorby no Pipeline 大作戦.nes 
    0xDE395EFD, //  Licensed Japan\Gradius (ArchiMENdes Hen).nes 
    0xD8EFF0DF, //  Licensed Japan\Gradius.nes 
    0x26BB1C8C, //  Licensed Japan\Hello Kittyのおはなばたけ.nes 
    0x3275FD7E, //  Licensed Japan\Hottermanの地底探検.nes 
    0xEB465156, //  Licensed Japan\Kero Kero Keroppiの大冒険.nes 
    0x9E4701CB, //  Licensed Japan\Mickey Mouse꞉ 不思議の国の大冒険.nes 
    0xEC8A884F, //  Licensed Japan\Moaiくん.nes 
    0x3D95D866, //  Licensed Japan\Sanrio Carnival 2.nes 
    0x485AC098, //  Licensed Japan\Sanrio Carnival.nes 
    0xF7B852E4, //  Licensed Japan\Sanrio Cup꞉ ポンポンVolley.nes 
    0x684AFCCD, //  Licensed Japan\Space Hunter.nes 
    0x1B421E9C, //  Licensed Japan\Star Soldier.nes 
    0xC7BCC981, //  Licensed Japan\Super Mogura たたき!! ぽっくん Mogura.nes 
    0x90ECDADE, //  Licensed Japan\つっぱり大相撲.nes 
    0x555042B3, //  Licensed Japan\みんなのたあ坊のなかよし大作戦 (rev0).nes 
    0xD1FE2138, //  Licensed Japan\みんなのたあ坊のなかよし大作戦 (rev1).nes 
    0x25468546, //  Licensed Japan\ゲゲゲの鬼太郎꞉ 妖怪大魔境.nes 
    0x3BF55966, //  Licensed Japan\元祖西遊記꞉ Super Monkey 大冒険.nes 
    0x9D34EDC5, //  Licensed Japan\六三四の剣꞉ ただいま修行中.nes 
    0x60A3B803, //  Licensed Japan\北斗の拳.nes 
    0xEA89963F, //  Licensed Japan\戦え! 超 Robot 生命体 Transformers꞉ Convoyの謎.nes 
    0x6776A977, //  Licensed Japan\所さんのまもるもせめるも.nes 
    0x43B0944B, //  Licensed Japan\新人類꞉ The New Type.nes 
    0x8B4A2866, //  Licensed Japan\算数 4年 けいさん Game.nes 
    0x9AACD75D, //  Licensed Japan\算数 5,6年 けいさん Game.nes 
    0xFA6D4281, //  Licensed Japan\聖飢魔 II꞉ 悪魔の逆襲.nes 
    0x3E00A373, //  Licensed Japan\迷宮組曲꞉ Milonの大冒険.nes 
    0x4BB6B430, //  Licensed Japan\鉄道王꞉ Famicom Boardgame.nes 
    0xE8AF6FF5, //  Licensed Japan\長靴をはいた猫꞉ 世界一周 80日大冒険.nes 
    0x6A457A43, //  Licensed Japan\高橋名人の冒険島.nes 
    0x9BDE3267, //  Licensed North America\Adventures of Dino Riki.nes 
    0x1771EA8F, //  Licensed North America\Athletic World.nes 
    0x6CD46979, //  Licensed North America\Bandai Golf꞉ Challenge Pebble Beach.nes 
    0x12906664, //  Licensed North America\Castlequest.nes 
    0xEAC38105, //  Licensed North America\Championship Bowling.nes 
    0xAE8666B4, //  Licensed North America\City Connection.nes 
    0xAC8DCDEA, //  Licensed North America\Cybernoid꞉ The Fighting Machine.nes 
    0x67811DA6, //  Licensed North America\Dash Galaxy in the Alien Asylum.nes 
    0xEB9960EE, //  Licensed North America\Destination Earthstar.nes 
    0x38946C43, //  Licensed North America\Fisher-Price꞉ Firehouse Rescue.nes 
    0x267DE4CC, //  Licensed North America\Fisher-Price꞉ I Can Remember.nes 
    0xB9762DA8, //  Licensed North America\Fisher-Price꞉ Perfect Fit.nes 
    0xBEB15855, //  Licensed North America\Friday the 13th.nes 
    0x4E959173, //  Licensed North America\Gotcha! The Sport!.nes 
    0x350D835E, //  Licensed North America\Gradius.nes 
    0x1D41CC8C, //  Licensed North America\Gyruss.nes 
    0xF8A713BE, //  Licensed North America\Hudson's Adventure Island.nes 
    0xCF322BB3, //  Licensed North America\John Elway's Quarterback.nes 
    0xF54B34BD, //  Licensed North America\Kings of the Beach.nes 
    0xA03A422B, //  Licensed North America\Major League Baseball (rev0).nes 
    0x96E6C1CE, //  Licensed North America\Major League Baseball (rev1).nes 
    0x7C6A3D51, //  Licensed North America\Mickey Mousecapade.nes 
    0x586A3277, //  Licensed North America\Milon's Secret Castle.nes 
    0x2F698C4D, //  Licensed North America\Monster Truck Rally.nes 
    0xB5D28EA2, //  Licensed North America\Mystery Quest.nes 
    0x02CC3973, //  Licensed North America\Ninja Kid.nes 
    0xBCE77871, //  Licensed North America\Pipe Dream.nes 
    0xC0B23520, //  Licensed North America\Q＊bert.nes 
    0x851EB9BE, //  Licensed North America\Shooting Range.nes 
    0xFCE408A4, //  Licensed North America\Star Force.nes 
    0x262B5A1D, //  Licensed North America\Star Soldier.nes 
    0xD74B2719, //  Licensed North America\Super Team Games.nes 
    0xD3BFF72E, //  Licensed North America\T&amp;C Surf Designs꞉ Wood and Water Rage.nes 
    0x983948A5, //  Licensed North America\The Karate Kid.nes 
    0x6997F5E1, //  Licensed North America\The Last Starfighter.nes 
    0x9C9F3571, //  Licensed North America\Track &amp; Field.nes 
    0xA6D73BC3, //  Modern\Hudson Best Collection\Star Soldier.nes 
    0xA0318BB4, //  Modern\Hudson Best Collection\迷宮組曲꞉ Milonの大冒険.nes 
    0x4D998633, //  Modern\Virtual Console\Milon's Secret Castle.nes 
    0xCFB4A313, //  Modern\Virtual Console\つっぱり大相撲.nes 
    0x4095BB67, //  Modern\Virtual Console\戦え! 超 Robot 生命体 Transformers꞉ Convoyの謎.nes 
    0xA080E7E1, //  Playchoice\Gradius (rev0).nes 
    0x5ACBD785, //  Playchoice\Gradius (rev1).nes 
    0x167C5EDB, //  Playchoice\Track &amp; Field.nes 
    0xAEC8D250, //  Samples\Cadillac.nes 
    0x22D7C95E, //  Samples\迷宮組曲꞉ Milonの大冒険.nes 
    0x0CEB9526, //  Unlicensed Taiwan&amp;Hong Kong\Elfland.nes 
    0xC70409AB, //  Unlicensed Taiwan&amp;Hong Kong\Pokey.nes 
    0x42110147, //  Unlicensed Taiwan&amp;Hong Kong\Tank 風雲 - King Tank.nes 
    0x1CF48EF1, //  Unlicensed Taiwan&amp;Hong Kong\双响炮.nes 
    0x722F4E38, //  Unlicensed Taiwan&amp;Hong Kong\戰國四川省 (C&amp;E).nes 
    0x3B90D11E, //  Unlicensed Taiwan&amp;Hong Kong\星際飛車 - The Universe Soldiers.nes 
    0xD898A900, //  Unlicensed Taiwan&amp;Hong Kong\闘智拼盤 - Wisdom Boy.nes 
    0xB96F8321, //  Unreleased\Hottermanの地底探検.nes 
    0xE81C71E9, //  Unreleased\Star Force.nes 
    0xD99A8804, //  Unreleased\Super Pitfall II.nes 
    0xB9DC755E, //  Unreleased\いきなり Musician.nes 
    0xCE2450C0, //  Unreleased\ゲゲゲの鬼太郎꞉ 妖怪大魔境.nes 
    0x2337F45E, //  Unreleased\北斗の拳.nes 
    0x162F328E, //  Unreleased\算数 4年 けいさん Game.nes 
    0x9EF351DC, //  Unreleased\算数 5,6年 けいさん Game.nes 
    0xFE9FE4DA, //  Unreleased\長靴をはいた猫 世界一周 80日大冒険.nes 
/**** 198 mapper=3 submapper=2 prgrom=32K chrrom=32K mirror=V system=SYSTEM_PAL */
    0x272A1063, //  Homebrew\Bust-A-Nut.nes 
    0xE592F53A, //  Licensed PAL\Athletic World.nes 
    0x681798A8, //  Licensed PAL\City Connection.nes 
    0xE402B134, //  Licensed PAL\Drop Zone.nes 
    0x51C51C35, //  Licensed PAL\Gradius.nes 
    0x86867830, //  Licensed PAL\Hudson's Adventure Island.nes 
    0x5CE55F5B, //  Licensed PAL\Star Force.nes 
    0x5D99053D, //  Licensed PAL\Track &amp; Field in Barcelona.nes 
/**** 199 mapper=3 submapper=2 prgrom=32K chrrom=32K mirror=V system=SYSTEM_NTSC */
    0x637BA508, //  Bad Dumps\Adam &amp; Eve [CHR overdump].nes 
    0xCB04726D, //  Bad Dumps\Magic Carpet 1001 [bad CHR].nes 
    0xC83A89DA, //  Bootleg Hacks\Harry Potter (Magic Carpet 1001).nes 
    0xFD5D51BE, //  Bootleg Hacks\Harry Tour.nes 
    0x4CB3E559, //  Homebrew\Hacks\Super Mario Bros. series\Lady Opera in Swf Panic! [Darkdata].nes 
    0x64BB4540, //  Homebrew\Hacks\Super Mario Bros. series\Mario is Lost on Mystic Island [teeporage777].nes 
    0x082626ED, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. with Super Mario Bros. 3 Graphics [GeneralGir2].nes 
    0xD01C81D9, //  Homebrew\Hacks\Super Mario Bros. series\Toad's Christmas Adventure.nes 
    0x9436C354, //  Homebrew\Hacks\Super Mario Bros. series\Toadette's Christmas Adventure.nes 
    0x2DBFB836, //  Homebrew\Inherent Smile (NESdev 2017).nes 
    0xDD7B06B5, //  Homebrew\Inherent Smile (v1.1).nes 
    0x9CE3374D, //  Homebrew\The Alfonzo Game.nes 
    0x0C54AB9D, //  Modern\Alfonzo Melee!.nes 
    0xEC9C9E93, //  Modern\Eskimo Bob Starring Alfonzo.nes 
    0x6DB09CC7, //  Modern\Virtual Console\City Connection (World).nes 
    0x86083FBC, //  Unlicensed Elsewhere\Go! Benny!.nes 
    0xBAACF521, //  Unlicensed Elsewhere\Magic Block.nes 
    0xC3AACD94, //  Unlicensed Elsewhere\Magic Carpet 1001.nes 
    0x4C1BFF78, //  Unlicensed Taiwan&amp;Hong Kong\Go! Benny!.nes 
    0x1A018A26, //  Unlicensed Taiwan&amp;Hong Kong\Huge Insect.nes 
    0x303D483D, //  Unlicensed Taiwan&amp;Hong Kong\Magic Carpet 1001.nes 
    0x2BA86F76, //  Unlicensed Taiwan&amp;Hong Kong\Sea of Dreamland.nes 
/**** 200 mapper=3 submapper=2 prgrom=32K prgram=2K chrrom=32K mirror=V */
    0xF2ABEB30, //  Bootleg Singles\Bung Game Doctor\早打ち Super 囲碁.nes 
    0x98087E4D, //  Bootleg Singles\Front Fareast Magic Card\(F1159) 早打ち Super 囲碁.nes 
/**** 201 mapper=3 submapper=2 prgrom=32K chrrom=128K mirror=V */
    0x6381A369, //  Bad Dumps\Galf [CHR overdump].nes 
    0xC569BBE1, //  Homebrew\Translations\English\Family Trainer꞉ Jogging Race [Jink640, MrRichard999 v1.01].nes 
    0x2F128512, //  Licensed Japan\Family Trainer꞉ Jogging Race.nes 
/**** 202 mapper=3 submapper=2 prgrom=128K chrrom=128K mirror=V */
    0x3A96F1B1, //  Bad Dumps\Galf [PRG+CHR overdump].nes 
/**** 203 mapper=4 prgrom=32K chrrom=16K mirror=4 system=SYSTEM_NTSC */
    0xAA1814F5, //  Homebrew\Crypto.nes 
    0xF6334554, //  Samples\Crypto.nes 
/**** 204 mapper=4 prgrom=64K chrrom=64K mirror=4 */
    0x404B2E8B, //  Licensed North America\Rad Racer II.nes 
    0x998BFC08, //  Playchoice\Rad Racer 2.nes 
/**** 205 mapper=4 prgrom=128K chrrom=64K mirror=4 */
    0xEC968C51, //  Licensed North America\Gauntlet.nes 
    0x35193823, //  Playchoice\Gauntlet.nes 
/**** 206 mapper=4 prgrom=256K chrrom=128K mirror=4 system=SYSTEM_NTSC */
    0x491CD95E, //  Unlicensed Taiwan&amp;Hong Kong\Jurassic Boy 2.nes 
/**** 207 mapper=4 prgrom=256K chrrom=256K mirror=4 system=SYSTEM_NTSC */
    0x9F7BF36F, //  Homebrew\Translations\Chinese (Traditional)\洛克人X.nes 
    0x36CA3102, //  Unlicensed Taiwan&amp;Hong Kong\Rocman X.nes 
/**** 208 mapper=4 prgrom=16K chrrom=8K mirror=H */
    0x70B4A437, //  Homebrew\Demos\Color 64.nes 
    0x2F95D72B, //  Plug-and-Play\extracts\Five Days.nes 
    0x85DA9C47, //  Plug-and-Play\extracts\Plush Dog.nes 
/**** 209 mapper=4 prgrom=32K chrram=8K mirror=H */
    0x81A2CC4A, //  Multicarts\extracts\(福州 Coolboy) 245-in-1 Real Game (menu).nes 
/**** 210 mapper=4 prgrom=32K prgram=8K chrram=8K mirror=H */
    0x2A72A051, //  Homebrew\鳥之诗.nes 
/**** 211 mapper=4 prgrom=32K chrrom=8K mirror=H */
    0x00514347, //  Educational Computers\extracts\Multiplication.nes 
    0x9CDC9AFD, //  Homebrew\Putt Putt.nes 
    0xBE003042, //  Homebrew\Tests\My First Sample Test.nes 
    0x8EFFF29B, //  Multicarts\extracts\Awful Rushing+Moto Boat.nes 
    0x140CDEFF, //  Multicarts\extracts\Bubbler+Strafe.nes 
    0xA23B0970, //  Multicarts\extracts\Garden War+Resistant.nes 
    0x13D96CBF, //  Plug-and-Play\extracts\Aether Copter.nes 
    0x098BC195, //  Plug-and-Play\extracts\Blob Man.nes 
    0x1C14ADC6, //  Plug-and-Play\extracts\Clown's Mission.nes 
    0x946A8E90, //  Plug-and-Play\extracts\Dinosaur.nes 
    0x44DC9F5C, //  Plug-and-Play\extracts\Golden Bird.nes 
    0xC1391218, //  Plug-and-Play\extracts\Jump-Jump.nes 
    0x865F2A44, //  Plug-and-Play\extracts\Mice Mission.nes 
    0xA13F3D2C, //  Plug-and-Play\extracts\Nature Clan꞉ Escapeway.nes 
    0x26F38C2C, //  Plug-and-Play\extracts\Rocketman.nes 
    0x0EC70C7C, //  Plug-and-Play\extracts\Slot Machine.nes 
    0x92324D59, //  Plug-and-Play\extracts\Twin Fish (FC).nes 
/**** 212 mapper=4 prgrom=32K chrrom=8K mirror=H system=SYSTEM_PAL */
    0xFFDB9179, //  Homebrew\Demos\Fullscreen Madness.nes 
/**** 213 mapper=4 prgrom=32K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x9BC42E51, //  Educational Computers\extracts\Add &amp; Sub.nes 
    0xC1980560, //  Educational Computers\extracts\Calculator 1.nes 
/**** 214 mapper=4 prgrom=32K prgram=8K chrrom=8K mirror=H */
    0xB04C8201, //  Homebrew\Demos\Mario Flicker - Color Demo.nes 
    0x34F94A8C, //  Homebrew\Demos\Pattern Table Pixel - Flickering Demo.nes 
    0x156DF4FB, //  Homebrew\Tests\Colorful.nes 
/**** 215 mapper=4 prgrom=32K chrrom=16K mirror=H */
    0xEF9859E4, //  Homebrew\Connect 4.nes 
    0x27F71D87, //  Homebrew\Tests\Serial.nes 
    0x5D4CAC4B, //  Homebrew\The Adventures of Lex &amp; Grim.nes 
    0x75FAC84A, //  Homebrew\超级贪吃蛇.nes 
    0xB0716CC2, //  Plug-and-Play\extracts\Deformable.nes 
    0x206DB218, //  Plug-and-Play\extracts\Lair's Secret.nes 
    0xFB9496CC, //  Plug-and-Play\extracts\Nature Clan꞉ Island.nes 
    0x2EC1BED7, //  Plug-and-Play\extracts\Nature Clan꞉ Mirror Devil World.nes 
    0x2A561F41, //  Plug-and-Play\extracts\Rabbit Village.nes 
    0x44A6CA07, //  Plug-and-Play\extracts\Road Worker.nes 
    0xC042B605, //  Plug-and-Play\extracts\Vigilant.nes 
/**** 216 mapper=4 prgrom=32K chrrom=32K mirror=H */
    0x73EA3196, //  Bootleg Hacks\7-up 2꞉ Spot the Cool Adventures (KT-2167).nes 
    0x3A3B5E59, //  Bootleg Singles\TouchGamePlayer\Adventures of Lolo 2.nes 
    0xD4FE398C, //  Bootleg Singles\TouchGamePlayer\Adventures of Lolo.nes 
    0x7AE99B38, //  Bootleg Singles\TouchGamePlayer\Burai Fighter.nes 
    0xA36C7598, //  Bootleg Singles\TouchGamePlayer\Hello Kittyのおはなばたけ.nes 
    0x1FC30390, //  Compatibility Hacks\Snake Rattle 'n Roll [m004].nes 
    0xBAF657B2, //  Homebrew\Cheats\Super City Tank [infinite lives].nes 
    0x8CD6652B, //  Homebrew\Hacks\Adventures of Lolo 2 [Cstutor].nes 
    0x13977E61, //  Homebrew\Hacks\Adventures of Lolo Remix (WIP)[Legoman].nes 
    0x51405110, //  Homebrew\Hacks\Adventures of Lolo Remix [Legoman].nes 
    0xA5DCE761, //  Homebrew\Hacks\Challenging Lolo 2 [Sivak].nes 
    0xB0CFCE26, //  Homebrew\Hacks\Googie's Hack of Adventures of Lolo 2 [Googie].nes 
    0x297198B9, //  Licensed Japan\Adventures of Lolo.nes 
    0xE9176129, //  Licensed Japan\Burai Fighter.nes 
    0x1992D163, //  Licensed North America\Adventures of Lolo 2.nes 
    0xCE228874, //  Licensed North America\Burai Fighter.nes 
    0xDE8FD935, //  Licensed North America\To the Earth.nes 
    0xF3ECFDAA, //  Modern\Virtual Console\Adventures of Lolo 2 (NTSC).nes 
    0x0751D47D, //  Multicarts\extracts\Panda Adventure (CB-4034).nes 
/**** 217 mapper=4 prgrom=32K chrrom=32K mirror=H system=SYSTEM_PAL */
    0x853FEEA4, //  Licensed PAL\Adventures of Lolo 2.nes 
    0x28492586, //  Licensed PAL\Burai Fighter.nes 
    0x790B295B, //  Licensed PAL\To the Earth.nes 
    0x6F41C26D, //  Modern\Virtual Console\Adventures of Lolo 2 (PAL).nes 
/**** 218 mapper=4 prgrom=32K chrrom=40K mirror=H */
    0xF249075D, //  Homebrew\Demos\Pulsewave.nes 
/**** 219 mapper=4 prgrom=32K chrrom=56K mirror=H */
    0xD47164E2, //  Homebrew\Demos\Super Stalin Bros. [TAONGAD].nes 
/**** 220 mapper=4 prgrom=32K chrrom=64K mirror=H */
    0x146B5C5A, //  Bootleg Singles\TouchGamePlayer\Angry Birds.nes 
    0x7A3D7004, //  Homebrew\Crazy Bus [FG Software].nes 
/**** 221 mapper=4 prgrom=32K chrrom=128K mirror=H */
    0x54346B3F, //  Homebrew\ChuChu Rocket!.nes 
/**** 222 mapper=4 prgrom=32K chrrom=256K mirror=H system=SYSTEM_PAL */
    0x5204949E, //  Homebrew\Demos\Shaping Reality [8bitpeoples].nes 
/**** 223 mapper=4 prgrom=64K chrram=8K mirror=H */
    0x54F93D92, //  Homebrew\NSF Players\Kirby's Dream Land.nes 
/**** 224 mapper=4 prgrom=64K chrrom=8K mirror=H */
    0xFD0BB368, //  Homebrew\Demos\Rockman 9.nes 
    0xB1C964A8, //  Homebrew\Mega Man 9 Project.nes 
/**** 225 mapper=4 prgrom=64K chrrom=16K mirror=H */
    0xF71AB1B0, //  Plug-and-Play\extracts\Sky Zone.nes 
/**** 226 mapper=4 prgrom=64K chrrom=16K mirror=H system=SYSTEM_NTSC */
    0xA39A8F6E, //  Homebrew\Hacks\Super Mario Bros. series\Extra Mario Bros. [ATA].nes 
/**** 227 mapper=4 prgrom=64K chrrom=32K mirror=H */
    0x9C2DE911, //  Bootleg Singles\The Tiny Toon 3 Crazy Castle.nes 
    0x95128CA0, //  Bootleg Singles\TouchGamePlayer\R.B.I. Baseball.nes 
    0xFD5F2D07, //  Compatibility Hacks\Vs. Gradius [home console].nes 
    0x8D7CBF44, //  Homebrew\Wraith.nes 
/**** 228 mapper=4 prgrom=64K chrrom=32K mirror=H system=SYSTEM_NTSC */
    0x235B54DB, //  Unlicensed China\TCL王牌꞉ 王牌海战.nes 
/**** 229 mapper=4 prgrom=64K chrrom=64K mirror=H */
    0xCCD18661, //  Bootleg Hacks\II International FIFA 97.nes 
    0xD56EF1E3, //  Bootleg Hacks\The Tiny Toon 3 Crazy Castle.nes 
    0x0A6E9645, //  Bootleg Singles\TouchGamePlayer\FIFA III - 2006.nes 
    0x7A2CC96C, //  Bootleg Singles\TouchGamePlayer\Tecmo World Cup Soccer.nes 
    0x94E27E04, //  Homebrew\Hacks\Aoki Densetsu Shoot꞉ Tecmo World Cup Soccer [Knighteen87].nes 
    0x9CBB0291, //  Licensed Japan\Super Sprint.nes 
    0x8A5BC0D3, //  Licensed Japan\Tecmo World Cup Soccer.nes 
    0x96F328FF, //  Plug-and-Play\extracts\Jungle Adventure.nes 
/**** 230 mapper=4 prgrom=64K chrrom=64K mirror=H system=SYSTEM_PAL */
    0x86E02D65, //  Licensed PAL\Tecmo World Cup Soccer.nes 
/**** 231 mapper=4 prgrom=64K chrrom=64K mirror=H system=SYSTEM_NTSC */
    0x153EB948, //  Unlicensed China\鋤大D.nes 
/**** 232 mapper=4 prgrom=128K chrram=8K mirror=H */
    0xEB637549, //  Homebrew\Demos\Sorcerian for FC [Coke774].nes 
    0xB5E24324, //  Licensed Japan\Ninja Crusaders꞉ 龍牙.nes 
    0x3D1C4894, //  Licensed North America\Ninja Crusaders.nes 
/**** 233 mapper=4 prgrom=128K prgram=8K chrram=8K mirror=H */
    0x023EDD1A, //  Compatibility Hacks\もえろ TwinBee꞉ Cinnamon 博士を救え! (LG25)[m004].nes 
    0xDE6A3FF9, //  Homebrew\Demos\Ray Casting Demo [Celius].nes 
    0x25E26057, //  Homebrew\NESdev Project.nes 
    0xCF992E34, //  Unreleased\Blockout.nes 
/**** 234 mapper=4 prgrom=128K chrrom=8K mirror=H */
    0x60037099, //  Homebrew\The Tao of 007.nes 
/**** 235 mapper=4 prgrom=128K chrrom=16K mirror=H */
    0x334E69AC, //  Homebrew\Bugfixes, Improvements\Dikki Paingun in TKO for the Third Reich [real hardware].nes 
    0x75378399, //  Homebrew\Dikki Paingun in TKO for the Third Reich.nes 
/**** 236 mapper=4 prgrom=128K chrrom=32K mirror=H */
    0x558F7A7C, //  Homebrew\Utaco (Final).nes 
    0xF7CF24FB, //  Homebrew\Utaco.nes 
    0x83FC38F8, //  Licensed North America\Mappy-Land.nes 
    0xDFEEF042, //  Plug-and-Play\extracts\Vanguard.nes 
/**** 237 mapper=4 prgrom=128K chrrom=32K mirror=H system=SYSTEM_NTSC */
    0xE2F77868, //  Modern\Virtual Console\Mappy-Land.nes 
/**** 238 mapper=4 prgrom=128K chrrom=64K mirror=H */
    0xF0B1B568, //  Bootleg Hacks\Coast Speed.nes 
    0x0652445D, //  Bootleg Hacks\Mario 12.nes 
    0x0EE54B6A, //  Bootleg Singles\TouchGamePlayer\Rock 'n Ball.nes 
    0x70178745, //  Bootleg Singles\TouchGamePlayer\The Tiny Toon 3 Crazy Castle.nes 
    0xBA6CA54A, //  Homebrew\Galaxxon꞉ The Third War (alt)[Psych].nes 
    0x565AB57E, //  Homebrew\Galaxxon꞉ The Third War [Psych].nes 
    0x3B341B9C, //  Homebrew\Vigilante Ninja 2.nes 
    0x3CCB5D57, //  Licensed Japan\Klax.nes 
    0xF66EC512, //  Licensed Japan\YoshiのCookie.nes 
    0xA0C31A57, //  Licensed North America\Indiana Jones and the Temple of Doom (rev0).nes 
    0xB4C81ADB, //  Licensed North America\Indiana Jones and the Temple of Doom (rev1).nes 
    0xF181C021, //  Licensed North America\Legacy of the Wizard.nes 
    0x263AC8A0, //  Licensed North America\Rampage.nes 
    0x476E022B, //  Licensed North America\Rock 'n Ball.nes 
    0x52B58732, //  Licensed North America\Yoshi's Cookie.nes 
    0xE421E2B3, //  Modern\Namco Museum Archives\Legacy of the Wizard.nes 
    0x775D14CC, //  Modern\Star Versus (TLROM).nes 
    0x64FDE029, //  Modern\Virtual Console\Yoshi's Cookie (NTSC).nes 
    0x4CAECD7D, //  Plug-and-Play\extracts\Justice.nes 
    0x98F71BB0, //  Plug-and-Play\extracts\War.nes 
    0x7D6B3BA2, //  Unlicensed Taiwan&amp;Hong Kong\Pocket Monster.nes 
/**** 239 mapper=4 prgrom=128K chrrom=64K mirror=H system=SYSTEM_PAL */
    0xD84AD16C, //  Homebrew\Demos\High Hopes [aspekt].nes 
    0xE37A39AB, //  Licensed PAL\Yoshi's Cookie.nes 
/**** 240 mapper=4 prgrom=128K chrrom=64K mirror=H system=SYSTEM_NTSC */
    0x6540FBE3, //  Unreleased\Music Box.nes 
/**** 241 mapper=4 prgrom=128K chrrom=88K mirror=H */
    0xC8D69902, //  Unreleased\Mick &amp; Mack as the Global Gladiators (1993-05-28 #1).nes 
    0xCDEE5934, //  Unreleased\Mick &amp; Mack as the Global Gladiators (1993-05-28 #2).nes 
/**** 242 mapper=4 prgrom=128K chrrom=96K mirror=H */
    0xAE4DD2E9, //  Unreleased\Mick &amp; Mack as the Global Gladiators (1993-06-16).nes 
/**** 243 mapper=4 prgrom=128K chrrom=128K mirror=H */
    0xC23025CD, //  Bad Dumps\Batman Flash [bad CHR].nes 
    0x52D8EE1C, //  Bad Dumps\Nintendo World Cup [missing PROM].nes 
    0x8A02A8FD, //  Bad Dumps\Pokémon Blue [bad CHR].nes 
    0x4A6E8161, //  Bad Dumps\Pokémon Silver [bad CHR, text missing].nes 
    0xA2BD26DE, //  Bad Dumps\SMB Tiny Mario Adventures [bad CHR].nes 
    0xFF5BC56A, //  Bootleg Hacks\4-in-1 忍者2系列 (AA6156).nes 
    0x64558FC4, //  Bootleg Hacks\4-in-1 忍者3系列 (AA6157).nes 
    0x5452966E, //  Bootleg Hacks\9-in-1 キャッ党忍伝てやんでえ.nes 
    0x259CFDFE, //  Bootleg Hacks\Barve Boy Kung Fu Pokechu.nes 
    0x6463537F, //  Bootleg Hacks\Batman Flash.nes 
    0x144DD305, //  Bootleg Hacks\Behind Enemy Lines.nes 
    0x4DD2E5AD, //  Bootleg Hacks\Caesar Captain.nes 
    0x030B8EDA, //  Bootleg Hacks\Chip &amp; Dale 3 (C-D3).nes 
    0xC5DFAB68, //  Bootleg Hacks\D.J. Boy.nes 
    0x0FA20273, //  Bootleg Hacks\Disney Timon and Pumbaa.nes 
    0x94C8C39C, //  Bootleg Hacks\Domkey Kong.nes 
    0x8550ADE9, //  Bootleg Hacks\Donkey Kong.nes 
    0x9066422E, //  Bootleg Hacks\FIFA Soccer 99.nes 
    0x2F24B4EE, //  Bootleg Hacks\Felix vs. Jerry (F-Y2).nes 
    0x05A8434E, //  Bootleg Hacks\Goal 3.nes 
    0x16D340C0, //  Bootleg Hacks\Golden Axe IV.nes 
    0xE9E12799, //  Bootleg Hacks\Gun-Nac Super 4-in-1.nes 
    0x1D90849B, //  Bootleg Hacks\Happy Angel Legend.nes 
    0xDB747F71, //  Bootleg Hacks\Harry Potter (Panic Restaurant).nes 
    0xCE083927, //  Bootleg Hacks\Harry Wrestle.nes 
    0x1657C053, //  Bootleg Hacks\Inventor\Bolide.nes 
    0x9F30F2F1, //  Bootleg Hacks\Inventor\Bruce &amp; Leo.nes 
    0x81348220, //  Bootleg Hacks\Inventor\Fairy.nes 
    0xFDF63E2D, //  Bootleg Hacks\Inventor\Harry Story.nes 
    0xB4912D58, //  Bootleg Hacks\Inventor\Hellfire.nes 
    0x22C50F32, //  Bootleg Hacks\Inventor\Space War.nes 
    0x00C76093, //  Bootleg Hacks\Inventor\Street War.nes 
    0x946F44C9, //  Bootleg Hacks\Inventor\X-War.nes 
    0x744A0721, //  Bootleg Hacks\Mario 16.nes 
    0x79644224, //  Bootleg Hacks\MewtwoのPokemon II.nes 
    0x7747C866, //  Bootleg Hacks\Monsters (Kero Kero Keroppiの大冒険 2).nes 
    0xD9CE842C, //  Bootleg Hacks\Monsters (The Krion Conquest).nes 
    0xF561F601, //  Bootleg Hacks\Panda World.nes 
    0x24F4F397, //  Bootleg Hacks\Pikachu.nes 
    0xD3D5E6B8, //  Bootleg Hacks\Pokémon 2000 (三つ目がとおる).nes 
    0x833C98CB, //  Bootleg Hacks\Pokémon Blue.nes 
    0x3E5673E1, //  Bootleg Hacks\Pokémon Golden.nes 
    0x41D88B6C, //  Bootleg Hacks\Pokémon Green.nes 
    0x1E4576CE, //  Bootleg Hacks\Pokémon Silver.nes 
    0x72E229FE, //  Bootleg Hacks\Racing Car.nes 
    0x25678547, //  Bootleg Hacks\Rush Hour.nes 
    0x843FA4D8, //  Bootleg Hacks\Silk 2.nes 
    0xEAC2F56F, //  Bootleg Hacks\Spartan X II 4-in-1 (KT-220B).nes 
    0x0A090C11, //  Bootleg Hacks\Spiderman 2.nes 
    0xC6E1FF1D, //  Bootleg Hacks\Street Fighter VI (12 People).nes 
    0xB19CBC98, //  Bootleg Hacks\Super Bros. 16.nes 
    0x08D9DDAB, //  Bootleg Hacks\Super Bros. 6.nes 
    0xAAC75B3A, //  Bootleg Hacks\Super Contra 6.nes 
    0xE0B73FC2, //  Bootleg Hacks\Super Fighter.nes 
    0x9BB66333, //  Bootleg Hacks\Super Fighters.nes 
    0x5DD6ECB9, //  Bootleg Hacks\Super Mario 15.nes 
    0xF2B44332, //  Bootleg Hacks\Super Mario World 9 (1996).nes 
    0x70667F85, //  Bootleg Hacks\Super Mario World 9 (Cheat).nes 
    0xA3952B0E, //  Bootleg Hacks\Super PoPo's Adventure.nes 
    0xA4F15D7E, //  Bootleg Hacks\The Hulk.nes 
    0xFFF564E7, //  Bootleg Hacks\The Legend of Kreshna.nes 
    0x1ADD955A, //  Bootleg Hacks\The Lion King (Wacky Races).nes 
    0x4C91050C, //  Bootleg Hacks\Toita Basketball.nes 
    0xAB9DE91F, //  Bootleg Hacks\Totally Rad 4-in-1 (KT-220B).nes 
    0x2B9BC5C6, //  Bootleg Hacks\Turtle Ninja 5.nes 
    0x29B1FDFC, //  Bootleg Hacks\Waddy Boy.nes 
    0x77DA46E1, //  Bootleg Hacks\X-Plan 勇斗士.nes 
    0xFE335482, //  Bootleg Hacks\ドキ! ドキ! 遊園地꞉ Crazy Land 大作戦 (level select).nes 
    0x14D71ACB, //  Bootleg Hacks\怪鴨 II - Darkwing Duck 2.nes 
    0x848DC2A4, //  Bootleg Hacks\獅子王 - The Lion King.nes 
    0x9CD5128C, //  Bootleg Hacks\金剛戰士 2 - Power Rangers.nes 
    0xEA2511A4, //  Bootleg Singles\Bananan 王子の大冒険 (S-P6).nes 
    0x0BF5F232, //  Bootleg Singles\Barbie (KT-2128).nes 
    0xE58D46F1, //  Bootleg Singles\Captain Planet and The Planeteers (VT-3121).nes 
    0xAB2DC186, //  Bootleg Singles\Disneyland Adventures (P-C5).nes 
    0xB2E9B97B, //  Bootleg Singles\Heavy Barrel.nes 
    0x5135EAE2, //  Bootleg Singles\Hunting in Red October (H-R1).nes 
    0x29F81288, //  Bootleg Singles\James Bond Jr. (J-D1).nes 
    0x13DEF8D9, //  Bootleg Singles\Joe &amp; Mac꞉ Caveman Ninja (J-C5).nes 
    0x186B3234, //  Bootleg Singles\Kick Master (NT-6079).nes 
    0x042107A8, //  Bootleg Singles\Kick Master.nes 
    0x858184F8, //  Bootleg Singles\Kiwi Kraze (K-E3).nes 
    0xC7F3616E, //  Bootleg Singles\Magical Doropie (M-D2).nes 
    0xEC106744, //  Bootleg Singles\Monster in My Pocket (M-I6).nes 
    0x59949CA4, //  Bootleg Singles\NBA '98 (NT-6098).nes 
    0x349678D7, //  Bootleg Singles\NBA Live '96 (NT-865).nes 
    0xB56DA862, //  Bootleg Singles\Samurai Zombie Nation (LEE08072).nes 
    0x945020B7, //  Bootleg Singles\Super 魂斗羅 (1990 Yellow).nes 
    0xEE21F263, //  Bootleg Singles\Super 魂斗羅 (S-P1).nes 
    0x07AC930F, //  Bootleg Singles\Super 魂斗羅.nes 
    0x28C4E1F8, //  Bootleg Singles\Tet Away (CTC-xx).nes 
    0xBC5F9C88, //  Bootleg Singles\The Punisher.nes 
    0xCA2BD63D, //  Bootleg Singles\The Terminator (A-Q3).nes 
    0x7E90F5B1, //  Bootleg Singles\Tiny Toon Adventures 2꞉ Trouble in Wackyland (LEE05040).nes 
    0xE7962A37, //  Bootleg Singles\TouchGamePlayer\Astyanax.nes 
    0xBFC07ACE, //  Bootleg Singles\TouchGamePlayer\Blaster Master.nes 
    0x1AF0296E, //  Bootleg Singles\TouchGamePlayer\Bubble Bobble Part 2.nes 
    0x08D5A6EC, //  Bootleg Singles\TouchGamePlayer\ChipとDaleの大作戦.nes 
    0x57B247B5, //  Bootleg Singles\TouchGamePlayer\Contra Force.nes 
    0xB5C286F6, //  Bootleg Singles\TouchGamePlayer\Contra.nes 
    0x517F9472, //  Bootleg Singles\TouchGamePlayer\Crash.nes 
    0x15F650B6, //  Bootleg Singles\TouchGamePlayer\Darkwing Duck.nes 
    0x9E65756D, //  Bootleg Singles\TouchGamePlayer\Double Dragon III꞉ The Sacred Stones.nes 
    0x398E7542, //  Bootleg Singles\TouchGamePlayer\Double Dragon II꞉ The Revenge.nes 
    0x95041F4F, //  Bootleg Singles\TouchGamePlayer\Double Dragon.nes 
    0xF0047ED9, //  Bootleg Singles\TouchGamePlayer\Fire 'n Ice.nes 
    0x845D01CB, //  Bootleg Singles\TouchGamePlayer\Gun-Nac.nes 
    0xFCEF82C4, //  Bootleg Singles\TouchGamePlayer\Hudson's Adventure Island III.nes 
    0x51460FB8, //  Bootleg Singles\TouchGamePlayer\Mad Max.nes 
    0x26D0FFF3, //  Bootleg Singles\TouchGamePlayer\Mission꞉ Impossible.nes 
    0xC216749C, //  Bootleg Singles\TouchGamePlayer\Monster in My Pocket.nes 
    0xBC6E3FA6, //  Bootleg Singles\TouchGamePlayer\P.O.W. - Prisoners of War.nes 
    0x7D02DACA, //  Bootleg Singles\TouchGamePlayer\Panic Restaurant.nes 
    0x94F40662, //  Bootleg Singles\TouchGamePlayer\Teenage Mutant Ninja Turtles꞉ Tournament Fighters.nes 
    0x65D83C5E, //  Bootleg Singles\TouchGamePlayer\Wacky Races.nes 
    0x2E181061, //  Bootleg Singles\TouchGamePlayer\Whomp'em.nes 
    0xD53001B1, //  Bootleg Singles\TouchGamePlayer\三つ目がとおる.nes 
    0x6EE42BE4, //  Bootleg Singles\TouchGamePlayer\忍者龍剣伝 III꞉ 黄泉の方船.nes 
    0x8B448883, //  Bootleg Singles\TouchGamePlayer\忍者龍剣伝 II꞉ 暗黒の邪神剣.nes 
    0x1D6FC99E, //  Bootleg Singles\ドキ! ドキ! 遊園地꞉ Crazy Land 大作戦 (KT-2214).nes 
    0x7BF23B82, //  Bootleg Singles\人間兵器 Dead Fox (P-H3).nes 
    0x1A788A4F, //  Compatibility Hacks\8 Eyes (Piko Interactive)[m004 extract].nes 
    0xAF660ED1, //  Compatibility Hacks\Alps Skiing [m004].nes 
    0x1162C238, //  Compatibility Hacks\Holy Diver (Collector's Edition) [m004 extract].nes 
    0x19BA3688, //  Compatibility Hacks\Mickey's Safari in Letterland [Nintendo MMC3].nes 
    0xBCC91AB3, //  Compatibility Hacks\Super Hang-On [m004].nes 
    0xDEE05A23, //  Compatibility Hacks\Super Mario 14 [m004].nes 
    0x1F41220E, //  Compatibility Hacks\TV System Conversions\Probotector II꞉ Return of the Evil Forces (NTSC)[NewRisingSun].nes 
    0x3E85BA0F, //  Compatibility Hacks\獵殺侏羅紀 - Blood of Jurassic [m004][NewRisingSun].nes 
    0xF911383F, //  Homebrew\Almost Hero.nes 
    0xCE47005A, //  Homebrew\Bugfixes, Improvements\Batman꞉ The Video Game.nes 
    0x9824E1ED, //  Homebrew\Bugfixes, Improvements\Golden Axe IV [correct mirroring glitch in stage 2-2][NewRisingSun].nes 
    0x1F20DF6E, //  Homebrew\Bugfixes, Improvements\War of Strike Mouse [Hot Dance 3-in-1 Disabled].nes 
    0xE04F341F, //  Homebrew\Cheats\Street Fighter VI (12 People)[select+d-pad].nes 
    0x0212CA04, //  Homebrew\D-Pad Hero (v1.0).nes 
    0x3B34D264, //  Homebrew\D-Pad Hero (v1.1).nes 
    0xA0DA7750, //  Homebrew\D-Pad Hero II.nes 
    0x187821EA, //  Homebrew\Demos\Heosphoros꞉ Embered Recollections.nes 
    0xE2BE7014, //  Homebrew\Hacks\8 Eyes to Castlevania Conversion (v0.1)[Elbobelo].nes 
    0xFE822F1A, //  Homebrew\Hacks\8 Eyes to Castlevania Conversion (v0.2)[Elbobelo].nes 
    0xFB2CC161, //  Homebrew\Hacks\8 Eyes to Castlevania Conversion (v0.3)[Elbobelo].nes 
    0xC0BFE9D1, //  Homebrew\Hacks\Adventure Island 3꞉ The Lost Aisles [Aether-K].nes 
    0x3B532B87, //  Homebrew\Hacks\Astyanax Remix [Googie].nes 
    0x422D0724, //  Homebrew\Hacks\Batman (v1.2)[Macbee].nes 
    0x97CAB75C, //  Homebrew\Hacks\Batman Robin꞉ Shadows of Gotham (v1.1)[Magnus Nilsson].nes 
    0xA382A2EC, //  Homebrew\Hacks\Batman with Assassins Creed Enzo [Cakewarden].nes 
    0xAFA64F3D, //  Homebrew\Hacks\Batman꞉ The Dark Knight [Osman Color].nes 
    0x318CC88C, //  Homebrew\Hacks\Batman꞉ The Video Game - Easy Mode [deespence2929].nes 
    0x9ED18B09, //  Homebrew\Hacks\Batman꞉ The Video Game - Simplified [Megafield64].nes 
    0x5629A3B3, //  Homebrew\Hacks\Bloody City.nes 
    0x8EF4A299, //  Homebrew\Hacks\Cokehead Junkie.nes 
    0x449D046E, //  Homebrew\Hacks\Fake News Covfefe [Lancuster].nes 
    0xE0088490, //  Homebrew\Hacks\German City.nes 
    0x423DE7C5, //  Homebrew\Hacks\Ghost Ninja.nes 
    0x27C920F2, //  Homebrew\Hacks\Inner City Ransom.nes 
    0x329B7D1A, //  Homebrew\Hacks\Pig City Ransom.nes 
    0x63E0DF7A, //  Homebrew\Hacks\Super Mario Adventures.nes 
    0x75355437, //  Homebrew\Hacks\Wendy 2꞉ Lost Krion [Korxtendo].nes 
    0x33D7B629, //  Homebrew\Hacks\Wolverine - Easy Version.nes 
    0xC11E40E9, //  Homebrew\Translations\Chinese (Simplified)\成龙的龙.nes 
    0xA8F4B45E, //  Homebrew\Translations\English\Adventures in Asmik Land [Gil Galad].nes 
    0x9C5C642E, //  Homebrew\Translations\English\Altered Beast [Aka Translations].nes 
    0xE548385B, //  Homebrew\Translations\English\Astro Fang꞉ Super Machine [Caution].nes 
    0x7562AE12, //  Homebrew\Translations\English\Brave Fighter of the Sun꞉ Fighbird [Zynk Oxhyde v1.2].nes 
    0xD038F796, //  Homebrew\Translations\English\Castle Quest [Hubz, Stardust Crusaders].nes 
    0x0E52E198, //  Homebrew\Translations\English\Chef Cookie in Gourmet World [Stardust Crusaders v1.01].nes 
    0x2CF36BD8, //  Homebrew\Translations\English\Contra [Quest Games v1.1].nes 
    0x15DABEEE, //  Homebrew\Translations\English\Crash 'n the Boys꞉ Fighting Challenge [oRdErEDchaos, El Duderino v1.1].nes 
    0xDC062DED, //  Homebrew\Translations\English\Crash 'n the Boys꞉ Ice Challenge [Disconnected Translations v0.99, GAFF Translations, El Duderino v1.0].nes 
    0xA4F28F29, //  Homebrew\Translations\English\Crash 'n the Boys꞉ Technos Cup [PentarouZero, Lipetsk, El Duderino].nes 
    0x94B3F887, //  Homebrew\Translations\English\Crossfire (v1.00)[Stardust Crusaders].nes 
    0x2546F95E, //  Homebrew\Translations\English\Directive꞉ Solbrain - Super Rescue [Aeon Genesis v1.01].nes 
    0x0E66F0CD, //  Homebrew\Translations\English\Doki! Doki! Amusement Park [Klarth Aileron v1.1].nes 
    0x7243CA86, //  Homebrew\Translations\English\Double Dragon III꞉ The Rosetta Stone [Pacnsacdave].nes 
    0x029A0A3C, //  Homebrew\Translations\English\Dream Penguin Adventure꞉ The Quest for Penko's Heart [Vice Translations v1.021].nes 
    0x47A55035, //  Homebrew\Translations\English\Exciting! Amusement Park [Pacnsacdave].nes 
    0x110D7233, //  Homebrew\Translations\English\Fighting Road [Immutable].nes 
    0x5128A451, //  Homebrew\Translations\English\Fuzzical Fighter [WakdHacks v1.00b].nes 
    0x7042F07A, //  Homebrew\Translations\English\Go Go! Nekketu Hockey Club꞉ Multi-Sport Battle [Disconnected Translations v0.99, GAFF Translations v1.00].nes 
    0x18F4F8BF, //  Homebrew\Translations\English\Gun-Nac [Zynk Oxhyde v1.1].nes 
    0x953FFE4C, //  Homebrew\Translations\English\Hero of the Sun꞉ Fire Bird [Dragoon-X Translations v1.01].nes 
    0xFC8F6A1C, //  Homebrew\Translations\English\Ice Ice! Hockey Challenge [TransBRC v1.1].nes 
    0x91E2EDE7, //  Homebrew\Translations\English\Jigoku Gokurakumaru - The Circle of Heaven &amp; Hell [Stardust Crusaders v1.01].nes 
    0xAD2C0E33, //  Homebrew\Translations\English\Jumpin' Kid꞉ Jack and the Beanstalk [RistarTheStar].nes 
    0xA23DEF56, //  Homebrew\Translations\English\Kunio-Kun's Nekketsu Soccer League [PentarouZero v1.2, Lipetsk v1.0].nes 
    0xDE8C17DC, //  Homebrew\Translations\English\Kunio-Kun's Nekketsu Soccer League [PentarouZero v1.2].nes 
    0x50B427DE, //  Homebrew\Translations\English\Kyoryu Sentai ZyuRanger [Grimm Translations v0.99].nes 
    0x980015F8, //  Homebrew\Translations\English\Legend of the River King [Pikachumanson v0.91].nes 
    0xB3EBA294, //  Homebrew\Translations\English\Magical Doropie [Video Smash Excellent].nes 
    0x762653B1, //  Homebrew\Translations\English\Mickey Mouse III꞉ Dream Balloon [NikcDC v1.1].nes 
    0x852F9556, //  Homebrew\Translations\English\Mighty Morphin Power Rangers [Grimm Translations v0.99, Pacnsacdave v1.0].nes 
    0x14BF3B83, //  Homebrew\Translations\English\Nekketsu Highschool Dodgeball Club꞉ Soccer Edition [Ghost-Tank v0.9].nes 
    0x8EFB0437, //  Homebrew\Translations\English\Nekketsu! Street Basket - Go for it, Dunk Heroes! [Farid v1.2].nes 
    0xFC217B45, //  Homebrew\Translations\English\Nekketu Cross-Country [Disconnected Translations v0.15].nes 
    0x3701D5B0, //  Homebrew\Translations\English\Parallel World [PentarouZero].nes 
    0x44D05FFB, //  Homebrew\Translations\English\Parasol Henbee [Zynk Oxhyde].nes 
    0xA9D9714A, //  Homebrew\Translations\English\Raging Fire꞉ Recca [Aeon Genesis].nes 
    0xECE951EA, //  Homebrew\Translations\English\Riki Kunio [oRdErEDchaos v0.95].nes 
    0x8CBC305C, //  Homebrew\Translations\English\River City Ransom [FANS v1.1].nes 
    0x9FC18DDF, //  Homebrew\Translations\English\SD Hero Total Battle꞉ Defeat! The Army of Evil [Corvo].nes 
    0xFB7B0EF7, //  Homebrew\Translations\English\Super Rescue꞉ Solbrain [Chronix].nes 
    0xE17C6B19, //  Homebrew\Translations\English\Superstar Pro Wrestling [Pacnsacdave].nes 
    0xA180BD71, //  Homebrew\Translations\English\Taito Chase H.Q. [Pacnsacdave].nes 
    0x31D019D5, //  Homebrew\Translations\English\The Three-Eyed One [Arrogant v1.1].nes 
    0xEF6508F0, //  Homebrew\Translations\English\The Three-Eyed One [Stardust Crusaders].nes 
    0xB5FC2388, //  Homebrew\Translations\English\Three Kingdoms꞉ Sichuan Mahjong [Pacnsacdave].nes 
    0x656D1479, //  Homebrew\Translations\English\Time Zone [KingMike's Translations v1.3].nes 
    0xB97C02C0, //  Homebrew\Translations\English\Underground Mission [Pacnsacdave].nes 
    0x0C41C069, //  Homebrew\Translations\English\Warwolf Chronicles [Stardust Crusaders].nes 
    0xF0529512, //  Homebrew\Translations\English\Zoids꞉ Apocalypse [MrRichard999 v1.3].nes 
    0xA3546A5A, //  Homebrew\Translations\English\それいけ! Anpanman꞉ Everyones Hiking Game! [Pope Hentai's Rom Hacking Fun! v0.99].nes 
    0x8FD22A43, //  Homebrew\Translations\English\わんぱくコックンのGourmet World [The Spoony Bard].nes 
    0xD3393171, //  Homebrew\Translations\English\キャッ党忍伝てやんでえ [Dark Mark].nes 
    0xFB8672A2, //  Homebrew\Translations\English\三つ目がとおる [The Spoony Bard v1.01].nes 
    0xF8D53171, //  Licensed Japan\8 Eyes.nes 
    0x15141401, //  Licensed Japan\Asmikくん Land.nes 
    0xE949EF8A, //  Licensed Japan\Astro Fang꞉ Super Machine.nes 
    0x66ED9C00, //  Licensed Japan\Bananan 王子の大冒険.nes 
    0x2B4D80AE, //  Licensed Japan\Battle Formula.nes 
    0x26E82008, //  Licensed Japan\Bucky O'Hare.nes 
    0xBF4F4BA6, //  Licensed Japan\Castle Quest.nes 
    0xE95454FC, //  Licensed Japan\Crossfire.nes 
    0x6AE69227, //  Licensed Japan\Double Dragon III꞉ The Rosetta Stone.nes 
    0xA66596D9, //  Licensed Japan\Double Dragon II꞉ The Revenge.nes 
    0x2A3CA509, //  Licensed Japan\Downtown 熱血物語.nes 
    0x3E470FE0, //  Licensed Japan\Downtown 熱血行進曲 - それゆけ大運動会.nes 
    0xC48DDB52, //  Licensed Japan\Dragon's Lair.nes 
    0x974D0745, //  Licensed Japan\Fighting Road.nes 
    0x5E24EEDA, //  Licensed Japan\Fuzzical Fighter.nes 
    0x27D34A57, //  Licensed Japan\Golgo 13 第二章꞉ Icarusの謎.nes 
    0x175EDA0B, //  Licensed Japan\Great Boxing꞉ Rush Up.nes 
    0x619BEA12, //  Licensed Japan\Gun-Dec.nes 
    0xD9084936, //  Licensed Japan\Gun-Nac.nes 
    0x2BCF2132, //  Licensed Japan\Heavy Barrel.nes 
    0x59280BEC, //  Licensed Japan\Jackie Chan.nes 
    0xB976219A, //  Licensed Japan\Jumpin' Kid꞉ Jackと豆の木ものがたり.nes 
    0x7EC6F75B, //  Licensed Japan\Magical Doropie.nes 
    0xFE4ED42B, //  Licensed Japan\Max Warrior꞉ 惑星戒厳令.nes 
    0x2B160BF0, //  Licensed Japan\Mighty Final Fight.nes 
    0x3B1A7EEF, //  Licensed Japan\Murder Club.nes 
    0x3BE91A23, //  Licensed Japan\Nemo꞉ Pajama Hero.nes 
    0x2DC331A2, //  Licensed Japan\New York Nyankies.nes 
    0xF08E8EF0, //  Licensed Japan\Parallel World.nes 
    0xC769BB34, //  Licensed Japan\Parasol Henbee.nes 
    0x530BCCB4, //  Licensed Japan\Red Arremer II.nes 
    0xC09227A0, //  Licensed Japan\RoboCop.nes 
    0xE78A394C, //  Licensed Japan\SD Battle 大相撲꞉ 平成 Hero 場所.nes 
    0xDBECE74F, //  Licensed Japan\SD Hero 総決戦꞉ 倒せ! 悪の軍団.nes 
    0x18A885B0, //  Licensed Japan\Street Fighter 2010.nes 
    0xF31DCC15, //  Licensed Japan\Summer Carnival '92꞉ Recca.nes 
    0x26BFED27, //  Licensed Japan\Super Chinese 2꞉ Dragon Kid.nes 
    0xB1250D0C, //  Licensed Japan\Super 魂斗羅.nes 
    0x705BD7C3, //  Licensed Japan\Superstar Pro Wrestling.nes 
    0xDF3776C6, //  Licensed Japan\Sword Master.nes 
    0x69565F13, //  Licensed Japan\TM Network꞉ Live in Power Bowl.nes 
    0x4F16C504, //  Licensed Japan\Taito Basketball.nes 
    0x99D38676, //  Licensed Japan\Taito Chase H.Q..nes 
    0xD27B9D50, //  Licensed Japan\Terminator 2꞉ Judgment Day.nes 
    0xF927FA43, //  Licensed Japan\The Blue Marlin.nes 
    0xE305202E, //  Licensed Japan\Time Zone.nes 
    0x4AEA40F7, //  Licensed Japan\Tom &amp; Jerry (&amp; Tuffy).nes 
    0x6800C5B3, //  Licensed Japan\Tom Sawyerの冒険.nes 
    0xCA96AD0E, //  Licensed Japan\Top Gun꞉ Dual Fighters.nes 
    0x213CB3FB, //  Licensed Japan\U.S. Championship V'Ball.nes 
    0x7980C4F7, //  Licensed Japan\Ultraman Club 2꞉ 帰ってきた Ultraman 倶楽部.nes 
    0xF3623561, //  Licensed Japan\Zoids꞉ 黙示録.nes 
    0x4F032933, //  Licensed Japan\いけいけ熱血 Hockey部꞉ すべってころんで大乱闘.nes 
    0x4B5177E9, //  Licensed Japan\くにおくんの熱血 Soccer League.nes 
    0xFE99BBED, //  Licensed Japan\それいけ! アンパンマン꞉ みんなで Hiking Game!.nes 
    0x4E99CEA4, //  Licensed Japan\びっくり熱血新記録! はるかなる金 Medal.nes 
    0xD568563F, //  Licensed Japan\わんぱくコックンのGourmet World.nes 
    0x45878D7F, //  Licensed Japan\キャッ党忍伝てやんでえ.nes 
    0x20AF7E1A, //  Licensed Japan\チキチキ Machine 猛 Race.nes 
    0xBD50F230, //  Licensed Japan\ドキ! ドキ! 遊園地꞉ Crazy Land 大作戦.nes 
    0x9EEFB4B4, //  Licensed Japan\パチ Slot Adventure 2꞉ そろっ太くんのパチSlot 探偵団.nes 
    0x051CD5F2, //  Licensed Japan\パチ Slot Adventure 3꞉ ビタオシー７見参!.nes 
    0xC8EDC97E, //  Licensed Japan\三つ目がとおる.nes 
    0x1C2A58FF, //  Licensed Japan\中島悟 F-1 Hero 2.nes 
    0x768A1B6A, //  Licensed Japan\中島悟 F-1 Hero.nes 
    0x2E1E7FD8, //  Licensed Japan\人間兵器 Dead Fox.nes 
    0x20C5D187, //  Licensed Japan\伝染るんです.nes 
    0xF32748A1, //  Licensed Japan\千代の富士の大銀杏.nes 
    0x7840B18D, //  Licensed Japan\地底戦空 Vazolder.nes 
    0x045E8CD8, //  Licensed Japan\地獄極楽丸.nes 
    0xCBFB6DE5, //  Licensed Japan\夢 Penguin 物語.nes 
    0x78211EBF, //  Licensed Japan\大工の源さん.nes 
    0x948E0BD6, //  Licensed Japan\太陽の勇者 Firebird.nes 
    0x1ED7D6BE, //  Licensed Japan\山村美紗 Suspense꞉ 京都花の密室殺人事件.nes 
    0xF4E5DF0E, //  Licensed Japan\川のぬし釣り.nes 
    0x2F2E30F7, //  Licensed Japan\忍者龍剣伝 III꞉ 黄泉の方船.nes 
    0x7BF8A890, //  Licensed Japan\忍者龍剣伝 II꞉ 暗黒の邪神剣.nes 
    0x516B2412, //  Licensed Japan\恐竜戦隊 Juranger.nes 
    0xCC7A4DCA, //  Licensed Japan\暴れん坊天狗.nes 
    0x88062D9A, //  Licensed Japan\熱血! Street Basket - がんばれ Dunk Heroes.nes 
    0x37E24797, //  Licensed Japan\熱血格闘伝説.nes 
    0x64FD3BA6, //  Licensed Japan\熱血高校 Dodgeball 部 Soccer 編.nes 
    0xDCD8D6F4, //  Licensed Japan\爆笑! 愛の劇場.nes 
    0xB70129F4, //  Licensed Japan\特救指令 Solbrain.nes 
    0x6772CA86, //  Licensed Japan\獣王記.nes 
    0x091ED5A9, //  Licensed Japan\究極 Tiger.nes 
    0x93A7D26C, //  Licensed Japan\競馬 Simulation꞉ 本命.nes 
    0xF56135C0, //  Licensed Japan\西村京太郎 Mystery꞉ Super Express 殺人事件.nes 
    0x85E0090B, //  Licensed Japan\超人狼戦記 Warwolf.nes 
    0x98977591, //  Licensed Japan\闇の仕事人 Kage.nes 
    0x8F628D51, //  Licensed Japan\飛龍の拳 III꞉ 五人の龍戦士.nes 
    0x701B1ADF, //  Licensed Japan\高橋名人の冒険島 II.nes 
    0x626ABD49, //  Licensed Japan\高橋名人の冒険島 III.nes 
    0xF2594374, //  Licensed Japan\魔天童子.nes 
    0x326AB3B6, //  Licensed North America\8 Eyes.nes 
    0x054CB4EB, //  Licensed North America\Astyanax.nes 
    0x161D717B, //  Licensed North America\Bad Dudes.nes 
    0x13C6617E, //  Licensed North America\Batman꞉ The Video Game.nes 
    0xE095C3F2, //  Licensed North America\Bubble Bobble Part 2.nes 
    0xE19EE99C, //  Licensed North America\Bucky O'Hare.nes 
    0x58C7DDAF, //  Licensed North America\Captain America and The Avengers.nes 
    0x0B404915, //  Licensed North America\Captain Planet and The Planeteers.nes 
    0x57C2AE4E, //  Licensed North America\Cliffhanger.nes 
    0xE2313813, //  Licensed North America\Codename꞉ Viper.nes 
    0x20A5219B, //  Licensed North America\Conquest of the Crystal Palace.nes 
    0xA94591B0, //  Licensed North America\Contra Force.nes 
    0xC7F0C457, //  Licensed North America\Crash 'n the Boys - Street Challenge.nes 
    0x88338ED5, //  Licensed North America\Cyberball.nes 
    0xA725B2D3, //  Licensed North America\Defenders of Dynatron City.nes 
    0x50FD0CC6, //  Licensed North America\Double Dragon III꞉ The Sacred Stones.nes 
    0x13C774DD, //  Licensed North America\Double Dragon II꞉ The Revenge (rev0).nes 
    0x8A640AEF, //  Licensed North America\Double Dragon II꞉ The Revenge (rev1).nes 
    0xD7E29C03, //  Licensed North America\Dragon Spirit꞉ The New Legend.nes 
    0xD534C98E, //  Licensed North America\Fire 'n Ice.nes 
    0x1D20A5C6, //  Licensed North America\Galaxy 5000꞉ Racing in the 51st Century.nes 
    0xF0E9971B, //  Licensed North America\Gargoyle's Quest II꞉ The Demon Darkness.nes 
    0x8A043CD6, //  Licensed North America\Golgo 13꞉ The Mafat Conspiracy.nes 
    0xD19DCB2B, //  Licensed North America\Gun-Nac.nes 
    0x34EAB034, //  Licensed North America\Heavy Barrel.nes 
    0x2E0741B6, //  Licensed North America\Home Alone 2꞉ Lost in New York.nes 
    0x771C8855, //  Licensed North America\Hudson's Adventure Island II.nes 
    0xBFBFD25D, //  Licensed North America\Hudson's Adventure Island III.nes 
    0xDF64963B, //  Licensed North America\Infiltrator.nes 
    0x6944A01A, //  Licensed North America\Isolated Warrior.nes 
    0x45A41784, //  Licensed North America\Jackie Chan's Action Kung Fu.nes 
    0xF6898A59, //  Licensed North America\James Bond Jr..nes 
    0x26D3082C, //  Licensed North America\Joe &amp; Mac꞉ Caveman Ninja.nes 
    0x7474AC92, //  Licensed North America\Kabuki Quantum Fighter.nes 
    0x5104833E, //  Licensed North America\KickMaster.nes 
    0xCD10DCE2, //  Licensed North America\Kickle Cubicle.nes 
    0x563C2CC0, //  Licensed North America\Kiwi Kraze꞉ A Bird-Brained Adventure!.nes 
    0xE7DA8A04, //  Licensed North America\Last Action Hero.nes 
    0x5B4B6056, //  Licensed North America\Little Nemo꞉ The Dream Master.nes 
    0xBC7FEDB9, //  Licensed North America\Little Ninja Brothers.nes 
    0x93991433, //  Licensed North America\Low G Man꞉ The Low Gravity Man.nes 
    0x026E41C5, //  Licensed North America\Mad Max.nes 
    0x2055971A, //  Licensed North America\Mario is Missing!.nes 
    0x55DB7E2A, //  Licensed North America\Mario's Time Machine.nes 
    0x12078AFD, //  Licensed North America\Mendel Palace.nes 
    0x942B1210, //  Licensed North America\Michael Andretti's World Grand Prix.nes 
    0x6FB349E2, //  Licensed North America\Mickey's Adventures in Numberland.nes 
    0x3F78037C, //  Licensed North America\Mighty Final Fight.nes 
    0xE3C5BB3D, //  Licensed North America\Mission꞉ Impossible.nes 
    0xE542E3CF, //  Licensed North America\Monster in My Pocket.nes 
    0x902E3168, //  Licensed North America\Ninja Gaiden III꞉ The Ancient Ship of Doom.nes 
    0xB780521C, //  Licensed North America\Ninja Gaiden II꞉ The Dark Sword of Chaos.nes 
    0xA22657FA, //  Licensed North America\Nintendo World Cup (rev0).nes 
    0xFF8203D5, //  Licensed North America\Nintendo World Cup (rev1).nes 
    0x435AEEC6, //  Licensed North America\Panic Restaurant.nes 
    0xD273B409, //  Licensed North America\Power Blade 2.nes 
    0x5CF536F4, //  Licensed North America\Power Blade.nes 
    0x26796758, //  Licensed North America\Rampart.nes 
    0xE9C387EC, //  Licensed North America\River City Ransom.nes 
    0x192D546F, //  Licensed North America\RoboCop.nes 
    0x8927FD4C, //  Licensed North America\Rockin' Kats.nes 
    0xAA4997C1, //  Licensed North America\Rollergames.nes 
    0x03FB57B6, //  Licensed North America\Samurai Zombie Nation.nes 
    0xDDD90C39, //  Licensed North America\Shadow of the Ninja.nes 
    0xAA20F73D, //  Licensed North America\Shatterhand.nes 
    0x6EE94D32, //  Licensed North America\Smash T.V..nes 
    0x62E2E7FC, //  Licensed North America\Stanley꞉ The Search for Dr. Livingston...nes 
    0x8DA651D4, //  Licensed North America\Street Fighter 2010.nes 
    0x305B4E62, //  Licensed North America\Super C.nes 
    0xE840FD21, //  Licensed North America\Super Spike V'Ball.nes 
    0xAB41445E, //  Licensed North America\Super Spy Hunter.nes 
    0x465E5483, //  Licensed North America\Sword Master.nes 
    0x86964EDD, //  Licensed North America\Teenage Mutant Ninja Turtles꞉ Tournament Fighters.nes 
    0x227CF577, //  Licensed North America\The Adventures of Rocky and Bullwinkle and Friends.nes 
    0xF37BEFD5, //  Licensed North America\The Blue Marlin.nes 
    0x03F899CD, //  Licensed North America\The Hunt for Red October (rev0).nes 
    0x4E77733A, //  Licensed North America\The Hunt for Red October (rev1).nes 
    0x61179BFA, //  Licensed North America\The Jungle Book.nes 
    0x03272E9B, //  Licensed North America\The Krion Conquest.nes 
    0xE353969F, //  Licensed North America\The Last Ninja.nes 
    0x27F8D0D2, //  Licensed North America\The Punisher.nes 
    0xE98AB943, //  Licensed North America\The Ren &amp; Stimpy Show꞉ Buckaroo$!.nes 
    0x35C6F574, //  Licensed North America\The Young Indiana Jones Chronicles.nes 
    0x81A5EB65, //  Licensed North America\Tiny Toon Adventures 2꞉ Trouble in Wackyland.nes 
    0x99DDDB04, //  Licensed North America\Tiny Toon Adventures.nes 
    0xD63B30F5, //  Licensed North America\Tom &amp; Jerry (&amp; Tuffy).nes 
    0x6F8AF3E8, //  Licensed North America\Top Gun꞉ The Second Mission.nes 
    0xB629D555, //  Licensed North America\Totally Rad.nes 
    0xF009DDD2, //  Licensed North America\Toxic Crusaders.nes 
    0x0EF730E7, //  Licensed North America\Twin Cobra.nes 
    0x9F6CE171, //  Licensed North America\Ultimate Basketball.nes 
    0x753768A6, //  Licensed North America\Vice꞉ Project Doom.nes 
    0x5EA7D410, //  Licensed North America\WCW World Championship Wrestling.nes 
    0xEB803610, //  Licensed North America\WURM꞉ Journey to the Center of the Earth.nes 
    0x401521F7, //  Licensed North America\Wacky Races.nes 
    0xB0CD000F, //  Licensed North America\Wayne's World.nes 
    0x333C48A0, //  Licensed North America\Werewolf꞉ The Last Warrior.nes 
    0x6FD5A271, //  Licensed North America\Whomp'em.nes 
    0xE7C981A2, //  Licensed North America\Widget.nes 
    0x35476E87, //  Licensed North America\Wolverine.nes 
    0x8593E5AD, //  Licensed North America\World Champ.nes 
    0xD8578BFD, //  Licensed North America\Zen꞉ Intergalactic Ninja.nes 
    0xA55F66D7, //  Modern\Capcom Classics꞉ Mini Mix\Mighty Final Fight.nes 
    0x36E71F0E, //  Modern\Hudson Best Collection\高橋名人の冒険島 II.nes 
    0x4B1F07F2, //  Modern\Hudson Best Collection\高橋名人の冒険島 III.nes 
    0xA21BA648, //  Modern\Namco Museum Archives\Dragon Spirit꞉ The New Legend.nes 
    0xE87092B9, //  Modern\Namco Museum Archives\Mendel Palace.nes 
    0x6CA0B45C, //  Modern\Super C (Contra Anniversary Collection).nes 
    0x69603DB7, //  Modern\Super C (Konami Collector's Series).nes 
    0x39A18397, //  Modern\Virtual Console\Downtown 熱血物語.nes 
    0xD1E6EAB9, //  Modern\Virtual Console\Ninja Gaiden III꞉ The Ancient Ship of Doom (rev0).nes 
    0xBEFB4FA8, //  Modern\Virtual Console\Ninja Gaiden III꞉ The Ancient Ship of Doom (rev1).nes 
    0x75CE6EE2, //  Modern\Virtual Console\Ninja Gaiden II꞉ The Dark Sword of Chaos.nes 
    0x29A12BEA, //  Modern\Virtual Console\Shadow of the Ninja.nes 
    0xC7F56088, //  Modern\Virtual Console\Super Chinese 2꞉ Dragon Kid.nes 
    0xC6B1A4C4, //  Modern\Virtual Console\熱血高校 Dodgeball 部 Soccer 編.nes 
    0x801BFFAE, //  Modern\くにおくん熱血 Collection\Downtown 熱血行進曲 ~それゆけ大運動会~.nes 
    0x82B54765, //  Modern\くにおくん熱血 Collection\いけいけ熱血 Hockey部꞉ すべってころんで大乱闘.nes 
    0x2B2ED4EC, //  Modern\くにおくん熱血 Collection\熱血! Street Basket ~がんばれ Dunk Heroes~.nes 
    0x1006EBC7, //  Modern\くにおくん熱血 Collection\熱血高校 Dodgeball 部 Soccer 編.nes 
    0xA42B731C, //  Multicarts\extracts\Chip &amp; Dale 3 (multicart extract).nes 
    0xF7E9D60D, //  Multicarts\extracts\Cross Pacific (3810).nes 
    0x1F09FC2B, //  Multicarts\extracts\Magic Imp (3810).nes 
    0x52ADD886, //  Multicarts\extracts\Martian (3810).nes 
    0xE93B0284, //  Multicarts\extracts\Massacre (3810).nes 
    0x1DAB6162, //  Multicarts\extracts\Monster Inc..nes 
    0xC8B5D39D, //  Multicarts\extracts\怪鴨 II - Darkwing Duck 2.nes 
    0x737CAD2B, //  Multicarts\extracts\超強魂斗羅.nes 
    0x5C9D57DA, //  Playchoice\Ninja Gaiden II - The Dark Sword of Chaos.nes 
    0xC9589199, //  Playchoice\Ninja Gaiden III - The Ancient Ship of Doom.nes 
    0x5EA8DEF3, //  Playchoice\Nintendo World Cup.nes 
    0x2AC4817F, //  Playchoice\Power Blade.nes 
    0xA1B9158D, //  Playchoice\Rockin' Kats.nes 
    0xCD47382E, //  Playchoice\Super C.nes 
    0x33FB8A12, //  Playchoice\Tiny Toon Adventures.nes 
    0x3C201DCC, //  Plug-and-Play\extracts\Difference.nes 
    0x9BB8F450, //  Plug-and-Play\extracts\Diveman.nes 
    0xF527CD98, //  Plug-and-Play\extracts\Dragon Running.nes 
    0x7981C762, //  Plug-and-Play\extracts\Metro Mania.nes 
    0x15F9138F, //  Plug-and-Play\extracts\Underground Mission.nes 
    0x94CDBC32, //  Plug-and-Play\extracts\War of Strike Mouse.nes 
    0x49FD2867, //  Plug-and-Play\extracts\Wonder Rabbit.nes 
    0x8BD7FDF3, //  Plug-and-Play\extracts\三國春秋꞉ 四川省 (Power Joy Navigator 50-in-1).nes 
    0xE56AA5E8, //  Samples\Double Dragon II꞉ The Revenge.nes 
    0x7D5F149B, //  Samples\Ninja Gaiden II꞉ The Dark Sword of Chaos.nes 
    0xD7077D96, //  Samples\US Championship V'Ball.nes 
    0x56E0E8E6, //  Unlicensed Elsewhere\Harry's Legend.nes 
    0xB77E7B27, //  Unlicensed Taiwan&amp;Hong Kong\Harry's Legend - 哈利傳奇.nes 
    0xA39311CA, //  Unlicensed Taiwan&amp;Hong Kong\Mortal Kombat 4.nes 
    0x831E6E0E, //  Unlicensed Taiwan&amp;Hong Kong\Super Donkey Kong 2.nes 
    0xA46D7F02, //  Unreleased\Astyanax.nes 
    0x816AD178, //  Unreleased\Batman꞉ The Video Game (Earlier).nes 
    0xCB8F9AB7, //  Unreleased\Batman꞉ The Video Game (Later).nes 
    0x190E52FF, //  Unreleased\Bio Force Ape.nes 
    0xBEE30C5F, //  Unreleased\Crossfire.nes 
    0xEAB002AE, //  Unreleased\Dino Hockey (1990-00-00).nes 
    0x78C8E949, //  Unreleased\Dino Hockey (1990-12-18).nes 
    0xDB0F299A, //  Unreleased\Hit the Ice (finished).nes 
    0x1CB9A019, //  Unreleased\Hit the Ice (unfinished).nes 
    0x8CACCA85, //  Unreleased\Hudson's Adventure Island II.nes 
    0xFC507AF1, //  Unreleased\Joe &amp; Mac꞉ Caveman Ninja.nes 
    0x250DC104, //  Unreleased\John Smith꞉ Special Agent.nes 
    0x592854A1, //  Unreleased\Magical Doropie.nes 
    0x7E146C3E, //  Unreleased\Magician (1990-03-02).nes 
    0x39D74458, //  Unreleased\Mick &amp; Mack as the Global Gladiators (1993-06-30).nes 
    0xA16128B8, //  Unreleased\Mick &amp; Mack as the Global Gladiators (1993-07-09).nes 
    0xDEDFB3DF, //  Unreleased\Mickey's Adventures in Numberland.nes 
    0xFE84FCAC, //  Unreleased\Monster in My Pocket.nes 
    0x7AC3E8A1, //  Unreleased\RoboCop.nes 
    0x63D3AFF4, //  Unreleased\Rockin' Kats.nes 
    0x4E36538F, //  Unreleased\Samurai Zombie Nation.nes 
    0xB4801882, //  Unreleased\Secret Ties.nes 
    0x15A1CBB0, //  Unreleased\Shatterhand.nes 
    0xD6EFAB8D, //  Unreleased\Solomon's Key 2.nes 
    0xF03E6D72, //  Unreleased\Squashed.nes 
    0x9C4C2B9C, //  Unreleased\Street Fighter 2010.nes 
    0x47F7F860, //  Unreleased\Superman (Sunsoft).nes 
    0x01934171, //  Unreleased\Terminator 2꞉ Judgment Day.nes 
    0xE46AEE21, //  Unreleased\Thomas the Tank Engine and Friends.nes 
    0xBFF1D847, //  Unreleased\Time Diver Eon Man (Japan).nes 
    0xE4E7C62D, //  Unreleased\Time Diver Eon Man (North America).nes 
/**** 244 mapper=4 prgrom=128K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x3CEEFAD6, //  Bad Dumps\Street Gangs [bad CHR].nes 
    0xF2A81699, //  Bootleg Singles\TouchGamePlayer\Blue Shadow.nes 
    0x5342C6F7, //  Bootleg Singles\TouchGamePlayer\Bucky O'Hare.nes 
    0xEFB1F076, //  Bootleg Singles\TouchGamePlayer\RoboCop.nes 
    0x1CED49AC, //  Bootleg Singles\TouchGamePlayer\Street Gangs.nes 
    0x7EFBC241, //  Homebrew\Hacks\Banana Prince 2꞉ Adventure of Banana Goat (v3)[Danger X].nes 
    0xE7EB2689, //  Homebrew\Translations\English\Banana Prince [KingMike, elbobelo].nes 
    0x10BD7F35, //  Homebrew\Translations\English\Banana Prince [KingMike].nes 
    0xDDC6D9C9, //  Licensed PAL\Alien 3.nes 
    0xB68F9814, //  Licensed PAL\Astyanax.nes 
    0x8C252AC4, //  Licensed PAL\Bad Dudes vs. Dragon Ninja.nes 
    0x8A65E57C, //  Licensed PAL\Banana Prince.nes 
    0xD0F70E36, //  Licensed PAL\Batman꞉ The Video Game.nes 
    0xC92B814B, //  Licensed PAL\Blue Shadow.nes 
    0x6720ABAC, //  Licensed PAL\Bucky O'Hare.nes 
    0x5A62F17F, //  Licensed PAL\Captain America and The Avengers.nes 
    0x8D901FAD, //  Licensed PAL\Captain Planet and The Planeteers.nes 
    0xE9D352EB, //  Licensed PAL\Double Dragon III꞉ The Sacred Stones.nes 
    0xAEB2D754, //  Licensed PAL\Double Dragon II꞉ The Revenge.nes 
    0xF919795D, //  Licensed PAL\Dragon's Lair.nes 
    0x4F089E8A, //  Licensed PAL\Galaxy 5000꞉ Racing in the 51st Century.nes 
    0x86C495C6, //  Licensed PAL\Gargoyle's Quest II꞉ The Demon Darkness.nes 
    0x67CBC0A0, //  Licensed PAL\Hammerin' Harry.nes 
    0x68C62E50, //  Licensed PAL\Home Alone 2꞉ Lost in New York.nes 
    0x7E4BA78F, //  Licensed PAL\Hudson's Adventure Island II.nes 
    0x4BB9B840, //  Licensed PAL\Isolated Warrior.nes 
    0x7AE5C002, //  Licensed PAL\Jackie Chan's Action Kung Fu.nes 
    0xA0A5A0B9, //  Licensed PAL\James Bond Jr..nes 
    0x04142764, //  Licensed PAL\Joe &amp; Mac꞉ Caveman Ninja.nes 
    0xBCCFEF1C, //  Licensed PAL\Kabuki Quantum Fighter.nes 
    0xE5901A99, //  Licensed PAL\Kickle Cubicle.nes 
    0xE0FFFBD2, //  Licensed PAL\Little Nemo꞉ The Dream Master.nes 
    0x3BB31E38, //  Licensed PAL\Little Ninja Brothers.nes 
    0xAF65AA84, //  Licensed PAL\Low G Man꞉ The Low Gravity Man.nes 
    0x967011AD, //  Licensed PAL\Mario is Missing!.nes 
    0x7653103A, //  Licensed PAL\Mighty Final Fight.nes 
    0x1353A134, //  Licensed PAL\Mission꞉ Impossible (English).nes 
    0x4ECD4624, //  Licensed PAL\Mission꞉ Impossible (French).nes 
    0x80250D64, //  Licensed PAL\Monster in My Pocket.nes 
    0x8DA6667D, //  Licensed PAL\Nintendo World Cup (rev0).nes 
    0x7C16F819, //  Licensed PAL\Nintendo World Cup (rev1).nes 
    0x7F08D0D9, //  Licensed PAL\Nintendo World Cup (rev2).nes 
    0xBBFE23F4, //  Licensed PAL\Panic Restaurant.nes 
    0xD0DF525E, //  Licensed PAL\Power Blade.nes 
    0x37A5EB52, //  Licensed PAL\Probotector II꞉ Return of the Evil Forces.nes 
    0x8FA6E92C, //  Licensed PAL\Rackets &amp; Rivals.nes 
    0xC115A022, //  Licensed PAL\Rampart.nes 
    0x6DCBAAFD, //  Licensed PAL\RoboCop.nes 
    0x9C924719, //  Licensed PAL\Rockin' Kats.nes 
    0xBED47813, //  Licensed PAL\Rollergames.nes 
    0xBC25A18B, //  Licensed PAL\Shadow Warriors Episode II꞉ The Dark Sword of Chaos.nes 
    0x348D3FF1, //  Licensed PAL\Shatterhand.nes 
    0x0B8F8128, //  Licensed PAL\Smash T.V..nes 
    0xF184EB2D, //  Licensed PAL\Solomon's Key 2.nes 
    0xF5A1B8FB, //  Licensed PAL\Street Gangs.nes 
    0xC05A63B2, //  Licensed PAL\Super Spike V'Ball.nes 
    0xC528ED56, //  Licensed PAL\Super Spy Hunter.nes 
    0x66066326, //  Licensed PAL\Sword Master.nes 
    0x34629104, //  Licensed PAL\Teenage Mutant Hero Turtles꞉ Tournament Fighters.nes 
    0x18A04825, //  Licensed PAL\Terminator 2꞉ Judgment Day.nes 
    0x9C304DEC, //  Licensed PAL\The Hunt for Red October.nes 
    0x7DA77F11, //  Licensed PAL\The Jungle Book.nes 
    0x666BE5EC, //  Licensed PAL\The New Zealand Story.nes 
    0xE681B300, //  Licensed PAL\The Trolls in Crazyland.nes 
    0xC32E9672, //  Licensed PAL\Tiny Toon Adventures 2꞉ Trouble in Wackyland.nes 
    0xA038AFF2, //  Licensed PAL\Tiny Toon Adventures.nes 
    0xB2781C19, //  Licensed PAL\Tom &amp; Jerry (&amp; Tuffy).nes 
    0xE1C59D94, //  Licensed PAL\Top Gun꞉ The Second Mission.nes 
    0x02E0ADA4, //  Licensed PAL\Totally Rad.nes 
    0x4EC0FECC, //  Licensed PAL\U-four-ia꞉ The Saga.nes 
    0x5D0D3047, //  Licensed PAL\Werewolf꞉ The Last Warrior.nes 
    0x9B05B278, //  Licensed PAL\World Champ.nes 
    0x0430DB08, //  Licensed PAL\Zen꞉ Intergalactic Ninja.nes 
    0x54BCF00C, //  Modern\Probotector II꞉ Return of the Evil Forces (Contra Anniversary Collection).nes 
    0x1EEEB556, //  Modern\Virtual Console\U-four-ia꞉ The Saga (rev0).nes 
    0x0BF2B8B1, //  Modern\Virtual Console\U-four-ia꞉ The Saga (rev1).nes 
    0xCAA76927, //  Modern\Virtual Console\Yoshi's Cookie (PAL).nes 
    0x0E945FA1, //  Unreleased\Hudson's Adventure Island III.nes 
    0x015D4555, //  Unreleased\Sunman.nes 
    0x56F05853, //  Unreleased\U-four-ia꞉ The Saga (PAL).nes 
/**** 245 mapper=4 prgrom=128K chrrom=128K mirror=H system=SYSTEM_NTSC */
    0xF0A65D85, //  Compatibility Hacks\Toy Story [m004][NewRisingSun].nes 
    0xB5A3461A, //  Homebrew\Indivisible.nes 
    0x6272C549, //  Licensed North America\The Terminator.nes 
/**** 246 mapper=4 prgrom=128K chrrom=128K mirror=H system=SYSTEM_NTSC */
    0x92D9D268, //  Bad Dumps\Music Box [CHR overdump].nes 
    0xEE5618C0, //  Bootleg Hacks\Super Contra 8.nes 
    0x37B6D70D, //  Bootleg Singles\Pokémon 4-in-1.nes 
    0x798A2012, //  Compatibility Hacks\TV System Conversions\Probotector II꞉ Return of the Evil Forces (Dendy)[NewRisingSun].nes 
    0xB3258212, //  Homebrew\Translations\English\Super C 7 [Pacnsacdave].nes 
    0x35C596B6, //  Plug-and-Play\extracts\Navigator.nes 
    0x140C6A3D, //  Unlicensed China\外星\超級戰魂 - Super Contra 7 (960418).nes 
    0x43DB0D06, //  Unlicensed China\外星\超級戰魂 - Super Contra 7.nes 
    0x4E4EA7C8, //  Unlicensed China\外星\鏡花緣.nes 
    0x0D9BF742, //  Unlicensed China\燕城\Pokémon 4-in-1.nes 
/**** 247 mapper=4 prgrom=128K prgram=8K chrrom=128K mirror=H */
    0x40B8EE9A, //  Bad Dumps\F-1 Sensation [bad CHR].nes 
    0xC3D4178F, //  Bootleg Hacks\Pokémon 2000 (Felix the Cat, Charmander).nes 
    0x00BECBFE, //  Bootleg Hacks\Pokémon 2000 (Felix the Cat, Pikachu).nes 
    0xE106FE8C, //  Bootleg Hacks\Teletubbies 2000.nes 
    0xBBA545BB, //  Bootleg Hacks\Tiny Toon 4.nes 
    0x42728718, //  Bootleg Singles\Over Horizon - 飛離航道.nes 
    0xFF2AAE8A, //  Compatibility Hacks\Pikachu Y2K [m004].nes 
    0xE46DB38C, //  Homebrew\Bugfixes, Improvements\Super Mario Bros. 2 (NTSC rev1) [animation].nes 
    0xAECF8729, //  Homebrew\Bugfixes, Improvements\Super Mario USA [animation].nes 
    0x3364F00A, //  Homebrew\Hacks\Super Mario Bros. series\Cemetery Bros 2 [Megafield64].nes 
    0xC1148548, //  Homebrew\Hacks\Super Mario Bros. series\Doki Doki Panik! [MB Hacks].nes 
    0x29E71F58, //  Homebrew\Hacks\Super Mario Bros. series\Legend of the Blob Bros. 2 (v2.0)[BMF54123].nes 
    0x41E52222, //  Homebrew\Hacks\Super Mario Bros. series\Mario Disco Bros. 2.nes 
    0x35362B91, //  Homebrew\Hacks\Super Mario Bros. series\Pony Poki Panic (v1.1)[Herooftime1000].nes 
    0x9DF8E4EF, //  Homebrew\Hacks\Super Mario Bros. series\Super Doki Bros. [togemet2].nes 
    0xE7D4AF5D, //  Homebrew\Hacks\Super Mario Bros. series\Super Luigi Bros. 2 [szemigi].nes 
    0x77CC1EAE, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - 2nd Run [Recovery1].nes 
    0xECC91535, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - Bowser Edition [pacnsacdave].nes 
    0x3D8DE75F, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - Carnage 2 [Kasady].nes 
    0xAAECF4F4, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - Graphic Overhaul [MaxT].nes 
    0x5D94A2EF, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - Improvement [SpiderDave].nes 
    0xE390375A, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - Invincible.nes 
    0xC9384965, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - Master Quest.nes 
    0xC7A8154A, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - POW and Eye Hack Improvement [Asaki, SpiderDave].nes 
    0x5065FD6B, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - POW and Eye Hack [Asaki].nes 
    0xC0EF49AC, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 - TIE Fighter 2.nes 
    0xC87186C6, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 Christmas Edition [BMF54123].nes 
    0xA5889082, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 New.nes 
    0x3E6F0173, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ Forgotten Dream [Hias].nes 
    0x62B9F18C, //  Homebrew\Hacks\Super Mario Bros. series\Super Pokemon Bros. 2 [pacnsacdave].nes 
    0xC4B008B9, //  Homebrew\Hacks\Super Mario Bros. series\Super Smashing Pumpkins Bros. [Macbee 2000].nes 
    0xC62D9630, //  Homebrew\Hacks\Super Mario Bros. series\Transformers Episode 2꞉ Return of Unicron [Grimlick](alt).nes 
    0x5B1B4B2A, //  Homebrew\Hacks\Super Mario Bros. series\Transformers Episode 2꞉ Return of Unicron [Grimlick].nes 
    0x81241DEC, //  Licensed Japan\F-1 Sensation.nes 
    0xB47569E2, //  Licensed Japan\Happy Birthday, Bugs!.nes 
    0x57D162F1, //  Licensed Japan\Mickey Mouse III꞉ 夢ふうせん.nes 
    0x0FC8E9B7, //  Licensed Japan\North &amp; South.nes 
    0xCE06F2D4, //  Licensed Japan\Over Horizon.nes 
    0xBAD36C17, //  Licensed Japan\Star Wars꞉ A New Hope (Victor).nes 
    0x80FB7E6B, //  Licensed Japan\Super Mario USA.nes 
    0x2746B39E, //  Licensed Japan\Tetris Flash.nes 
    0x9E36080E, //  Licensed Japan\名門! 多古西応援団꞉ 硬派六人衆.nes 
    0xDFA111F1, //  Licensed North America\Bram Stoker's Dracula.nes 
    0x2CAAE01C, //  Licensed North America\Felix the Cat.nes 
    0x1B71CCDB, //  Licensed North America\Gauntlet II.nes 
    0xCA5EDBFC, //  Licensed North America\Home Alone (rev0).nes 
    0xF31D36A3, //  Licensed North America\Home Alone (rev1).nes 
    0x058F23A2, //  Licensed North America\Image Fight.nes 
    0x3B7F5B3B, //  Licensed North America\Jurassic Park.nes 
    0x8EE7C43E, //  Licensed North America\Kid Klown in Night Mayor World.nes 
    0xB0EBF3DB, //  Licensed North America\M.C. Kids.nes 
    0xAE9F33D0, //  Licensed North America\North &amp; South.nes 
    0x6E4DCFD2, //  Licensed North America\Roundball 2-on-2 Challenge.nes 
    0xC1C3636B, //  Licensed North America\Star Wars꞉ A New Hope.nes 
    0x57AC67AF, //  Licensed North America\Super Mario Bros. 2 (rev0).nes 
    0xCA594ACE, //  Licensed North America\Super Mario Bros. 2 (rev1).nes 
    0x9C537919, //  Licensed North America\Tetris 2.nes 
    0x126EBF66, //  Licensed North America\The Bugs Bunny Birthday Blowout.nes 
    0x9E6092A4, //  Licensed North America\Tiny Toon Adventures꞉ Cartoon Workshop.nes 
    0xC3463A3D, //  Licensed North America\Where's Waldo？.nes 
    0x056F3063, //  Modern\Virtual Console\Super Mario Bros. 2 (NTSC).nes 
    0x4FCD04C6, //  Modern\Virtual Console\Super Mario USA.nes 
    0x8BC588CB, //  Playchoice\Super Mario Bros. 2.nes 
    0xAE7DF77F, //  Unreleased\M.C. Kids.nes 
    0x899213DC, //  Unreleased\Mickey Mouse꞉ Dream Balloon.nes 
    0xDF43E073, //  Unreleased\RoboCop vs. The Terminator.nes 
    0xB30599A1, //  Unreleased\Star Wars꞉ A New Hope.nes 
    0x85D02CD4, //  Unreleased\The Bugs Bunny Birthday Bash [broken].nes 
/**** 248 mapper=4 prgrom=128K prgram=8K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x635A7C12, //  Bootleg Hacks\Super Bros. 2.nes 
    0xE9E1C6CC, //  Bootleg Hacks\Super Bros. 5.nes 
    0xC77A717F, //  Homebrew\Bugfixes, Improvements\Super Mario Bros. 2 (PAL) [animation].nes 
    0x89A45446, //  Licensed PAL\Bram Stoker's Dracula.nes 
    0x256392F1, //  Licensed PAL\F-1 Sensation.nes 
    0xFBD48274, //  Licensed PAL\Felix the Cat.nes 
    0x79F688BC, //  Licensed PAL\Gauntlet II.nes 
    0x6C1AB645, //  Licensed PAL\Jurassic Park.nes 
    0xFC2F9B2D, //  Licensed PAL\M.C. Kids (English).nes 
    0x8650BE49, //  Licensed PAL\M.C. Kids (French).nes 
    0x81B2A3CD, //  Licensed PAL\Noah's Ark.nes 
    0x7BA3F8AE, //  Licensed PAL\North &amp; South.nes 
    0x9237B447, //  Licensed PAL\Over Horizon.nes 
    0x083E4FC1, //  Licensed PAL\Parodius.nes 
    0xAD0394F0, //  Licensed PAL\Roundball 2-on-2 Challenge.nes 
    0xFCD772EB, //  Licensed PAL\Star Wars꞉ A New Hope.nes 
    0xE94E883D, //  Licensed PAL\Super Mario Bros. 2.nes 
    0x5D2B1962, //  Licensed PAL\Tetris 2.nes 
    0x9BD3F3C2, //  Licensed PAL\The Bugs Bunny Blowout.nes 
    0xC8EBD977, //  Licensed PAL\Tiny Toon Adventures꞉ Cartoon Workshop.nes 
    0x037CB1B4, //  Modern\Virtual Console\Super Mario Bros. 2 (PAL).nes 
    0x6328B44D, //  Unreleased\Parodius.nes 
/**** 249 mapper=4 prgrom=128K prgram=8K chrrom=128K mirror=H system=SYSTEM_NTSC */
    0x88C30FDA, //  Licensed PAL\Super Turrican.nes 
/**** 250 mapper=4 prgrom=128K chrrom=232K mirror=H */
    0x73663E35, //  Multicarts\extracts\Titenic [超强年度新卡 15-in-1].nes 
/**** 251 mapper=4 prgrom=128K chrrom=256K mirror=H */
    0x62377191, //  Bad Dumps\3-in-1 Street Blaster II Pro [incomplete].nes 
    0x229EEBEB, //  Bad Dumps\Street Fighter V Turbo 60 [bad CHR].nes 
    0x6A3E9E43, //  Bad Dumps\武士魂 (4 characters)[bad CHR].nes 
    0x3F20B2B5, //  Bootleg Hacks\7 Grand Dad.nes 
    0x163A5EDA, //  Bootleg Hacks\Batman III.nes 
    0xA5BDF9E7, //  Bootleg Hacks\Bollywood 2007.nes 
    0xA438C05B, //  Bootleg Hacks\Fighter V (Street Fighter II Pro).nes 
    0xE52225C8, //  Bootleg Hacks\Street Fighter VI (16 People)(YY-01).nes 
    0xCB662466, //  Bootleg Hacks\Street Fighter VI (16 People).nes 
    0xEDE708B9, //  Bootleg Hacks\Super Contra 5.nes 
    0x08F3544B, //  Bootleg Hacks\The King of Fighters '99.nes 
    0xE271190E, //  Bootleg Hacks\鉄拳 - Tekken 3.nes 
    0x178A61DB, //  Bootleg Hacks\鉄拳 - Tekken 6.nes 
    0xEFB0D32F, //  Bootleg Hacks\鉄拳 - Tekken 8.nes 
    0x181D8CDF, //  Bootleg Hacks\鉄拳 - Tekken Tag Tournament.nes 
    0xFFB3E74A, //  Bootleg Singles\Colour 2001 Streetfighter II.nes 
    0x718B3BC6, //  Bootleg Singles\Kart Fighter (K-F1).nes 
    0xE6857563, //  Bootleg Singles\Mike Tyson's Intergalactic Power Punch.nes 
    0x0118D870, //  Bootleg Singles\Mortal Kombat II (晶太)(Blue).nes 
    0x7C459689, //  Bootleg Singles\Mortal Kombat II (晶太)(M-O9).nes 
    0xFF29E7EB, //  Bootleg Singles\Mortal Kombat III Turbo (JY-609).nes 
    0x06130A0E, //  Bootleg Singles\Mortal Kombat III Turbo (MD102).nes 
    0x3F0EB040, //  Bootleg Singles\Mortal Kombat III Turbo (NT-851).nes 
    0x27F3A202, //  Bootleg Singles\Mortal Kombat V Turbo 30 (V1995).nes 
    0x2D69B08A, //  Bootleg Singles\Mortal Kombat V Turbo 30 (VT-482).nes 
    0xBE25460F, //  Bootleg Singles\Mortal Kombat V Turbo 30 (ZR006).nes 
    0x1CD627F2, //  Bootleg Singles\Street Blaster V Turbo 20.nes 
    0x8CD7BB21, //  Bootleg Singles\Street Fighter V Turbo 60 (S-F6).nes 
    0xF0A68E94, //  Bootleg Singles\TouchGamePlayer\G.I. Joe꞉ A Real American Hero.nes 
    0x9BF4E80B, //  Compatibility Hacks\Dragon Ball Z꞉ 超武闘伝 2 [m004].nes 
    0xF7C5245D, //  Compatibility Hacks\Dragon Ball Z꞉ 超武闘伝 III [m004].nes 
    0xF3ED2AEE, //  Compatibility Hacks\Gravity Trooper Metal Storm (Collector's Edition) [m004 extract].nes 
    0x405E817C, //  Compatibility Hacks\The Incredible Crash Dummies [Nintendo MMC3].nes 
    0xD1597209, //  Compatibility Hacks\The King of Fighters '97 [m004][NewRisingSun].nes 
    0xC2FAA42C, //  Compatibility Hacks\Ultimate Mortal Kombat 3 [m004].nes 
    0x3C927D34, //  Homebrew\Bugfixes, Improvements\Titenic [CB-4xxx][memory initialization][NewRisingSun].nes 
    0xFDC50EB3, //  Homebrew\Bugfixes, Improvements\Titenic [超强年度新卡 15-in-1][Санчес restoration].nes 
    0xABFF6BD4, //  Homebrew\Bugfixes, Improvements\Titenic [超强年度新卡 15-in-1][Санчес restoration][Memory initialization].nes 
    0x501B41EB, //  Homebrew\Translations\English\Cosmic Epsilon [Stardust Crusaders].nes 
    0xF610D497, //  Homebrew\Translations\English\Gravity Armor Metal Storm [Sliver X v1.01].nes 
    0xF0EF949B, //  Homebrew\Translations\English\Gravity Armor Metal Storm [Sliver X v1.1](USA palette).nes 
    0x1C575064, //  Homebrew\Translations\English\Gravity Armor Metal Storm [Sliver X v1.1].nes 
    0x0D6F3712, //  Homebrew\Translations\English\Kart Fighter [Translations].nes 
    0xF738D6F9, //  Homebrew\Translations\English\Samurai Shodown III [Pacnsacdave].nes 
    0xDD1B5012, //  Homebrew\Translations\English\Ultraman Club꞉ Great Kaiju Battle!! [Ccmar &amp; Eric Engel].nes 
    0xDC75732F, //  Licensed Japan\Cosmic Epsilon.nes 
    0xCCAF543A, //  Licensed Japan\JuJu 伝説.nes 
    0x9ADFC8F0, //  Licensed Japan\Ultraman Club꞉ 怪獣大決戦!!.nes 
    0xFDB8AA9A, //  Licensed Japan\重力装甲 Metal Storm.nes 
    0xC247A23D, //  Licensed North America\Batman Returns.nes 
    0x12748678, //  Licensed North America\Days of Thunder.nes 
    0x1D2D93FF, //  Licensed North America\G.I. Joe꞉ A Real American Hero.nes 
    0x8C8DEDB6, //  Licensed North America\G.I. Joe꞉ The Atlantis Factor.nes 
    0x0ED96F42, //  Licensed North America\Gremlins 2꞉ The New Batch.nes 
    0x05CE560C, //  Licensed North America\Legends of the Diamond꞉ The Baseball Championship Game.nes 
    0xBCACBBF4, //  Licensed North America\Metal Storm.nes 
    0x90226E40, //  Licensed North America\Power Punch 2.nes 
    0x2FE20D79, //  Licensed North America\The Flintstones꞉ The Rescue of Dino &amp; Hoppy.nes 
    0xDAB84A9C, //  Licensed North America\The Flintstones꞉ The Surprise at Dinosaur Peak!.nes 
    0x8889C564, //  Licensed North America\The Immortal.nes 
    0x2BF61C53, //  Licensed North America\The Jetsons꞉ Cogswell's Caper!.nes 
    0x7FB74A43, //  Licensed North America\Toki.nes 
    0x911B67FF, //  Multicarts\extracts\Fighter V.nes 
    0x903AA8AA, //  Multicarts\extracts\Mortal Kombat 6 (NT-639).nes 
    0xA1E99DB4, //  Multicarts\extracts\Mortal Kombat 7 (NT-639).nes 
    0x681FFECC, //  Multicarts\extracts\Mortal Kombat III Turbo (KY-9006).nes 
    0xF9CAFA80, //  Multicarts\extracts\Mortal Kombat III Turbo (YH-2001).nes 
    0x94347727, //  Multicarts\extracts\Street Fighter VI (16 People).nes 
    0xF65D93A9, //  Multicarts\extracts\Titenic [CB-40xx].nes 
    0x6FB28AF7, //  Unlicensed Elsewhere\Super Donkey Kong 2.nes 
    0x51CEAC29, //  Unlicensed Taiwan&amp;Hong Kong\Mortal Kombat Trilogy MK5 (16 Fighters).nes 
    0x53FFA0D5, //  Unlicensed Taiwan&amp;Hong Kong\Mortal Kombat Trilogy MK5 (8 Fighters).nes 
    0x3704C1B9, //  Unlicensed Taiwan&amp;Hong Kong\Samurai Shodown III.nes 
    0x6C98E873, //  Unlicensed Taiwan&amp;Hong Kong\Street Fighter V Turbo 60.nes 
    0xEDB662C5, //  Unlicensed Taiwan&amp;Hong Kong\武士魂 (4 characters).nes 
    0x9DDF9017, //  Unreleased\G.I. Joe꞉ The Atlantis Factor.nes 
    0xDCEC4A59, //  Unreleased\Gremlins 2꞉ The New Batch (NTSC).nes 
    0xDB7F07BE, //  Unreleased\Toki.nes 
    0x1675A6C1, //  Unreleased\War on Wheels.nes 
/**** 252 mapper=4 prgrom=128K chrrom=256K mirror=H system=SYSTEM_PAL */
    0x79D48F34, //  Licensed PAL\Batman Returns.nes 
    0xC4E81924, //  Licensed PAL\Beauty and the Beast.nes 
    0xFA73D3A2, //  Licensed PAL\Days of Thunder.nes 
    0x8C88536F, //  Licensed PAL\George Foreman's KO Boxing.nes 
    0x2B20B022, //  Licensed PAL\Gremlins 2꞉ The New Batch.nes 
    0xED3FA60E, //  Licensed PAL\Spider-Man꞉ Return of the Sinister Six.nes 
    0xAC609320, //  Licensed PAL\The Flintstones꞉ The Rescue of Dino &amp; Hoppy.nes 
    0x4B750880, //  Licensed PAL\The Flintstones꞉ The Surprise at Dinosaur Peak!.nes 
    0xC8228B54, //  Licensed PAL\The Incredible Crash Dummies.nes 
    0x65D1AB64, //  Licensed PAL\The Jetsons꞉ Cogswell's Caper!.nes 
    0x071D4C2D, //  Licensed PAL\WWF King of the Ring.nes 
    0x21F2A1A6, //  Licensed PAL\WWF WrestleMania Steel Cage Challenge.nes 
    0x78CC796B, //  Unreleased\Batman Returns.nes 
    0x8BDD3D93, //  Unreleased\Gremlins 2꞉ The New Batch (PAL).nes 
/**** 253 mapper=4 prgrom=128K chrrom=256K mirror=H system=SYSTEM_NTSC */
    0xA7EF8F80, //  Unlicensed Taiwan&amp;Hong Kong\Gaiapolis.nes 
/**** 254 mapper=4 prgrom=128K chrrom=256K mirror=H system=SYSTEM_NTSC */
    0xB7116595, //  Unlicensed China\外星\Impartial Judge.nes 
    0x4B83D1D3, //  Unlicensed China\外星\夜明珠 - Pearl.nes 
/**** 255 mapper=4 prgrom=128K prgram=8K chrrom=256K mirror=H */
    0x1D89610E, //  Licensed Japan\Great Battle Cyber.nes 
    0x847D672D, //  Licensed North America\Bill Elliott's NASCAR Challenge.nes 
    0x5FD2AAB1, //  Licensed North America\Bo Jackson Baseball.nes 
    0xBEE1C0D9, //  Licensed North America\Silver Surfer.nes 
    0x917770D8, //  Licensed North America\Where in Time is Carmen Sandiego？.nes 
/**** 256 mapper=4 prgrom=256K chrram=8K mirror=H */
    0x6A562927, //  Homebrew\Translations\English\Wily &amp; Right's Rockboard꞉ That's Paradise!! [Interordi Software v1.1](Mega Man names).nes 
    0x234799E5, //  Homebrew\Translations\English\Wily &amp; Right's Rockboard꞉ That's Paradise!! [Interordi Software v1.1](Rockman names).nes 
    0x23F4B48F, //  Licensed Japan\Wily &amp; Light no Rockboard꞉ That's Paradise.nes 
/**** 257 mapper=4 prgrom=256K chrram=8K mirror=H system=SYSTEM_NTSC */
    0xC2BC279A, //  Compatibility Hacks\Dead Tomb꞉ A Temporal Adventure [m004 extract].nes 
/**** 258 mapper=4 prgrom=256K prgram=8K chrram=8K mirror=H */
    0x8D77E5E6, //  Licensed Japan\Business Wars.nes 
    0x5A6860F1, //  Licensed Japan\将棋名鑑 '92.nes 
    0xAE280E20, //  Licensed Japan\将棋名鑑 '93.nes 
/**** 259 mapper=4 prgrom=256K chrrom=128K mirror=H */
    0x06F70455, //  Bootleg Singles\Robocco Wars (K-W3).nes 
    0xAFCBEBAA, //  Homebrew\Hacks\Bases Loaded 3 2004 [GRG].nes 
    0x88C41E40, //  Homebrew\Hacks\Captain 翼 series\Angel Wings 2 (with Features Keeper) [Heroy].nes 
    0x5B171E5C, //  Homebrew\Hacks\Captain 翼 series\Angel Wings 2 [Heroy].nes 
    0x0A2BC394, //  Homebrew\Hacks\Captain 翼 series\Angel Wings 3 [Heroy].nes 
    0xB8B9EDC4, //  Homebrew\Hacks\Captain 翼 series\Angel Wings [Heroy].nes 
    0x95AA50B4, //  Homebrew\Hacks\Captain 翼 series\Captain Edwar Hector Efekli Edition [Heroy].nes 
    0x89B41BE5, //  Homebrew\Hacks\Captain 翼 series\Captain Edwar Hector Efeksiz Edition [Heroy].nes 
    0x5FB56CFA, //  Homebrew\Hacks\Captain 翼 series\Captain Hyuga 2꞉ Toho Soccer School - Gece Mac.nes 
    0x40E12F1F, //  Homebrew\Hacks\Captain 翼 series\Captain Hyuga 2꞉ Toho Soccer School - Gun Batimi.nes 
    0x26E7AEA9, //  Homebrew\Hacks\Captain 翼 series\Captain Hyuga 2꞉ Toho Soccer School.nes 
    0x7C20C3C6, //  Homebrew\Hacks\Captain 翼 series\Captain Misugi꞉ King of the Football - Kolay.nes 
    0x3F7457CE, //  Homebrew\Hacks\Captain 翼 series\Captain Misugi꞉ King of the Football - Zor.nes 
    0x8E493950, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - All Star Friendship Tournament.nes 
    0x1B43D06D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - All Star Mode 2009.nes 
    0x2B2C697A, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Allstar-A vs. Allstar-B.nes 
    0x97C564D0, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Amerika Karmasi vs. Avrupa Karmasi.nes 
    0x70462F2D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Autumn-Jun 锦丘的传奇 [神龙天舞].nes 
    0xC68B8ED5, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Barselonadan Bir Yildiz 2.nes 
    0x925C7CD1, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Besiktas.nes 
    0x2C475D8F, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Best Lineup.nes 
    0xF8097449, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Blue Stome.nes 
    0xDA6D0373, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Burning Ambition of Dongbang [Shanshan].nes 
    0x002F39EF, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Burning with Fire Emblem.nes 
    0xE7431E55, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Captain Carlos Alfonzo.nes 
    0xEC1F9DA3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Challenge of Football Emperor Q Edition [Shanshan].nes 
    0xF4BE8A8B, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Challenge of Football Emperor R Edition [Shanshan].nes 
    0x7AA30971, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Chapter of Twin.nes 
    0x902E5343, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Comback of Twins J Edition [Shanshan].nes 
    0x6A60E26C, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Comback of Twins K Edition [Shanshan].nes 
    0xEF03082C, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Dragon Dance 2꞉ 新たなる伝説・予章 [英木王者].nes 
    0x55E9ADAA, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Dragon Warrior IV [Shanshan].nes 
    0xF0042597, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Dream Holland.nes 
    0x92D6FBA3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Fated Battle.nes 
    0xFE9869C8, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Final Evolution (alt).nes 
    0xB932B2AF, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Final Evolution.nes 
    0x92C993F0, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Go With Maximum Speed.nes 
    0xB685D9F3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Hard Journey of New Japan.nes 
    0xD7F6633E, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Honor of Brazil (v2.1)[mazong1123].nes 
    0xDC3E8C19, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Hyper Edition [Whipon].nes 
    0x2AD2C72F, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Japonya vs. Japonya.nes 
    0x655DD07D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - KG꞉ Meon [hy1897].nes 
    0x67C61982, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Last Impact (alt)[Efsane].nes 
    0xA5C001F9, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Last Impact [Efsane].nes 
    0x7753EED3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Legend Of New Star.nes 
    0x47DF4FDE, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Legend of Big Three.nes 
    0xCF001089, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Legend of New Star (Harder).nes 
    0x974B9FE3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Legends of the Soccer.nes 
    0x6AD7F1CB, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Liga Argentina Boca [Axila].nes 
    0x39733088, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - More Hard [Yyhxyr].nes 
    0x24558C53, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Most Powerful Goalkeeper.nes 
    0x171187FA, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Mustafa Rezan.nes 
    0xA1EA0D39, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Omg [Heroy].nes 
    0x324447C8, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Oyuncular Degisiyor.nes 
    0xF1C3E910, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Pierre Hack [Kral 89].nes 
    0x0AED41CC, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Prince with unlimited allure.nes 
    0x28556A22, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Remade Edition [翼幻翼真].nes 
    0xCD3BE475, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Road of Nange's Glory.nes 
    0xB2CB9F8D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Secret Weapons (Gun Batm).nes 
    0xAAAAEAAD, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Secret Weapons.nes 
    0x40602A2B, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Shin All-Star Battle.nes 
    0xD4EA7F66, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Sinhistoria.nes 
    0xD08BFA11, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Sonunda Basardim Ilk Hackim.nes 
    0x9588B197, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Speed Of Ball Controllable [Shinwa].nes 
    0x4557D8E3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Speed of Ball Controllable &amp; All Footballer [Shinwa].nes 
    0xE9A65B27, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Strongest Youth in History [Shanshan].nes 
    0x97828086, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Super Dream Team [Whipon].nes 
    0x804C9EF7, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Tiger Coming.nes 
    0x840129EA, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Tiger Threaten The Border A Edition [Shanshan].nes 
    0xC8E1F82A, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Tiger Threaten The Border B Edition [Shanshan].nes 
    0xF29C50E7, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Tong's Final Perfect Edition.nes 
    0x8ABA86DF, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - Wind And Cloud Moving.nes 
    0xF6FB9857, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 南葛之路.nes 
    0x0F762E45, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 双龙天地 [翼幻翼真].nes 
    0x74573982, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 只手遮天 Good Luck.nes 
    0xB2DAFF81, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 天使之翼外章 [翼幻翼真].nes 
    0xF39DA66D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 天才・迪亚斯 [Hy1897].nes 
    0xA56341EF, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 奈依之章 (Easier)[翼幻翼真].nes 
    0x30FE69F3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 奈依之章 [翼幻翼真].nes 
    0x46C5EA60, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 恶魔降临 [翼幻翼真].nes 
    0x1138E83A, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 旋转天翼.nes 
    0xA78FC41B, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 极限冲击 [Uifaufs].nes 
    0x96B88B6B, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 橙色风暴 [Master Ryu].nes 
    0xACBE05CF, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 爆笑三人传 [鹂鸢修改].nes 
    0xDA476143, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 猛虎压境 [Hy1897].nes 
    0x226E5B5D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 王者天下.nes 
    0xCD6A4B27, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 球王之战.nes 
    0x5B28E7C0, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 皇帝的挑战.nes 
    0x86C51B00, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 皇牌杀手 [翼幻翼真].nes 
    0x31B6022A, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 皇牌杀手 with Stronger Senqi [翼幻翼真].nes 
    0xEF771BC0, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 皇者归来 [翼幻翼真].nes 
    0x3A9EC84D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 神传说的存章.nes 
    0x7716518F, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 翼の嘉年华 [翼幻翼真].nes 
    0x7E5D70E7, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 翼の挑战.nes 
    0xB6FA20AB, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 胜者为王.nes 
    0x66A97EB3, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 西部崛起.nes 
    0xD656CA37, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 足球皇帝 [Shanshan].nes 
    0xA7C9DBFB, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 雨中蛟龙 [翼幻翼真].nes 
    0x1F12F329, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker - 魔幻乱舞.nes 
    0x25CB1E3E, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [285c8 Ofseti Ile Ilgili].nes 
    0xBFDE81EB, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Ericponti].nes 
    0x4CF70700, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Greatsocrar].nes 
    0x8C7BBBAB, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Kral 89 &amp; Sisqo].nes 
    0x0B696A41, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Lejyoner159].nes 
    0xF8290879, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Lejyoner].nes 
    0x167F6FC6, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Lndirebilirsiniz Edition].nes 
    0x7237E27A, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Nba01].nes 
    0x276FA66E, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Raiju &amp; Ocelot].nes 
    0x31F13C13, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Rampion].nes 
    0xD75C5790, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Ucuncu].nes 
    0xEC7F897F, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Super Striker [Xiangfuxi].nes 
    0x03D2686D, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ World Star Team Final Edition.nes 
    0xEBEFA348, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ World Star Team Original Edition.nes 
    0x11421963, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. II꞉ Yeni Takimlar Edition.nes 
    0x206FFFE9, //  Homebrew\Hacks\Captain 翼 series\Captain 翼 Vol. I꞉ Super Striker Past [Rui].nes 
    0x340EBBD2, //  Homebrew\Hacks\Captain 翼 series\Contentious Lilinho 2.nes 
    0xD3980629, //  Homebrew\Hacks\Captain 翼 series\Kaslghnoon 1.nes 
    0xC3FB9D72, //  Homebrew\Hacks\Captain 翼 series\Ufuk ve Mahir.nes 
    0x9CAABA9E, //  Homebrew\Hacks\Mega Man series\Afroman.nes 
    0x87AD4D32, //  Homebrew\Hacks\Mega Man series\Break Man III.nes 
    0x07ED3F95, //  Homebrew\Hacks\Mega Man series\Link Man (v2)[Megafield64].nes 
    0x4C4D6607, //  Homebrew\Hacks\Mega Man series\Mega Man 3 - Speed patches [StalkerMaestro].nes 
    0xACDDAF7B, //  Homebrew\Hacks\Mega Man series\Mega Man 3꞉ Gammas Revenge (Hack 4)[Ridley X].nes 
    0x99B73CC5, //  Homebrew\Hacks\Mega Man series\Mega Man 3꞉ Gammas Revenge - Easy (Hack 4)[Ridley X].nes 
    0x8E3A720F, //  Homebrew\Hacks\Mega Man series\Mega Man 3꞉ Justiley [Jriva].nes 
    0x95881DC5, //  Homebrew\Hacks\Mega Man series\Mega Man 3꞉ The Battle of Gamma [Durfarc].nes 
    0x6D96F15D, //  Homebrew\Hacks\Mega Man series\Mega Man 3꞉ The Rise and Fall of Dr. Wily [Durfarc].nes 
    0xF06A5730, //  Homebrew\Hacks\Mega Man series\Mega Man III - Enhanced.nes 
    0x092C6FE2, //  Homebrew\Hacks\Mega Man series\Mega Man III - Remix (v0.10).nes 
    0x4AF5CFB7, //  Homebrew\Hacks\Mega Man series\Mega Man III - Return [Pantheon Sasuke].nes 
    0x63BD9789, //  Homebrew\Hacks\Mega Man series\Mega Man III Plus (broken).nes 
    0xDFF7EABC, //  Homebrew\Hacks\Mega Man series\Mega Man꞉ The Hedgehog Trap - Easy Mode [Za909].nes 
    0x335936F4, //  Homebrew\Hacks\Mega Man series\Mega Man꞉ The Hedgehog Trap - Extreme Mode [Za909].nes 
    0x6A3BD8CD, //  Homebrew\Hacks\Mega Man series\Mega Man꞉ The Hedgehog Trap - Normal Mode [Za909].nes 
    0xD417FEE3, //  Homebrew\Hacks\Mega Man series\Rockman 3꞉ Dr. Wilyの最期!？- Alpha.nes 
    0x2C7E6682, //  Homebrew\Hacks\Mega Man series\Rockman 3꞉ Dr. Wilyの最期!？- Endless.nes 
    0x48827DFD, //  Homebrew\Hacks\Mega Man series\Rockman 3꞉ Return of Wily.nes 
    0xB44FFC2E, //  Homebrew\Hacks\Mega Man series\Rockman Deft.nes 
    0x650E7C22, //  Homebrew\Hacks\Mega Man series\Rockman꞉ Zone of Eden [SOPHIA].nes 
    0x80C15E96, //  Homebrew\Hacks\Mega Man series\Tom Servo [Kasady].nes 
    0x45F4FA8C, //  Homebrew\Translations\English\Adventure Island 4 [Demiforce].nes 
    0x401EC2C3, //  Homebrew\Translations\English\Captain Tsubasa Vol. II꞉ Super Striker [Pacnsacdave].nes 
    0x980E06FD, //  Homebrew\Translations\English\Captain Tsubasa Vol. II꞉ Super Striker [hayabusakun].nes 
    0x2E6B5447, //  Homebrew\Translations\English\Gun-Dec [Stardust Crusaders].nes 
    0xE4BC40AD, //  Homebrew\Translations\English\Hudson's Adventure Island IV [Zynk Oxhyde].nes 
    0x8A9642CC, //  Homebrew\Translations\English\Hudson’s Adventure Island IV [Zynk Oxhyde](alt).nes 
    0xBBE065C8, //  Homebrew\Translations\English\Robocco Wars [Stardust Crusaders].nes 
    0xBEA58679, //  Homebrew\Translations\English\Samurai Pizza Cats [Vice Translations v1.01](US adaptation).nes 
    0xB5BBEDE9, //  Homebrew\Translations\English\Samurai Pizza Cats [Vice Translations v1.01].nes 
    0xFE2FC8B2, //  Homebrew\Translations\English\Tendou - Heavenly Child [Stardust Crusaders].nes 
    0x29EC0FD1, //  Licensed Japan\1999꞉ ほれ、みたことか!世紀末.nes 
    0x31B44C65, //  Licensed Japan\Captain 翼 Vol. II꞉ Super Striker.nes 
    0x5F14DC48, //  Licensed Japan\FC 原人꞉ Freakthoropus Computerus.nes 
    0x36584C96, //  Licensed Japan\Robocco Wars.nes 
    0xD9F1E47C, //  Licensed Japan\Rockman 3꞉ Dr. Wilyの最期!？.nes 
    0x0D3482D7, //  Licensed Japan\聖鈴伝説 Lickle.nes 
    0x7BD8F902, //  Licensed Japan\高橋名人の冒険島 IV.nes 
    0x3ECA3DDA, //  Licensed North America\Bases Loaded 3.nes 
    0x28F9B41F, //  Licensed North America\Bases Loaded 4.nes 
    0x4E44FF44, //  Licensed North America\Bonk's Adventure.nes 
    0x0C2E7863, //  Licensed North America\Dirty Harry꞉ The War Against Drugs.nes 
    0xB5E392E2, //  Licensed North America\Little Samson.nes 
    0x603AAA57, //  Licensed North America\Mega Man III.nes 
    0x23D17F5E, //  Licensed North America\The Lone Ranger.nes 
    0xA0DF4B8F, //  Licensed North America\The Simpsons꞉ Krusty's Fun House.nes 
    0xC5D48067, //  Modern\Expedition.nes 
    0x754D9130, //  Modern\Hudson Best Collection\高橋名人の冒険島 IV.nes 
    0x599629D5, //  Modern\Mega Man III (Mega Man Legacy Collection).nes 
    0x16B06537, //  Modern\Virtual Console\Mega Man III (NTSC).nes 
    0x11C87C02, //  Modern\Virtual Console\Rockman 3꞉ Dr. Wilyの最期!？.nes 
    0xED09FAF0, //  Multicarts\extracts\Angry Birds (KY-1008).nes 
    0x863FDEB8, //  Playchoice\Mega Man III.nes 
    0x59114E78, //  Unlicensed Taiwan&amp;Hong Kong\Aladdin II.nes 
    0x12481CC0, //  Unreleased\Mega Man III.nes 
    0x77F0F71D, //  Unreleased\The Simpsons꞉ Bartman Meets Radioactive Man.nes 
    0xAADA620C, //  Unreleased\U-four-ia꞉ The Saga (North America).nes 
/**** 260 mapper=4 prgrom=256K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x5B5AB1F8, //  Licensed PAL\Little Samson.nes 
    0x837A3D8A, //  Licensed PAL\Mega Man III (rev0).nes 
    0x001388B3, //  Licensed PAL\Mega Man III (rev1).nes 
    0xE9F8EF15, //  Licensed PAL\The Simpsons꞉ Bart vs. the World.nes 
    0x95CE3B58, //  Licensed PAL\The Simpsons꞉ Bartman Meets Radioactive Man.nes 
    0x585BA83D, //  Licensed PAL\The Simpsons꞉ Krusty's Fun House.nes 
    0x8A88AE79, //  Modern\Virtual Console\Mega Man III (PAL).nes 
/**** 261 mapper=4 prgrom=256K chrrom=128K mirror=H system=SYSTEM_NTSC */
    0xBC7364BB, //  Unlicensed Elsewhere\Ну, погоди!.nes 
/**** 262 mapper=4 prgrom=256K prgram=8K chrrom=128K mirror=H */
    0x6BD0717A, //  Bootleg Hacks\Super Mario Sister.nes 
    0xCA3CBFAC, //  Bootleg Singles\Super Mario Bros. 3 (047).nes 
    0xE39ECF54, //  Homebrew\Brony Blaster (NESWorld).nes 
    0x25642456, //  Homebrew\Hacks\Mega Man series\Rockman 3 ꞉ Winternight (v1.1)[StalkerMaestro].nes 
    0xC093383A, //  Homebrew\Hacks\Mega Man series\Rockman 3 ꞉ Winternight - Endless Version [StalkerMaestro].nes 
    0x1C5D9462, //  Homebrew\Hacks\Mega Man series\Rockman 3꞉ Dr. Wilyの最期!？- Unknown hack [nbroken].nes 
    0x0E7B6CC6, //  Homebrew\Hacks\Mega Man series\Rockman 3꞉ Speed Bound [StalkerMaestro].nes 
    0xCBD9B687, //  Homebrew\Hacks\Super Mario Bros. series\Blue Mario Bros. 3.nes 
    0xF819EBC1, //  Homebrew\Hacks\Super Mario Bros. series\Bowser's Revenge (v2.0a)[pacnsacdave].nes 
    0xD8BFD22F, //  Homebrew\Hacks\Super Mario Bros. series\Castle! Mario Bros. 3 [ramheroe].nes 
    0x33D16158, //  Homebrew\Hacks\Super Mario Bros. series\Dorio Challenge (v1.1)[鉴刀].nes 
    0xB0081A18, //  Homebrew\Hacks\Super Mario Bros. series\Kaizo Mario Bros. 3 [HunterW].nes 
    0x1B30245B, //  Homebrew\Hacks\Super Mario Bros. series\Kamikaze Bros. 3 [Barbarian].nes 
    0x9947C9E6, //  Homebrew\Hacks\Super Mario Bros. series\Kawa Mario Bros. 3 [Ramses G].nes 
    0x6C93ED57, //  Homebrew\Hacks\Super Mario Bros. series\Luigi's Chronicles! 2 [Googie].nes 
    0x5F066085, //  Homebrew\Hacks\Super Mario Bros. series\Luigi's Quest (World 1 Demo)[Trelior].nes 
    0x665E4B49, //  Homebrew\Hacks\Super Mario Bros. series\Mario 3 Oasis (v1.8)[Blue Finch].nes 
    0x13D59586, //  Homebrew\Hacks\Super Mario Bros. series\Peach and Daisy in The Ultimate Quest (v2b)[Big Eli King].nes 
    0x15675C29, //  Homebrew\Hacks\Super Mario Bros. series\Quick Bros. [Quick Curly].nes 
    0xEE55D5A8, //  Homebrew\Hacks\Super Mario Bros. series\Retro Mario Bros. 3.nes 
    0xD4689E42, //  Homebrew\Hacks\Super Mario Bros. series\Robo's Super Mario Bros. 3 [Robo].nes 
    0xB106FD0C, //  Homebrew\Hacks\Super Mario Bros. series\SMB3꞉ The Rainbow Realms (v1.3)[bluecrush].nes 
    0x21A78A98, //  Homebrew\Hacks\Super Mario Bros. series\Super Barb Bros. (v1.1)[LuckyLewin].nes 
    0x7B94A8A2, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 (Invincible).nes 
    0xAF60CC47, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 - Advanced.nes 
    0xC1A466D6, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 - Blob Bros. 2.nes 
    0x3957FE82, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 - CBMario 3.nes 
    0x773E6904, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 - Escape from Skull Island [ChaosComposer].nes 
    0x988CFBBE, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 - Lost Levels.nes 
    0x02207FA2, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 2nd Run.nes 
    0x4379EEFB, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3.87 Heartless - Burst Shoes Edition [Aubrey Hakomi].nes 
    0x0A8ACDC1, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3rd Root of 3.nes 
    0x7B118919, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3꞉ A New Journey [AlanJacobs].nes 
    0x7388BEBD, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3꞉ Super Smashing Pumpkins Bros. 2 [Steven &amp; David Pukin].nes 
    0x06A5CEE4, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3꞉ The Lost Levels [pacnsacdave].nes 
    0x0E1032AA, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3꞉ 修罗版 (v1.1)[胡锁、鑫鑫と鉴刀].nes 
    0x2C8C7BDC, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 9th Root of 3.nes 
    0x33220C06, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Chaos Control (v1.3)[Schwa].nes 
    0x8B3654CB, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Chaos Control.nes 
    0x4A5F9E7E, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Ridley X Hack 3 [Ridley X].nes 
    0xC6D74CA0, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. Ridley X Hack 4 [Ridley X].nes 
    0x2D1249DC, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. in the Mystery of the Flying Fish.nes 
    0xFCC3C259, //  Homebrew\Hacks\Super Mario Bros. series\Super Mitch Bros. 3 (v1.1)[Chipius].nes 
    0xAE7389DE, //  Homebrew\Hacks\Super Mario Bros. series\Super Sheffy Bros. 3 (v1.2)[West].nes 
    0x1CB9FA2D, //  Homebrew\Hacks\Super Mario Bros. series\Super Wario Bros. 3 [Googie].nes 
    0x7F752C9F, //  Homebrew\Hacks\Super Mario Bros. series\Super Waterfall Bros. 3 (v0.1)[xkeeper].nes 
    0x8AD0F417, //  Homebrew\Hacks\Super Mario Bros. series\Ultimate Super Mario Bros. 3.nes 
    0x0ED1E1D3, //  Homebrew\Hacks\Super Mario Bros. series\Uncharted Territory [Kaizo, Joe Smo].nes 
    0x881665FA, //  Homebrew\Translations\English\Kid Niki 3 [Suicidal Translations, MottZilla, Proveaux].nes 
    0xA8416EDF, //  Homebrew\Translations\English\Kid Niki 3 [Suicidal Translations, MottZilla].nes 
    0xE7DDFEE3, //  Licensed Japan\Super Mario Bros. 3 (rev0).nes 
    0x52E2B5E0, //  Licensed Japan\Super Mario Bros. 3 (rev1).nes 
    0xA0B0B742, //  Licensed North America\Super Mario Bros. 3 (rev0).nes 
    0x2E6301ED, //  Licensed North America\Super Mario Bros. 3 (rev1).nes 
    0xF12997EA, //  Modern\Virtual Console\Super Mario Bros. 3 (Japan).nes 
    0xF4DFA033, //  Modern\Virtual Console\Super Mario Bros. 3 (North America).nes 
    0xAA9A34A4, //  Playchoice\Super Mario Bros. 3.nes 
/**** 263 mapper=4 prgrom=256K prgram=8K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x2778B826, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ The Second World of the Super Mario Bros. 3 [John].nes 
    0x1ED5C801, //  Licensed PAL\Super Mario Bros. 3.nes 
    0xC46969DF, //  Modern\Virtual Console\Super Mario Bros. 3 (PAL).nes 
/**** 264 mapper=4 prgrom=256K chrrom=256K mirror=H */
    0xB9AA4EEB, //  Bad Dumps\Earthworm Jim 3 [bad CHR data].nes 
    0x5983D32D, //  Bad Dumps\Somari the Adventurer (NT-616)[bad CHR].nes 
    0xAFAD321B, //  Bad Dumps\Street Blaster V Turbo 20 [PRG overdump].nes 
    0xBF78E8C1, //  Bad Dumps\Street Fighter V Turbo 60 [PRG overdump].nes 
    0x0B6E91DD, //  Bad Dumps\Street Fighter V Turbo 60 [bad CHR][PRG overdump].nes 
    0xA38DF799, //  Bad Dumps\Super Aladdin [bad CHR].nes 
    0x3627D662, //  Bootleg Hacks\Rockman 6꞉ Bluesの罠!？.nes 
    0x7638BDB3, //  Bootleg Singles\Aladdin - 阿拉丁 (A-N7).nes 
    0x9CD7D385, //  Bootleg Singles\Aladdin - 阿拉丁 (NT-622).nes 
    0xBD79514D, //  Bootleg Singles\Aladdin 4.nes 
    0xDC5959D2, //  Bootleg Singles\Somari the Adventurer (NT-616).nes 
    0x10CB935F, //  Bootleg Singles\Super Lion King (NT-656).nes 
    0xA88C6388, //  Bootleg Singles\Tiny Toon Adventures 6 (NT-6088).nes 
    0xB14209F0, //  Bootleg Singles\TouchGamePlayer\Teenage Mutant Ninja Turtles III꞉ The Manhattan Project.nes 
    0x085B7023, //  Bootleg Singles\TouchGamePlayer\Teenage Mutant Ninja Turtles II꞉ The Arcade Game.nes 
    0x2BB08BCA, //  Compatibility Hacks\Bomber Boy [m004].nes 
    0xBC45F99E, //  Compatibility Hacks\Jim Power꞉ The Lost Dimension [m004 extract].nes 
    0xC919F31D, //  Compatibility Hacks\Nightshade Part 1꞉ The Claws of Sutekh (Piko Interactive)[m004 extract].nes 
    0x2CC408D8, //  Compatibility Hacks\Sonic 3D Blast 6 [m004].nes 
    0x1FE3160A, //  Compatibility Hacks\Sonic 3D Blast 6 [m004][f frameIRQ].nes 
    0x31BFEFDD, //  Compatibility Hacks\Super Lion King 2 [m004].nes 
    0xBFAFFB62, //  Compatibility Hacks\Super Lion King [m004 rev0][NewRisingSun].nes 
    0xD49073F3, //  Compatibility Hacks\Super Lion King [m004 rev1][NewRisingSun].nes 
    0xEE1CAD03, //  Compatibility Hacks\TV System Conversions\西天取经 II - Journey to the West (NTSC)[NewRisingSun].nes 
    0xAAB0F29E, //  Compatibility Hacks\The Panda Prince [m004][NewRisingSun].nes 
    0xC3B2E39E, //  Compatibility Hacks\The Super Shinobi [m004].nes 
    0x7914B6E2, //  Homebrew\8 Bit Riki Illustrations [Riki].nes 
    0xCE17D7CB, //  Homebrew\Bugfixes, Improvements\Aladdin - 阿拉丁 (A-N7)[music replacement v1.1][Gigasoft].nes 
    0x4660A73A, //  Homebrew\Bugfixes, Improvements\Aladdin - 阿拉丁 (NT-622)[music replacement v1.1][Gigasoft].nes 
    0x05563B35, //  Homebrew\Bugfixes, Improvements\Aladdin 4 [music replacement v1.1][Gigasoft].nes 
    0xDB02C7F6, //  Homebrew\Bugfixes, Improvements\Bomber Boy [m004][All levels finishable][NewRisingSun].nes 
    0x24032C59, //  Homebrew\Bugfixes, Improvements\Sonic the Hedgehog.nes 
    0x0DC36AA4, //  Homebrew\Cheats\Super Lion King (NT-656)[fly].nes 
    0x3A60A9A2, //  Homebrew\Demos\Microsoft Windows 98.nes 
    0x0A0121DC, //  Homebrew\Demos\Neotoxin [Snobro, EFX].nes 
    0x71DC445A, //  Homebrew\Hacks\Captain 翼 series\المهاجم الهداف꞉كابتن ماجد 2 - Black Hack.nes 
    0x41A8AD48, //  Homebrew\Hacks\Captain 翼 series\المهاجم الهداف꞉كابتن ماجد 2 - Deadly Laugh.nes 
    0xED26AE4E, //  Homebrew\Hacks\Captain 翼 series\المهاجم الهداف꞉كابتن ماجد 2 - Design Stoom.nes 
    0xC6576EAA, //  Homebrew\Hacks\Captain 翼 series\المهاجم الهداف꞉كابتن ماجد 2 - Unknown Hack [Farouk].nes 
    0x9FF5324B, //  Homebrew\Hacks\Mega Man series\Mega Lion II [WarrantX].nes 
    0x213F243F, //  Homebrew\Hacks\Mega Man series\Mega Man 5꞉ Protoman's Revenge [Ridley X].nes 
    0xA738249F, //  Homebrew\Hacks\Mega Man series\Mega Man 5꞉ Protoman's Trap [FlashPV].nes 
    0xF7E39791, //  Homebrew\Hacks\Mega Man series\Mega Man Indonesia Tour [Anandastoon].nes 
    0x347FC782, //  Homebrew\Hacks\Mega Man series\Mega Man Overload 3 - Normal [Ozma].nes 
    0xBC956304, //  Homebrew\Hacks\Mega Man series\Mega Man Overload 3 - Original [Ozma].nes 
    0x02E3DB40, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown V [Matrixz].nes 
    0x6F17D637, //  Homebrew\Hacks\Mega Man series\Mega Man V - Debug.nes 
    0x44B028CA, //  Homebrew\Hacks\Mega Man series\Mega Man V - Fly [Matrixz].nes 
    0xAC4A1254, //  Homebrew\Hacks\Mega Man series\Mega Man V - Mega Mario.nes 
    0x0C0E7D3F, //  Homebrew\Hacks\Mega Man series\Mega Man V - Random Palette [Matrixz].nes 
    0x8CB4E2E5, //  Homebrew\Hacks\Mega Man series\Mega Man V 1977.nes 
    0xE2B47712, //  Homebrew\Hacks\Mega Man series\Mega Man V Remix [Matrixz].nes 
    0x6A6B1DFB, //  Homebrew\Hacks\Mega Man series\Mega Man V Second Strike꞉ Protoman Edition (v1.1)[MetHy].nes 
    0xD71CFD09, //  Homebrew\Hacks\Mega Man series\Mega Man V꞉ Protoman Edition (v1.1)[MetHy].nes 
    0xD627ECB9, //  Homebrew\Hacks\Mega Man series\Proto Man [Sephiroth X].nes 
    0x9DD45A2A, //  Homebrew\Hacks\Mega Man series\Rockman 2008 New Year [Puresabe].nes 
    0x4FE38E4F, //  Homebrew\Hacks\Mega Man series\Rockman 2015 New Year [Puresabe].nes 
    0x1245ABA1, //  Homebrew\Hacks\Mega Man series\Rockman 5 EX꞉ 史上最大の危機 [AYCNN].nes 
    0xCC636724, //  Homebrew\Hacks\Mega Man series\Rockman 5-7꞉ Bluesの罠!？ [Dark Mark].nes 
    0xDE3AC7DB, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ A.S..nes 
    0x847AB480, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Bluesの罠!？ (v1.1)[Krizal].nes 
    0x61A676F7, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Bluesの罠!？.nes [KT].nes 
    0xFDC88A41, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Dark Return.nes 
    0xA638B940, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Dood in Gate.nes 
    0x8ADD6480, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Dood in Wily's Last [Chiko].nes 
    0x301F462A, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Killer to Hell.nes 
    0x69CF3DC6, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Metropolis [Stalkermaestro].nes 
    0xD9A0A758, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Wily's Dream Space (alt) [TAR].nes 
    0x0139897F, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Wily's Dream Space [TAR].nes 
    0x001EC6AD, //  Homebrew\Hacks\Mega Man series\Rockman X Cross (v4.1) [つらら].nes 
    0x297EF3C8, //  Homebrew\Hacks\Mega Man series\Rockman X Cross - Boss Rush (v6.1) [つらら].nes 
    0x128C1D19, //  Homebrew\Hacks\Mega Man series\とっても Luckyman!! 5 - 現る!! のまき.nes 
    0x4E554BF7, //  Homebrew\Hacks\Mega Man series\とっても Luckyman!! Part 5 - 再来!! のまき.nes 
    0xD9EBFC0D, //  Homebrew\Hacks\Sonic the Hedgehog Improvement [the jabu].nes 
    0x94E11574, //  Homebrew\Translations\English\Monkey King [Pacnsacdave v1.0].nes 
    0x8085CA5D, //  Homebrew\Translations\English\Moon Crystal [Alex W. Jackson].nes 
    0xF79E6A3F, //  Homebrew\Translations\English\Speeding Hummer [Pacnsacdave].nes 
    0x7E6E6E78, //  Homebrew\Translations\English\Tetrastar꞉ The Fighter [Gaijin Productions v1.00beta].nes 
    0x7C27AB86, //  Licensed Japan\Aces꞉ Iron Eagle 3.nes 
    0x26E39935, //  Licensed Japan\Moon Crystal.nes 
    0xFDDF2135, //  Licensed Japan\Rockman 5꞉ Bluesの罠!？.nes 
    0x0B6443D4, //  Licensed Japan\Star Wars꞉ The Empire Strikes Back.nes 
    0xF85E264D, //  Licensed Japan\Tetrastar꞉ The Fighter.nes 
    0x0FD6BFC8, //  Licensed Japan\Western Kids.nes 
    0x2C5908A7, //  Licensed North America\Advanced Dungeons &amp; Dragons꞉ DragonStrike.nes 
    0xD18E6BE3, //  Licensed North America\Cowboy Kid.nes 
    0x0A7E62D4, //  Licensed North America\F-117A Stealth Fighter.nes 
    0x0BCAA4D7, //  Licensed North America\F-15 Strike Eagle.nes 
    0x3EDCF7E8, //  Licensed North America\Mega Man V.nes 
    0xA60CA3D6, //  Licensed North America\Nightshade Part 1꞉ The Claws of Sutekh.nes 
    0x16EBA50A, //  Licensed North America\Star Trek꞉ 25th Anniversary.nes 
    0x240DE736, //  Licensed North America\Star Wars꞉ The Empire Strikes Back.nes 
    0xBB6D7949, //  Licensed North America\Teenage Mutant Ninja Turtles III꞉ The Manhattan Project.nes 
    0xA9217EA2, //  Licensed North America\Teenage Mutant Ninja Turtles II꞉ The Arcade Game.nes 
    0xE387C77F, //  Licensed North America\Ultimate Air Combat.nes 
    0x6512605B, //  Modern\8 Bit Music Power Encore.nes 
    0x68B85928, //  Modern\Mega Man V (Mega Man Legacy Collection).nes 
    0x005C7569, //  Modern\Virtual Console\Mega Man V (NTSC).nes 
    0x1BC3F53B, //  Modern\Virtual Console\Rockman 5꞉ Bluesの罠!？.nes 
    0xCBE29C42, //  Modern\キラキラ Star Night.nes 
    0xD3AC6990, //  Multicarts\extracts\餓狼伝説 Special.nes 
    0x0496DD98, //  Playchoice\Teenage Mutant Ninja Turtles II꞉ The Arcade Game.nes 
    0xC46EDA12, //  Plug-and-Play\extracts\Super Hero.nes 
    0xBDA183BB, //  Samples\Teenage Mutant Ninja Turtles II꞉ The Arcade Game.nes 
    0xCF3CB261, //  Unlicensed China\The Lion King 5꞉ Timon and Pumbaa.nes 
    0x513EB779, //  Unlicensed China\The Lion King III꞉ Timon and Pumbaa.nes 
    0xA81C485B, //  Unlicensed Taiwan&amp;Hong Kong\Earthworm Jim 3.nes 
    0x3A6577CD, //  Unlicensed Taiwan&amp;Hong Kong\Jurassic Park꞉ The Lost World.nes 
    0x78872BD7, //  Unlicensed Taiwan&amp;Hong Kong\Sonic the Hedgehog.nes 
    0xD9261FEE, //  Unlicensed Taiwan&amp;Hong Kong\Super Aladdin.nes 
    0x7CF90D81, //  Unreleased\Earthworm Jim 2 (Test build).nes 
    0xDBC6E234, //  Unreleased\Karaoke.nes 
    0x6C6565A2, //  Unreleased\Pocahontas Part 2 (debug).nes 
    0xE9A151CB, //  Unreleased\Pocahontas Part 2 (dev).nes 
    0xCFD29C93, //  Unreleased\Star Wars꞉ The Empire Strikes Back.nes 
    0x0754544F, //  Unreleased\Super Boogerman.nes 
/**** 265 mapper=4 prgrom=256K chrrom=256K mirror=H system=SYSTEM_PAL */
    0x989C1019, //  Licensed PAL\F-15 Strike Eagle (English).nes 
    0xBBB710D9, //  Licensed PAL\F-15 Strike Eagle (French).nes 
    0xD353D351, //  Licensed PAL\F-15 Strike Eagle (German).nes 
    0xFA74F656, //  Licensed PAL\F-15 Strike Eagle (Italian).nes 
    0x872DE7A2, //  Licensed PAL\F-15 Strike Eagle (Swedish).nes 
    0xA4DCF72E, //  Licensed PAL\Mega Man V.nes 
    0xCC553FC4, //  Licensed PAL\Star Trek꞉ 25th Anniversary.nes 
    0x54E43C57, //  Licensed PAL\Star Wars꞉ The Empire Strikes Back.nes 
    0xC5657C12, //  Licensed PAL\Teenage Mutant Hero Turtles II꞉ The Arcade Game.nes 
    0x7BCCAFBB, //  Licensed PAL\Teenage Mutant Ninja Turtles II꞉ The Arcade Game.nes 
    0xC0F251EA, //  Licensed PAL\Ultimate Air Combat.nes 
    0x3DCA964A, //  Modern\Virtual Console\Mega Man V (PAL).nes 
    0x690AFE9F, //  Unreleased\Ultimate Air Combat.nes 
/**** 266 mapper=4 prgrom=256K chrrom=256K mirror=H system=SYSTEM_NTSC */
    0x6912DE82, //  Bootleg Singles\The Dragon (李小龍傳奇).nes 
    0xE6402758, //  Bootleg Singles\中國兔寶寶 Rabbit (NT-6089).nes 
    0xA9C07FF3, //  Unlicensed China\Felix the Cat.nes 
    0x64F19E5C, //  Unlicensed China\Panda Baby.nes 
    0xB7456A6A, //  Unlicensed Elsewhere\Family Kid.nes 
    0xDDED013E, //  Unlicensed Taiwan&amp;Hong Kong\The Dragon.nes 
/**** 267 mapper=4 prgrom=256K chrrom=256K mirror=H system=SYSTEM_NTSC */
    0x659C8EF4, //  Bad Dumps\机器猫 - 小叮当 - socat [dumper graffiti].nes 
    0x1C718B68, //  Bad Dumps\西天取经 II - Journey to the West [bad PRG].nes 
    0xE82714C5, //  Bad Dumps\鏡花緣 [PRG&amp;CHR overdump].nes 
    0x3A9D43C3, //  Compatibility Hacks\戰斧 - Golden Axe [m004].nes 
    0x74F1E4DF, //  Homebrew\Translations\English\Dragon Knife [Pacnsacdave].nes 
    0xB5B49F83, //  Homebrew\Translations\English\Flowers in the Mirror [Pacnsacdave].nes 
    0x062CC3FB, //  Homebrew\Translations\English\Genuine Monkey [Pacnsacdave].nes 
    0x43B0FA71, //  Homebrew\Translations\English\Super Contra X [Pacnsacdave].nes 
    0x54FCBE43, //  Homebrew\Translations\English\Super Donkey Kong꞉ Banana Boat [Pacnsacdave].nes 
    0x31E8B65C, //  Homebrew\Translations\English\The Hacker [Pacnsacdave].nes 
    0xE1BF81CA, //  Homebrew\Translations\English\The Lion King Legend [Pacnsacdave].nes 
    0x2FD52AF3, //  Homebrew\Translations\English\Titanic [Pacnsacdave v2.1].nes 
    0x8CEB23F7, //  Homebrew\Translations\English\Titanic [Pacnsacdave vx.x].nes 
    0xBF4E9925, //  Unlicensed China\Street Dance.nes 
    0xA462B119, //  Unlicensed China\Super Contra X.nes 
    0x0AA49929, //  Unlicensed China\Tom &amp; Jerry 3.nes 
    0xA031C422, //  Unlicensed China\南晶\The Hacker.nes 
    0xA6E4F246, //  Unlicensed China\南晶\Titanic.nes 
    0x9C596EC8, //  Unlicensed China\外星\Dragon Knife.nes 
    0xFD2387D0, //  Unlicensed China\外星\The Lion King Legend - 獅王傳奇.nes 
    0x497FF14A, //  Unlicensed China\外星\天才小子.nes 
    0xA4762B7A, //  Unlicensed China\外星\天王降魔传.nes 
    0x8B837EFF, //  Unlicensed China\外星\屠龙刀.nes 
    0x2A3B1913, //  Unlicensed China\外星\星际武士 - Super Fighter.nes 
    0x02ED9012, //  Unlicensed China\外星\机器猫 - 小叮当 - socat.nes 
    0xA273590F, //  Unlicensed China\外星\真假猴王.nes 
    0xBA433EBB, //  Unlicensed China\外星\西天取经 II - Journey to the West.nes 
    0xA410290B, //  Unlicensed China\外星\靈界護法 - Shock.nes 
    0xD96D9924, //  Unlicensed China\香蕉船 - Super Donkey Kong.nes 
    0xCDF63F0C, //  Unreleased\2002 World Cup PK.nes 
/**** 268 mapper=4 prgrom=256K prgram=8K chrrom=256K mirror=H */
    0xEBF01E65, //  Homebrew\Gotta Protectors꞉ Amazon's Running Diet [Ancient].nes 
    0x2DDE1B73, //  Homebrew\Hacks\Mega Man series\Mega Man V Time Attack (v0.311)[Matrixz].nes 
    0xAA98E50A, //  Homebrew\Hacks\Super Mario Bros. series\Mario Chronicles [RingoDoggie].nes 
    0xB88866EC, //  Homebrew\みんなでまもって騎士꞉ AmazonのDiet大作戦.nes 
    0x00837960, //  Licensed North America\King's Quest V꞉ Absence Makes the Heart Go Yonder!.nes 
    0x6FFACEE5, //  Samples\東方老桜夢.nes 
/**** 269 mapper=4 prgrom=512K chrram=8K mirror=H */
    0xE3526D8B, //  Bootleg Hacks\Rockman 7꞉ 史上最大の戦い!!.nes 
    0x368C3A96, //  Homebrew\Demos\Bad Apple PV-FC.nes 
    0x04379E63, //  Homebrew\Demos\Megaman Forever - Unabridged Demo.nes 
    0xEB9E3D7B, //  Homebrew\Hacks\Mega Man series\Mega Man 6꞉ Ridley X Hack 2 [Ridley X].nes 
    0x97BDE71F, //  Homebrew\Hacks\Mega Man series\Mega Man C4.nes 
    0xDB538452, //  Homebrew\Hacks\Mega Man series\Mega Man IV - Gadget Master.nes 
    0x9DBC9906, //  Homebrew\Hacks\Mega Man series\Mega Man IV - Ridley X Hack 8.nes 
    0xF15165BF, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV - Boss Mode [InsectDuel's Domain].nes 
    0xB050C940, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV - Hard Boss Mode [InsectDuel's Domain].nes 
    0x833454A7, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV - Hard [InsectDuel's Domain].nes 
    0x6C939294, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV Ex - Boss Mode [InsectDuel's Domain].nes 
    0x2D923E6B, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV Ex - Hard Boss Mode [InsectDuel's Domain].nes 
    0x1EF6A38C, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV Ex - Hard [InsectDuel's Domain].nes 
    0x5FF70F73, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV Ex.nes 
    0xC235F858, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown IV [InsectDuel's Domain].nes 
    0x9DE876D1, //  Homebrew\Hacks\Mega Man series\Mega Man Showdown VI [DES].nes 
    0x75F63EBA, //  Homebrew\Hacks\Mega Man series\Mega Man꞉ Maverick Revenge.nes 
    0xE0D3B18E, //  Homebrew\Hacks\Mega Man series\Mockman 6 (v.001).nes 
    0x153A065D, //  Homebrew\Hacks\Mega Man series\Proto Man VI (v0.70)[Nazeji].nes 
    0x390F0F39, //  Homebrew\Hacks\Mega Man series\Rockman 2014 New Year [Puresabe].nes 
    0x558EDFF9, //  Homebrew\Hacks\Mega Man series\Rockman 4 DX [Chiko].nes 
    0xCE6C9C9C, //  Homebrew\Hacks\Mega Man series\Rockman 4 EX꞉ 新たなる野望!! [AYCNN].nes 
    0xE9744705, //  Homebrew\Hacks\Mega Man series\Rockman 4꞉ Sparking To Hero [Yosaku].nes 
    0x30AF92A7, //  Homebrew\Hacks\Mega Man series\Rockman 4꞉ 新たなる野望!! - Hack (v1.1)[Krizal].nes 
    0xA7C5B4FF, //  Homebrew\Hacks\Mega Man series\Rockman 6 EX꞉ 新たなる挑戦者!! [AYCNN].nes 
    0x5548385C, //  Homebrew\Hacks\Mega Man series\Rockman 6꞉ Mr. X Return.nes 
    0x3FF14304, //  Homebrew\Hacks\Mega Man series\Rockman 6꞉ Ridley X Hack.nes 
    0x300B3C57, //  Homebrew\Hacks\Mega Man series\Rockman 6꞉ 史上最低の戦い!! [再].nes 
    0xC3C91AC2, //  Homebrew\Hacks\Mega Man series\Rockman 6꞉ 史上最大の戦い!! [Krizal].nes 
    0xA6048958, //  Homebrew\Hacks\Mega Man series\Rockman꞉ Sparking To Hero [Yosaku].nes 
    0x6CEFFEDC, //  Homebrew\Translations\English\Mega Man 6 [Her-Saki v1.1, DarthVaderX .0.90].nes 
    0x698D049F, //  Homebrew\Translations\English\Mega Man 6꞉ The Greatest Battle in History!! [Her-Saki v1.1, DarthVaderX v0.90].nes 
    0x87A6E5BB, //  Homebrew\Translations\English\Rockman 6꞉ The Greatest Battle in History!! [Her-Saki v1.1].nes 
    0x3256114C, //  Licensed Japan\America 横断 Ultra Quiz꞉ 史上最大の戦い (rev0).nes 
    0xC7ED0348, //  Licensed Japan\America 横断 Ultra Quiz꞉ 史上最大の戦い (rev1).nes 
    0xF161A5D8, //  Licensed Japan\Rockman 4꞉ 新たなる野望!! (rev0).nes 
    0xE0FFECCD, //  Licensed Japan\Rockman 4꞉ 新たなる野望!! (rev1).nes 
    0x2D664D99, //  Licensed Japan\Rockman 6꞉ 史上最大の戦い!!.nes 
    0x9FD718FD, //  Licensed Japan\The Gorilla Man.nes 
    0xB7773A07, //  Licensed Japan\嗚呼! 野球人生一直線.nes 
    0x98A97A59, //  Licensed Japan\舛添要一꞉ 朝まで Famicom.nes 
    0x2BC67AA8, //  Licensed North America\Mega Man IV (rev0).nes 
    0x18A2E74F, //  Licensed North America\Mega Man IV (rev1).nes 
    0x988798A8, //  Licensed North America\Mega Man VI.nes 
    0x0FAA8F73, //  Modern\Mega Man IV (Mega Man Legacy Collection).nes 
    0xDCC74EF2, //  Modern\Mega Man VI (Mega Man Legacy Collection).nes 
    0x659266E8, //  Modern\Virtual Console\Mega Man IV (NTSC).nes 
    0x111B2B1D, //  Modern\Virtual Console\Rockman 4꞉ 新たなる野望!!.nes 
/**** 270 mapper=4 prgrom=512K chrram=8K mirror=H system=SYSTEM_PAL */
    0x1B932BEA, //  Licensed PAL\Mega Man IV.nes 
    0x66A3AA4D, //  Modern\Virtual Console\Mega Man IV (PAL).nes 
/**** 271 mapper=4 prgrom=512K chrram=32K mirror=H */
    0xF401D3EE, //  Modern\Haunted Halloween '86꞉ The Curse of Possum Hollow (alt).nes 
    0xA7682169, //  Modern\Haunted Halloween '86꞉ The Curse of Possum Hollow.nes 
    0x614B8850, //  Unreleased\Haunted Halloween '86.nes 
/**** 272 mapper=4 prgrom=512K chrrom=8K mirror=H */
    0x999107A7, //  Homebrew\Demos\Max-300.nes 
/**** 273 mapper=4 prgrom=512K chrrom=64K mirror=H */
    0x2922CA6C, //  Homebrew\Demos\Aaaeeeooo 8 Bit [N.K.].nes 
/**** 274 mapper=4 prgrom=512K chrrom=128K mirror=H */
    0x40DB45E0, //  Modern\NEO 平安京 Alien.nes 
/**** 275 mapper=4 prgrom=512K chrrom=256K mirror=H */
    0x949B3EEA, //  Homebrew\Hacks\Mega Man series\Mega Man III - Improvement (v2.01).nes 
    0x3BEE76DD, //  Homebrew\Hacks\Mega Man series\Rockman 3꞉ Dr. Wilyの最期!？- Improvement (v2.01).nes 
    0xF53F83DD, //  Modern\8 Bit Music Power Final (rev0).nes 
    0x5CCD20C9, //  Modern\8 Bit Music Power Final (rev1).nes 
    0x00263E3C, //  Modern\8 Bit Music Power.nes 
    0x2A5BCA85, //  Modern\8 Bit Rhythm Land.nes 
    0x96D8E95E, //  Modern\Trophy (Limited Edition).nes 
    0xF61F3B0A, //  Modern\Trophy.nes 
/**** 276 mapper=4 prgrom=512K prgram=8K chrrom=256K mirror=H */
    0x683B053D, //  Homebrew\Demos\Bad Apple 2.5 2015 [Little Limit].nes 
    0x70A077E4, //  Homebrew\Demos\Bad Apple 2.5 [Little Limit].nes 
    0xCA45F105, //  Homebrew\Hacks\Mega Man series\Mega Man 3꞉ Dr. Wily's End!？ [FlashPV].nes 
    0x2D70BBBD, //  Homebrew\Hacks\Super Mario Bros. series\Communist Mario 3 [KP9000].nes 
/**** 277 mapper=4 prgrom=512K chrrom=512K mirror=H */
    0x8F6CC85A, //  Bad Dumps\(KT-220B) Totally Rad 4-in-1 [overdump].nes 
/**** 278 mapper=4 prgrom=32K prgnvram=8K chrrom=40K mirror=H battery=1 */
    0x8095AD30, //  Samples\Nanaca-Crash!!.nes 
/**** 279 mapper=4 prgrom=32K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x64594DA3, //  Modern\Russian Roulette.nes 
/**** 280 mapper=4 prgrom=64K prgnvram=8K chrrom=8K mirror=H battery=1 */
    0x5C61A993, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 Special [Jordan Bieber].nes 
/**** 281 mapper=4 prgrom=64K prgnvram=8K chrrom=16K mirror=H battery=1 */
    0x00C32856, //  Compatibility Hacks\Super Mario Bros. 2 (Loopy rev0).nes 
    0x7F3A4ADD, //  Compatibility Hacks\Super Mario Bros. 2 (Loopy rev1).nes 
    0xC39743EE, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 with Super Mario Bros. Grahics [Vallenatero2015].nes 
/**** 282 mapper=4 prgrom=64K prgnvram=8K chrrom=128K mirror=H battery=1 system=SYSTEM_NTSC */
    0x2BD30289, //  Modern\Justice Duel.nes 
    0x0CE9CA00, //  Samples\Justice Duel (Demo 1).nes 
    0xA04AD3BB, //  Samples\Justice Duel (Demo 2).nes 
/**** 283 mapper=4 prgrom=128K prgnvram=8K chrram=8K mirror=H battery=1 */
    0xCEB1FD42, //  Homebrew\Hacks\Super Pitfall 30th Anniversary Edition [Nesrocks].nes 
    0xA81294B1, //  Homebrew\Hacks\The Goonies II꞉ SRAM Saving Edition [8-bit fan].nes 
/**** 284 mapper=4 prgrom=128K prgnvram=8K chrram=8K mirror=H battery=1 system=SYSTEM_NTSC */
    0x2C3AD058, //  Bad Dumps\神秘金三角꞉ The Hyrule Fantasy [SRAM enable fail].nes 
    0xB5766659, //  Unlicensed China\外星\東方的傳說꞉ The Hyrule Fantasy.nes 
/**** 285 mapper=4 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x553AD67C, //  Bootleg Hacks\5 Winning Goal.nes 
    0x040ABC34, //  Bootleg Hacks\France 98.nes 
    0x7314F5C9, //  Homebrew\Hacks\8-bit Smash Bros..nes 
    0xEFB3B017, //  Homebrew\Nanaca-Crash!!.nes 
    0xC7D62AF0, //  Homebrew\Translations\English\Christopher Columbus [PL Trans Force].nes 
    0x32E92DD4, //  Homebrew\Translations\English\Columbus꞉ Golden Dawn [aishsha].nes 
    0xD1690E64, //  Homebrew\Translations\English\J-League Winning Goal [Stardust Crusaders].nes 
    0x764C2880, //  Homebrew\Translations\English\Little Magic [Gaijin Productions, TransBRC v0.33 alpha].nes 
    0x809C3878, //  Homebrew\Translations\English\Nadia꞉ Secret of the Blue Water [J2e Translations v1.3].nes 
    0x3F82A7DD, //  Homebrew\Translations\English\SD Gundam - Gachapon Soldier 2꞉ Capsule War [MrRichard999].nes 
    0x860B8AAC, //  Homebrew\Translations\English\Sugoro Quest꞉ The Quest of Dice Heroes [AlanMidas koogly.titivilus, KingMike's Translations].nes 
    0x0E9D9622, //  Homebrew\Translations\English\Technos Samurai꞉ Downtown Special! [Technos Samurai Translation Project].nes 
    0xCDC69231, //  Homebrew\Translations\English\Wizardry II꞉ Legacy of Llylgamyn [MrRichard999 v1.1].nes 
    0xCB5ACB49, //  Licensed Japan\Capcom Barcelona '92.nes 
    0x30A225A8, //  Licensed Japan\Columbus꞉ 黄金の夜明け.nes 
    0xDC45A886, //  Licensed Japan\Downtown Special꞉ くにおくんの時代劇だよ - 全員集合!.nes 
    0x0AE3CC5E, //  Licensed Japan\Home Run Nighter '90꞉ The Pennant League.nes 
    0x803B9979, //  Licensed Japan\J.League Fighting Soccer꞉ The King of Ace Strikers.nes 
    0x3D4B64F1, //  Licensed Japan\J.League Winning Goal.nes 
    0xC9EDF585, //  Licensed Japan\Little Magic.nes 
    0xA2F713C0, //  Licensed Japan\SD Gundam ガチャポン戦士 2꞉ Capsule 戦記.nes 
    0x1066B66D, //  Licensed Japan\SD Gundam ガチャポン戦士 3꞉ 英雄戦記.nes 
    0x6C940A59, //  Licensed Japan\SD Gundam ガチャポン戦士 5꞉ Battle of Universal Century.nes 
    0x8D26FDEA, //  Licensed Japan\SD 戦国武将列伝꞉ 烈火のごとく天下を盗れ!.nes 
    0xCF40B1C5, //  Licensed Japan\Shadowgate.nes 
    0xEEE6314E, //  Licensed Japan\Solomonの鍵 2꞉ Coolmin 島救出作戦.nes 
    0x5C123EF7, //  Licensed Japan\Wizardry꞉ Diamondの騎士.nes 
    0x55397DB3, //  Licensed Japan\Wizardry꞉ Llylgamynの遺産.nes 
    0x9D9A4A26, //  Licensed Japan\からくり剣豪伝꞉ 武蔵 Lord - からくり人走る!.nes 
    0x1411005B, //  Licensed Japan\すごろ Quest꞉ Diceの戦士たち.nes 
    0x0546BD12, //  Licensed Japan\不思議の海 Nadia.nes 
    0x2A1919FE, //  Licensed Japan\仮面 Rider SD꞉ Grand Shockerの野望.nes 
    0x31C7AD13, //  Licensed Japan\少年 Ashibe꞉ Nepal 大冒険の巻.nes 
    0x028374F2, //  Licensed Japan\山村美紗 Suspense꞉ 京都財テク殺人事件.nes 
    0xC9187B43, //  Licensed Japan\影狼伝説.nes 
    0x1248326D, //  Licensed Japan\悪魔の招待状.nes 
    0xC22BC87B, //  Licensed Japan\政略 Simulation꞉ 陰謀の惑星 - Shancara.nes 
    0xFA2A8A8B, //  Licensed Japan\日物麻雀 III꞉ 麻雀 G Men.nes 
    0x8F011713, //  Licensed Japan\黒鉄ヒロシの予想大好き! 勝馬伝説.nes 
    0xBE250388, //  Licensed North America\Capcom's Gold Medal Challenge '92.nes 
    0xF00584B6, //  Licensed North America\Cyber Stadium Series꞉ Base Wars.nes 
    0x91E2E863, //  Licensed North America\Magician.nes 
    0x6A1F628A, //  Licensed North America\Shadowgate.nes 
    0x9BAC73EF, //  Licensed North America\Uninvited.nes 
    0x6BC33D2F, //  Licensed North America\Wizardry꞉ The Knight of Diamonds.nes 
    0x7E549E49, //  Modern\Namco Museum Archives\Gaplus.nes 
    0x2628E6D4, //  Modern\Virtual Console\すごろ Quest꞉ Diceの戦士たち.nes 
    0xD3EC98AA, //  Samples\Columbus꞉ 黄金の夜明け.nes 
    0xB7D69A6D, //  Unreleased\Magician (Beta 2).nes 
    0x2D1FEE70, //  Unreleased\Magician (Beta 3).nes 
/**** 286 mapper=4 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 system=SYSTEM_PAL */
    0x1590CF62, //  Licensed PAL\Capcom's Gold Medal Challenge '92.nes 
    0x13E01649, //  Licensed PAL\Shadowgate (English).nes 
    0x9F01687D, //  Licensed PAL\Shadowgate (French).nes 
    0xB64078F3, //  Licensed PAL\Shadowgate (German).nes 
    0x2B1497DC, //  Licensed PAL\Shadowgate (Swedish).nes 
/**** 287 mapper=4 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 system=SYSTEM_NTSC */
    0xC57060CE, //  Compatibility Hacks\宠物宝宝 [m004].nes 
    0x343611E3, //  Unlicensed China\振华\大戰略.nes 
/**** 288 mapper=4 prgrom=128K prgnvram=16K chrrom=128K mirror=H battery=1 */
    0xA5C5A521, //  Modern\くにおくん熱血 Collection\Downtown Special꞉ くにおくんの時代劇だよ ~全員集合!~.nes 
/**** 289 mapper=4 prgrom=128K prgram=8K chrrom=128K mirror=H battery=1 */
    0xE783DE86, //  Bootleg Hacks\FIFA International Soccer '96.nes 
    0xE557307F, //  Bootleg Hacks\FIFA International Soccer '97.nes 
/**** 290 mapper=4 prgrom=128K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x0AA1D73D, //  Bootleg Singles\Tecmo NBA Basketball.nes 
    0x70A5485F, //  Homebrew\Blurred Lines 2048 (2015-04).nes 
    0xBC571CA7, //  Homebrew\Blurred Lines 2048 (2016-03).nes 
    0x3491253E, //  Homebrew\Translations\English\Armed Dragon Fantasy Villgust [J2e Translations].nes 
    0xDFD4E833, //  Homebrew\Translations\English\Armed Dragon Fantasy Villgust [The Dungeon].nes 
    0x7947EEC6, //  Homebrew\Translations\English\Nakayoshi n' Me [HTI](DiC Sailor Moon dub).nes 
    0x143BF3DF, //  Homebrew\Translations\English\Nakayoshi n' Me [HTI].nes 
    0x7CC3A055, //  Homebrew\Translations\English\うしおととら꞉ 深淵の大妖 [Neophyte Productions v0.20].nes 
    0x75BC4325, //  Homebrew\Translations\English\大怪獣 Deburas [Manipulate v0.30].nes 
    0xF053AC5F, //  Licensed Japan\Déjà Vu.nes 
    0x50DA4867, //  Licensed Japan\Shadow Brain.nes 
    0xA58A8DA1, //  Licensed Japan\うしおととら꞉ 深淵の大妖.nes 
    0xFFE8507E, //  Licensed Japan\なかよしとおっしょ.nes 
    0x956E3D90, //  Licensed Japan\大怪獣 Deburas.nes 
    0x5318CDB9, //  Licensed Japan\甲竜伝説 Villgust 外伝.nes 
    0x43D01C10, //  Licensed North America\Déjà Vu.nes 
    0x2651F227, //  Licensed North America\Tecmo NBA Basketball (rev0).nes 
    0xDA8E4AF4, //  Licensed North America\Tecmo NBA Basketball (rev1).nes 
    0xB54BAEBE, //  Licensed North America\Tecmo NBA Basketball (rev2).nes 
/**** 291 mapper=4 prgrom=128K prgnvram=8K chrrom=256K mirror=H battery=1 system=SYSTEM_PAL */
    0x39D43261, //  Licensed PAL\Déjà Vu.nes 
/**** 292 mapper=4 prgrom=128K prgnvram=8K chrrom=256K mirror=H battery=1 system=SYSTEM_NTSC */
    0xB7CED210, //  Modern\Log Jammers.nes 
    0x616B55D1, //  Samples\LogJammers.nes 
/**** 293 mapper=4 prgrom=128K prgnvram=8K chrrom=256K mirror=H battery=1 system=SYSTEM_NTSC */
    0xA50124FF, //  Unlicensed China\外星\疯狂鸡蛋仔 - Flighty Chicken.nes 
    0x12B1C8F3, //  Unlicensed China\外星\魔道士阴谋.nes 
/**** 294 mapper=4 prgrom=256K prgnvram=8K chrram=8K mirror=H battery=1 */
    0xC1BA8BB9, //  Licensed Japan\Project Q.nes 
    0xBDC124E5, //  Licensed Japan\Shuffle Fight.nes 
/**** 295 mapper=4 prgrom=256K prgnvram=8K chrram=8K mirror=H battery=1 system=SYSTEM_NTSC */
    0xD78638EC, //  Unlicensed China\外星\勇者斗恶龙 - Dragon Quest IV.nes 
    0x29EB8331, //  Unlicensed China\外星\勇者斗惡龍 2 - Dragon Quest.nes 
    0xD3A269DC, //  Unlicensed China\外星\勇者斗惡龍 5 - Dragon Quest.nes 
    0x77EFB5D9, //  Unlicensed China\外星\龙珠英雄 - Dragongen Hero.nes 
    0xDFAD3F66, //  Unlicensed China\恒格\英雄源义经传奇.nes 
/**** 296 mapper=4 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x3F9524EC, //  Homebrew\Blade Buster (2010-09-28).nes 
    0x325C3E52, //  Homebrew\Blade Buster (2012-03-01).nes 
    0x37E0D156, //  Homebrew\Hacks\Super Mario Bros. series\New Super Mario Bros. 3 [Pacnsacdave, Silas].nes 
    0xA6EAE553, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 (SRAM v7.0)[infidelity].nes 
    0xF7A01F6A, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - 1972 - 1 week.nes 
    0xCF5992F1, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Ata Season.nes 
    0xE443042C, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Buck CPU 7.nes 
    0x87DFAADF, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Clock.nes 
    0x8020DC07, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - ETL 1 Week 3.nes 
    0x2211B1B4, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - ETL 1 Week 8.nes 
    0xAF6328C5, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - ETL 2_1 Week 3.nes 
    0x63DAAD78, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - ETL 3.nes 
    0x6878FA62, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - ETL 3_001.nes 
    0xFF452CE8, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - ETL Clock.nes 
    0xE25518B9, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - ETL.nes 
    0x66C9E55C, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - HSTLs16 Draft.nes 
    0x96FF9E96, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NCAA 2005 at Texas Memorial Stadium.nes 
    0x4EDA8CF9, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NCAA 2007.nes 
    0x58FA1272, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NCAA Alumni Edition Version 2.nes 
    0x48C6F262, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NCAA Alumni Edition Version 3.nes 
    0x92E07ED2, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 1978 Redux - Coach Mode Only.nes 
    0x29F5D7BE, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 1978 Redux.nes 
    0xDD475247, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 1991 Juice Mode 2.nes 
    0xA28E8201, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 1991 with Playbook.nes 
    0x5448F769, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 199X Tecmo Super Bowl - Greats.nes 
    0x4D89397C, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 95 Week 13.nes 
    0xD2147050, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 House Week 1.nes 
    0x4C34FA54, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 1.nes 
    0x1AD73CCD, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 10.nes 
    0x17F13E7D, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 11.nes 
    0x98CBC5D6, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 12.nes 
    0xA3A6936F, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 13.nes 
    0x81959AB1, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 14.nes 
    0x3CB1716D, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 2.nes 
    0x05B3310E, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 3.nes 
    0x482610A3, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 4.nes 
    0x01F8D4EF, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 5.nes 
    0x11E6F6CB, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 6.nes 
    0x021AEC3E, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 7.nes 
    0x30AD4402, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 8.nes 
    0x70CBAD26, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 96 Week 9.nes 
    0x7D5F54D1, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL Dynasties Version 4.nes 
    0x55BF474C, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL Steelers Edition.nes 
    0x7E9E6B6D, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL2 - Grapple - Preseason.nes 
    0x066FACF6, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Play 01.nes 
    0x393B0534, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Play 02.nes 
    0xC1AF4F60, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - X CFL Version 2 (No Roster Update).nes 
    0x66FBA5D1, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - X CIFL 2008.nes 
    0xAA488EC3, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl 2004-05 [EdibleAntiPerspirant].nes 
    0x1590F407, //  Homebrew\Translations\English\Chronicle of the Radia War [Dreamless &amp; Jair &amp; [cx]].nes 
    0xE3C99000, //  Homebrew\Translations\English\Dark Lord [Aeon Genesis].nes 
    0x6F210564, //  Homebrew\Translations\English\F1 Circus [MrRichard999 v1.0].nes 
    0x6C01631B, //  Homebrew\Translations\English\Rainbow Silkroad [Gaijin Productions v1.01, Killa B v1.1].nes 
    0x5D965B46, //  Homebrew\Translations\English\Rainbow Silkroad [Gaijin Productions v1.01].nes 
    0x1A1A3688, //  Homebrew\Translations\English\The Samurai Lord Musashi꞉ Gimmicks on the Run!! [Aishsha &amp; Stardust Crusaders].nes 
    0xA38D21F6, //  Homebrew\Translations\English\Ys II꞉ Ancient Ys Vanished - The Final Chapter [David Mullen].nes 
    0xBF7F54B4, //  Licensed Japan\Baseball Fighter.nes 
    0x3EFF62E4, //  Licensed Japan\Dark Lord.nes 
    0x87CE3F34, //  Licensed Japan\Dragon Wars.nes 
    0xECBF33CE, //  Licensed Japan\F1 Circus.nes 
    0x79698B98, //  Licensed Japan\God Slayer꞉ はるか天空のSonata.nes 
    0x20CC079D, //  Licensed Japan\Mother.nes 
    0x332C47E0, //  Licensed Japan\Radia 戦記꞉ 黎明篇.nes 
    0xEBCF8419, //  Licensed Japan\Sansara Naga.nes 
    0x022589B9, //  Licensed Japan\Tecmo Super Bowl.nes 
    0x4942BDA8, //  Licensed Japan\The Magic Candle.nes 
    0x923F915B, //  Licensed Japan\Ultraman Club 3.nes 
    0x5A0454F3, //  Licensed Japan\Ys II꞉ Ancient Ys Vanished - The Final Chapter.nes 
    0x4582F22E, //  Licensed Japan\おたくの星座꞉ An Adventure in the Otaku Galaxy.nes 
    0x8BCB0993, //  Licensed Japan\パチ夫くん 3 (rev0).nes 
    0xAFE03802, //  Licensed Japan\パチ夫くん 3 (rev1).nes 
    0x9B3C5124, //  Licensed Japan\パチ夫くん 5꞉ Jrの挑戦.nes 
    0x5D105C10, //  Licensed Japan\必殺仕事人.nes 
    0x5337F73C, //  Licensed Japan\虹のSilk Road꞉ Zig Zag 冒険記.nes 
    0x59449E3B, //  Licensed Japan\麻雀大戦.nes 
    0x18A9F0D9, //  Licensed North America\Baseball Stars II.nes 
    0x1335CB05, //  Licensed North America\Crystalis.nes 
    0x179A0D57, //  Licensed North America\Tecmo Super Bowl.nes 
    0xBBE393DB, //  Modern\Crystalis (SNK 40th Anniversary Collection).nes 
    0x3B95C155, //  Modern\God Slayer꞉ はるか天空のSonata (SNK 40th Anniversary Collection).nes 
    0xB3736AE3, //  Modern\Virtual Console\Mother.nes 
    0x340713DD, //  Unreleased\Crystalis.nes 
    0x13E09D7A, //  Unreleased\Dragon Wars.nes 
    0xDB2D4F9D, //  Unreleased\Tecmo Super Bowl.nes 
    0xD6190C63, //  Unreleased\Tower of Radia.nes 
/**** 297 mapper=4 prgrom=256K chrrom=256K mirror=H battery=1 */
    0xE674099E, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Double Jumper [Rock5easily 2015].nes 
/**** 298 mapper=4 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x40CDD563, //  Bad Dumps\魔道士阴谋 [PRG overdump].nes 
    0x846C9304, //  Compatibility Hacks\林則徐禁烟 [invincibility][SMD132 lockout].nes 
    0xE042519C, //  Homebrew\Creepy Brawlers.nes 
    0x1640420A, //  Homebrew\Hacks\Cooey Found Weed.nes 
    0xF0473130, //  Homebrew\Hacks\Mega Man series\Mega Man Ultimate+Save [RetroRain].nes 
    0xEE8D1754, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Bluesの罠!？ - Endless (Laser Areas Omitted).nes 
    0x2D903030, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Bluesの罠!？ - Endless (alt).nes 
    0x3FDABFCA, //  Homebrew\Hacks\Mega Man series\Rockman 5꞉ Bluesの罠!？ - Endless.nes 
    0x0BBFF5CB, //  Homebrew\Hacks\Super Mario Bros. series\Mario Adventure (compatible with real hardware)[DahrkDaiz].nes 
    0x87F4FA66, //  Homebrew\Hacks\Super Mario Bros. series\Mario in꞉ Some Usual Day [Jasp].nes 
    0xC5F7C8E2, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Cardinals At Cowboys.nes 
    0x67A6B919, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2006 32 Teams.nes 
    0x498BBACA, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2007 32 Teams Version 2.nes 
    0xDD375E36, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2007 32 Teams Version 3.nes 
    0xF3EEDD72, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2007 32 Teams.nes 
    0x68829B79, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2007.nes 
    0x544079E4, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008 Super Coach.nes 
    0xC3E2F6C9, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008 Version 2 (Graphic Updates).nes 
    0xFD32C75E, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008 Version 3 (Conservative).nes 
    0x3BC0266D, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008 Version 4 (No JJs) Juiced.nes 
    0x0607E9F3, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008 Version 4 (No JJs).nes 
    0xD8E1FDD8, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008 Version 5.nes 
    0x6EE28072, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008 XLIII.nes 
    0xF9F84B03, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2008.nes 
    0xE401ABD4, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2009 Beta Alternate Uniforms.nes 
    0xD0C065B6, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2009 Beta Juiced.nes 
    0xD724712F, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2009 Beta.nes 
    0xFE09A3E6, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2009 Super Coach.nes 
    0x8FA51743, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL Dynasties Version 5 (Time Machine).nes 
    0xEA3D2860, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL - Preseason - Alignment.nes 
    0xEE157F14, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL Preseason5.nes 
    0xE949E031, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL- 041200.nes 
    0xABB95CBF, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL- 1- Preseason.nes 
    0x32523EB2, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL4 - Preseason 1 (Fixed).nes 
    0x02A13CC9, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL4 - Preseason 3.nes 
    0x3983B183, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL4 Test Onebacks and Rollouts - RS.nes 
    0x8BA0FED3, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NTL4 Test Onebacks and Rollouts.nes 
    0xE14F002B, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Play 03.nes 
    0x369779B2, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Play 04.nes 
    0x921355FA, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Play 05.nes 
    0xE682C4AA, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Play 06.nes 
    0x9F0BB8BD, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Play 3 P01.nes 
    0xD48B5D7D, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 01.nes 
    0x46F105E0, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 02.nes 
    0xF0D54443, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 03.nes 
    0x4CA22308, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 04.nes 
    0x74DB96B3, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 05.nes 
    0x4D3B570D, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 06.nes 
    0xBF2143C7, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 07.nes 
    0x1523BC69, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 08.nes 
    0xFC8657D8, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 09.nes 
    0xB6566600, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 10.nes 
    0xE1EA2167, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 11.nes 
    0x38EFA136, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 12.nes 
    0x1CBB5703, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 13.nes 
    0x2EC15C42, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - Week 14.nes 
    0x2583D8D3, //  Homebrew\Hacks\Wario's Woods - Improved [PKMNwww411].nes 
    0x783146B2, //  Homebrew\Translations\English\Armed Dragon Fantasy Villgust [cccmar].nes 
    0x2DFE0EEC, //  Homebrew\Translations\English\Joy Mech Fight [AlanMidas joy.mech].nes 
    0x1F07DDC1, //  Homebrew\Translations\English\SD Gundam - Gachapon Senshi 3꞉ Hero Warrior [MrRichard999].nes 
    0xFC52B870, //  Homebrew\Translations\English\SD Gundam - Gachapon Soldier 4꞉ NewType Story [MrRichard999 &amp; Proveaux v1.01].nes 
    0x9F0FB1CD, //  Homebrew\Translations\English\Shadow Brain [Aishsha &amp; Stardust Crusaders, MrRichard999].nes 
    0xA840FD50, //  Homebrew\Translations\English\Shadow Brain [Aishsha &amp; Stardust Crusaders].nes 
    0xE17B6E7C, //  Homebrew\Translations\English\Super Robot Wars 2 [Aeon Genesis](Gallant Char)(Music Persistence).nes 
    0xCF90DF9D, //  Homebrew\Translations\English\Super Robot Wars 2 [Aeon Genesis](Gallant Char).nes 
    0xEC216AA7, //  Homebrew\Translations\English\Super Robot Wars 2 [Aeon Genesis](Music Persistence).nes 
    0xC2CADB46, //  Homebrew\Translations\English\Super Robot Wars 2 [Aeon Genesis].nes 
    0x708EA2BE, //  Licensed Japan\Joy Mech Fight.nes 
    0x9509F703, //  Licensed Japan\Metal Max.nes 
    0x9BDCD892, //  Licensed Japan\Might and Magic꞉ Secret of the Inner Sanctum.nes 
    0x26049798, //  Licensed Japan\My Life, My Love - ぼくの夢わたしの願い.nes 
    0x67555417, //  Licensed Japan\SD Gundam ガチャポン戦士 4꞉ New Type Story.nes 
    0x900E3A23, //  Licensed Japan\Silva Saga.nes 
    0x03E2898F, //  Licensed Japan\Warioの森.nes 
    0xC48363B4, //  Licensed Japan\第2次 Super Robot 大戦.nes 
    0x532A27E6, //  Licensed North America\Might and Magic꞉ Secret of the Inner Sanctum.nes 
    0xF79A75D7, //  Licensed North America\Wario's Woods.nes 
    0x53A9E2BA, //  Modern\EarthBound Beginnings.nes 
    0x4F6FBA72, //  Modern\Virtual Console\Joy Mech Fight.nes 
    0x7FEAE646, //  Modern\Virtual Console\Metal Max.nes 
    0x3823BDD1, //  Modern\キラキラ Star Night AC.nes 
    0xC0E5B3A8, //  Modern\キラキラ Star Night DX (rev0).nes 
    0x9976C419, //  Modern\キラキラ Star Night DX (rev1).nes 
    0x7414DFD8, //  Modern\キラキラ Star Night DX Tournament Edition.nes 
    0xE1879278, //  Plug-and-Play\extracts\International Cricket.nes 
    0x538FFC02, //  Samples\Creepy Brawlers.nes 
/**** 299 mapper=4 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 system=SYSTEM_PAL */
    0x668D1715, //  Licensed PAL\Wario's Woods.nes 
/**** 300 mapper=4 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 system=SYSTEM_NTSC */
    0xFC41249E, //  Modern\Little Medusa.nes 
    0x19074D1D, //  Samples\Little Medusa.nes 
/**** 301 mapper=4 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 system=SYSTEM_NTSC */
    0x6CEDCA3D, //  Bad Dumps\侠客情꞉ 荆轲刺秦王 (TKROM)[dumper graffiti].nes 
    0x70026D3F, //  Bad Dumps\匹诺槽的复苏 [dumper graffiti].nes 
    0x520E93D3, //  Bad Dumps\帝国风暴 - Napoleon's War (980100029)[dumper graffiti].nes 
    0xCAEAEE6A, //  Bad Dumps\魔域英雄传 - Hero on Devil Lands (TKROM)[dumper graffiti].nes 
    0xF8690050, //  Bad Dumps\魔神法师 - Demogorgon Monk (TKROM)[dumper graffiti].nes 
    0x87EA86FD, //  Compatibility Hacks\三十六計 - The Thirty-Six Tricks (970158)[m004].nes 
    0x0E3B1D39, //  Compatibility Hacks\夺宝小英豪꞉ 光明與黑暗傳説 [m004].nes 
    0xE13399EC, //  Compatibility Hacks\林則徐禁烟 [SMD132 lockout].nes 
    0x1B503BC5, //  Compatibility Hacks\林則徐禁烟 [invincibility].nes 
    0x507982E1, //  Compatibility Hacks\楚留香꞉ 香帥傳奇之血海飘零 [m004].nes 
    0x352B93D6, //  Compatibility Hacks\水滸傳 - Marsh Outlaws [m004].nes 
    0xE905A251, //  Compatibility Hacks\魔法门之英雄无敌 (980100295)[m004].nes 
    0xF31D8F5D, //  Homebrew\Original Mapper Restorations\絶代英雄 - Unrivalled Hero.nes 
    0x729EF1E3, //  Homebrew\Translations\English\Lin Zexu No Smoking [Pacnsacdave].nes 
    0x9CDE3036, //  Homebrew\Translations\English\The Recovery of Pinocchio [Pacnsacdave].nes 
    0x1CD1EAE7, //  Homebrew\Translations\English\The Wizard of Oz [Pacnsacdave].nes 
    0x48EDE7F6, //  Unlicensed China\外星\东周列国志 (960100577).nes 
    0x116E74DB, //  Unlicensed China\外星\争霸世纪.nes 
    0x0DF38D2B, //  Unlicensed China\外星\侠客情꞉ 荆轲刺秦王 (TKROM).nes 
    0x22A5D663, //  Unlicensed China\外星\匹诺槽的复苏.nes 
    0x655859DA, //  Unlicensed China\外星\地道戰 (970266).nes 
    0x426BD504, //  Unlicensed China\外星\地道戰.nes 
    0xA58D7260, //  Unlicensed China\外星\基督山恩仇记 - Le Comte de Monte Cristo (0100551).nes 
    0x5DC85420, //  Unlicensed China\外星\帝国风暴 - Napoleon's War (980100029).nes 
    0x991E2B2B, //  Unlicensed China\外星\快乐比奇III꞉ 地球战士 (970163).nes 
    0x08AB020E, //  Unlicensed China\外星\戰國群雄傳.nes 
    0x5B613ED0, //  Unlicensed China\外星\拯救世紀꞉ 光明之神 (2006SR04303).nes 
    0x82605520, //  Unlicensed China\外星\拯救世紀꞉ 光明之神.nes 
    0x4E55149D, //  Unlicensed China\外星\杨家将 - Yang's Troops (970100588).nes 
    0x7E0F312D, //  Unlicensed China\外星\林則徐禁烟.nes 
    0xB9AF1DA3, //  Unlicensed China\外星\楚漢爭霸 - The War Between Chu &amp; Han (0100396).nes 
    0xCE6F0CAE, //  Unlicensed China\外星\欧陆之战.nes 
    0x1104A5B9, //  Unlicensed China\外星\緑野仙踪 - Fairy Land (960100549).nes 
    0xE9B3E837, //  Unlicensed China\外星\緑野仙踪 - Fairy Land (970307).nes 
    0x09740409, //  Unlicensed China\外星\隋唐演义 (970100587).nes 
    0xD82AE4A7, //  Unlicensed China\外星\魔域英雄传 - Hero on Devil Lands (TKROM).nes 
    0x21444EDA, //  Unlicensed China\外星\魔神法师 - Demogorgon Monk (TKROM).nes 
    0x79CF3C0E, //  Unlicensed China\振华\财神到.nes 
    0x7AE3E007, //  Unlicensed China\晶科泰\三国群英传.nes 
    0xFBDAF6EF, //  Unlicensed China\晶科泰\魔幻三国志.nes 
    0x20D5BA91, //  Unlicensed China\福州 Coolboy\光明之神.nes 
    0x58B57FFD, //  Unlicensed China\福州 Coolboy\虎门硝烟.nes 
    0x0C87BBF5, //  Unlicensed China\红楼梦.nes 
/**** 302 mapper=4 prgrom=256K prgnvram=16K chrrom=256K mirror=H battery=1 system=SYSTEM_NTSC */
    0xE92A3B98, //  Homebrew\Translations\English\Tunnel War [Pacnsacdave].nes 
/**** 303 mapper=4 prgrom=512K prgnvram=8K chrram=8K mirror=H battery=1 */
    0x1568198B, //  Homebrew\Hacks\Mega Man series\Mega Man CX [pacnsacdave, Himajin].nes 
    0x4FBAB3E1, //  Homebrew\Hacks\Mega Man series\Rockman 4꞉ 新たなる野望!! - Endless.nes 
    0x2D2C99BF, //  Homebrew\Translations\English\Destiny of an Emperor II꞉ The Story of Zhuge Liang [Dark Force, Dynamic Designs, Jair, Taskforce v1.12].nes 
    0x3413789E, //  Homebrew\Translations\English\Final Fantasy III [Alex W. Jackson, Neill Corlett, SoM2Freak v1.1].nes 
    0x290DF36D, //  Homebrew\Translations\English\Final Fantasy III [Chaos Rush 2019 v1.3+Font Patch].nes 
    0xDB15AAE8, //  Homebrew\Translations\English\Final Fantasy III [Chaos Rush 2019 v1.3].nes 
    0xE383403A, //  Homebrew\Translations\English\The Legend of Double Moon [Aishsha, Stardust Crusaders v1.01].nes 
    0x83EA7B04, //  Licensed Japan\Battle Baseball.nes 
    0xA60FBA51, //  Licensed Japan\Double Moon 伝説.nes 
    0x57E220D0, //  Licensed Japan\Final Fantasy III.nes 
    0x8EEF8B76, //  Licensed Japan\Last Armageddon.nes 
    0xE08C8A60, //  Licensed Japan\パチ夫くん 4.nes 
    0xCB32E243, //  Licensed Japan\天地を喰らう II꞉ 諸葛孔明伝 (rev0).nes 
    0xB84A73CC, //  Licensed Japan\天地を喰らう II꞉ 諸葛孔明伝 (rev1).nes 
    0x9C58F4A6, //  Licensed Japan\桃太郎伝説外伝.nes 
    0x85BC0777, //  Licensed Japan\麻雀 Club 永田町.nes 
    0x6313EBC7, //  Modern\Virtual Console\Final Fantasy III (rev0).nes 
    0xACAEEE0A, //  Modern\Virtual Console\Final Fantasy III (rev1).nes 
    0x00890F1E, //  Modern\Virtual Console\Final Fantasy III (rev2).nes 
/**** 304 mapper=4 prgrom=512K prgnvram=8K chrram=8K mirror=H battery=1 system=SYSTEM_NTSC */
    0x33B43FA0, //  Bad Dumps\EverQuest - 无尽的任务 [dumper graffiti].nes 
    0x30284404, //  Bad Dumps\创世纪英雄 [dumper graffiti].nes 
    0x6BEA1235, //  Compatibility Hacks\勇者斗恶龙 - Dragon Quest VI [m004].nes 
    0x663F3912, //  Unlicensed China\外星\Age of Empires - 帝国时代 (0100725).nes 
    0xA39543FB, //  Unlicensed China\外星\EverQuest - 无尽的任务.nes 
    0xDA07391E, //  Unlicensed China\外星\三國志꞉ 英杰傳 (Simplified Chinese).nes 
    0xF2072ABE, //  Unlicensed China\外星\三國志꞉ 英杰傳 (Traditional Chinese).nes 
    0x0F8900E4, //  Unlicensed China\外星\创世纪英雄.nes 
    0xF4445037, //  Unlicensed China\外星\宇宙戰將 - Space General.nes 
    0x69E13748, //  Unlicensed China\外星\幽灵列车.nes 
    0x7355F8E3, //  Unlicensed China\外星\荣誉勋章.nes 
    0x40E9255A, //  Unlicensed China\外星\银河时代.nes 
    0xB5E9E394, //  Unlicensed China\福州 Coolboy\列车奇案.nes 
    0x6C935F22, //  Unlicensed China\福州 Coolboy\吞食天地.nes 
/**** 305 mapper=4 prgrom=512K prgnvram=8K chrrom=8K mirror=H battery=1 */
    0x4C9824C4, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 2 with Super Mario Bros. Characters.nes 
/**** 306 mapper=4 prgrom=512K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x675619AC, //  Homebrew\Translations\English\God Slayer꞉ Sonata of the Distant Heavens [Stardust Crusaders v1.02].nes 
    0x06539EF4, //  Homebrew\Translations\English\Rainbow Silkroad [Aishsha &amp; Stardust Crusaders].nes 
    0x1AA81F17, //  Homebrew\Translations\English\Sansara Naga [Aishsha &amp; Stardust Crusaders v1.01].nes 
    0x5ABB947D, //  Homebrew\Translations\English\The Constellation Nerdulus꞉ An Adventure in the Otaku Galaxy (v1.01)[Stardust Crusaders].nes 
    0xEBBB8C92, //  Homebrew\Translations\English\The Magic Candle [Aishsha, Stardust Crusaders].nes 
    0xCA730971, //  Licensed Japan\Advanced Dungeons &amp; Dragons꞉ Pool of Radiance.nes 
    0x25952141, //  Licensed North America\Advanced Dungeons &amp; Dragons꞉ Pool of Radiance.nes 
/**** 307 mapper=4 prgrom=512K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x8638208B, //  Homebrew\Hacks\Deadpool (2018)[pacnsacdave].nes 
    0x47B758CD, //  Homebrew\Hacks\Deadpool (2019)[pacnsacdave].nes 
    0x09BB565A, //  Homebrew\Hacks\Deadpool - Final [Techmoon].nes 
    0x6B300CEF, //  Homebrew\Hacks\Deadpool - Hardcore Edition.nes 
    0x85534EE9, //  Homebrew\Hacks\Deadpool - Right back at you, buddy Edition [Techmoon].nes 
    0xD6985B26, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 Mix (v2b)[sonicepoch].nes 
    0xB8CCBF29, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Ultimate (v1.1)[Silas].nes 
    0xFC0D3A2C, //  Homebrew\Hacks\Tecmo Bowl series\Tecmo Super Bowl - NFL 2007 32 Teams UPDATE.nes 
    0xB483F704, //  Homebrew\Translations\English\Metal Max [Supper].nes 
    0xE4F8B4FE, //  Homebrew\Translations\English\Silva Saga [aishsha v1.01].nes 
    0xE4A7D436, //  Licensed Japan\星のKirby꞉ 夢の泉の物語.nes 
    0xD7794AFC, //  Licensed North America\Kirby's Adventure (English rev0).nes 
    0x5ED6F221, //  Licensed North America\Kirby's Adventure (English rev1).nes 
    0x37088EFF, //  Licensed North America\Kirby's Adventure (French).nes 
    0xC52D10AB, //  Modern\8-Bit Adv Steins;Gate.nes 
    0x95D7B116, //  Modern\Astro Ninja Man.nes 
/**** 308 mapper=4 prgrom=512K prgnvram=8K chrrom=256K mirror=H battery=1 system=SYSTEM_PAL */
    0x2C088DC5, //  Licensed PAL\Kirby's Adventure (English).nes 
    0xB2EF7F4B, //  Licensed PAL\Kirby's Adventure (French).nes 
    0x127D76F4, //  Licensed PAL\Kirby's Adventure (German).nes 
/**** 309 mapper=4 prgrom=1024K prgnvram=8K chrram=8K mirror=H battery=1 */
    0xF011AFD6, //  Homebrew\Hacks\Mega Man series\Rockman 4- Minus Infinity [Infinite Life].nes 
/**** 310 mapper=4 submapper=1 prgrom=256K prgnvram=1K chrrom=256K mirror=H battery=1 */
    0xFCEDB11A, //  Homebrew\Bugfixes, Improvements\Startropics [music synchronization].nes 
    0xD054FFB0, //  Licensed North America\Startropics II꞉ Zoda's Revenge.nes 
    0x889129CB, //  Licensed North America\Startropics.nes 
    0xE7B34C02, //  Modern\Virtual Console\StarTropics (NTSC).nes 
    0x33751782, //  Modern\Virtual Console\Startropics II꞉ Zoda's Revenge.nes 
/**** 311 mapper=4 submapper=1 prgrom=256K prgnvram=1K chrrom=256K mirror=H battery=1 system=SYSTEM_PAL */
    0x998422FC, //  Licensed PAL\Startropics.nes 
    0xF6A64735, //  Modern\Virtual Console\StarTropics (PAL).nes 
/**** 312 mapper=4 submapper=2 prgrom=256K chrrom=64K mirror=H */
    0x282DA88C, //  Homebrew\Translations\English\Detective Sanma [Ccmar].nes 
/**** 313 mapper=4 submapper=2 prgrom=256K prgnvram=8K chrrom=64K mirror=V battery=1 */
    0x9AE7B722, //  Homebrew\Translations\English\Debias꞉ Legendary Space-Time Hero [Gil Galad].nes 
/**** 314 mapper=4 submapper=3 prgrom=128K chrrom=128K mirror=H */
    0x292DF6A2, //  Compatibility Hacks\Everdrive N8\Terminator 2 - Judgment Day [RAM initialization].nes 
    0x25D006F2, //  Homebrew\Hacks\Alien 3 - Lomax Edition.nes 
    0xC527C297, //  Licensed North America\Alien 3.nes 
    0x982DFB38, //  Licensed North America\Mickey's Safari in Letterland.nes 
    0x2370C0A9, //  Licensed North America\Rollerblade Racer.nes 
    0x7E57FBEC, //  Licensed North America\T&amp;C Surf Designs II꞉ Thrilla's Surfari.nes 
    0xEA27B477, //  Licensed North America\Terminator 2꞉ Judgment Day.nes 
/**** 315 mapper=4 submapper=3 prgrom=128K chrrom=256K mirror=H */
    0xAF05F37E, //  Licensed North America\George Foreman's KO Boxing.nes 
    0x018A8699, //  Licensed North America\Roger Clemens' MVP Baseball (rev0).nes 
    0x394D6E2F, //  Licensed North America\Roger Clemens' MVP Baseball (rev1).nes 
    0xD679627A, //  Licensed North America\Spider-Man꞉ Return of the Sinister Six.nes 
    0xA80A0F01, //  Licensed North America\The Incredible Crash Dummies.nes 
    0x7B4ED0BB, //  Licensed North America\WWF King of the Ring.nes 
    0xD4611B79, //  Licensed North America\WWF WrestleMania Steel Cage Challenge.nes 
    0xB4FDE978, //  Unreleased\The Incredible Crash Dummies.nes 
/**** 316 mapper=4 submapper=3 prgrom=256K chrrom=128K mirror=H */
    0x7416903F, //  Licensed North America\The Simpsons꞉ Bart vs. the World.nes 
    0x5991B9D0, //  Licensed North America\The Simpsons꞉ Bartman Meets Radioactive Man.nes 
/**** 317 mapper=4 submapper=4 prgrom=256K chrrom=256K mirror=H */
    0x97B6CB19, //  Bootleg Singles\Aladdin (A-N8).nes 
/**** 318 mapper=5 prgrom=16K chrrom=8K chrram=8K mirror=H */
    0x3CBCFC54, //  BIOS\Demo Boy 2.nes 
/**** 319 mapper=5 prgrom=16K chrrom=16K mirror=H */
    0x5A265CC1, //  Homebrew\Demos\Cat.nes 
/**** 320 mapper=5 prgrom=32K prgram=8K chrrom=32K mirror=H */
    0xCC653CCE, //  Homebrew\Demos\Multi-Layered Scrolling Demo.nes 
/**** 321 mapper=5 prgrom=64K chrrom=8K mirror=H */
    0xB93B1326, //  Homebrew\Tests\Color Test.nes 
/**** 322 mapper=5 prgrom=128K prgram=8K chrrom=64K mirror=H */
    0x24BB055E, //  Compatibility Hacks\Impossible Mission 2 [m005].nes 
/**** 323 mapper=5 prgrom=128K chrrom=128K mirror=H */
    0x0AFB395E, //  Licensed Japan\Gun Sight.nes 
    0xBB7F829A, //  Licensed Japan\宇宙警備隊 SDF.nes 
    0xB0480AE9, //  Licensed North America\Laser Invasion.nes 
/**** 324 mapper=5 prgrom=256K chrrom=128K mirror=H */
    0xC4EE22BA, //  Homebrew\Hacks\Castlevania series\Castlevania III꞉ Dracula's Curse - All Bugfixes.nes 
    0xF2A59A7C, //  Homebrew\Hacks\Castlevania series\Castlevania III꞉ Dracula's Curse - Evgeny Edition.nes 
    0x2A2D83D5, //  Homebrew\Hacks\Castlevania series\Castlevania III꞉ Dracula's Curse - Uncensored Edition.nes 
    0xED2465BE, //  Licensed North America\Castlevania III꞉ Dracula's Curse.nes 
    0x3095A71C, //  Modern\Castlevania III꞉ Dracula's Curse (Castlevania Anniversary Collection).nes 
    0xCD4E7430, //  Modern\Castlevania III꞉ Dracula's Curse (Konami Collector's Series).nes 
    0xE7015A29, //  Modern\Virtual Console\Castlevania III꞉ Dracula's Curse (NTSC).nes 
/**** 325 mapper=5 prgrom=256K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x671F23A8, //  Licensed PAL\Castlevania III꞉ Dracula's Curse.nes 
    0x04C2DC72, //  Modern\Virtual Console\Castlevania III꞉ Dracula's Curse (PAL).nes 
/**** 326 mapper=5 prgrom=512K chrram=8K mirror=H */
    0xB7A640AA, //  Homebrew\Hacks\Mega Man series\Mega Man Origins [RetroRain].nes 
/**** 327 mapper=5 prgrom=512K chrrom=256K mirror=H */
    0xD8CF3573, //  Compatibility Hacks\Caltron 9-in-1 [m005].nes 
    0xED53266B, //  Unreleased\Caltron 9-in-1 [m005 WIP].nes 
/**** 328 mapper=5 prgrom=512K chrrom=512K mirror=H */
    0xB4735FAC, //  Licensed Japan\Metal Slader Glory.nes 
    0x51D4B441, //  Modern\Virtual Console\Metal Slader Glory.nes 
/**** 329 mapper=5 prgrom=512K chrrom=512K mirror=H system=SYSTEM_NTSC */
    0xB1C7EB21, //  Samples\Street Fighter II Nostalgic Edition.nes 
/**** 330 mapper=5 prgrom=1024K prgram=8K chrrom=256K mirror=H */
    0x3A49CF82, //  Homebrew\Hacks\Super Mario Bros. series\Reuben.nes 
    0xE08EE48C, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. 3 Raeneske.nes 
/**** 331 mapper=5 prgrom=1024K chrrom=512K mirror=H */
    0xC4FE19F7, //  Homebrew\Translations\English\Metal Slader Glory [Stardust Crusaders].nes 
/**** 332 mapper=5 prgrom=1024K chrrom=520K mirror=H */
    0x186BB7BF, //  Homebrew\Translations\English\Metal Slader Glory [FCandChill, Her-Saki v1.1].nes 
/**** 333 mapper=5 prgrom=1024K prgram=8K chrrom=1024K mirror=H */
    0xD9B1A41F, //  Homebrew\Hacks\44 Balloons꞉ The Balloon Fight Collection.nes 
/**** 334 mapper=5 prgrom=1024K prgram=8K chrrom=1024K mirror=H system=SYSTEM_NTSC */
    0xAC9321EF, //  Homebrew\Hacks\Ultimate Mortal Kombat 3 (early).nes 
    0xE2FD1490, //  Homebrew\Hacks\Ultimate Mortal Kombat 3 (final).nes 
/**** 335 mapper=5 prgrom=128K prgram=8K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xE5584D9F, //  Homebrew\Bugfixes, Improvements\Sim City [Санчес 2018-11-06].nes 
    0xFDC7C50B, //  Unreleased\Sim City.nes 
/**** 336 mapper=5 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xD532E98F, //  Licensed Japan\新 4人打ち麻雀꞉ 役満天国.nes 
/**** 337 mapper=5 prgrom=256K prgram=8K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x6396B988, //  Licensed Japan\L'Empereur.nes 
    0xEEE9A682, //  Licensed Japan\信長の野望꞉ 戦国群雄伝 (rev0).nes 
    0xF9B4240F, //  Licensed Japan\信長の野望꞉ 戦国群雄伝 (rev1).nes 
    0x39F2CE4B, //  Licensed Japan\水滸伝꞉ 天命の誓い.nes 
    0x1CED086F, //  Licensed Japan\維新の嵐.nes 
    0x15FE6D0F, //  Licensed North America\Bandit Kings of Ancient China.nes 
    0x9C18762B, //  Licensed North America\L'Empereur.nes 
    0x8CE478DB, //  Licensed North America\Nobunaga's Ambition 2.nes 
/**** 338 mapper=5 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0xBC80FB52, //  Licensed Japan\Royal Blood.nes 
    0x0EC6C023, //  Licensed North America\Gemfire.nes 
/**** 339 mapper=5 prgrom=256K prgnvram=32K chrrom=256K mirror=H battery=1 */
    0x184C2124, //  Licensed Japan\三國志 II (rev0).nes 
    0xEE8E6553, //  Licensed Japan\三國志 II (rev1).nes 
    0xF011E490, //  Licensed North America\Romance of the Three Kingdoms 2.nes 
/**** 340 mapper=5 prgrom=512K prgram=8K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xFE3488D1, //  Licensed Japan\大航海時代.nes 
    0xACA15643, //  Licensed North America\Uncharted Waters.nes 
/**** 341 mapper=5 prgrom=512K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x735528D8, //  Homebrew\Translations\English\Just Breed [Stealth Translations v1.00].nes 
    0x9CBADC25, //  Licensed Japan\Just Breed.nes 
/**** 342 mapper=5 prgrom=512K prgnvram=32K chrrom=256K mirror=H battery=1 */
    0xF540677B, //  Licensed Japan\信長の野望꞉ 武将風雲録 (rev0).nes 
    0xA1617DFA, //  Licensed Japan\信長の野望꞉ 武将風雲録 (rev1).nes 
    0x6F4E4312, //  Licensed Japan\蒼き狼と白き牝鹿꞉ 元朝秘史.nes 
/**** 343 mapper=5 prgrom=1024K prgnvram=8K chrram=8K mirror=H battery=1 */
    0x6822DE93, //  Homebrew\Hacks\Mega Man series\Rockman 4 Minus Infinity (v1.01).nes 
    0xD7DBEFDE, //  Homebrew\Translations\English\Final Fantasy III [Chaos Rush 2016 v1.3].nes 
/**** 344 mapper=5 prgrom=1024K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x8DDD7754, //  Homebrew\Hacks\Teenage Mutant Ninja Turtles - Cowabunga Edition Ultimate [Maggiore, Rockman].nes 
    0x6DC89DF1, //  Homebrew\Hacks\Teenage Mutant Ninja Turtles - Cowabunga Edition [Retrorain].nes 
    0x83E68F3F, //  Homebrew\Hacks\Teenage Mutant Ninja Turtles꞉ Return to New York [guitarpalooz].nes 
/**** 345 mapper=5 prgrom=1024K prgnvram=32K chrrom=1024K mirror=H battery=1 */
    0xD430A9EF, //  Homebrew\Hacks\Zelda series\Zelda꞉ The Legend of Link (2018-12-23).nes 
    0x6FC02929, //  Homebrew\Multicarts\Super Mario All-Stars NES (cartridge).nes 
    0x06143FB3, //  Homebrew\Multicarts\Super Mario All-Stars NES (emulator).nes 
/**** 346 mapper=7 prgrom=128K chrram=8K mirror=H */
    0xC2073301, //  Homebrew\Hacks\Captain Skyhawk - Lomax Edition.nes 
    0x8F3E25F9, //  Homebrew\The Alphabet.nes 
/**** 347 mapper=7 prgrom=256K chrram=8K mirror=H */
    0x3AF644B6, //  Homebrew\Demos\MOON8 - Dark Side of the Moon (Pink Floyd).nes 
/**** 348 mapper=7 submapper=1 prgrom=64K chrram=8K mirror=H */
    0xDD062F9C, //  Licensed North America\R.C. Pro-Am (rev0).nes 
/**** 349 mapper=7 submapper=1 prgrom=64K chrram=8K mirror=H system=SYSTEM_PAL */
    0xE5A972BE, //  Licensed PAL\R.C. Pro-Am (rev0).nes 
/**** 350 mapper=7 submapper=1 prgrom=128K chrram=8K mirror=H */
    0xC8AD4F32, //  Licensed North America\Cobra Triangle.nes 
    0xB5D10D5C, //  Licensed North America\Jeopardy! (rev0).nes 
    0x2A662AC7, //  Licensed North America\Jeopardy! (rev1).nes 
    0x0BDD8DD9, //  Licensed North America\Jeopardy! 25th Anniversary Edition.nes 
    0x6A88579F, //  Licensed North America\Jeopardy! Junior Edition.nes 
    0x09874777, //  Licensed North America\Marble Madness.nes 
    0xEDCF1B71, //  Licensed North America\Solstice꞉ The Quest for the Staff of Demnos.nes 
    0x37138039, //  Licensed North America\WWF WrestleMania.nes 
    0xFBF8A785, //  Licensed North America\Wheel of Fortune (rev0).nes 
    0x3368F7FB, //  Licensed North America\Wheel of Fortune (rev1).nes 
    0x009AF6BE, //  Licensed North America\Wheel of Fortune Family Edition.nes 
    0x68383607, //  Licensed North America\Wheel of Fortune Junior Edition.nes 
    0x4220C170, //  Licensed North America\Wheel of Fortune Starring Vanna White.nes 
    0x12B2C361, //  Licensed North America\Who Framed Roger Rabbit？.nes 
    0x505F9715, //  Licensed North America\Wizards &amp; Warriors (rev0).nes 
    0x26535EF5, //  Licensed North America\Wizards &amp; Warriors (rev1).nes 
    0xF613A8F9, //  Licensed North America\World Games.nes 
    0x5DC9BC41, //  Unreleased\Solstice꞉ The Quest for the Staff of Demnos.nes 
    0xA8B1D1D1, //  Unreleased\The Tale꞉ Elrondの英雄.nes 
/**** 351 mapper=7 submapper=1 prgrom=128K chrram=8K mirror=H system=SYSTEM_PAL */
    0x898E4232, //  Licensed PAL\Cobra Triangle.nes 
    0x51BF28AF, //  Licensed PAL\Marble Madness.nes 
    0x7CB0D70D, //  Licensed PAL\Solstice꞉ The Quest for the Staff of Demnos.nes 
    0x6C4A9735, //  Licensed PAL\WWF WrestleMania.nes 
    0x5E6D9975, //  Licensed PAL\Wizards &amp; Warriors.nes 
/**** 352 mapper=7 submapper=1 prgrom=256K chrram=8K mirror=H */
    0x670C900A, //  Homebrew\Bugfixes, Improvements\Ironsword꞉ Wizards &amp; Warriors II (NTSC)[status bar position, noise channel][NewRisingSun].nes 
    0x2B378D11, //  Licensed North America\Double Dare.nes 
    0x2328046E, //  Licensed North America\Ironsword꞉ Wizards &amp; Warriors II.nes 
/**** 353 mapper=7 submapper=1 prgrom=256K chrram=8K mirror=H system=SYSTEM_PAL */
    0x3F8C48FF, //  Homebrew\Bugfixes, Improvements\Ironsword꞉ Wizards &amp; Warriors II (PAL)[status bar position, noise channel][NewRisingSun].nes 
    0x694C801F, //  Licensed PAL\Ironsword꞉ Wizards &amp; Warriors II.nes 
/**** 354 mapper=7 submapper=2 prgrom=64K chrram=8K mirror=H system=SYSTEM_NTSC */
    0x106180D6, //  Samples\Super NeSnake 2.nes 
/**** 355 mapper=7 submapper=2 prgrom=128K chrram=8K mirror=H */
    0xEB88D662, //  Homebrew\Translations\Russian\NARC.nes 
    0xA91460B8, //  Licensed Japan\Solstice꞉ 三次元迷宮の狂獣.nes 
    0xA72FDE03, //  Licensed Japan\伝説の騎士 Elrond.nes 
    0xDA2CB59A, //  Licensed North America\A Nightmare on Elm Street.nes 
    0xC740EB46, //  Licensed North America\Arch Rivals꞉ A Basketbrawl!.nes 
    0xEB84C54C, //  Licensed North America\Beetlejuice.nes 
    0xBDF046EF, //  Licensed North America\Cabal.nes 
    0xF518DD58, //  Licensed North America\Captain Skyhawk (rev0).nes 
    0xEFD26E37, //  Licensed North America\Captain Skyhawk (rev1).nes 
    0xC1B43207, //  Licensed North America\Danny Sullivan's Indy Heat.nes 
    0x1CEE0C21, //  Licensed North America\Digger T. Rock꞉ The Legend of the Lost City.nes 
    0x4B041B6B, //  Licensed North America\Ivan ''Ironman'' Stewart's Super Off-Road.nes 
    0x0537322A, //  Licensed North America\NARC.nes 
    0x13D5B1A4, //  Licensed North America\Time Lord.nes 
    0x1248895C, //  Playchoice\Captain Skyhawk.nes 
/**** 356 mapper=7 submapper=2 prgrom=128K chrram=8K mirror=H system=SYSTEM_PAL */
    0xDB9C072D, //  Licensed PAL\Arch Rivals꞉ A Basketbrawl!.nes 
    0x1EFE38EB, //  Licensed PAL\Captain Skyhawk.nes 
    0x27CA0679, //  Licensed PAL\Danny Sullivan's Indy Heat.nes 
    0x96CFB4D8, //  Licensed PAL\Digger T. Rock꞉ The Legend of the Lost City.nes 
    0x05104517, //  Licensed PAL\Ivan ''Ironman'' Stewart's Super Off-Road.nes 
    0x9198279E, //  Licensed PAL\Time Lord.nes 
/**** 357 mapper=7 submapper=2 prgrom=128K chrram=8K mirror=H system=SYSTEM_NTSC */
    0x4EB77C64, //  Homebrew\Super NeSnake 2.nes 
/**** 358 mapper=7 submapper=2 prgrom=256K chrram=8K mirror=H */
    0xC0A650F8, //  Bootleg Singles\Battletoads &amp; Double Dragon꞉ The Ultimate Team (NT-672).nes 
    0x2E43FAA7, //  Homebrew\Bugfixes, Improvements\Battletoads [bugfix][Ti].nes 
    0x82E4EBBA, //  Homebrew\Hacks\Battletoads - CO-OP Mod for Famicom [Dushbadge].nes 
    0x5C602471, //  Homebrew\Hacks\Battletoads - Famicom Expansion Port Compatibility [Dushbadge].nes 
    0x9806CB84, //  Licensed Japan\Battletoads.nes 
    0xCEB65B06, //  Licensed North America\Battletoads &amp; Double Dragon꞉ The Ultimate Team.nes 
    0x279710DC, //  Licensed North America\Battletoads.nes 
    0x9EDD2159, //  Licensed North America\R.C. Pro-Am II.nes 
    0x8111BA08, //  Licensed North America\Solar Jetman꞉ Hunt for the Golden Warpship.nes 
    0xD2562072, //  Licensed North America\Wizards &amp; Warriors III꞉ Kuros꞉ Visions of Power.nes 
    0x2D417253, //  Playchoice\Solar Jetman꞉ Hunt for the Golden Warpship.nes 
/**** 359 mapper=7 submapper=2 prgrom=256K chrram=8K mirror=H system=SYSTEM_PAL */
    0x41D32FD7, //  Licensed PAL\Aladdin.nes 
    0x23D7D48F, //  Licensed PAL\Battletoads &amp; Double Dragon꞉ The Ultimate Team.nes 
    0x524A5A32, //  Licensed PAL\Battletoads.nes 
    0x308DA987, //  Licensed PAL\R.C. Pro-Am II.nes 
    0x8904149E, //  Licensed PAL\Solar Jetman꞉ Hunt for the Golden Warpship.nes 
    0x89984244, //  Licensed PAL\The Lion King.nes 
    0x806DE21E, //  Licensed PAL\Wizards &amp; Warriors III꞉ Kuros꞉ Visions of Power.nes 
/**** 360 mapper=7 submapper=2 prgrom=512K chrram=8K mirror=H */
    0x31CBCD41, //  Bad Dumps\Battletoads &amp; Double Dragon꞉ The Ultimate Team - 4 Players (v0.9)[NakeuD2007].nes 
    0x3FEAFD85, //  Homebrew\Hacks\Battletoads &amp; Double Dragon on Ragnarok (v1.2)]Corpse Grinder].nes 
/**** 361 mapper=9 prgrom=128K chrrom=128K mirror=H */
    0x6F5685EA, //  Homebrew\Hacks\Ben Weasel's Punch Out [Punktendo].nes 
    0x9A836761, //  Homebrew\Hacks\Punch-Out!! Kirby [Byrahga].nes 
    0x92A2185C, //  Licensed Japan\Mike Tyson's Punch-Out!! (rev0).nes 
    0x2C818014, //  Licensed Japan\Mike Tyson's Punch-Out!! (rev1).nes 
    0x84382231, //  Licensed Japan\Punch-Out!! Prize Edition.nes 
    0xB95E9E7F, //  Licensed North America\Punch-Out!! Starring Mr. Dream.nes 
    0xB7BDA5FA, //  Modern\Virtual Console\Punch-Out!! (NTSC).nes 
/**** 362 mapper=9 prgrom=128K chrrom=128K mirror=H system=SYSTEM_PAL */
    0x3A4D4D10, //  Licensed PAL\Mike Tyson's Punch-Out!! (rev0).nes 
    0x25551F3F, //  Licensed PAL\Mike Tyson's Punch-Out!! (rev1).nes 
    0xD229FD5C, //  Licensed PAL\Punch-Out!! Starring Mr. Dream.nes 
    0xDCCAC6D9, //  Modern\Virtual Console\Punch-Out!! (PAL).nes 
/**** 363 mapper=9 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x7B837FDE, //  Bad Dumps\Mike Tyson's Punch-Out!! [missing PROM].nes 
    0xB6900E4F, //  Playchoice\Mike Tyson's Punch-Out!!.nes 
/**** 364 mapper=10 prgrom=128K prgnvram=8K chrrom=64K mirror=H battery=1 */
    0x9B547E05, //  Licensed Japan\Famicom Wars.nes 
    0xB6F4E666, //  Modern\Virtual Console\Famicom Wars.nes 
/**** 365 mapper=10 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xEE0A9B41, //  Homebrew\Translations\English\Famicom Wars [Aka Translations v1.11, Pacnsacdave v1.0].nes 
    0x40D5FCD1, //  Homebrew\Translations\English\Famicom Wars [Aka Translations v1.11].nes 
/**** 366 mapper=10 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x489DE293, //  Homebrew\Translations\English\Fire Emblem Gaiden [Artemis251 v1.01, Gaiden Guy v0.9a].nes 
    0x4EECDBF6, //  Homebrew\Translations\English\Fire Emblem Gaiden [Artemis251 v1.01].nes 
    0xE2CF01D6, //  Homebrew\Translations\English\Fire Emblem꞉ Dark Dragon and the Sword of Light [Quirino v1.0, Megakoopax Beta 4].nes 
    0xCD3F78FD, //  Homebrew\Translations\English\Fire Emblem꞉ Dark Dragon and the Sword of Light [Quirino v1.0].nes 
    0x6C65E146, //  Homebrew\Translations\English\Fire Emblem꞉ Shadow Dragon and the Blade of Light [Polinym v1.0].nes 
    0xA98046B8, //  Licensed Japan\Fire Emblem 外伝.nes 
    0x97CAD370, //  Licensed Japan\Fire Emblem꞉ 暗黒竜と光の剣.nes 
    0xB76DEEBD, //  Modern\Virtual Console\Fire Emblem 外伝.nes 
/**** 367 mapper=11 prgrom=32K chrrom=16K mirror=H */
    0x5DA9CEC8, //  Unlicensed North America\Mission Cobra.nes 
/**** 368 mapper=11 prgrom=32K chrrom=32K mirror=H */
    0x09C31CD4, //  Unlicensed North America\Galactic Crusader.nes 
/**** 369 mapper=11 prgrom=128K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0xEBB56E10, //  Unlicensed Elsewhere\балда.nes 
/**** 370 mapper=11 prgrom=512K chrrom=128K mirror=H */
    0x5CCED4EC, //  Homebrew\Mojonian Tales.nes 
/**** 371 mapper=11 prgrom=32K chrrom=16K mirror=V */
    0x5231A114, //  Homebrew\Tests\Color Dreams Color Test.nes 
    0x4D527D4A, //  Unlicensed North America\Tagin' Dragon.nes 
/**** 372 mapper=11 prgrom=32K chrrom=32K mirror=V */
    0x3C7E38F5, //  Unlicensed North America\Master Chu &amp; The Drunkard Hu.nes 
    0x11D08CC6, //  Unlicensed North America\Metal Fighter µ.nes 
    0xC73B82FC, //  Unlicensed North America\Shockwave.nes 
/**** 373 mapper=11 prgrom=32K chrrom=32K mirror=V system=SYSTEM_NTSC */
    0xFD9E5DD6, //  Bad Dumps\Chiller [bad PRG+CHR].nes 
    0xD15009CC, //  Bad Dumps\Chiller [bad PRG].nes 
    0x231BC76E, //  Unlicensed Elsewhere\Chiller.nes 
    0xB79F2651, //  Unlicensed North America\Chiller.nes 
/**** 374 mapper=11 prgrom=64K chrrom=16K mirror=V */
    0xBBE40DC4, //  Unlicensed North America\Baby Boomer.nes 
    0x72E66392, //  Unlicensed North America\Crystal Mines.nes 
    0xB133CFA7, //  Unlicensed South Korea\Baby Boomer.nes 
/**** 375 mapper=11 prgrom=64K chrrom=32K mirror=V */
    0x9AC319E5, //  Modern\Cheetahmen꞉ The Creation.nes 
    0xCB53C523, //  Unlicensed North America\King Neptune's Adventure.nes 
    0x61253D1C, //  Unlicensed North America\Raid 2020.nes 
    0xF1E6F5E8, //  Unlicensed South Korea\Crystal 특공대.nes 
    0xEB4D468B, //  Unreleased\Raid 2020.nes 
/**** 376 mapper=11 prgrom=64K chrrom=64K mirror=V */
    0x9CADA174, //  Bad Dumps\Bible Adventures (v7)[Immutable].nes 
    0xCA179DAD, //  Bad Dumps\Free Fall [broken 2].nes 
    0x71EA1872, //  Bad Dumps\Free Fall [broken].nes 
    0x26B48204, //  Bootleg Singles\Happy Camper (NintendoAge).nes 
    0x4D92EF00, //  Compatibility Hacks\Death Race [m011].nes 
    0x2038983A, //  Homebrew\Copyright Restorations\Happy Camper [NewRisingSun].nes 
    0xB7CC8FA4, //  Multicarts\extracts\Fish Fall (Sunday Funday).nes 
    0x1223757C, //  Unlicensed Japan\Miss Peach World 1꞉ Super L.A. Cop.nes 
    0x40A5E676, //  Unlicensed North America\Bible Adventures (v1.0).nes 
    0x73C246D4, //  Unlicensed North America\Bible Adventures (v1.1).nes 
    0x7F24EFC0, //  Unlicensed North America\Bible Adventures (v1.2).nes 
    0x73140EEF, //  Unlicensed North America\Bible Adventures (v1.3).nes 
    0x680DA78D, //  Unlicensed North America\Bible Adventures (v1.4).nes 
    0x8E011A8B, //  Unlicensed North America\Bible Adventures (v5.0).nes 
    0x345D3A1A, //  Unlicensed North America\Castle of Deceit.nes 
    0xD188963D, //  Unlicensed North America\Challenge of the Dragon.nes 
    0x22276213, //  Unlicensed North America\Menace Beach.nes 
    0x14A81635, //  Unlicensed North America\Moon Ranger.nes 
    0xEA113128, //  Unlicensed North America\Operation Secret Storm.nes 
    0x6A483073, //  Unlicensed North America\Pesterminator꞉ The Western Exterminator.nes 
    0x10124E09, //  Unlicensed North America\Robodemons.nes 
    0x973BBF75, //  Unlicensed North America\Secret Scout in the Temple of Demise.nes 
    0xCA0A869E, //  Unlicensed North America\Silent Assault.nes 
    0xA5E89675, //  Unlicensed North America\The Adventures of Captain Comic.nes 
    0x42749A95, //  Unlicensed North America\The P'radikus Conflict.nes 
    0x16188B09, //  Unlicensed South Korea\Black Dragon.nes 
    0x9DA818F8, //  Unlicensed South Korea\날아라 Superboy.nes 
    0xA781FFAA, //  Unreleased\Free Fall.nes 
    0x57D32D1B, //  Unreleased\Happy Camper.nes 
    0x4BC75F16, //  Unreleased\King Neptune's Adventure.nes 
    0x39B7D24A, //  Unreleased\Mr. Assy꞉ Momoco &amp; Hanako.nes 
    0x692F2096, //  Unreleased\Secret Scout in the Temple of Demise.nes 
    0xBD139BE7, //  Unreleased\The Escape from Atlantis (v1).nes 
    0x0AEA38F7, //  Unreleased\The Escape from Atlantis (v2).nes 
    0x6025C660, //  Unreleased\Wally Bear &amp; the No Gang.nes 
/**** 377 mapper=11 prgrom=128K chrram=8K mirror=V */
    0x81ECDA0D, //  Unlicensed North America\Wally Bear &amp; the No Gang.nes 
/**** 378 mapper=11 prgrom=128K chrrom=128K mirror=V */
    0xCFD5AC62, //  Unlicensed North America\Bible Buffet.nes 
    0x0AB26DB6, //  Unlicensed North America\Exodus꞉ Journey to the Promised Land (v4.0).nes 
    0x5F5BFA54, //  Unlicensed North America\Exodus꞉ Journey to the Promised Land (v5.0).nes 
    0x51C70247, //  Unlicensed North America\Joshua &amp; the Battle of Jericho (v5.0).nes 
    0x99083B3A, //  Unlicensed North America\Joshua &amp; the Battle of Jericho (v6.0).nes 
    0x1948810E, //  Unlicensed North America\Spiritual Warfare (v5.1).nes 
    0xB786C2AC, //  Unlicensed North America\Spiritual Warfare (v6.0).nes 
    0x14105C13, //  Unlicensed North America\Spiritual Warfare (v6.1).nes 
    0x5B16A3C8, //  Unlicensed North America\Sunday Funday.nes 
    0xA55701DD, //  Unlicensed North America\The King of Kings꞉ The Early Years (v1.1).nes 
    0x560BF5A6, //  Unlicensed North America\The King of Kings꞉ The Early Years (v1.2).nes 
    0x86974CCC, //  Unlicensed North America\The King of Kings꞉ The Early Years (v1.3).nes 
    0x7EABDA5C, //  Unlicensed North America\The King of Kings꞉ The Early Years (v5.0).nes 
    0x2AAF0804, //  Unreleased\Spiritual Warfare.nes 
/**** 379 mapper=16 submapper=4 prgrom=128K chrrom=128K mirror=H */
    0x84344FF5, //  Homebrew\Translations\English\Charge!! Men's Private School꞉ Number One Student [Pacnsacdave].nes 
    0xD19E3506, //  Homebrew\Translations\English\Devil Boy꞉ Trap of Hell [S. K. Dumbbus v1.0].nes 
    0x92D192CB, //  Homebrew\Translations\English\Devil Boy꞉ Trap of Hell [S. K. Dumbbus v1.1].nes 
    0xD75D5589, //  Homebrew\Translations\English\Devil Boy꞉ Trap of Hell [S. K. Dumbbus v1.2].nes 
    0x33B899C9, //  Licensed Japan\Dragon Ball꞉ 大魔王復活.nes 
    0x7B5206AF, //  Licensed Japan\名門! 第三野球部.nes 
    0x2C4421B2, //  Licensed Japan\悪魔くん꞉ 魔界の罠.nes 
    0x9C04C8D5, //  Licensed Japan\魁! 男塾꞉ 疾風一号生.nes 
/**** 380 mapper=16 submapper=4 prgrom=128K chrrom=256K mirror=H */
    0x8341A385, //  Homebrew\Translations\English\Dragon Ball 3꞉ 悟空伝 [Dodgy Translations v0.15].nes 
    0x52EEE276, //  Homebrew\Translations\English\Dragon Ball 3꞉ 悟空伝 [Dodgy Translations v0.xx].nes 
    0x6E68E31A, //  Licensed Japan\Dragon Ball 3꞉ 悟空伝 (rev0).nes 
    0xBFC7A2E9, //  Licensed Japan\Dragon Ball 3꞉ 悟空伝 (rev1).nes 
    0xA851CAE9, //  Licensed Japan\西村京太郎꞉ Blue Train 殺人事件.nes 
/**** 381 mapper=16 submapper=4 prgrom=256K chrrom=128K mirror=H */
    0x03E78D60, //  Homebrew\Translations\English\Dragon Ball꞉ Revival of the Dark Lord [Stardust Crusaders].nes 
    0xD343C66A, //  Licensed Japan\Famicom Jump꞉ 英雄列伝.nes 
/**** 382 mapper=16 submapper=5 prgrom=128K chrrom=128K mirror=H */
    0xB3E39555, //  Homebrew\Translations\English\Crayon しんちゃん꞉ Ora to Poi Poi [Alcy v0.91].nes 
    0xDB05106E, //  Licensed Japan\Crayon しんちゃん꞉ オラとポイポイ.nes 
/**** 383 mapper=16 submapper=5 prgrom=256K prgnvram=256B chrrom=256K mirror=H battery=1 */
    0xFE52D435, //  Homebrew\Translations\English\Dragon Ball Z Gaiden꞉ Plan to Eliminate the Saiyans [Twilight Translations](Vegeta control).nes 
    0x9D6766E5, //  Homebrew\Translations\English\Dragon Ball Z Gaiden꞉ Plan to Eliminate the Saiyans [Twilight Translations].nes 
    0x5A5B9332, //  Homebrew\Translations\English\Dragon Ball Z III꞉ Killer Androids [Twilight Translations].nes 
    0xB51335F7, //  Homebrew\Translations\English\Dragon Ball Z II꞉ Tyrant Freeza!! [Gyroballer v1.02].nes 
    0xDC52BF0C, //  Licensed Japan\Dragon Ball Z III꞉ 烈戦人造人間.nes 
    0x99240573, //  Licensed Japan\Dragon Ball Z II꞉ 激神 Freezer!! (rev0).nes 
    0xA9541452, //  Licensed Japan\Dragon Ball Z II꞉ 激神 Freezer!! (rev1).nes 
    0x136CA449, //  Licensed Japan\Dragon Ball Z 外伝꞉ サイヤ人絶滅計画.nes 
    0xB049A8C4, //  Licensed Japan\SD Gundam 外伝 - Knight Gundam 物語 2꞉ 光の騎士.nes 
    0xC2840372, //  Licensed Japan\SD Gundam 外伝 - Knight Gundam 物語 3꞉ 伝説の騎士団.nes 
    0xA262A81F, //  Licensed Japan\ろくでなし Blues.nes 
/**** 384 mapper=18 prgrom=128K chrrom=128K mirror=H */
    0xD98745E7, //  Homebrew\Translations\English\Magic John [KingMike's Translations &amp; Video Smash Excellent].nes 
    0xF6230855, //  Homebrew\Translations\English\Ninja Jajamaru꞉ Galactic Battle [HTI].nes 
    0xC31368A0, //  Homebrew\Translations\English\Pizza Pop! [Pacnsacdave].nes 
    0x3A6DA481, //  Homebrew\Translations\English\Saiyuki World II꞉ Evil Spirit of Heaven [PentarouZero].nes 
    0xDAB5A187, //  Homebrew\Translations\English\The Lord of King [BlackPaladin v1.0].nes 
    0x5B7AC91F, //  Licensed Japan\Goal!!.nes 
    0xD8748E0A, //  Licensed Japan\Magic John.nes 
    0x49DA2F76, //  Licensed Japan\Pizza Pop!.nes 
    0x0DB4B382, //  Licensed Japan\Plasma Ball.nes 
    0xD0EB749F, //  Licensed Japan\The Lord of King.nes 
    0x91D52E9A, //  Licensed Japan\USA Ice Hockey in FC.nes 
    0x652F3324, //  Licensed Japan\つるピカハゲ丸꞉ めざせ! つるセコの証.nes 
    0x6479E76A, //  Licensed Japan\寺尾のどすこい大相撲.nes 
    0x06F9C714, //  Licensed Japan\忍者じゃじゃ丸꞉ 銀河大作戦.nes 
    0x75B3EB37, //  Licensed Japan\西遊記 World 2 天上界の魔神.nes 
    0x965834BD, //  Unreleased\忍者じゃじゃ丸꞉ 銀河大作戦.nes 
/**** 385 mapper=18 prgrom=128K chrrom=256K mirror=H */
    0x3B215AB6, //  Homebrew\Translations\English\Fighting Spirit꞉ Toukon Club [Eric Engel v1.1].nes 
    0xEA3E78DD, //  Licensed Japan\闘魂 Club.nes 
/**** 386 mapper=18 prgrom=256K chrrom=128K mirror=H */
    0xA912B6E1, //  Homebrew\Translations\English\Plasma Ball [MrRichard999].nes 
    0x4AE58F5D, //  Licensed Japan\新燃えろ!! Pro 野球.nes 
    0x9DC96EC7, //  Licensed Japan\燃えPro! '90꞉ 感動編.nes 
    0x3F8D6889, //  Licensed Japan\燃えPro! 最強編.nes 
/**** 387 mapper=18 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xE5D6773D, //  Homebrew\Translations\English\Ninja Jajamaru꞉ Legend of a Demon's Attack - Castle of Dreams [Aishsha, Stardust Crusaders; Proveaux v1.01].nes 
    0x7DF67642, //  Homebrew\Translations\English\Ninja Jajamaru꞉ Legend of a Demon's Attack - Castle of Dreams [Aishsha, Stardust Crusaders].nes 
    0x0F1BABE7, //  Licensed Japan\じゃじゃ丸撃魔伝꞉ 幻の金魔城.nes 
/**** 388 mapper=18 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x41D321D3, //  Homebrew\Translations\English\Aim! Top Pro꞉ Dream in Green [Immutable].nes 
    0x3691C120, //  Licensed Japan\めざせ Top Pro꞉ Greenに賭ける夢.nes 
/**** 389 mapper=19 submapper=2 prgrom=128K chrrom=128K mirror=H */
    0x98BADEA1, //  Homebrew\Hacks\Big Trouble in Little China [pacnsacdave].nes 
    0xA86303E2, //  Homebrew\Translations\English\Dragon Ninja (rev0)[Stardust Crusaders].nes 
    0x25FA8D0E, //  Homebrew\Translations\English\Dragon Ninja (rev1)[Stardust Crusaders].nes 
    0x8A8CC1DD, //  Homebrew\Translations\English\Star Wars (Namco) [Mukimuki &amp; Shinta v2.00].nes 
    0xF57C84D7, //  Homebrew\Translations\English\Star Wars꞉ A New Hope (Namco)[Gil Galad].nes 
    0x2A7D3ADF, //  Licensed Japan\Dragon Ninja (rev0).nes 
    0x2AE535CA, //  Licensed Japan\Dragon Ninja (rev1).nes 
    0xCA69751B, //  Licensed Japan\Star Wars꞉ A New Hope (Namco).nes 
/**** 390 mapper=19 submapper=2 prgrom=256K chrrom=256K mirror=H */
    0x4C5836BD, //  Licensed Japan\Namco Classic.nes 
/**** 391 mapper=19 submapper=2 prgrom=128K chrrom=128K mirror=H battery=1 */
    0x1C33C23E, //  Homebrew\Translations\English\Famista '90 [MrRichard999, TheMajinZenki, FlashPV v0.91].nes 
    0x1C0B1B63, //  Homebrew\Translations\English\Famista '90 [MrRichard999, TheMajinZenki, FlashPV v0.92].nes 
    0xB5FF71AB, //  Licensed Japan\Battle Fleet.nes 
    0x0C1792DA, //  Licensed Japan\Famista '90.nes 
    0xACE56F39, //  Licensed Japan\Mindseeker.nes 
    0x10C8F2FA, //  Licensed Japan\独眼竜政宗.nes 
    0xBC11E61A, //  Licensed Japan\貝獣物語.nes 
    0x71DE7CCA, //  Modern\Namco Museum Archives\貝獣物語.nes 
/**** 392 mapper=19 submapper=2 prgrom=256K chrrom=128K mirror=H battery=1 */
    0x55320014, //  Homebrew\Translations\English\Hydlide 3꞉ Visitor from the Dark [Pacnsacdave].nes 
    0x47C2020B, //  Licensed Japan\Hydlide 3꞉ 闇からの訪問者.nes 
/**** 393 mapper=19 submapper=2 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xE81DD8F6, //  Homebrew\Translations\English\Shell Monsters Story [KingMike's Translations v1.0beta2].nes 
/**** 394 mapper=19 submapper=2 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0xCF23290F, //  Licensed Japan\Juvei Quest (rev0).nes 
    0x716DAEA5, //  Licensed Japan\Juvei Quest (rev1).nes 
/**** 395 mapper=19 submapper=2 prgrom=512K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0xE864AA4F, //  Homebrew\Translations\English\Jubei Quest [Aishsha &amp; Stardust Crusaders v1.01].nes 
/**** 396 mapper=19 submapper=3 prgrom=128K chrrom=128K mirror=H */
    0x5746A461, //  Licensed Japan\Final Lap.nes 
/**** 397 mapper=19 submapper=3 prgrom=256K chrrom=256K mirror=H */
    0x99B90FA5, //  Homebrew\Translations\English\Namco Classic II [MrRichard999 v0.80].nes 
    0x684B292F, //  Licensed Japan\Namco Classic II.nes 
    0xCB88D71D, //  Modern\Namco Museum Archives\Namco Classic II.nes 
/**** 398 mapper=19 submapper=3 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xFF5FB7D2, //  Modern\Namco Museum Archives\Pac-Man Championship Edition (Japan).nes 
    0x7D39449C, //  Modern\Namco Museum Archives\Pac-Man Championship Edition (World).nes 
/**** 399 mapper=19 submapper=3 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x5E9DF982, //  Homebrew\Translations\English\Digital Devil Story꞉ 女神転生 II [Dushbadge v5.0].nes 
    0x96773F32, //  Licensed Japan\Digital Devil Story꞉ 女神転生 II (rev0).nes 
    0x10C9A789, //  Licensed Japan\Digital Devil Story꞉ 女神転生 II (rev1).nes 
    0x098C672A, //  Licensed Japan\三国志 II꞉ 覇王の大陸.nes 
/**** 400 mapper=19 submapper=4 prgrom=128K chrrom=128K mirror=H */
    0x9EDBE2E2, //  Licensed Japan\Rolling Thunder.nes 
    0x6DACA23A, //  Modern\Namco Museum Archives\Rolling Thunder.nes 
/**** 401 mapper=19 submapper=5 prgrom=128K chrrom=128K mirror=H */
    0x3C38637D, //  Homebrew\Translations\English\Mappy Kids [Zynk Oxhyde].nes 
    0x6903F6BC, //  Homebrew\Translations\English\Phantom Travel Journal [Jackic v0.9].nes 
    0x35D8C961, //  Licensed Japan\Mappy Kids.nes 
    0xEF7996BF, //  Licensed Japan\えりかとさとるの夢冒険.nes 
    0xC811DC7A, //  Licensed Japan\妖怪道中記.nes 
    0x87E00045, //  Modern\Namco Museum Archives\妖怪道中記.nes 
    0xEB190367, //  Modern\Virtual Console\妖怪道中記.nes 
/**** 402 mapper=19 submapper=5 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x4CB2A50E, //  Homebrew\Translations\English\King of Kings [MrRichard999 v0.99b].nes 
    0x1A04C23B, //  Homebrew\Translations\English\Three Kingdoms꞉ Champion of the Center [Green Dark Software v0.1].nes 
    0x369DA42D, //  Licensed Japan\King of Kings.nes 
    0xE64B8975, //  Licensed Japan\三国志꞉ 中原の覇者.nes 
    0x35E2518A, //  Modern\Namco Museum Archives\King of Kings.nes 
/**** 403 mapper=21 submapper=1 prgrom=256K chrrom=128K mirror=H */
    0xA7AF0BB9, //  Homebrew\Translations\English\Wai Wai World 2꞉ S.O.S. from Parsley Castle [Vice Translations v1.01, Proveaux v1.0].nes 
    0x79C5987D, //  Homebrew\Translations\English\Wai Wai World 2꞉ S.O.S. from Parsley Castle [Vice Translations v1.01].nes 
    0x2520408A, //  Homebrew\Translations\English\Wai Wai World 2꞉ SOS!! Parsley Castle [Vice Translations v1.01, Chronix v1.0].nes 
    0x8B03F74D, //  Licensed Japan\ワイワイ World 2꞉ S.O.S.!! Parsley城.nes 
    0x4B9ECFB2, //  Modern\Virtual Console\ワイワイ World 2꞉ S.O.S.!! Parsley城.nes 
/**** 404 mapper=21 submapper=2 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0xB8109B0E, //  Homebrew\Translations\English\Ganbare Goemon Gaiden 2꞉ Treasures of the World [Adventurous Translations v1.01].nes 
    0x286FCD20, //  Licensed Japan\がんばれゴエモン外伝 2꞉ 天下の財宝.nes 
/**** 405 mapper=23 submapper=1 prgrom=32K chrrom=40K mirror=H */
    0x67C1A27D, //  Homebrew\Demos\Axelay.nes 
/**** 406 mapper=23 submapper=1 prgrom=128K prgram=2K chrrom=128K mirror=H */
    0xF6036ED8, //  Samples\Crisis Force.nes 
/**** 407 mapper=23 submapper=1 prgrom=128K chrrom=512K mirror=H */
    0x335E6339, //  Homebrew\Copyright Restorations\英雄傳 - Kung Fu Legend (rev0).nes 
    0xE4A291CE, //  Unlicensed Taiwan&amp;Hong Kong\英雄傳 - World Hero (rev0).nes 
/**** 408 mapper=23 submapper=1 prgrom=128K chrrom=512K mirror=H system=SYSTEM_NTSC */
    0x1A8D767B, //  Homebrew\Copyright Restorations\英雄傳 - Kung Fu Legend (rev1).nes 
    0x0E263D47, //  Unlicensed Taiwan&amp;Hong Kong\英雄傳 - World Hero (rev1).nes 
/**** 409 mapper=23 submapper=2 prgrom=128K chrrom=128K mirror=H */
    0xD83879C9, //  Homebrew\Translations\English\Castlevania Special꞉ Kid Dracula [Chronix].nes 
    0xF526B000, //  Homebrew\Translations\English\Castlevania꞉ Kid Dracula [Kalas].nes 
    0xCFE0FF2C, //  Homebrew\Translations\English\Demon Castle Special꞉ I'm Kid Dracula! [Vice Translations].nes 
    0x91328C1D, //  Licensed Japan\Tiny Toon Adventures.nes 
    0xC1FBF659, //  Licensed Japan\悪魔城 Special꞉ ぼく Dracula君.nes 
/**** 410 mapper=23 submapper=2 prgrom=128K prgram=2K chrrom=128K mirror=H */
    0xB2E81F64, //  Homebrew\Translations\English\Crisis Force (v1.0)[Stardust Crusaders].nes 
    0xFCBF28B1, //  Licensed Japan\Crisis Force.nes 
    0xD467C0CC, //  Licensed Japan\Parodius だ! 神話からお笑いへ.nes 
/**** 411 mapper=23 submapper=2 prgrom=128K prgram=8K chrrom=128K mirror=H */
    0x927DD49F, //  Bad Dumps\Kid Dracula (Castlevania Anniversary Collection)[SRAM initialize fix with glitched ending][missing PRG banks].nes 
    0x8281C50F, //  Modern\悪魔城 Special꞉ ぼくDracula君 (Castlevania Anniversary Collection).nes 
/**** 412 mapper=23 submapper=2 prgrom=256K chrrom=128K mirror=H */
    0xA24C6E22, //  Homebrew\Translations\English\Parodius꞉ From Myth to Laughter [Stardust Crusaders].nes 
/**** 413 mapper=23 submapper=2 prgrom=256K prgram=8K chrrom=128K mirror=H */
    0x166C2418, //  Homebrew\Bugfixes, Improvements\Kid Dracula (Castlevania Anniversary Collection)[SRAM initialize fix with fixed ending].nes 
    0xE3A1CE4D, //  Homebrew\Bugfixes, Improvements\Kid Dracula (Castlevania Anniversary Collection)[SRAM initialize fix with glitched ending].nes 
    0xEB6A29D6, //  Homebrew\Bugfixes, Improvements\Kid Dracula (Castlevania Anniversary Collection)[SRAM initialize fix with glitched ending][Manji restored].nes 
    0x394AC7B2, //  Homebrew\Bugfixes, Improvements\Kid Dracula (Castlevania Anniversary Collection)[SRAM initialize fix with glitched ending][Manji restored][Lemmy removed].nes 
/**** 414 mapper=23 submapper=2 prgrom=128K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xC74871BD, //  Homebrew\Bugfixes, Improvements\Kid Dracula (Castlevania Anniversary Collection)[WRAM enable].nes 
    0x64AFD592, //  Modern\Kid Dracula (Castlevania Anniversary Collection).nes 
/**** 415 mapper=23 submapper=3 prgrom=128K chrrom=128K mirror=H */
    0x599B6378, //  Bad Dumps\がんばれゴエモン 2 (Virtual Console)[PRG corrupt].nes 
    0x06145246, //  Bootleg Hacks\Super Mario Bros. 13.nes 
    0xC4DCBB18, //  Homebrew\Translations\English\Contra [MadHacker].nes 
    0xE1D20375, //  Homebrew\Translations\English\Contra [Stardust Crusaders].nes 
    0x8A1309F7, //  Homebrew\Translations\English\Ganbare Goemon 2 [Stardust Crusaders v1.02].nes 
    0xFC8BC5B8, //  Homebrew\Translations\English\Go for it Goemon 2 [Stardust Crusaders v1.02, Green Jerry v1.0].nes 
    0xAF85B53E, //  Homebrew\Translations\English\Gryzor [MadHacker].nes 
    0xCE853F0D, //  Homebrew\Translations\English\Konami Wai Wai World [Zynk Oxhyde v2.2].nes 
    0x1DB4E7FC, //  Homebrew\Translations\English\Konami ワイワイ World [Demiforce v3.00].nes 
    0xCFE61D2B, //  Homebrew\Translations\English\Mystical Ninja 2 [Stardust Crusaders v1.02, El Duderino v1.0].nes 
    0x48A17165, //  Homebrew\Translations\English\The Legend of Getsu Fuma [RPGe, Pacnsacdave].nes 
    0xFB847D1A, //  Homebrew\Translations\English\月風魔伝 [RPGe].nes 
    0xAC9895CC, //  Licensed Japan\Dragon Scroll꞉ 甦りし魔竜.nes 
    0x8A96E00D, //  Licensed Japan\Konami ワイワイ World.nes 
    0x0CC9FFEC, //  Licensed Japan\がんばれゴエモン 2.nes 
    0x49123146, //  Licensed Japan\月風魔伝.nes 
    0xB27B8CF4, //  Licensed Japan\魂斗羅.nes 
    0x11732CD9, //  Modern\Virtual Console\月風魔伝.nes 
    0x255063BD, //  Modern\月風魔伝 (Steam).nes 
    0x8DCC1194, //  Modern\魂斗羅 (Contra Anniversary Collection).nes 
    0x203583D5, //  Samples\TwinBee 3꞉ ポコポコ大魔王.nes 
    0xCB35FA90, //  Samples\魂斗羅.nes 
    0xAA9F9765, //  Unreleased\Mad City.nes 
    0xE6C94541, //  Unreleased\月風魔伝.nes 
/**** 416 mapper=23 submapper=3 prgrom=128K chrrom=256K mirror=H */
    0x39B68AA3, //  Licensed Japan\じゃりン子チエ꞉ ばくだん娘の幸せさがし.nes 
/**** 417 mapper=23 submapper=3 prgrom=256K chrrom=128K mirror=H */
    0x0889A5B1, //  Homebrew\Translations\English\The Legend of Fuma [Nebulous Translations].nes 
/**** 418 mapper=23 submapper=3 prgrom=128K chrrom=128K mirror=H battery=1 */
    0x24C66CC4, //  Homebrew\Translations\English\Dragon Scroll꞉ Resurrection of the Demon Dragon [KingMike].nes 
/**** 419 mapper=24 prgrom=32K chrrom=8K mirror=H */
    0xEDC84DF3, //  Homebrew\Demos\Mind Control Trilogy Preview [Bilotrip].nes 
/**** 420 mapper=24 prgrom=64K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0xA8A6FFEB, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (Two Simultaneous Players)[Corpse Grinder].nes 
/**** 421 mapper=24 prgrom=128K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0x5ADB9474, //  Homebrew\Demos\Rudeboy.nes 
/**** 422 mapper=24 prgrom=128K chrrom=32K mirror=H system=SYSTEM_NTSC */
    0x99D867FB, //  Homebrew\DENDY Compo II.nes 
/**** 423 mapper=24 prgrom=256K chrrom=8K mirror=H */
    0xCF91588A, //  Homebrew\Demos\VGM player 2011 Mic - 1 Ultra.nes 
    0x71B986A5, //  Homebrew\Demos\VGM player 2011 Mic - FF3 Battle.nes 
    0x502575D1, //  Homebrew\Demos\VGM player 2011 Mic - Gunstar Heroes - 02 - Legend of the Gunstars.nes 
    0x31708C32, //  Homebrew\Demos\VGM player 2011 Mic - Tintin.nes 
    0x982DA55F, //  Homebrew\Tools\VGM player 2011 Mic.nes 
/**** 424 mapper=24 prgrom=256K chrrom=128K mirror=H */
    0x5137BC52, //  Homebrew\Frog [Boston Breams].nes 
    0x27D15629, //  Homebrew\Hacks\Castlevania series\悪魔城伝説 (Improved Controls)(v1.4)[NaOH].nes 
    0xE38B017B, //  Homebrew\Translations\English\Castlevania III꞉ Dracula's Curse [ShadowOne333 v7.1](new Alucard sprite).nes 
    0x939EF07A, //  Homebrew\Translations\English\Castlevania III꞉ Dracula's Curse [ShadowOne333 v7.1].nes 
    0xD4BFC296, //  Homebrew\Translations\English\Legend of Demon Castle꞉ Castlevania III [Vice Translations].nes 
    0xE349AF38, //  Licensed Japan\悪魔城伝説.nes 
    0x1ED3CAA3, //  Modern\Virtual Console\悪魔城伝説.nes 
    0xF27D873B, //  Modern\悪魔城伝説 (Castlevania Anniversary Collection).nes 
/**** 425 mapper=24 prgrom=256K chrrom=256K mirror=H */
    0x41E2825B, //  Homebrew\Hacks\Castlevania series\Castlevania Resurrection.nes 
/**** 426 mapper=24 prgrom=256K prgram=8K chrrom=256K mirror=H */
    0x4D476097, //  Homebrew\Bugfixes, Improvements\Dizzy꞉ Melanchony of Existance Chapter 0 (beta)[WRAM enable].nes 
    0x1F8E0403, //  Homebrew\Dizzy꞉ Melanchony of Existance Chapter 0 (beta).nes 
/**** 427 mapper=25 submapper=1 prgrom=32K chrrom=16K mirror=H system=SYSTEM_NTSC */
    0xEC9BBC11, //  Homebrew\Guardian 5.nes 
/**** 428 mapper=25 submapper=1 prgrom=128K chrrom=128K mirror=H */
    0xF880E010, //  Bootleg Hacks\蠟筆小新 (JY-035).nes 
    0xB960B13C, //  Homebrew\Translations\English\Bio Miracle Baby Upa!! [Vice Translations].nes 
    0x6DC28B5A, //  Licensed Japan\Bio Miracle ぼくってUpa (Re-release).nes 
    0xF6271A51, //  Licensed Japan\Racer Mini 四駆꞉ Japan Cup.nes 
/**** 429 mapper=25 submapper=1 prgrom=128K prgram=2K chrrom=128K mirror=H */
    0xBB0DB1F1, //  Homebrew\Bugfixes, Improvements\Gradius II꞉ Goferの野望 [DPCM bit order, DPCM clicking].nes 
    0x5ADBF660, //  Licensed Japan\Gradius II꞉ Goferの野望.nes 
    0x7F7AB2E2, //  Modern\Virtual Console\Gradius II.nes 
/**** 430 mapper=25 submapper=2 prgrom=128K chrrom=128K mirror=H */
    0x2EA914FA, //  Bootleg Hacks\FIFA International 2 96.nes 
    0x7075BF5E, //  Bootleg Hacks\Pizza Pop! Mario.nes 
/**** 431 mapper=25 submapper=2 prgrom=128K prgram=8K chrrom=256K mirror=H */
    0x5E153659, //  Bootleg Hacks\Batman 4.nes 
/**** 432 mapper=25 submapper=2 prgrom=256K chrrom=256K mirror=H */
    0x490E8A4C, //  Licensed Japan\Teenage Mutant Ninja Turtles 2꞉ The Manhattan Project.nes 
    0x4A601A2C, //  Licensed Japan\Teenage Mutant Ninja Turtles꞉ Super 亀忍者.nes 
/**** 433 mapper=25 submapper=3 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x089B6CCE, //  Bad Dumps\がんばれゴエモン外伝꞉ きえた黄金キセル (Virtual Console)[PRG corrupt].nes 
    0x3F43AC2E, //  Homebrew\Translations\English\Ganbare Goemon Gaiden꞉ The Missing Golden Pipe [Adventurous Translations v0.99c, FlashPV].nes 
    0x577BC924, //  Homebrew\Translations\English\Ganbare Goemon Gaiden꞉ The Missing Golden Pipe [Adventurous Translations v0.99c].nes 
    0x36333952, //  Homebrew\Translations\English\Go for it! Goemon Gaiden꞉ The Missing Golden Pipe [Adventurous Translations v0.99c, FlashPV].nes 
    0x4C53A5F1, //  Homebrew\Translations\English\Mystical Ninja꞉ Saga of the Golden Pipe [Adventurous Translations v0.99c, El Duderino v1.0].nes 
    0xEB92B32A, //  Licensed Japan\がんばれゴエモン外伝꞉ きえた黄金キセル (rev0).nes 
    0x36D22AD5, //  Licensed Japan\がんばれゴエモン外伝꞉ きえた黄金キセル (rev1).nes 
/**** 434 mapper=26 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0xDB84361B, //  Homebrew\Translations\English\Esper Dream 2 [Aeon Genesis].nes 
    0x209B4BED, //  Licensed Japan\Esper Dream 2꞉ 新たなる戦い.nes 
/**** 435 mapper=26 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x7289042B, //  Homebrew\Translations\English\Madara [Aeon Genesis].nes 
    0xE1383DEB, //  Licensed Japan\魍魎戦記 Madara.nes 
/**** 436 mapper=28 prgrom=32K chrram=16K mirror=H system=SYSTEM_NTSC */
    0xA9392746, //  Homebrew\Nalle Land (v0.3.4).nes 
/**** 437 mapper=28 prgrom=64K chrram=32K mirror=H */
    0x44AA2039, //  Homebrew\Filthy Kitchen.nes 
    0x93F4E749, //  Samples\Witch n' Wiz.nes 
/**** 438 mapper=28 prgrom=64K chrram=32K mirror=H system=SYSTEM_NTSC */
    0x1D09FF55, //  Homebrew\Twin Dragons (NESdev 2016).nes 
/**** 439 mapper=28 prgrom=512K chrram=32K mirror=H */
    0xD665D531, //  Homebrew\Multicarts\Action 53 Function 16 Volume 1 (R.C.#1).nes 
    0x6441E8F6, //  Homebrew\Multicarts\Action 53 Function 16 Volume 1 (R.C.#2).nes 
    0x6E61971F, //  Homebrew\Multicarts\Action 53 Volume 2꞉ Double Action (R.C.#2).nes 
    0xF0D58EEC, //  Homebrew\Multicarts\Action 53 Volume 2꞉ Double Action (R.C.#4a).nes 
/**** 440 mapper=28 prgrom=1024K chrram=32K mirror=H */
    0x660F8E82, //  Homebrew\Multicarts\Action 53 Volume 3꞉ Revenge of the Twins.nes 
/**** 441 mapper=30 prgrom=512K chrram=32K mirror=1 */
    0xF8253658, //  Modern\Basse Def Adventures.nes 
/**** 442 mapper=30 prgrom=512K chrram=32K mirror=1 system=SYSTEM_NTSC */
    0x4E8B051B, //  Modern\Twin Dragons.nes 
/**** 443 mapper=30 prgrom=512K chrram=32K mirror=1 battery=1 system=SYSTEM_NTSC */
    0x31BE333B, //  Modern\Larry and the Long Look for a Luscious Lover (alt 1).nes 
    0x8BC0340C, //  Modern\Larry and the Long Look for a Luscious Lover (alt 2).nes 
/**** 444 mapper=30 prgrom=512K chrram=32K mirror=4 battery=1 */
    0x57C68295, //  Homebrew\Black Box Challenge.nes 
/**** 445 mapper=30 prgrom=32K chrram=32K mirror=H system=SYSTEM_NTSC */
    0x78156E3C, //  Homebrew\Quadralords.nes 
/**** 446 mapper=30 prgrom=64K chrram=32K mirror=H */
    0x318C0A59, //  Homebrew\Mystic Pillars.nes 
/**** 447 mapper=30 prgrom=512K chrram=8K mirror=H */
    0x74B823A1, //  Modern\Battle Kid 2꞉ Mountain of Torment (v1.000).nes 
    0xFA5D5A4D, //  Modern\Battle Kid꞉ Fortress of Peril (v2.000).nes 
/**** 448 mapper=30 prgrom=512K chrram=32K mirror=H */
    0x330F6838, //  Homebrew\Twelve Seconds.nes 
    0x46AF36EB, //  Modern\Mystic Pillars.nes 
    0x32AEA02F, //  Modern\Quadralords.nes 
/**** 449 mapper=30 prgrom=512K chrram=32K mirror=H system=SYSTEM_NTSC */
    0x0D71192B, //  Homebrew\Power Coloring.nes 
    0x583C7C22, //  Modern\Tapeworm Disco Puzzle.nes 
/**** 450 mapper=30 prgrom=2048K chrram=8K mirror=H system=SYSTEM_NTSC */
    0x2E86704D, //  Modern\Polygondwanaland - 2nd_Edition.nes 
/**** 451 mapper=30 prgrom=128K chrram=8K mirror=H battery=1 */
    0x35FC9101, //  Homebrew\Study Hall [khan].nes 
/**** 452 mapper=30 prgrom=128K chrram=32K mirror=H battery=1 */
    0xCD35F4C6, //  Modern\8 Bit Xmas 2012꞉ Santa's Biplane (8 Bit Xmas 2017 extract).nes 
    0x0ABC7758, //  Modern\8 Bit Xmas 2013꞉ Santa vs. the Aliens (8 Bit Xmas 2017 extract).nes 
    0x4BA368D6, //  Modern\8 Bit Xmas 2014꞉ Squish Everyone! (8 Bit Xmas 2017 extract).nes 
    0x05C89787, //  Modern\8 Bit Xmas 2015꞉ Twelve Seconds 'til Xmas (8 Bit Xmas 2017 extract).nes 
/**** 453 mapper=30 prgrom=512K chrram=32K mirror=H battery=1 */
    0xF9B944CF, //  Homebrew\The Adventures of Panzer (v1.3)[PixelCraft].nes 
/**** 454 mapper=30 prgrom=512K chrram=32K mirror=H battery=1 system=SYSTEM_NTSC */
    0xE5BD8692, //  Homebrew\Super Tilt Bro. (v2.α1).nes 
/**** 455 mapper=30 prgrom=256K chrram=8K mirror=V */
    0x675BD9CB, //  Modern\Nomolos꞉ Storming the CATsle (v1.00).nes 
    0x20E7A49E, //  Modern\Nomolos꞉ Storming the CATsle (v1.00D #1).nes 
    0x78A81C0B, //  Modern\Nomolos꞉ Storming the CATsle (v1.00D #2).nes 
/**** 456 mapper=30 prgrom=256K chrram=32K mirror=V */
    0xE41220D8, //  Homebrew\Assimilate (2015).nes 
/**** 457 mapper=30 prgrom=512K chrram=8K mirror=V */
    0x222B13A0, //  Modern\Nomolos꞉ Storming the CATsle (v1.01).nes 
/**** 458 mapper=30 prgrom=512K chrram=32K mirror=V */
    0x45493D2D, //  Homebrew\Mystic Origins.nes 
    0x7145F667, //  Modern\Assimilate.nes 
    0x95DFC71C, //  Modern\The Legends of Owlia.nes 
    0x02F215CB, //  Samples\The Legends of Owlia.nes 
/**** 459 mapper=30 prgrom=32K chrram=8K mirror=V battery=1 */
    0x3AE2A150, //  Modern\8 Bit Xmas 2008꞉ Christmas Songs (cartridge).nes 
/**** 460 mapper=30 prgrom=128K chrram=32K mirror=V battery=1 */
    0x19EB1773, //  Bad Dumps\8 Bit Xmas 2016 [bad PRG].nes 
    0x9E8ECA2C, //  Homebrew\E.T. [Khan Games].nes 
    0x9ED5BE48, //  Modern\8 Bit Xmas 2008꞉ Christmas Songs (8 Bit Xmas 2017 extract).nes 
    0xBD071E27, //  Modern\8 Bit Xmas 2009꞉ Snowball Fight (8 Bit Xmas 2017 extract).nes 
    0x9E0F20B7, //  Modern\8 Bit Xmas 2010꞉ Jolly Joyriding (8 Bit Xmas 2017 extract).nes 
    0xA21DD50F, //  Modern\8 Bit Xmas 2011꞉ Fireplace Bash (8 Bit Xmas 2017 extract).nes 
    0xBE1FF39B, //  Modern\8 Bit Xmas 2016 (8 Bit Xmas 2017 extract).nes 
    0x6ACF425D, //  Modern\8 Bit Xmas 2016 (cartridge).nes 
    0xBAD62C7B, //  Modern\8 Bit Xmas 2020꞉ Dr. Covio.nes 
    0x2543CEE8, //  Modern\8 Bit Xmas 2021꞉ Exciteduck (cartridge).nes 
/**** 461 mapper=30 prgrom=512K chrram=8K mirror=V battery=1 system=SYSTEM_NTSC */
    0x9383C016, //  Unreleased\Dungeons and DoomKnights (ROM 240).nes 
    0x4371849D, //  Unreleased\Dungeons and DoomKnights (ROM 241).nes 
    0x0C5BAF4B, //  Unreleased\Dungeons and DoomKnights (ROM 243).nes 
/**** 462 mapper=30 prgrom=512K chrram=32K mirror=V battery=1 */
    0x122C1413, //  Homebrew\AES_Atlas (0_00w).nes 
    0x67F7EF59, //  Homebrew\Alex Adventure.nes 
    0x36C360FA, //  Homebrew\Carver's Epic Quest!.nes 
    0x2FE9559A, //  Homebrew\Chrono Knight [Artix Games].nes 
    0x8B7D802A, //  Homebrew\Copter Bot.nes 
    0x68EF4929, //  Homebrew\Crap Job!! [Oniric Factor].nes 
    0x05E8C5F8, //  Homebrew\D-Society (Demo).nes 
    0xF9DDDEFF, //  Homebrew\Desolate Desert.nes 
    0x11A8A0D8, //  Homebrew\Dimension Drive (death animations).nes 
    0xE663D775, //  Homebrew\Dimension Drive (now with blobs).nes 
    0xCB3C8A03, //  Homebrew\Doodle World [Nate Peters].nes 
    0x154FD27E, //  Homebrew\Fistin' Uranus (Demo).nes 
    0xAE995E01, //  Homebrew\Game.nes 
    0xF0D8FC84, //  Homebrew\Happy NES Year 2019.nes 
    0x52533F56, //  Homebrew\Harry Dwarf [Byte-off].nes 
    0x778E803E, //  Homebrew\Haunted House Adventure [M. Stave].nes 
    0x28FB3277, //  Homebrew\Jood the Adventurer (Demo).nes 
    0xA5B23471, //  Homebrew\Labyrinth of Yggdrasil.nes 
    0x1BF1BBAE, //  Homebrew\Leggite Luta Livre [John Vanderhoef].nes 
    0xBB033B3C, //  Homebrew\Mermay's Den꞉ Deep Sea Escape.nes 
    0x71E04844, //  Homebrew\Molex.nes 
    0x02196A3E, //  Homebrew\More Glider.nes 
    0x20775721, //  Homebrew\Mr Boot (Demo).nes 
    0xE3BF8EE6, //  Homebrew\Mr. Spooks Tall House.nes 
    0x68370478, //  Homebrew\My NESmaker Game.nes 
    0x85B6ADEF, //  Homebrew\Narman.nes 
    0x4D8B01FF, //  Homebrew\Nessy!! The NES Robot (v1.25)[Daniel T. Gaming].nes 
    0x30FFF6EB, //  Homebrew\Omega Driver.nes 
    0x601B54A1, //  Homebrew\Ploid (v2.0)[NAPE Games].nes 
    0xD98E4BDC, //  Homebrew\Plummet Challenge Game [Fista Productions].nes 
    0x4B3E3C2C, //  Homebrew\PressStartGame.nes 
    0xC4850BD0, //  Homebrew\Shadow꞉ An Adventure In Monochrome.nes 
    0xF6DF51D1, //  Homebrew\Skateboard vs. Aliens 3.nes 
    0x4D916209, //  Homebrew\Slow Mole (v1.1)[Erik Rosenlund].nes 
    0xB41FB3CD, //  Homebrew\Stellarator (Demo).nes 
    0x2B5951AC, //  Homebrew\Super Beta Bros. 3.nes 
    0xD9CC183A, //  Homebrew\Super G!  The good one.nes 
    0x9E0EE3DD, //  Homebrew\The Ninja of the 4 Seasons [Kool Doob].nes 
    0xB557E32C, //  Homebrew\The Tower of Turmoil (v1.01).nes 
    0x2E86F4C2, //  Homebrew\The Tower of Turmoil (v1.03).nes 
    0x7BB4D931, //  Homebrew\Tobey the Turtle in Turtle Rescue (Demo).nes 
    0x1CE7E290, //  Homebrew\Trollburner (Demo).nes 
    0x1A3234F7, //  Homebrew\Untitled (Kidnapped By Aliens).nes 
    0xE34E720F, //  Homebrew\Vector Run - A DJ LCM Game.nes 
    0xF90B5433, //  Homebrew\Vector Run - The Trilogy.nes 
    0x67CA0BA9, //  Homebrew\Vector Run.nes 
    0xCE423562, //  Homebrew\Wampus DX [John Vanderhoef].nes 
    0x51300A20, //  Homebrew\Wampus [John Vanderhoef].nes 
    0x61751839, //  Homebrew\Witches Weed 3.nes 
    0xDC877532, //  Homebrew\Witches Weed.nes 
    0xE355DC71, //  Modern\Bat Lizard Bonanza.nes 
    0xFE52AFFC, //  Modern\Battery Chad꞉ Shock the World.nes 
    0x2DF8DE62, //  Modern\Carpet Shark.nes 
    0x7545EA58, //  Modern\Flea!.nes 
    0x921A09AC, //  Modern\Ghoul Grind Night of the Necromancer.nes 
    0xE797AB97, //  Modern\Shera &amp; the 40 Thieves [CGT Games].nes 
    0xB322A3EE, //  Modern\Spirit Impel.nes 
    0xC8B95138, //  Modern\The Magnilo Case.nes 
    0xCA55DB46, //  Samples\Flea!.nes 
    0xDE342FED, //  Unreleased\Flea!.nes 
/**** 463 mapper=30 prgrom=512K chrram=32K mirror=V battery=1 system=SYSTEM_PAL */
    0xF5FDD0EF, //  Homebrew\Kubo.nes 
    0x0FC7D1FE, //  Homebrew\Underground Adventure.nes 
/**** 464 mapper=30 prgrom=512K chrram=32K mirror=V battery=1 system=SYSTEM_NTSC */
    0x0BA0737B, //  Homebrew\Babel Blox.nes 
    0x7C982E51, //  Homebrew\Nix꞉ The Paradox Relic v0.2 [Dustin Huddleston].nes 
    0x6F13574F, //  Homebrew\Nix꞉ The Paradox Relic v1.9 [Dustin Huddleston].nes 
    0x3D095DC7, //  Homebrew\Solaris.nes 
    0x4DBB5CAB, //  Homebrew\Underground Adventure [Dale Coop].nes 
    0x2CD0A4A5, //  Homebrew\Wart Worm Wingding [John Vanderhoef].nes 
    0xE22284A8, //  Modern\Dungeons &amp; DoomKnights.nes 
    0x7835083C, //  Samples\Dungeons and DoomKnights (Beta).nes 
/**** 465 mapper=30 submapper=1 prgrom=256K chrram=8K mirror=H */
    0x891C14BC, //  Modern\Mega Man II (30th Anniversary Edition).nes 
/**** 466 mapper=33 prgrom=128K chrrom=128K mirror=H */
    0x4925313F, //  Homebrew\Translations\English\Insector X [Stardust Crusaders].nes 
    0x2C1D7EFC, //  Homebrew\Translations\English\Power Blazer [Stardust Crusaders].nes 
    0xF80BDC50, //  Licensed Japan\Insector X.nes 
    0x2A6559A1, //  Licensed Japan\Operation Wolf.nes 
    0xAEB7FCE9, //  Licensed Japan\Power Blazer.nes 
    0xD920F9DF, //  Licensed Japan\たけしの戦国風雲児.nes 
    0x3CD4B420, //  Unreleased\たけしの戦国風雲児.nes 
/**** 467 mapper=33 prgrom=128K chrrom=256K mirror=H */
    0x7A497AE3, //  Licensed Japan\Don Doko Don.nes 
    0xBACA10A9, //  Licensed Japan\Golfっ子 Open.nes 
    0x8A7D0ABE, //  Licensed Japan\アキラ - Akira.nes 
    0xBC7B1D0F, //  Licensed Japan\爆笑!! 人生劇場 2.nes 
/**** 468 mapper=33 prgrom=256K chrrom=128K mirror=H */
    0xADF606F6, //  Licensed Japan\爆笑!! 人生劇場.nes 
/**** 469 mapper=33 prgrom=256K chrrom=256K mirror=H */
    0x9D8C987E, //  Homebrew\Translations\English\アキラ - Akira (v0.99)[Grimm Translations].nes 
    0xD1206D8E, //  Homebrew\Translations\English\アキラ - Akira (v0.995)[Grimm Translations].nes 
/**** 470 mapper=34 prgrom=128K chrram=8K mirror=V system=SYSTEM_NTSC */
    0xAE1A88A9, //  Educational Computers\2合1 小百科 精选教育启思卡꞉  嘟嘟哇哇大冒险 &amp; 智力拼图.nes 
/**** 471 mapper=34 prgrom=128K prgram=8K chrrom=32K mirror=V */
    0x2B95E895, //  Compatibility Hacks\Nesticle\Dragon Ball꞉ Dragon Mystery (v1.11)[TransBRC].nes 
    0xBDBC6E8F, //  Compatibility Hacks\Nesticle\Dragon Ball꞉ Dragon's Mystery (Goku Gi)[pacnsacdave].nes 
    0x393FE0C2, //  Compatibility Hacks\Nesticle\Dragon Ball꞉ Dragon's Mystery (Roshi Gi)[pacnsacdave].nes 
    0x1E2CB8CC, //  Compatibility Hacks\Nesticle\Dragon Ball꞉ 神龍の謎.nes 
/**** 472 mapper=34 prgrom=512K chrram=8K mirror=V */
    0x043E2545, //  Modern\Haunted Halloween '85.nes 
/**** 473 mapper=34 prgrom=512K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x68546B2F, //  Modern\Lizard (English).nes 
    0x0F9333BD, //  Modern\Lizard (French).nes 
    0x4FE78041, //  Samples\Lizard (Demo 2).nes 
/**** 474 mapper=34 submapper=1 prgrom=64K prgram=8K chrrom=64K mirror=V */
    0x92A3D007, //  Unlicensed North America\Impossible Mission 2.nes 
/**** 475 mapper=34 submapper=2 prgrom=128K chrram=8K mirror=H */
    0xA21E675C, //  Licensed Japan\魔鐘.nes 
    0xC2730C30, //  Licensed North America\Deadly Towers.nes 
/**** 476 mapper=34 submapper=2 prgrom=32K chrram=8K mirror=V */
    0x5B8D4378, //  Homebrew\Nnnnnn.nes 
/**** 477 mapper=34 submapper=2 prgrom=64K chrram=8K mirror=V */
    0x3446EAE7, //  Homebrew\Super Homebrew War (BNROM, 2018-11-08).nes 
    0x274FAE3E, //  Homebrew\Super Homebrew War (BNROM, 2018-11-21).nes 
    0x9E7590BA, //  Homebrew\Super Homebrew War (BNROM, 2019-01-04).nes 
/**** 478 mapper=34 submapper=2 prgrom=128K chrram=8K mirror=V */
    0x58011551, //  Compatibility Hacks\TV System Conversions\西天取经 - Journey to the West (NTSC)[NewRisingSun].nes 
/**** 479 mapper=34 submapper=2 prgrom=128K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x368C19A8, //  Educational Computers\Три в одной на английском и русском.nes 
    0x162F1311, //  Unlicensed Taiwan&amp;Hong Kong\西天取经 - Journey to the West.nes 
/**** 480 mapper=34 submapper=2 prgrom=128K chrram=32K mirror=V system=SYSTEM_NTSC */
    0xAC5C1840, //  Homebrew\2-in-1 Geminim-Siamond.nes 
/**** 481 mapper=34 submapper=2 prgrom=256K chrram=8K mirror=V */
    0x1E36D20A, //  Homebrew\Multicarts\Action 53 Games (Build 0.02).nes 
    0xE645B187, //  Homebrew\Multicarts\Action 53 Games Midwest Gaming Classic 2012 (2 Mbit).nes 
    0xF407377E, //  Samples\Lizard (Demo 1).nes 
/**** 482 mapper=34 submapper=2 prgrom=512K chrram=8K mirror=V */
    0xCDF55296, //  Homebrew\Multicarts\Action 53 Games (Build 0.03).nes 
    0xAD9F8ED6, //  Homebrew\Multicarts\Action 53 Games Midwest Gaming Classic 2012 (4 Mbit).nes 
/**** 483 mapper=34 submapper=2 prgrom=512K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x0C48BADE, //  Modern\Project Blue.nes 
/**** 484 mapper=66 prgrom=64K chrrom=8K mirror=H system=SYSTEM_NTSC */
    0xC03B9077, //  Homebrew\Journey to the Center of the Alien [Mojon Twins].nes 
/**** 485 mapper=66 prgrom=64K chrrom=32K mirror=H */
    0xE128E92D, //  Compatibility Hacks\迷魂車 - Jovial Race [m066].nes 
    0x5B1CDD48, //  Homebrew\Hacks\The Real Ghostbusters Remastered [Nesrocks].nes 
    0x14F477C3, //  Unlicensed Taiwan&amp;Hong Kong\工藤ひとみ、紺野麻美のA.V. 麻雀 Club.nes 
/**** 486 mapper=66 prgrom=128K chrrom=32K mirror=H */
    0x9F056212, //  Bootleg Singles\Bung Game Doctor\(H001) Mobile Suit Z Gundam꞉ Hot Scramble.nes 
    0x0BB5B3A0, //  Licensed Japan\Family Block.nes 
    0xCE07194F, //  Licensed Japan\Mobile Suit Z Gundam꞉ Hot Scramble.nes 
    0xD80B44BC, //  Licensed North America\Thunder &amp; Lightning.nes 
/**** 487 mapper=66 prgrom=32K chrrom=32K mirror=V */
    0xD996AB4E, //  Bootleg Singles\U-Force Power Games (NintendoAge).nes 
    0xB459EDC4, //  Unreleased\U-Force Power Games.nes 
/**** 488 mapper=66 prgrom=64K chrrom=16K mirror=V */
    0xD26EFD78, //  Licensed North America\NES Action Set.nes 
/**** 489 mapper=66 prgrom=64K chrrom=16K mirror=V system=SYSTEM_PAL */
    0x91B4B1D7, //  Licensed PAL\NES Action Set.nes 
/**** 490 mapper=66 prgrom=64K chrrom=16K mirror=V system=SYSTEM_NTSC */
    0xFD6C31AF, //  Homebrew\Lala the Magical.nes 
/**** 491 mapper=66 prgrom=64K chrrom=64K mirror=V */
    0x63D78AA7, //  Compatibility Hacks\Policeman [m066].nes 
    0x5AECC042, //  Unlicensed Taiwan&amp;Hong Kong\Magic Bubble.nes 
/**** 492 mapper=66 prgrom=128K chrrom=32K mirror=V */
    0x738275D9, //  Bootleg Singles\Bung Game Doctor\(H002) Dragon Ball꞉ 神龍の謎.nes 
    0xD538669C, //  Bootleg Singles\Bung Game Doctor\(H003) ドラえもん.nes 
    0x9FBBB70B, //  Bootleg Singles\Bung Game Doctor\(H006) 妖怪 Club.nes 
    0x0BE5995C, //  Bootleg Singles\Bung Game Doctor\Thunder &amp; Lightning.nes 
    0x1DA88F85, //  Bootleg Singles\Front Fareast Magic Card\(F3002) Gumshoe.nes 
    0x36C23E9C, //  Homebrew\Hacks\Dragon Ball꞉ Dragon's Mystery (Goku Gi)[pacnsacdave].nes 
    0xB241B0D1, //  Homebrew\Hacks\Dragon Ball꞉ Dragon's Mystery (Roshi Gi)[pacnsacdave].nes 
    0x47045397, //  Homebrew\Translations\English\Doraemon [Neokid v1.1].nes 
    0x159A133F, //  Homebrew\Translations\English\Doraemon [Sky Yoshi].nes 
    0xA0EBB886, //  Homebrew\Translations\English\Dragon Ball꞉ Dragon Mystery？ [TransBRC v1.11].nes 
    0x62ED58BF, //  Homebrew\Translations\English\Hudson's Bug Honey on Adventure Island [FlashPV].nes 
    0xF6B11D60, //  Homebrew\Translations\English\Paris-Dakar Rally Special! [MrRichard999 v1.02].nes 
    0x9552E8DF, //  Licensed Japan\Dragon Ball꞉ 神龍の謎.nes 
    0x5CD5FDA4, //  Licensed Japan\Family Trainer꞉ 風雲!! たけし城 2.nes 
    0x5F6E8A07, //  Licensed Japan\Paris-Dakar Rally Special!.nes 
    0xBDE3AE9B, //  Licensed Japan\ドラえもん (rev0).nes 
    0x336093EF, //  Licensed Japan\ドラえもん (rev1).nes 
    0x2526C943, //  Licensed Japan\高橋名人のBugってHoney.nes 
    0x811F06D9, //  Licensed North America\Dragon Power.nes 
/**** 493 mapper=66 prgrom=128K chrrom=32K mirror=V system=SYSTEM_PAL */
    0x4F3B2E57, //  Licensed PAL\Dragon Ball꞉ Le Secret du Dragon (rev0).nes 
    0xB0BC46D1, //  Licensed PAL\Dragon Ball꞉ Le Secret du Dragon (rev1).nes 
/**** 494 mapper=66 prgrom=128K chrrom=32K mirror=V system=SYSTEM_NTSC */
    0xBEB8AB01, //  Licensed North America\Gumshoe.nes 
/**** 495 mapper=66 prgrom=128K chrrom=128K mirror=V system=SYSTEM_NTSC */
    0xC49F6407, //  Compatibility Hacks\Strike Wolf [m066].nes 
    0xD5BCF1E5, //  Compatibility Hacks\Strike Wolf [m066][h Zapper Detection].nes 
/**** 496 mapper=69 prgrom=128K mirror=H */
    0xFCF799D3, //  Bad Dumps\Forple [no CHR].nes 
/**** 497 mapper=69 prgrom=128K chrrom=128K mirror=H */
    0x93EE763F, //  Homebrew\Hacks\Batman꞉ The Video Game (unknown hack).nes 
    0x0236EA28, //  Homebrew\Translations\English\Pyokotan's Big Maze [MrRichard999].nes 
    0x4D7859A9, //  Licensed Japan\Batman꞉ The Video Game.nes 
    0x72928698, //  Licensed Japan\Hebereke.nes 
    0x4339865C, //  Licensed Japan\ぴょこたんの大迷路.nes 
    0x37A0C0E5, //  Modern\Virtual Console\Hebereke.nes 
    0x1A15E253, //  Unreleased\Forple.nes 
/**** 498 mapper=69 prgrom=128K chrrom=256K mirror=H */
    0xA59467AC, //  Homebrew\Translations\English\Dan's Dodgeball [TransGen v1.0b].nes 
    0x0897021B, //  Licensed Japan\Gremlins 2꞉ 新種誕生.nes 
    0x4C049CFE, //  Licensed Japan\炎の闘球児꞉ Dodge 弾平.nes 
/**** 499 mapper=69 prgrom=128K prgram=8K chrrom=256K mirror=H */
    0xB06ADBC8, //  Bootleg Hacks\Batman 2.nes 
    0x0FF6A3B5, //  Licensed Japan\Dynamite Batman.nes 
    0x03EC46AF, //  Licensed North America\Batman꞉ Return of The Joker.nes 
    0x29DD37F4, //  Unreleased\Batman꞉ Return of The Joker.nes 
/**** 500 mapper=69 prgrom=128K prgram=8K chrrom=256K mirror=H system=SYSTEM_PAL */
    0xBA327FD9, //  Licensed PAL\Batman꞉ Return of The Joker.nes 
/**** 501 mapper=69 prgrom=256K chrrom=128K mirror=H */
    0xC35497E4, //  Bad Dumps\Gimmick! (Memorial Series)[PRG bad].nes 
    0x0A00EF55, //  Homebrew\Translations\English\Gimmick! [LucianoTheWindowsFan].nes 
    0x4FFD424C, //  Homebrew\Translations\English\Hebereke [BMF54123].nes 
    0x0D65E7C7, //  Licensed Japan\Gimmick!.nes 
    0xE661918C, //  Unreleased\Gimmick!.nes 
    0x67FC2E40, //  Unreleased\Mr. Gimmick.nes 
/**** 502 mapper=69 prgrom=256K chrrom=128K mirror=H system=SYSTEM_PAL */
    0xA713DD30, //  Licensed PAL\Mr. Gimmick.nes 
/**** 503 mapper=69 prgrom=128K prgnvram=32K chrram=8K mirror=H battery=1 */
    0xD8D39A25, //  Homebrew\Böbl (v1.2).nes 
/**** 504 mapper=69 prgrom=128K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x48CA0EE1, //  Licensed Japan\Barcode World.nes 
/**** 505 mapper=69 prgrom=256K prgnvram=8K chrrom=256K mirror=H battery=1 */
    0x55C9C5B4, //  Homebrew\Translations\English\Dan's Dodgeball 2 [TransGen v1.0a].nes 
    0xA4E935DF, //  Licensed Japan\炎の闘球児꞉ Dodge 弾平 2.nes 
/**** 506 mapper=71 prgrom=64K chrram=8K mirror=V */
    0x9D03A5B9, //  Multicarts\extracts\Baseball Pro's (Quattro Sports).nes 
/**** 507 mapper=71 prgrom=64K chrram=8K mirror=V system=SYSTEM_NTSC */
    0xBD75ED79, //  Bootleg Singles\BMX Simulator (B-S2).nes 
    0xC469EC55, //  Compatibility Hacks\BMX Simulator (B-S2)[multicart check removed].nes 
    0x40E1F09E, //  Compatibility Hacks\BMX Simulator [controller read].nes 
    0x514770A0, //  Multicarts\extracts\BMX Simulator (Quattro Sports).nes 
    0xD27F6A2A, //  Multicarts\extracts\Boomerang Kid (Quattro Adventure).nes 
    0xF3086346, //  Multicarts\extracts\C.J.'s Elephant Antics (Quattro Arcade).nes 
    0x14003573, //  Multicarts\extracts\F-16 Renegade (Quattro Arcade).nes 
    0xEA72E419, //  Multicarts\extracts\Go! Dizzy Go! (Quattro Arcade).nes 
    0x1C3F54E3, //  Multicarts\extracts\Pro Tennis Simulator (Quattro Sports).nes 
    0xA53BC455, //  Multicarts\extracts\Soccer Simulator (Quattro Sports).nes 
    0x6DA10DAC, //  Multicarts\extracts\Stunt Buggies (Quattro Arcade).nes 
    0xA4D96251, //  Multicarts\extracts\Super Robin Hood (Quattro Adventure).nes 
    0x20B20C18, //  Multicarts\extracts\Treasure Island Dizzy (Quattro Adventure).nes 
    0x6C93377C, //  Unlicensed North America\Bee 52.nes 
    0x98430F49, //  Unreleased\Panic! Dizzy.nes 
/**** 508 mapper=71 prgrom=128K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x210EFCDC, //  Bootleg Singles\Super Robin Hood.nes 
    0xDB99D0CB, //  Unlicensed North America\Aladdin Compact Cartridge\Dizzy the Adventurer.nes 
    0xE62E3382, //  Unlicensed North America\MiG-29 Soviet Fighter.nes 
    0x3A990EE0, //  Unlicensed North America\Stunt Kids.nes 
    0x63D38B86, //  Unreleased\Dreamworld Pogie (1993-10-19).nes 
    0xA9630578, //  Unreleased\Dreamworld Pogie (2016).nes 
    0x2735FB3F, //  Unreleased\Dreamworld Pogie (2017).nes 
    0x4F299FC8, //  Unreleased\Dreamworld Pogie (xxxx).nes 
/**** 509 mapper=71 prgrom=256K chrram=8K mirror=V */
    0x96F1A437, //  Homebrew\Bugfixes, Improvements\Big Nose Freaks Out (rev1)[expansion controller].nes 
    0x9235B57B, //  Unlicensed North America\Micro Machines.nes 
    0xF62B0327, //  Unreleased\Big Nose and the Witchdoctor.nes 
/**** 510 mapper=71 prgrom=256K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x767F468C, //  Bootleg Singles\Bung Game Doctor\Ultimate Stuntman.nes 
    0x4F74E236, //  Modern\Wonderland Dizzy.nes 
    0x70F31D2C, //  Unlicensed Elsewhere\Cosmic Spacehead.nes 
    0xDB1FD64E, //  Unlicensed Elsewhere\Micro Machines (rev1).nes 
    0x9429B2F6, //  Unlicensed Elsewhere\The Fantastic Adventures of Dizzy.nes 
    0x9E379698, //  Unlicensed North America\Aladdin Compact Cartridge\Linus Spacehead's Cosmic Crusade.nes 
    0x24BA12DD, //  Unlicensed North America\Aladdin Compact Cartridge\Micro Machines.nes 
    0xF732C8FD, //  Unlicensed North America\Aladdin Compact Cartridge\The Fantastic Adventures of Dizzy.nes 
    0x5B2B72CB, //  Unlicensed North America\Big Nose Freaks Out (rev0).nes 
    0xCCDCBFC6, //  Unlicensed North America\Big Nose Freaks Out (rev1).nes 
    0xBD154C3E, //  Unlicensed North America\Big Nose the Caveman.nes 
    0x38FBCC85, //  Unlicensed North America\The Fantastic Adventures of Dizzy.nes 
    0x892434DD, //  Unlicensed North America\Ultimate Stuntman.nes 
    0xF520845F, //  Unreleased\Mystery World Dizzy (alt).nes 
    0x2EAFD5A9, //  Unreleased\Mystery World Dizzy.nes 
    0x2C7440EF, //  Unreleased\Wonderland Dizzy.nes 
/**** 511 mapper=71 submapper=1 prgrom=128K chrram=8K mirror=H system=SYSTEM_NTSC */
    0x1BC686A8, //  Unlicensed North America\Fire Hawk.nes 
/**** 512 mapper=71 submapper=1 prgrom=128K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x7ABB81FD, //  Unlicensed Elsewhere\Fire Hawk.nes 
/**** 513 mapper=76 prgrom=128K chrrom=128K mirror=V */
    0x5393D949, //  Licensed Japan\Digital Devil Story꞉ 女神転生.nes 
    0x0FC21FE5, //  Modern\Namco Museum Archives\Digital Devil Story꞉ 女神転生.nes 
/**** 514 mapper=76 prgrom=256K chrrom=128K mirror=V */
    0xA7B0717E, //  Homebrew\Translations\English\Digital Devil Story꞉ Megami Tensei [EsperKnight, Tom, Pennywise].nes 
    0x27433951, //  Homebrew\Translations\English\Digital Devil Story꞉ 女神転生 [EsperKnight, Tom, Pennywise, Masaru].nes 
/**** 515 mapper=78 submapper=1 prgrom=128K chrrom=128K mirror=H */
    0xCDEBA71E, //  Homebrew\Translations\English\宇宙船 - Cosmo Carrier [Gil Galad v0.25].nes 
    0x3D1C3137, //  Licensed Japan\宇宙船 - Cosmo Carrier.nes 
/**** 516 mapper=78 submapper=3 prgrom=128K chrrom=128K mirror=H */
    0x564F7161, //  Homebrew\Translations\English\Holy Diver [Monaco].nes 
    0xBA51AC6F, //  Licensed Japan\Holy Diver.nes 
/**** 517 mapper=79 prgrom=16K chrrom=16K mirror=H system=SYSTEM_NTSC */
    0xA2D074F5, //  Unlicensed Elsewhere\Lucky 777.nes 
    0xF5350410, //  Unlicensed Elsewhere\Sidewinder.nes 
/**** 518 mapper=79 prgrom=32K chrrom=16K mirror=H */
    0xA62B79E1, //  Bad Dumps\Sidewinder [PRG overdump].nes 
    0xDC8E49BF, //  Unlicensed Taiwan&amp;Hong Kong\Colorful Dragon.nes 
/**** 519 mapper=79 prgrom=32K chrrom=32K mirror=H */
    0xBD29178A, //  Unlicensed North America\Dudes with Attitude (rev0).nes 
    0x0955B54C, //  Unlicensed North America\Dudes with Attitude (rev1).nes 
    0x88A6B192, //  Unlicensed North America\F-15 City War (rev0).nes 
    0xF3841DCD, //  Unlicensed North America\F-15 City War (rev1).nes 
    0xEA19080A, //  Unlicensed North America\Puzzle.nes 
    0xC47EFC0E, //  Unlicensed North America\Trolls on Treasure Island.nes 
    0xD360D5F9, //  Unlicensed Taiwan&amp;Hong Kong\75 賓果.nes 
    0x80F39D59, //  Unreleased\Poke Block.nes 
    0x492FC509, //  Unreleased\Stakk'm.nes 
/**** 520 mapper=79 prgrom=32K chrrom=32K mirror=H system=SYSTEM_NTSC */
    0xF597E3B4, //  Unlicensed Elsewhere\75 Bingo.nes 
    0x3F2450EA, //  Unlicensed Elsewhere\Galactic Crusader.nes 
/**** 521 mapper=79 prgrom=32K chrrom=64K mirror=H */
    0x3BA4D2BB, //  Homebrew\Copyright Restorations\マリ・アヤミ・ルカのA.V. Poker.nes 
    0x22F8E680, //  Unlicensed Elsewhere\Futebol.nes 
    0xB6A727FA, //  Unlicensed Japan\Papillion Gals.nes 
    0xC594E134, //  Unlicensed Japan\マリ・アヤミ・ルカのA.V. Poker (rev0).nes 
    0x831F9C1A, //  Unlicensed North America\Ultimate League Soccer.nes 
    0xB790864C, //  Unlicensed Taiwan&amp;Hong Kong\マリ・アヤミ・ルカのA.V. Poker.nes 
/**** 522 mapper=79 prgrom=32K chrrom=64K mirror=H system=SYSTEM_NTSC */
    0xFFFDC310, //  Unlicensed Elsewhere\Ultimate League Soccer.nes 
/**** 523 mapper=79 prgrom=64K chrrom=64K mirror=H */
    0xD114F544, //  Unlicensed Japan\A.V. Super Real Pachinko.nes 
    0x850F25B3, //  Unlicensed Japan\工藤ひとみ、紺野麻美のA.V. 麻雀 Club (rev0).nes 
    0xC829007E, //  Unlicensed Japan\工藤ひとみ、紺野麻美のA.V. 麻雀 Club (rev1).nes 
    0x3D3FF543, //  Unlicensed Japan\風間ジュンと浅間夕子のA.V. Dragon 麻雀.nes 
    0x48239B42, //  Unlicensed Japan\麻雀 Companion (聖謙 3015 PCB).nes 
    0xB8DAD5D2, //  Unlicensed Japan\麻雀 Summit꞉ 歌舞伎町篇.nes 
/**** 524 mapper=79 prgrom=128K chrrom=128K mirror=H */
    0xA863645C, //  Bad Dumps\麻雀 Summit꞉ 歌舞伎町篇 [has data from other game].nes 
/**** 525 mapper=79 prgrom=16K chrrom=16K mirror=V */
    0x31AC927A, //  Unlicensed Taiwan&amp;Hong Kong\777 幸運輪盤.nes 
/**** 526 mapper=79 prgrom=32K chrrom=16K mirror=V system=SYSTEM_NTSC */
    0xF8C358D7, //  Unlicensed Taiwan&amp;Hong Kong\Millionaire.nes 
    0x68379FDB, //  Unlicensed Taiwan&amp;Hong Kong\Pipe V.nes 
/**** 527 mapper=79 prgrom=32K chrrom=32K mirror=V */
    0xC682E53C, //  Bad Dumps\Double Strike꞉ Aerial Attack Force (rev1) [bad CHR].nes 
    0x0554394F, //  Unlicensed Japan\Metal Fighter µ.nes 
    0xC2A4612E, //  Unlicensed North America\Blackjack.nes 
    0x36B35988, //  Unlicensed North America\Double Strike꞉ Aerial Attack Force (rev0).nes 
    0x1EB4A920, //  Unlicensed North America\Double Strike꞉ Aerial Attack Force (rev1).nes 
    0x85323FD6, //  Unlicensed North America\Krazy Kreatures (rev0).nes 
    0x637FE65C, //  Unlicensed North America\Krazy Kreatures (rev1).nes 
    0xBDE7A7B5, //  Unlicensed North America\Rad Racket - Deluxe Tennis II.nes 
    0xB6A2B981, //  Unlicensed North America\Solitaire.nes 
    0x3E1271D5, //  Unlicensed North America\Tiles of Fate.nes 
    0x882E1901, //  Unlicensed North America\Venice Beach Volleyball.nes 
    0xC1B79B14, //  Unlicensed Taiwan&amp;Hong Kong\双鷹 - Twin Eagle.nes 
    0x5E16861D, //  Unreleased\Rad Racket - Deluxe Tennis II.nes 
    0xC226157D, //  Unreleased\Venice Beach Volleyball.nes 
/**** 528 mapper=79 prgrom=32K chrrom=32K mirror=V system=SYSTEM_PAL */
    0xDDA190F9, //  Unlicensed Elsewhere\Twin Eagle.nes 
/**** 529 mapper=79 prgrom=32K chrrom=32K mirror=V system=SYSTEM_NTSC */
    0xEB4CCA31, //  Unlicensed Elsewhere\Master Chu &amp; The Drunkard Hu.nes 
    0x7739672E, //  Unlicensed Elsewhere\Metal Fighter µ.nes 
/**** 530 mapper=79 prgrom=64K chrrom=16K mirror=V system=SYSTEM_NTSC */
    0x58152B42, //  Bad Dumps\Pipe 5 [PRG overdump, bad PRG].nes 
    0xBBF464EB, //  Unlicensed Taiwan&amp;Hong Kong\Pyramid 2.nes 
/**** 531 mapper=79 prgrom=64K chrrom=32K mirror=V */
    0x6BD7047A, //  Unreleased\Robert Byrnes Pool Challenge.nes 
/**** 532 mapper=79 prgrom=64K chrrom=64K mirror=V */
    0xBEE54426, //  Unlicensed North America\Deathbots (rev0).nes 
    0x8EAB381C, //  Unlicensed North America\Deathbots (rev1).nes 
    0xF05870D5, //  Unlicensed North America\Mermaids of Atlantis꞉ The Riddle of the Magic Bubble.nes 
    0x257D09C5, //  Unlicensed Taiwan&amp;Hong Kong\突擊 - Raid (rev0).nes 
    0x3F5C53AD, //  Unlicensed Taiwan&amp;Hong Kong\突擊 - Raid (rev1).nes 
/**** 533 mapper=79 prgrom=64K chrrom=64K mirror=V system=SYSTEM_NTSC */
    0x1488E95F, //  Unlicensed Elsewhere\Silent Assault.nes 
/**** 534 mapper=85 prgrom=128K chrrom=128K mirror=H */
    0xB3FA69A9, //  Bootleg Hacks\Mario Bros. 14 Adventures.nes 
/**** 535 mapper=85 submapper=1 prgrom=128K chrrom=128K mirror=H */
    0xE4362167, //  Licensed Japan\Tiny Toon Adventures 2꞉ Montana Landへようこそ.nes 
/**** 536 mapper=85 submapper=1 prgrom=128K chrrom=256K mirror=H */
    0xA608DE02, //  Bootleg Singles\餓狼伝説 2.nes 
/**** 537 mapper=85 submapper=1 prgrom=256K chrrom=256K mirror=H */
    0x4933E97F, //  Bootleg Singles\Mortal Kombat V Pro.nes 
/**** 538 mapper=85 submapper=2 prgrom=512K prgnvram=8K chrram=8K mirror=H battery=1 */
    0x00F49381, //  Homebrew\Translations\English\Lagrange Point [Aeon Genesis v1.01].nes 
    0x743387FF, //  Licensed Japan\Lagrange Point.nes 
/**** 539 mapper=87 prgrom=32K chrrom=16K mirror=H */
    0xA4928409, //  Bootleg Hacks\Inventor\Van-1.nes 
    0x985B1D05, //  Licensed Japan\TwinBee.nes 
    0xBDA7925E, //  Licensed Japan\影の伝説.nes 
    0xC12E5842, //  Modern\Virtual Console\影の伝説.nes 
/**** 540 mapper=87 prgrom=16K chrrom=16K mirror=V */
    0xCF0A5C48, //  Homebrew\Translations\English\Ninja Jajamaru [Aishsha &amp; Stardust Crusaders].nes 
    0x20F98977, //  Licensed Japan\City Connection.nes 
    0xD97595A3, //  Licensed Japan\忍者じゃじゃ丸くん.nes 
    0x88EC55F0, //  Modern\じゃじゃ丸 Jr. 伝承記꞉ Jalecoもあり候う\City Connection.nes 
    0xA3369B82, //  Modern\じゃじゃ丸 Jr. 伝承記꞉ Jalecoもあり候う\忍者じゃじゃ丸くん.nes 
/**** 541 mapper=87 prgrom=32K chrrom=16K mirror=V */
    0x16221014, //  Bootleg Hacks\Futbol Colombiano.nes 
    0x656FA3B5, //  Licensed Japan\Argus.nes 
    0x883454EA, //  Licensed Japan\Choplifter! (rev0).nes 
    0x38EF66B5, //  Licensed Japan\Choplifter! (rev1).nes 
    0x21F85681, //  Licensed Japan\Hyper Olympic (限定版!!).nes 
    0xE6DF6616, //  Licensed Japan\The Goonies.nes 
    0xD20775DA, //  Modern\Virtual Console\City Connection (Japan).nes 
    0xEDF60D23, //  Modern\Virtual Console\忍者じゃじゃ丸くん.nes 
    0xAD0B71E2, //  Playchoice\The Goonies.nes 
/**** 542 mapper=87 prgrom=32K chrrom=32K mirror=V */
    0xC34E9802, //  Homebrew\Translations\English\Ninja Jajamaru's Big Adventure [Aishsha &amp; Stardust Crusaders].nes 
    0xD740B35C, //  Homebrew\Translations\English\Urusei Yatsura꞉ Lum's Wedding Bell [Stardust Crusaders].nes 
    0x0DA5E32E, //  Licensed Japan\うる星やつら꞉ LumのWedding Bell.nes 
    0x61B4295A, //  Licensed Japan\じゃじゃ丸の大冒険.nes 
    0x2B750BF9, //  Unreleased\うる星やつら꞉ LumのWedding Bell.nes 
/**** 543 mapper=118 prgrom=128K chrrom=128K mirror=H */
    0x0A90D4A3, //  Homebrew\Translations\English\Major League [FlashPV].nes 
    0x90C773C1, //  Licensed North America\Goal! Two.nes 
    0xB9B4D9E0, //  Licensed North America\NES Play Action Football.nes 
    0x41F9E0AA, //  Licensed North America\Pro Sport Hockey.nes 
/**** 544 mapper=118 prgrom=128K chrrom=128K mirror=H system=SYSTEM_PAL */
    0xD20BB617, //  Licensed PAL\Goal! 2.nes 
/**** 545 mapper=118 prgrom=128K chrrom=256K mirror=H */
    0x23F33D22, //  Modern\Virtual Console\NES Play Action Football.nes 
/**** 546 mapper=118 prgrom=256K chrrom=128K mirror=H */
    0xC4F9251A, //  Bootleg Hacks\超級兄弟 IV.nes 
    0xB3D92E78, //  Homebrew\Translations\English\Armadillo [Vice Translations].nes 
    0x78B657AC, //  Licensed Japan\Armadillo.nes 
/**** 547 mapper=118 prgrom=256K prgnvram=8K chrrom=128K mirror=H battery=1 */
    0x5C565F25, //  Homebrew\Translations\English\The Game of Life RPG [Nebulous Translations].nes 
    0x68064D76, //  Homebrew\Translations\English\Ys III꞉ Wanderers from Ys [Vice Translations].nes 
    0x07D92C31, //  Licensed Japan\RPG人生Game.nes 
    0x37B62D04, //  Licensed Japan\Ys III꞉ Wanderers from Ys.nes 
/**** 548 mapper=163 prgrom=2048K prgnvram=8K chrram=8K mirror=H battery=1 system=SYSTEM_NTSC */
    0x3869DB94, //  Bad Dumps\梁山英雄 [wrong PRG bank order].nes 
/**** 549 mapper=163 prgrom=512K prgnvram=8K chrram=8K mirror=V battery=1 system=SYSTEM_NTSC */
    0x4CE082F8, //  Bad Dumps\拳皇R-1꞉ 最强格斗王 [wrong PRG bank order].nes 
    0xBBAB3A61, //  Unlicensed China\南晶\拳皇R-1꞉ 最强格斗王.nes 
/**** 550 mapper=163 prgrom=1024K prgnvram=8K chrram=8K mirror=V battery=1 system=SYSTEM_NTSC */
    0x9D8AA034, //  Bad Dumps\Final Fantasy IV - 最终幻想4꞉ 光与暗 水晶纷争 [wrong PRG bank order].nes 
    0x63C41F82, //  Bad Dumps\World of Warcraft꞉ Demon Hunter [pacnsacdave][wrong PRG bank order].nes 
    0x9DE10A91, //  Bad Dumps\七龙珠大冒险 - Dragon Ball [wrong PRG bank order].nes 
    0xFC209609, //  Bad Dumps\三国大乱斗꞉ 战国 - Orochi [wrong PRG bank order].nes 
    0xC2B02B71, //  Bad Dumps\吞食天地 VI꞉ 刘备传 [wrong PRG bank order].nes 
    0x054444A0, //  Bad Dumps\圣斗士星矢꞉ 北欧突击篇 [wrong PRG bank order].nes 
    0x9BA518BA, //  Bad Dumps\宠物高达战记 [wrong PRG bank order].nes 
    0x57414FB6, //  Bad Dumps\武侠天地 [wrong PRG bank order].nes 
    0xEFF96E8A, //  Bad Dumps\魔兽世界꞉ 恶魔猎人 [wrong PRG bank order].nes 
    0xD7CD5EFD, //  Homebrew\Translations\English\World of Warcraft꞉ Demon Hunter [Pacnsacdave].nes 
    0x6F94C5E5, //  Unlicensed China\南晶\Final Fantasy IV - 最终幻想 4꞉ 光与暗 水晶纷争.nes 
    0xD7A4CBA5, //  Unlicensed China\南晶\七龙珠大冒险 - Dragon Ball.nes 
    0xDF27B96C, //  Unlicensed China\南晶\三国大乱斗꞉ 战国 - 無双 Orochi.nes 
    0x3A613060, //  Unlicensed China\南晶\吞食天地 VI꞉ 刘备传.nes 
    0xB35BE92F, //  Unlicensed China\南晶\圣斗士星矢꞉ 北欧突击篇.nes 
    0x9237C200, //  Unlicensed China\南晶\大话西游.nes 
    0x4FB02A43, //  Unlicensed China\南晶\天龙八部.nes 
    0x15E50ECD, //  Unlicensed China\南晶\宠物高达战记.nes 
    0x80A18CDC, //  Unlicensed China\南晶\机器人大战.nes 
    0x4752BD5E, //  Unlicensed China\南晶\柒國大戰.nes 
    0x695A7A70, //  Unlicensed China\南晶\楚汉风云.nes 
    0x222A136A, //  Unlicensed China\南晶\武侠天地.nes 
    0xD0807FD2, //  Unlicensed China\南晶\水浒 III.nes 
    0x2802E40F, //  Unlicensed China\南晶\汉刘邦.nes 
    0x4E3EDF88, //  Unlicensed China\南晶\超级机器人大战 A.nes 
    0xDA47B05A, //  Unlicensed China\南晶\隋唐英雄.nes 
    0xE08AB52E, //  Unlicensed China\南晶\魔兽世界꞉ 恶魔猎人.nes 
/**** 551 mapper=163 prgrom=2048K prgnvram=8K chrram=8K mirror=V battery=1 system=SYSTEM_NTSC */
    0x5E66E6C4, //  Bad Dumps\Naruto RPG - 火影忍者 [wrong PRG bank order].nes 
    0x696D98E3, //  Bad Dumps\三国志꞉ 吕布传 [wrong PRG bank order].nes 
    0x9F197F2B, //  Bad Dumps\三国群侠传 [wrong PRG bank order].nes 
    0x975F64E2, //  Bad Dumps\倚天屠龙记 [wrong PRG bank order].nes 
    0x915C5179, //  Bad Dumps\傲视三国志 [wrong PRG bank order].nes 
    0x8F9B2B38, //  Bad Dumps\农场小精灵 [wrong PRG bank order].nes 
    0x852BDB36, //  Bad Dumps\口袋宝石 - 红 [wrong PRG bank order].nes 
    0xB41CF445, //  Bad Dumps\口袋宝石 - 蓝 [wrong PRG bank order].nes 
    0x7829C3A9, //  Bad Dumps\口袋宝石 - 金 [wrong PRG bank order].nes 
    0xBC383C09, //  Bad Dumps\口袋水晶 [wrong PRG bank order].nes 
    0xA9C4712A, //  Bad Dumps\口袋钻石 [wrong PRG bank order].nes 
    0x2DA3A49C, //  Bad Dumps\时空斗士 - Pegasus Senya.nes 
    0x979239DE, //  Bad Dumps\毁灭之神 [wrong PRG bank order].nes 
    0x31C1BF98, //  Bad Dumps\牧场物语 [dumper graffiti].nes 
    0xC156ACEE, //  Homebrew\Translations\Chinese (Traditional)\三国志꞉ 曹操传 [小铭].nes 
    0x682D0F96, //  Homebrew\Translations\Chinese (Traditional)\真 Samurai Spirits - 武士道列传꞉ 侍魂.nes 
    0xF48A46C3, //  Homebrew\Translations\English\Chrono Trigger [Darrman].nes 
    0x609458B6, //  Homebrew\Translations\English\Final Fantasy VII [Lindblum v1.0].nes 
    0x33B208C0, //  Homebrew\Translations\English\Final Fantasy VII [Xaronyr, EnCardier v0.97].nes 
    0x7B923F26, //  Homebrew\Translations\English\Pokémon꞉ Yellow Version [Lugia2009].nes 
    0x9688AEEA, //  Unlicensed China\Subor\倚天传说.nes 
    0xC9ABA7F0, //  Unlicensed China\南晶\Chrono Trigger - 时空之轮.nes 
    0x33DB45BA, //  Unlicensed China\南晶\Final Fantasy 7 - 核心危机.nes 
    0x143B4D30, //  Unlicensed China\南晶\Final Fantasy VII - 最终幻想 7.nes 
    0xD6CBB05D, //  Unlicensed China\南晶\Naruto RPG - 火影忍者.nes 
    0x3CC55A44, //  Unlicensed China\南晶\The Legend of Zelda - 塞尔达传说꞉ 神奇的帽子.nes 
    0x1121C0D1, //  Unlicensed China\南晶\三国志 之 傲视天地.nes 
    0x8C60CECF, //  Unlicensed China\南晶\三国志꞉ 吕布传.nes 
    0x8E4294A9, //  Unlicensed China\南晶\三国志꞉ 曹操传.nes 
    0xF52468E7, //  Unlicensed China\南晶\三国无双꞉ 猛将传.nes 
    0x44AC9C8E, //  Unlicensed China\南晶\三国群侠传.nes 
    0xEF7BA485, //  Unlicensed China\南晶\仙剑奇侠.nes 
    0x20E1CF44, //  Unlicensed China\南晶\仙界精灵.nes 
    0x04DFE0D4, //  Unlicensed China\南晶\倚天剑传奇.nes 
    0xA3193C51, //  Unlicensed China\南晶\倚天屠龙记.nes 
    0x98266D3A, //  Unlicensed China\南晶\傲视三国志.nes 
    0x049DA14E, //  Unlicensed China\南晶\剑侠情缘 (南晶).nes 
    0xA026AE52, //  Unlicensed China\南晶\口袋宝石꞉ 红.nes 
    0xB6F72A18, //  Unlicensed China\南晶\口袋宝石꞉ 蓝.nes 
    0x5108AB7F, //  Unlicensed China\南晶\口袋宝石꞉ 金.nes 
    0x6CB6D619, //  Unlicensed China\南晶\口袋宝石꞉ 银.nes 
    0x8EB1B4CF, //  Unlicensed China\南晶\口袋水晶.nes 
    0xA66756AD, //  Unlicensed China\南晶\口袋钻石.nes 
    0x8C73A47B, //  Unlicensed China\南晶\哪吒传奇.nes 
    0xE40DA18F, //  Unlicensed China\南晶\圣剑传说.nes 
    0xC07CD2CE, //  Unlicensed China\南晶\圣斗士星矢꞉ 天马之幻想.nes 
    0x52D7FE18, //  Unlicensed China\南晶\太空幻想.nes 
    0xA01CA587, //  Unlicensed China\南晶\幻世録 - The Legend of Fancy Realm.nes 
    0x89F4ACD1, //  Unlicensed China\南晶\幻想传说.nes 
    0x65C63CC3, //  Unlicensed China\南晶\数码战队 II.nes 
    0x2C01DE06, //  Unlicensed China\南晶\数码暴龙.nes 
    0xB614AAA2, //  Unlicensed China\南晶\暗黑破坏神 - Diablo.nes 
    0x1B74A022, //  Unlicensed China\南晶\梦幻沙漏.nes 
    0xEBC6E2E2, //  Unlicensed China\南晶\毁灭之神.nes 
    0x524AF6E8, //  Unlicensed China\南晶\水浒神兽.nes 
    0xAC491507, //  Unlicensed China\南晶\游戏王.nes 
    0x9244E0BB, //  Unlicensed China\南晶\牧场物语.nes 
    0x191F7D5E, //  Unlicensed China\南晶\真 Samurai Spirits - 武士道列传꞉ 侍魂.nes 
    0x1FE67BB3, //  Unlicensed China\南晶\真 Samurai Spirits - 魂之利刃.nes 
    0x4973B16B, //  Unlicensed China\南晶\石器时代.nes 
    0x09C7AED3, //  Unlicensed China\南晶\葫芦金刚.nes 
    0x74C1EDC7, //  Unlicensed China\南晶\轩辕剑外传 之 天之痕.nes 
    0x723C6345, //  Unlicensed China\南晶\金庸群侠传꞉ 书剑江山.nes 
    0x056F2B8E, //  Unlicensed China\南晶\雷电皇꞉ Pikachu 传说.nes 
    0x04166E96, //  Unlicensed China\南晶\魔幻世界.nes 
    0x85FA53E1, //  Unlicensed China\南晶\魔界塔士.nes 
    0x2121DAB2, //  Unlicensed China\南晶\魔界霸主.nes 
    0xE3EF9739, //  Unlicensed China\南晶\黄金の太陽.nes 
/**** 552 mapper=206 prgrom=64K prgram=2K chrrom=32K mirror=4 */
    0xE45485A5, //  Vs. System\Atari R.B.I. Baseball (set 1).nes 
    0x8337E123, //  Vs. System\Atari R.B.I. Baseball (set 2).nes 
/**** 553 mapper=206 prgrom=64K prgram=2K chrrom=64K mirror=4 */
    0x52C501D0, //  Vs. System\Vs. T.K.O. Boxing.nes 
/**** 554 mapper=206 prgrom=128K prgram=2K chrrom=32K mirror=4 */
    0x12012CD9, //  Vs. System\Super Xevious꞉ Gampの謎.nes 
/**** 555 mapper=206 prgrom=128K chrrom=64K mirror=4 */
    0xCD50A092, //  Unlicensed North America\Gauntlet.nes 
/**** 556 mapper=206 prgrom=128K prgram=2K chrrom=64K mirror=4 */
    0x832CF592, //  Vs. System\Freedom Force.nes 
/**** 557 mapper=206 prgrom=32K chrrom=16K mirror=H */
    0xDB1131F0, //  Bootleg Singles\TouchGamePlayer\Super Chinese.nes 
    0x540B04EE, //  Multicarts\extracts\Space Shuttle Exploration (CB-4035).nes 
/**** 558 mapper=206 prgrom=32K chrrom=32K mirror=H */
    0x559DE7CA, //  Bootleg Singles\TouchGamePlayer\Babelの塔.nes 
    0x8AF25130, //  Licensed Japan\Babelの塔.nes 
    0xE9990228, //  Modern\Namco Museum Archives\Babelの塔.nes 
    0x55463622, //  Modern\Namco Museum Archives\Family Jockey.nes 
    0x145FEE2E, //  Modern\Namco Museum Archives\Valkyrieの冒険꞉ 時の鍵伝説.nes 
    0x2457A912, //  Modern\Virtual Console\Valkyrieの冒険꞉ 時の鍵伝説.nes 
/**** 559 mapper=206 prgrom=64K chrrom=64K mirror=H */
    0x0FFDE258, //  Unlicensed North America\Fantasy Zone.nes 
/**** 560 mapper=206 prgrom=128K chrrom=32K mirror=H */
    0x3CFEB4E1, //  Homebrew\Translations\English\Super Xevious꞉ The Riddle of GAMP [Psyklax].nes 
    0xF7762A20, //  Licensed Japan\Side Pocket.nes 
    0x7BB5664F, //  Licensed Japan\Super Xevious꞉ Gampの謎.nes 
    0x1A77388B, //  Modern\Namco Museum Archives\Super Xevious꞉ Gampの謎.nes 
/**** 561 mapper=206 prgrom=128K chrrom=64K mirror=H */
    0x56E0E6C6, //  Homebrew\Translations\English\Family Pinball [Suicidal Translations].nes 
    0x87C5CF7C, //  Homebrew\Translations\English\Karnov [Eien Ni Hen &amp; Vice Translations v1.10].nes 
    0x491D8CDB, //  Licensed Japan\Family Pinball.nes 
    0xC6557E02, //  Licensed Japan\Family 麻雀 (rev0).nes 
    0xF5CEEF8F, //  Licensed Japan\Family 麻雀 (rev1).nes 
    0xDD29FD59, //  Licensed Japan\Family 麻雀 II꞉ 上海への道.nes 
    0x267E592F, //  Licensed Japan\Karnov (rev0).nes 
    0x276237B3, //  Licensed Japan\Karnov (rev1).nes 
    0xF41ADD60, //  Licensed Japan\さんまの名探偵꞉ 改造品.nes 
    0xC372399B, //  Licensed Japan\源平討魔伝꞉ Computer Boardgame.nes 
    0x548A2C3C, //  Licensed North America\Karnov.nes 
    0x029594A1, //  Modern\Namco Museum Archives\Family Pinball.nes 
    0x139EB5B5, //  Unlicensed North America\Indiana Jones and the Temple of Doom.nes 
    0x5800BE2D, //  Unlicensed North America\Toobin'.nes 
    0x10D17B9B, //  Unlicensed South Korea\Toobin'.nes 
/**** 562 mapper=206 prgrom=32K chrrom=16K mirror=V system=SYSTEM_NTSC */
    0x4FF561BB, //  Homebrew\Hacks\Super Mario Bros. series\Randomize [Dot].nes 
    0xF558F82D, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros.꞉ 2014-12 Beta.nes 
/**** 563 mapper=206 prgrom=32K chrrom=32K mirror=V */
    0x235B71ED, //  Modern\Namco Museum Archives\Metro-Cross.nes 
    0x26CB086B, //  Modern\Namco Museum Archives\Sky Kid.nes 
    0xF49EBCFE, //  Modern\Virtual Console\Sky Kid.nes 
    0x48FD2FB7, //  Multicarts\extracts\Super City Tank.nes 
/**** 564 mapper=206 prgrom=64K chrrom=8K mirror=V system=SYSTEM_NTSC */
    0x5E25F869, //  Homebrew\Hacks\Super Mario Bros. series\Power Peach Sis. [SiWu].nes 
/**** 565 mapper=206 prgrom=64K chrrom=24K mirror=V system=SYSTEM_NTSC */
    0x7F5A6B56, //  Homebrew\Hacks\Super Mario Bros. series\Super Mario Bros. (animated background).nes 
/**** 566 mapper=206 prgrom=64K chrrom=32K mirror=V */
    0x5C5D8F8C, //  Homebrew\Hacks\R.B.I. Baseball (R.B.I Players' Circuit).nes 
    0x1300A8B7, //  Licensed Japan\Pro 野球꞉ Family Stadium '87.nes 
    0xDCDF06DE, //  Licensed Japan\Pro 野球꞉ Family Stadium.nes 
    0x3C5C81D4, //  Licensed North America\R.B.I. Baseball.nes 
    0xE73E7260, //  Unlicensed North America\Pac-Mania.nes 
    0x2E326A1D, //  Unlicensed North America\R.B.I. Baseball.nes 
    0xA8F5C2AB, //  Unlicensed North America\Vindicators.nes 
/**** 567 mapper=206 prgrom=64K chrrom=64K mirror=V */
    0xA8B7D027, //  Compatibility Hacks\Vs. TKO Boxing [home console v0.9b].nes 
    0xBD3D4EA8, //  Homebrew\Translations\English\Family Tennis [Goldenband v10-05-2015].nes 
    0x5B4C6146, //  Licensed Japan\Family Boxing.nes 
    0xA49253C6, //  Licensed Japan\Family Tennis.nes 
    0x5BB62688, //  Licensed North America\Ring King.nes 
    0x5F2C3195, //  Unlicensed North America\Super Sprint.nes 
/**** 568 mapper=206 prgrom=128K chrrom=32K mirror=V */
    0xCDB3424D, //  Homebrew\Translations\English\Family Circuit [MrRichard999 v0.95].nes 
    0x9CBC8253, //  Licensed Japan\Family Circuit.nes 
    0x2E563C66, //  Licensed Japan\Mappy-Land.nes 
    0x7C38CF24, //  Modern\Namco Museum Archives\Family Circuit.nes 
    0x2A4F30CA, //  Modern\Namco Museum Archives\Mappy-Land.nes 
/**** 569 mapper=206 prgrom=128K chrrom=64K mirror=V */
    0xAD66557F, //  Bootleg Singles\Lupin 三世꞉ Pandoraの遺産 (Cocot).nes 
    0x0955D16B, //  Homebrew\Hacks\Pokémon꞉ Pikachuの冒険 [Yuifa].nes 
    0x97C28E04, //  Homebrew\Translations\English\Dragon Buster II꞉ Seal of Darkness [PentarouZero].nes 
    0x8CDF2309, //  Homebrew\Translations\English\Dragon Slayer IV꞉ Drasle Family [Asiwish].nes 
    0x4A2CA81B, //  Homebrew\Translations\English\Famista '89꞉ Opening Game!! [MrRichard999, TheMajinZenki, AgentOrange, FlashPV v0.91].nes 
    0x21582E53, //  Homebrew\Translations\English\Famista '89꞉ Opening Game!! [MrRichard999, TheMajinZenki, AgentOrange, FlashPV v0.9].nes 
    0xA690AE9D, //  Homebrew\Translations\English\LaSalle Ishii's Childs Quest [Woolsey Fan Company, Polynim].nes 
    0x4DC2D2EB, //  Homebrew\Translations\English\Lupin the 3rd꞉ Pandora's Legacy [Vice Translations].nes 
    0x926D1CF4, //  Homebrew\Translations\English\Stardom Warriors [Woolsey Fan Company].nes 
    0x36F6D20B, //  Homebrew\Translations\English\The Quest of Ki [Zynk Oxhyde v2.0](+ version).nes 
    0x2FEA30C2, //  Homebrew\Translations\English\The Quest of Ki [Zynk Oxhyde v2.0].nes 
    0x4833A958, //  Homebrew\Translations\English\Wagyan Land [AlanMidas wag.yan].nes 
    0xD7AA0B6D, //  Licensed Japan\Dragon Buster II꞉ 闇の封印.nes 
    0xA5E6BAF9, //  Licensed Japan\Dragon Slayer 4꞉ Drasle Family.nes 
    0x4F2F1846, //  Licensed Japan\Famista '89꞉ 開幕版!!.nes 
    0xD97C31B0, //  Licensed Japan\LaSalle 石井のChild's Quest.nes 
    0x9D21FE96, //  Licensed Japan\Lupin 三世꞉ Pandoraの遺産.nes 
    0xAE321339, //  Licensed Japan\Pro 野球꞉ Family Stadium '88.nes 
    0xE1526228, //  Licensed Japan\The Quest of Ki.nes 
    0x2A01F9D1, //  Licensed Japan\ワギャン Land.nes 
    0x5397E80B, //  Licensed Japan\天下一武士꞉ ケルナグール.nes 
    0x22D6D5BD, //  Licensed Japan\時空勇伝 Debias.nes 
    0xC72DD5B4, //  Modern\Namco Museum Archives\Dragon Buster II꞉ 闇の封印.nes 
    0x65C9AA8D, //  Modern\Namco Museum Archives\The Quest of Ki.nes 
    0x7FA65186, //  Modern\Namco Museum Archives\ワギャン Land.nes 
    0x22D1427A, //  Modern\Namco Museum Archives\天下一武士꞉ ケルナグール.nes 
    0x96DFC776, //  Unlicensed North America\R.B.I. Baseball 2.nes 
    0xFD63E7AC, //  Unlicensed North America\R.B.I. Baseball 3.nes 
/**** 570 mapper=206 submapper=1 prgrom=32K prgram=2K chrrom=32K mirror=4 */
    0x21A653C7, //  Vs. System\Super Sky Kid.nes 
/**** 571 mapper=206 submapper=1 prgrom=32K chrrom=16K mirror=H */
    0x711C2B0E, //  Licensed Japan\Super Chinese.nes 
/**** 572 mapper=206 submapper=1 prgrom=32K chrrom=32K mirror=H */
    0xE03329D0, //  Homebrew\Translations\English\Family Jockey [MrRichard999].nes 
    0xCD527549, //  Homebrew\Translations\English\Valkyrie's Adventure꞉ Legend of the Time Key [DvD Translations revA].nes 
    0x3719A26D, //  Licensed Japan\Family Jockey.nes 
    0xC6ADD8C5, //  Licensed Japan\Valkyrieの冒険꞉ 時の鍵伝説.nes 
/**** 573 mapper=206 submapper=1 prgrom=32K chrrom=32K mirror=V */
    0xE40B4973, //  Licensed Japan\Metro-Cross.nes 
    0xCA6A7BF1, //  Licensed Japan\Sky Kid.nes 
/**** 574 mapper=210 submapper=1 prgrom=256K chrrom=256K mirror=H */
    0xBD523011, //  Licensed Japan\Dream Master.nes 
/**** 575 mapper=210 submapper=1 prgrom=512K chrrom=256K mirror=H */
    0x1696354C, //  Homebrew\Translations\English\Namco Prism Zone꞉ Dream Master [Aishsha v1.01].nes 
/**** 576 mapper=210 submapper=1 prgrom=128K chrrom=128K mirror=V */
    0xB9B2E109, //  Homebrew\Translations\English\Famista '91 [MrRichard999, TheMajinZenki, Jink640, FlashPV v0.991].nes 
    0x077A9F0E, //  Homebrew\Translations\English\Famista '91 [MrRichard999, TheMajinZenki, Jink640, FlashPV v0.99].nes 
    0x808606F0, //  Licensed Japan\Famista '91.nes 
    0x0C47946D, //  Licensed Japan\ちびまる子ちゃん꞉ うきうき Shopping.nes 
    0x81B7F1A8, //  Licensed Japan\平成天才バカボン.nes 
/**** 577 mapper=210 submapper=1 prgrom=256K chrrom=128K mirror=V */
    0x71C8243B, //  Homebrew\Translations\English\The Genius Bakabon [KingMike's Translations].nes 
/**** 578 mapper=210 submapper=1 prgrom=512K prgnvram=2K chrrom=128K mirror=V battery=1 */
    0xC247CC80, //  Licensed Japan\Family Circuit '91.nes 
/**** 579 mapper=210 submapper=2 prgrom=128K chrrom=128K mirror=V */
    0xB94C2CA8, //  Homebrew\Hacks\Evil Dead Ash Lives! [Crying Onion].nes 
    0x164C86A3, //  Homebrew\Translations\English\Famista '92 [MrRichard999, TheMajinZenki, Jink640, FlashPV v1.01].nes 
    0x370C849B, //  Homebrew\Translations\English\Famista '93 [MrRichard999, TheMajinZenki, Jink640, FlashPV v0.991].nes 
    0x7D3BB7C3, //  Homebrew\Translations\English\Famista '94 [MrRichard999, TheMajinZenki, FlashPV v1.01].nes 
    0xD0C50F17, //  Homebrew\Translations\English\Splatter House꞉ Super Deformed [Spinner 8 and friends v2.00].nes 
    0x6EC51DE5, //  Licensed Japan\Famista '92.nes 
    0xADFFD64F, //  Licensed Japan\Famista '93.nes 
    0x429103C9, //  Licensed Japan\Famista '94.nes 
    0x46FD7843, //  Licensed Japan\Splatterhouse꞉ わんぱくGraffiti.nes 
    0x2447E03B, //  Licensed Japan\Top Striker.nes 
    0x1595B7D5, //  Modern\Namco Museum Archives\Splatterhouse꞉ わんぱくGraffiti.nes 
/**** 580 mapper=210 submapper=2 prgrom=256K chrrom=128K mirror=V */
    0x97267E45, //  Homebrew\Translations\English\Wagyan Land 2 [Abstract Crouton Productions, Terminus Traduction].nes 
    0x7431D203, //  Homebrew\Translations\English\Wagyan Land 2 [Abstract Crouton Productions].nes 
    0x1DC0F740, //  Licensed Japan\ワギャン Land 2.nes 
    0x162B85B6, //  Modern\Virtual Console\ワギャン Land 2.nes 
/**** 581 mapper=210 submapper=2 prgrom=256K chrrom=256K mirror=V */
    0xD323B806, //  Licensed Japan\ワギャン Land 3.nes 
/**** 582 mapper=232 prgrom=256K chrram=8K mirror=V system=SYSTEM_NTSC */
    0xB462718E, //  Unlicensed Elsewhere\Super Sports Challenge (rev1).nes 
    0xB89888C9, //  Unlicensed North America\Quattro Adventure.nes 
    0x792070A9, //  Unlicensed North America\Quattro Arcade.nes 
    0xCCCAF368, //  Unlicensed North America\Quattro Sports.nes 
/**** 583 mapper=232 submapper=1 prgrom=256K chrram=8K mirror=V system=SYSTEM_NTSC */
    0x4B40CBD9, //  Unlicensed Elsewhere\Pegasus 4-in-1.nes 
    0x6A7BF037, //  Unlicensed Elsewhere\Super Adventure Quests.nes 
    0xA045FE1D, //  Unlicensed Elsewhere\Super Sports Challenge (rev0).nes 
};

static const u32 s_cart_crc_count[] = {
  /*   0 */  1,   2,  20,  10,   5, 196,  12,  27,   1,  45,   1,   4, 206,   7, 
  /*  14 */ 40,   3,   2,   4,   3,   2,   3,  22,   7,   1, 176,   3,  41,   2, 
  /*  28 */  1,   1,  10,   6,   1,   2, 300,  17, 192,   5,   2,   6,   2,   4, 
  /*  42 */  1,   1,   6,   1,   5,   1,   1,   1,   1,   1,   1,   1,   1,   2, 
  /*  56 */  1,   1,   7,   1,   1,   1,   2,   1,   1,   1,   3,   1,   7,   2, 
  /*  70 */ 30,   3,  24,   4,   5,  30,   4,   4,   2,   4,   1, 381,  80,   2, 
  /*  84 */ 15,  96,   9,  11,   2,   1,   2,   5,   1,   2,  11,   2,   2,   6, 
  /*  98 */  2,   1,   1,   1,   1,   3,   3,   1,   1,   5,   2,  25,   5,   4, 
  /* 112 */  1,   2,   2,  10,   3,   2,  88,   7,   1, 114,   8,   3,   8,   1, 
  /* 126 */ 10,  12,   2,   3,   1,  33,   6,   1,   3,   1,   5,   1,   1,   2, 
  /* 140 */  6,   1,   1,  14,   1,   6,   1,   2,   1,   1,  13,   1,  74,  11, 
  /* 154 */  1,  10,   1,   2,   1,   1,  11,  13,   1, 418,  47,   8,   2,  18, 
  /* 168 */  1,   2,   1,   1,   2,   1,   1,   1,   1,   3,   1,   1,   1,   8, 
  /* 182 */  1,   1,  18,   3,   1,  84,   9,   9,   3,   1,   1,   1,   1,  39, 
  /* 196 */  5, 200,   8,  22,   2,   3,   1,   2,   2,   2,   1,   2,   3,   1, 
  /* 210 */  1,  17,   1,   2,   3,  11,  19,   4,   1,   1,   2,   1,   1,   1, 
  /* 224 */  2,   1,   1,   4,   1,   8,   1,   1,   3,   4,   1,   2,   4,   1, 
  /* 238 */ 21,   2,   1,   2,   1, 489,  80,   3,  10,  68,  22,   1,   1,  79, 
  /* 252 */ 14,   1,   2,   5,   3,   1,   3, 176,   7,   1,  57,   3, 116,  13, 
  /* 266 */  6,  30,   6,  48,   2,   3,   1,   1,   1,   8,   4,   1,   1,   1, 
  /* 280 */  1,   3,   3,   2,   2,  50,   5,   2,   1,   2,  19,   1,   2,   2, 
  /* 294 */  2,   5,  85,   1,  86,   1,   2,  43,   1,  19,  14,   1,   7,  16, 
  /* 308 */  3,   1,   5,   2,   1,   1,   7,   8,   2,   1,   1,   1,   1,   1, 
  /* 322 */  1,   3,   7,   2,   1,   2,   2,   1,   2,   1,   1,   1,   2,   2, 
  /* 336 */  1,   8,   2,   3,   2,   2,   3,   2,   3,   3,   2,   1,   1,   1, 
  /* 350 */ 19,   5,   3,   2,   1,  15,   6,   1,  11,   7,   2,   7,   4,   2, 
  /* 364 */  2,   2,   8,   1,   1,   1,   1,   2,   3,   4,   3,   5,  35,   1, 
  /* 378 */ 14,   8,   5,   2,   2,  11,  16,   2,   4,   3,   2,   8,   1,   8, 
  /* 392 */  2,   1,   2,   1,   1,   3,   2,   4,   2,   7,   5,   5,   2,   1, 
  /* 406 */  1,   2,   2,   5,   3,   2,   1,   4,   2,  24,   1,   1,   1,   1, 
  /* 420 */  1,   1,   1,   5,   8,   1,   2,   1,   4,   3,   2,   1,   2,   7, 
  /* 434 */  2,   2,   1,   2,   1,   4,   1,   1,   1,   2,   1,   1,   1,   2, 
  /* 448 */  3,   2,   1,   1,   4,   1,   1,   3,   1,   1,   4,   1,  10,   3, 
  /* 462 */ 60,   2,   8,   1,   7,   4,   1,   2,   1,   4,   1,   3,   1,   2, 
  /* 476 */  1,   3,   1,   2,   1,   3,   2,   1,   1,   3,   4,   2,   1,   1, 
  /* 490 */  1,   2,  19,   2,   1,   2,   1,   7,   3,   4,   1,   6,   1,   1, 
  /* 504 */  1,   2,   1,  15,   8,   3,  16,   1,   1,   2,   2,   2,   2,   2, 
  /* 518 */  2,   9,   2,   6,   1,   6,   1,   1,   2,  14,   1,   2,   2,   1, 
  /* 532 */  5,   1,   1,   1,   1,   1,   2,   4,   5,   9,   5,   4,   1,   1, 
  /* 546 */  3,   4,   1,   2,  27,  68,   2,   1,   1,   1,   1,   2,   6,   1, 
  /* 560 */  4,  15,   2,   4,   1,   1,   7,   6,   5,  28,   1,   1,   4,   2, 
  /* 574 */  1,   1,   5,   1,   1,  11,   4,   1,   4,   3, 
};

