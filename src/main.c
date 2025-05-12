#include "eadk.h"
#include <stdio.h>

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Periodic";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

#define LCD_HEIGHT_PX 222
#define TOOLBAR_HEIGHT_PX 18

// table periodique, d'apres https://bitbucket.org/m4x1m3/nw-atom/src/master/
// par M4x1m3 https://tiplanet.org/forum/viewtopic.php?f=102&t=23054
enum AtomType {
  ALKALI_METAL,
  ALKALI_EARTH_METAL,
  LANTHANIDE,
  ACTINIDE,
  TRANSITION_METAL,
  POST_TRANSITION_METAL,
  METALLOID,
  HALOGEN,
  REACTIVE_NONMETAL,
  NOBLE_GAS,
  UNKNOWN
};

struct AtomDef {
  uint8_t num;
  uint8_t x;
  uint8_t y;
  enum AtomType type;
  const char* name;
  const char* symbol;
  uint8_t neutrons;
  double mass;
  double electroneg;
};

const struct AtomDef atomsdefs[] = {
  {  1,  0,  0, REACTIVE_NONMETAL       , "Hydrogen"     , "H"   ,   0, 1.00784     , 2.2   },
  {  2, 17,  0, NOBLE_GAS               , "Helium"       , "He"  ,   2, 4.002602    , -1    },
  {  3,  0,  1, ALKALI_METAL            , "Lithium"      , "Li"  ,   4, 6.938       , 0.98  },
  {  4,  1,  1, ALKALI_EARTH_METAL      , "Beryllium"    , "Be"  ,   5, 9.012182    , 1.57  },
  {  5, 12,  1, METALLOID               , "Boron"        , "B"   ,   6, 10.806      , 2.04  },
  {  6, 13,  1, REACTIVE_NONMETAL       , "Carbon"       , "C"   ,   6, 12.0096     , 2.55  },
  {  7, 14,  1, REACTIVE_NONMETAL       , "Nitrogen"     , "N"   ,   7, 14.00643    , 3.04  },
  {  8, 15,  1, REACTIVE_NONMETAL       , "Oxygen"       , "O"   ,   8, 15.99903    , 3.44  },
  {  9, 16,  1, HALOGEN                 , "Fluorine"     , "F"   ,  10, 18.9984032  , 3.98  },
  { 10, 17,  1, NOBLE_GAS               , "Neon"         , "Ne"  ,  10, 20.1797     , -1    },
  { 11,  0,  2, ALKALI_METAL            , "Sodium"       , "Na"  ,  12, 22.9897693  , 0.93  },
  { 12,  1,  2, ALKALI_EARTH_METAL      , "Magnesium"    , "Mg"  ,  12, 24.3050     , 1.31  },
  { 13, 12,  2, POST_TRANSITION_METAL   , "Aluminium"    , "Al"  ,  14, 26.9815386  , 1.61  },
  { 14, 13,  2, METALLOID               , "Silicon"      , "Si"  ,  14, 28.084      , 1.9   },
  { 15, 14,  2, REACTIVE_NONMETAL       , "Phosphorus"   , "P"   ,  16, 30.973762   , 2.19  },
  { 16, 15,  2, REACTIVE_NONMETAL       , "Sulfur"       , "S"   ,  16, 32.059      , 2.58  },
  { 17, 16,  2, HALOGEN                 , "Chlorine"     , "Cl"  ,  18, 35.446      , 3.16  },
  { 18, 17,  2, NOBLE_GAS               , "Argon"        , "Ar"  ,  22, 39.948      , -1    },
  { 19,  0,  3, ALKALI_METAL            , "Potassium"    , "K"   ,  20, 39.0983     , 0.82  },
  { 20,  1,  3, ALKALI_EARTH_METAL      , "Calcium"      , "Ca"  ,  20, 40.078      , 1     },
  { 21,  2,  3, TRANSITION_METAL        , "Scandium"     , "Sc"  ,  24, 44.955912   , 1.36  },
  { 22,  3,  3, TRANSITION_METAL        , "Titanium"     , "Ti"  ,  26, 47.867      , 1.54  },
  { 23,  4,  3, TRANSITION_METAL        , "Vanadium"     , "V"   ,  28, 50.9415     , 1.63  },
  { 24,  5,  3, TRANSITION_METAL        , "Chromium"     , "Cr"  ,  28, 51.9961     , 1.66  },
  { 25,  6,  3, TRANSITION_METAL        , "Manganese"    , "Mn"  ,  30, 54.938045   , 1.55  },
  { 26,  7,  3, TRANSITION_METAL        , "Iron"         , "Fe"  ,  30, 55.845      , 1.83  },
  { 27,  8,  3, TRANSITION_METAL        , "Cobalt"       , "Co"  ,  32, 58.933195   , 1.88  },
  { 28,  9,  3, TRANSITION_METAL        , "Nickel"       , "Ni"  ,  31, 58.6934     , 1.91  },
  { 29, 10,  3, TRANSITION_METAL        , "Copper"       , "Cu"  ,  35, 63.546      , 1.9   },
  { 30, 11,  3, POST_TRANSITION_METAL   , "Zinc"         , "Zn"  ,  35, 65.38       , 1.65  },
  { 31, 12,  3, POST_TRANSITION_METAL   , "Gallium"      , "Ga"  ,  39, 69.723      , 1.81  },
  { 32, 13,  3, METALLOID               , "Germanium"    , "Ge"  ,  41, 72.63       , 2.01  },
  { 33, 14,  3, METALLOID               , "Arsenic"      , "As"  ,  42, 74.92160    , 2.18  },
  { 34, 15,  3, REACTIVE_NONMETAL       , "Selenium"     , "Se"  ,  45, 78.96       , 2.55  },
  { 35, 16,  3, HALOGEN                 , "Bromine"      , "Br"  ,  45, 79.904      , 2.96  },
  { 36, 17,  3, NOBLE_GAS               , "Krypton"      , "Kr"  ,  48, 83.798      , -1    },
  { 37,  0,  4, ALKALI_METAL            , "Rubidium"     , "Rb"  ,  20, 85.4678     , 0.82  },
  { 38,  1,  4, ALKALI_EARTH_METAL      , "Strontium"    , "Sr"  ,  20, 87.62       , 0.95  },
  { 39,  2,  4, TRANSITION_METAL        , "Yttrium"      , "Y"   ,  24, 88.90585    , 1.22  },
  { 40,  3,  4, TRANSITION_METAL        , "Zirconium"    , "Zr"  ,  26, 91.224      , 1.33  },
  { 41,  4,  4, TRANSITION_METAL        , "Niobium"      , "Nb"  ,  28, 92.90638    , 1.6   },
  { 42,  5,  4, TRANSITION_METAL        , "Molybdenum"   , "Mo"  ,  28, 95.96       , 2.16  },
  { 43,  6,  4, TRANSITION_METAL        , "Technetium"   , "Tc"  ,  30, 98          , 2.10  },
  { 44,  7,  4, TRANSITION_METAL        , "Ruthemium"    , "Ru"  ,  30, 101.07      , 2.2   },
  { 45,  8,  4, TRANSITION_METAL        , "Rhodium"      , "Rh"  ,  32, 102.90550   , 2.28  },
  { 46,  9,  4, TRANSITION_METAL        , "Palladium"    , "Pd"  ,  31, 106.42      , 2.20  },
  { 47, 10,  4, TRANSITION_METAL        , "Silver"       , "Ag"  ,  35, 107.8682    , 1.93  },
  { 48, 11,  4, POST_TRANSITION_METAL   , "Cadmium"      , "Cd"  ,  35, 112.411     , 1.69  },
  { 49, 12,  4, POST_TRANSITION_METAL   , "Indium"       , "In"  ,  39, 114.818     , 1.78  },
  { 50, 13,  4, POST_TRANSITION_METAL   , "Tin"          , "Sn"  ,  41, 118.710     , 1.96  },
  { 51, 14,  4, METALLOID               , "Antimony"     , "Sb"  ,  42, 121.760     , 2.05  },
  { 52, 15,  4, METALLOID               , "Tellurium"    , "Te"  ,  45, 127.60      , 2.1   },
  { 53, 16,  4, HALOGEN                 , "Indine"       , "I"   ,  45, 126.90447   , 2.66  },
  { 54, 17,  4, NOBLE_GAS               , "Xenon"        , "Xe"  ,  48, 131.293     , 2.60  },
  { 55,  0,  5, ALKALI_METAL            , "Caesium"      , "Cs"  ,  78, 132.905452  , 0.79  },
  { 56,  1,  5, ALKALI_EARTH_METAL      , "Barium"       , "Ba"  ,  81, 137.327     , 0.89  },
  { 57,  3,  7, LANTHANIDE              , "Lanthanum"    , "La"  ,  82, 138.90547   , 1.10  },
  { 58,  4,  7, LANTHANIDE              , "Cerium"       , "Ce"  ,  82, 140.116     , 1.12  },
  { 59,  5,  7, LANTHANIDE              , "Praseodymium" , "Pr"  ,  82, 140.90765   , 1.13  },
  { 60,  6,  7, LANTHANIDE              , "Neodymium"    , "Nd"  ,  84, 144.242     , 1.14  },
  { 61,  7,  7, LANTHANIDE              , "Promethium"   , "Pm"  ,  84, 145         , 1.13  },
  { 62,  8,  7, LANTHANIDE              , "Samarium"     , "Sm"  ,  88, 150.36      , 1.17  },
  { 63,  9,  7, LANTHANIDE              , "Europium"     , "Eu"  ,  89, 151.964     , 1.12  },
  { 64, 10,  7, LANTHANIDE              , "Gadolinium"   , "Gd"  ,  93, 157.25      , 1.20  },
  { 65, 11,  7, LANTHANIDE              , "Terbium"      , "Tb"  ,  94, 158.92535   , 1.12  },
  { 66, 12,  7, LANTHANIDE              , "Dyxprosium"   , "Dy"  ,  97, 162.500     , 1.22  },
  { 67, 13,  7, LANTHANIDE              , "Holmium"      , "Ho"  ,  98, 164.93032   , 1.23  },
  { 68, 14,  7, LANTHANIDE              , "Erbium"       , "Er"  ,  99, 167.259     , 1.24  },
  { 69, 15,  7, LANTHANIDE              , "Thulium"      , "Tm"  , 100, 168.93421   , 1.25  },
  { 70, 16,  7, LANTHANIDE              , "Ytterbium"    , "Yb"  , 103, 173.054     , 1.1   },
  { 71, 17,  7, LANTHANIDE              , "Lutetium"     , "Lu"  , 104, 174.9668    , 1.0   },
  { 72,  3,  5, TRANSITION_METAL        , "Hafnium"      , "Hf"  , 106, 178.49      , 1.3   },
  { 73,  4,  5, TRANSITION_METAL        , "Tantalum"     , "Ta"  , 108, 180.94788   , 1.5   },
  { 74,  5,  5, TRANSITION_METAL        , "Tungsten"     , "W"   , 110, 183.84      , 1.7   },
  { 75,  6,  5, TRANSITION_METAL        , "Rhenium"      , "Re"  , 111, 186.207     , 1.9   },
  { 76,  7,  5, TRANSITION_METAL        , "Osmium"       , "Os"  , 114, 190.23      , 2.2   },
  { 77,  8,  5, TRANSITION_METAL        , "Iridium"      , "Ir"  , 115, 192.217     , 2.2   },
  { 78,  9,  5, TRANSITION_METAL        , "Platinum"     , "Pt"  , 117, 195.084     , 2.2   },
  { 79, 10,  5, TRANSITION_METAL        , "Gold"         , "Au"  , 118, 196.966569  , 2.4   },
  { 80, 11,  5, POST_TRANSITION_METAL   , "Mercury"      , "Hg"  , 121, 200.59      , 1.9   },
  { 81, 12,  5, POST_TRANSITION_METAL   , "Thalium"      , "Tl"  , 123, 204.382     , 1.8   },
  { 82, 13,  5, POST_TRANSITION_METAL   , "Lead"         , "Pb"  , 125, 207.2       , 1.8   },
  { 83, 14,  5, POST_TRANSITION_METAL   , "Bismuth"      , "Bi"  , 126, 208.98040   , 1.9   },
  { 84, 15,  5, POST_TRANSITION_METAL   , "Polonium"     , "Po"  , 125, 209         , 2.0   },
  { 85, 16,  5, HALOGEN                 , "Astatine"     , "At"  , 125, 210         , 2.2   },
  { 86, 17,  5, NOBLE_GAS               , "Radon"        , "Rn"  , 136, 222         , 2.2   },
  { 87,  0,  6, ALKALI_METAL            , "Francium"     , "Fr"  , 136, 223         , 0.7   },
  { 88,  1,  6, ALKALI_EARTH_METAL      , "Radium"       , "Ra"  , 138, 226         , 0.9   },
  { 89,  3,  8, ACTINIDE                , "Actinium"     , "Ac"  , 138, 227         , 1.1   },
  { 90,  4,  8, ACTINIDE                , "Thorium"      , "Th"  , 142, 232.03806   , 1.3   },
  { 91,  5,  8, ACTINIDE                , "Protactinium" , "Pa"  , 140, 231.03588   , 1.5   },
  { 92,  6,  8, ACTINIDE                , "Uranium"      , "U"   , 146, 238.02891   , 1.38  },
  { 93,  7,  8, ACTINIDE                , "Neptunium"    , "Np"  , 144, 237         , 1.36  },
  { 94,  8,  8, ACTINIDE                , "Plutonium"    , "Pu"  , 150, 244         , 1.28  },
  { 95,  9,  8, ACTINIDE                , "Americium"    , "Am"  , 148, 243         , 1.13  },
  { 96, 10,  8, ACTINIDE                , "Curium"       , "Cm"  , 151, 247         , 1.28  },
  { 97, 11,  8, ACTINIDE                , "Berkellum"    , "Bk"  , 150, 247         , 1.3   },
  { 98, 12,  8, ACTINIDE                , "Californium"  , "Cf"  , 153, 251         , 1.3   },
  { 99, 13,  8, ACTINIDE                , "Einsteinium"  , "Es"  , 153, 252         , 1.3   },
  {100, 14,  8, ACTINIDE                , "Fermium"      , "Fm"  , 157, 257         , 1.3   },
  {101, 15,  8, ACTINIDE                , "Mendelevium"  , "Md"  , 157, 258         , 1.3   },
  {102, 16,  8, ACTINIDE                , "Nobelium"     , "No"  , 157, 259         , 1.3   },
  {103, 17,  8, ACTINIDE                , "Lawrencium"   , "Lr"  , 159, 262         , 1.3   },
  {104,  3,  6, TRANSITION_METAL        , "Rutherfordium", "Rf"  , 157, 261         , -1    },
  {105,  4,  6, TRANSITION_METAL        , "Dubnium"      , "Db"  , 157, 262         , -1    },
  {106,  5,  6, TRANSITION_METAL        , "Seaborgium"   , "Sg"  , 157, 263         , -1    },
  {107,  6,  6, TRANSITION_METAL        , "Bohrium"      , "Bh"  , 157, 264         , -1    },
  {108,  7,  6, TRANSITION_METAL        , "Hassium"      , "Hs"  , 157, 265         , -1    },
  {109,  8,  6, UNKNOWN                 , "Meitnerium"   , "Mt"  , 159, 268         , -1    },
  {110,  9,  6, UNKNOWN                 , "Damstadtium"  , "Ds"  , 171, 281         , -1    },
  {111, 10,  6, UNKNOWN                 , "Roentgenium"  , "Rg"  , 162, 273         , -1    },
  {112, 11,  6, POST_TRANSITION_METAL   , "Coppernicium" , "Cn"  , 165, 277         , -1    },
  {113, 12,  6, UNKNOWN                 , "Nihonium"     , "Nh"  , 170, 283         , -1    },
  {114, 13,  6, UNKNOWN                 , "Flerovium"    , "Fl"  , 171, 285         , -1    },
  {115, 14,  6, UNKNOWN                 , "Moscovium"    , "Mv"  , 172, 287         , -1    },
  {116, 15,  6, UNKNOWN                 , "Livermorium"  , "Lv"  , 173, 289         , -1    },
  {117, 16,  6, UNKNOWN                 , "Tennessine"   , "Ts"  , 177, 294         , -1    },
  {118, 17,  6, NOBLE_GAS               , "Oganesson"    , "Og"  , 175, 293         , -1    },
};

void draw_rectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
  eadk_display_push_rect_uniform((eadk_rect_t){x, y + TOOLBAR_HEIGHT_PX, w, h}, color);
}

void stroke_rectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
  eadk_display_push_rect_uniform((eadk_rect_t){x, y + TOOLBAR_HEIGHT_PX, w, 1}, color);
  eadk_display_push_rect_uniform((eadk_rect_t){x, y + h -1 + TOOLBAR_HEIGHT_PX, w, 1}, color);
  eadk_display_push_rect_uniform((eadk_rect_t){x, y + TOOLBAR_HEIGHT_PX, 1, h}, color);
  eadk_display_push_rect_uniform((eadk_rect_t){x + w - 1, y + TOOLBAR_HEIGHT_PX, 1, h}, color);
}

void draw_string(int16_t x, int16_t y, const char *text) {
  eadk_display_draw_string(text, (eadk_point_t){x, y + TOOLBAR_HEIGHT_PX}, true, eadk_color_black, eadk_color_white);
}

void draw_string_small(int16_t x, int16_t y, uint16_t fg, uint16_t bg, const char *text) {
  eadk_display_draw_string(text, (eadk_point_t){x, y + TOOLBAR_HEIGHT_PX}, false, fg, bg);
}

int rgb24to16(int c) {
  int r=(c>>16)&0xff,g=(c>>8)&0xff,b=c&0xff;
  return (((r*32)/256)<<11) | (((g*64)/256)<<5) | (b*32/256);
}

void drawAtom(uint8_t id) {
  int fill = rgb24to16(0xeeeeee);

  switch(atomsdefs[id].type) {
    case ALKALI_METAL:
      fill = rgb24to16(0xffaa00);
      break;
    case ALKALI_EARTH_METAL:
      fill = rgb24to16(0xf6f200);
      break;
    case LANTHANIDE:
      fill = rgb24to16(0xffaa8b);
      break;
    case ACTINIDE:
      fill = rgb24to16(0xdeaacd);
      break;
    case TRANSITION_METAL:
      fill = rgb24to16(0xde999c);
      break;
    case POST_TRANSITION_METAL:
      fill = rgb24to16(0x9cbaac);
      break;
    case METALLOID:
      fill = rgb24to16(0x52ce8b);
      break;
    case REACTIVE_NONMETAL:
      fill = rgb24to16(0x00ee00);
      break;
    case NOBLE_GAS:
      fill = rgb24to16(0x8baaff);
      break;
    case HALOGEN:
      fill = rgb24to16(0x00debd);
      break;
    default:
      break;
  }

  if (atomsdefs[id].y >= 7) {
    draw_rectangle(6 + atomsdefs[id].x * 17, 15 + atomsdefs[id].y * 17, 18, 18, fill);
    stroke_rectangle(6 + atomsdefs[id].x * 17, 15 + atomsdefs[id].y * 17, 18, 18, rgb24to16(0x525552));
    draw_string_small(8 + atomsdefs[id].x * 17, 17 + atomsdefs[id].y * 17, eadk_color_black, fill, atomsdefs[id].symbol);
  } else {
    draw_rectangle(6 + atomsdefs[id].x * 17, 6 + atomsdefs[id].y * 17, 18, 18, fill);
    stroke_rectangle(6 + atomsdefs[id].x * 17, 6 + atomsdefs[id].y * 17, 18, 18, rgb24to16(0x525552));
    draw_string_small(8 + atomsdefs[id].x * 17, 8 + atomsdefs[id].y * 17, eadk_color_black, fill, atomsdefs[id].symbol);
  }
}

void copy(const char * text) {
  /*
  draw_string(130, 100, "Copie !");
  eadk_timing_msleep(500);
  */
}

/*---------------------------------------------------------------------------------------------------*/


int scroll_offset = 0;

void draw_text(const char *text, eadk_point_t start_point, eadk_color_t fg, eadk_color_t bg) {
  eadk_point_t point = start_point;
  point.y += 16;
  const size_t max_line_length = 40; 
  char line[max_line_length + 1]; 
  size_t line_length = 0; 
  int line_number = 1; 

  while (*text) {
    size_t word_end = line_length; 
    while (line_length < max_line_length && *text && *text != '\n') {
      if (*text == ' ') {
        word_end = line_length;
      }
      line[line_length++] = *text++;
    }

    if (line_length == max_line_length && *text && *text != ' ' && *text != '\n') {
      if (word_end > 0) {
        text -= (line_length - word_end); 
        line_length = word_end; 
      }
    }

    line[line_length] = '\0'; 
    eadk_display_draw_string(line, point, false, fg, bg);

    char line_number_str[10];
    snprintf(line_number_str, sizeof(line_number_str), "%d", line_number);
    eadk_display_draw_string(line_number_str, (eadk_point_t){EADK_SCREEN_WIDTH - 21, point.y}, false, fg, bg);

    point.y += 16; 
    line_length = 0; 
    line_number++; 

    while (*text == ' ') {
      text++;
    }

    if (*text == '\n') {
      text++;
    }
  }

  if (line_length > 0) {
    line[line_length] = '\0';
    eadk_display_draw_string(line, point, false, fg, bg);

    char line_number_str[10];
    snprintf(line_number_str, sizeof(line_number_str), "%d", line_number);
    eadk_display_draw_string(line_number_str, (eadk_point_t){EADK_SCREEN_WIDTH - 30, point.y}, false, fg, bg);
  }
}

/*---------------------------------------------------------------------------*/

void to_lowercase(const char* src, char* dest, size_t length) {
  for (size_t i = 0; i < length; i++) {
      dest[i] = tolower((unsigned char)src[i]);
  }
  dest[length] = '\0';
}

bool* search_in_external_data(const char* search_string, size_t* line_count) {
  if (eadk_external_data == NULL || eadk_external_data_size == 0 || search_string == NULL || strlen(search_string) == 0) {
      *line_count = 0;
      return NULL;
  }

  const char* data = eadk_external_data;
  const size_t max_line_length = 40; // Longueur maximale d'une ligne
  *line_count = 0;

  // Compter les lignes virtuelles
  size_t current_line_length = 0;
  for (size_t i = 0; i < eadk_external_data_size; i++) {
      current_line_length++;
      if (data[i] == '\n' || current_line_length >= max_line_length) {
          (*line_count)++;
          current_line_length = 0;
      }
  }
  if (current_line_length > 0) {
      (*line_count)++;
  }

  bool* apparitions = (bool*)calloc(*line_count, sizeof(bool));
  if (apparitions == NULL) {
      *line_count = 0;
      return NULL;
  }

  size_t current_line = 0;
  size_t line_start_index = 0;
  current_line_length = 0;

  for (size_t i = 0; i <= eadk_external_data_size; i++) {
      current_line_length++;
      if (data[i] == '\n' || current_line_length >= max_line_length || data[i] == '\0') {
          size_t line_length = i - line_start_index;
          char* line = (char*)malloc(line_length + 1);
          if (line == NULL) {
              free(apparitions);
              *line_count = 0;
              return NULL;
          }
          strncpy(line, &data[line_start_index], line_length);
          line[line_length] = '\0';

          // Convertir la ligne en minuscules
          char* lower_line = (char*)malloc(line_length + 1);
          if (lower_line == NULL) {
              free(apparitions);
              free(line);
              *line_count = 0;
              return NULL;
          }
          to_lowercase(line, lower_line, line_length);

          // Rechercher la chaîne dans la ligne convertie
          if (strstr(lower_line, search_string) != NULL) {
              apparitions[current_line] = true;
          }

          free(line);
          free(lower_line);
          line_start_index = i + 1;
          current_line++;
          current_line_length = 0;
      }
  }

  return apparitions;
}

void search() {
  eadk_display_push_rect_uniform((eadk_rect_t){0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT}, eadk_color_white);
  eadk_display_draw_string("Search :", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press exe to finish", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press back to quit", (eadk_point_t){0, 220}, false, eadk_color_black, eadk_color_white);

  char* search_string = input((eadk_point_t){0, 80}, true, true);
  if (strlen(search_string) == 0) {
      eadk_display_draw_string("Empty search string.", (eadk_point_t){0, 100}, false, eadk_color_black, eadk_color_white);
      eadk_timing_msleep(1000);
      return;
  }

  size_t line_count = 0;
  bool* apparitions = search_in_external_data(search_string, &line_count);

  if (apparitions != NULL) {
      const int lines_per_column = 10; 
      const int columns_per_page = 5;
      const int results_per_page = lines_per_column * columns_per_page; 
      int current_page = 0;

      size_t total_results = 0;
      for (size_t i = 0; i < line_count; i++) {
          if (apparitions[i]) {
              total_results++;
          }
      }

      int total_pages = (total_results + results_per_page - 1) / results_per_page;

      size_t* result_indices = (size_t*)malloc(total_results * sizeof(size_t));
      if (result_indices == NULL) {
          free(apparitions);
          return;
      }

      size_t result_index = 0;
      for (size_t i = 0; i < line_count; i++) {
          if (apparitions[i]) {
              result_indices[result_index++] = i;
          }
      }

      while (true) {
          eadk_display_push_rect_uniform((eadk_rect_t){0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT}, eadk_color_white);
          eadk_display_draw_string("Results:", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
          eadk_display_draw_string("Press left/right to change page", (eadk_point_t){0, 25}, false, eadk_color_black, eadk_color_white);
          eadk_display_draw_string("Press back to quit search", (eadk_point_t){0, 40}, false, eadk_color_black, eadk_color_white);

          char page_info[30];
          snprintf(page_info, sizeof(page_info), "Page %d/%d", current_page + 1, total_pages);
          eadk_display_draw_string(page_info, (eadk_point_t){EADK_SCREEN_WIDTH - 65, 0}, false, eadk_color_black, eadk_color_white);

          char result[50];
          int displayed_lines = 0;

          for (size_t i = current_page * results_per_page; i < total_results && displayed_lines < results_per_page; i++) {
              int column = displayed_lines / lines_per_column;
              int row = displayed_lines % lines_per_column;

              int x_position = column * 63;
              int y_position = 75 + row * 16; 

              snprintf(result, sizeof(result), "Line %d", (int)(result_indices[i] + 1));
              eadk_display_draw_string(result, (eadk_point_t){x_position, y_position}, false, eadk_color_black, eadk_color_white);

              displayed_lines++;
          }

          while (true) {
              eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan();
              if (eadk_keyboard_key_down(keyboard_state, eadk_key_back) || eadk_keyboard_key_down(keyboard_state, eadk_key_home)) {
                  free(apparitions);
                  free(result_indices);
                  return; 
              }
              if (eadk_keyboard_key_down(keyboard_state, eadk_key_left)) {
                  if (current_page > 0) {
                      current_page--; 
                      eadk_timing_msleep(300);
                  }
                  break;
              }
              if (eadk_keyboard_key_down(keyboard_state, eadk_key_right)) {
                  if (current_page < total_pages - 1) {
                      current_page++; 
                      eadk_timing_msleep(300);
                  }
                  break;
              }
              eadk_timing_msleep(100); 
          }
      }
  } else {
      eadk_display_draw_string("Error or no results.", (eadk_point_t){0, 75}, false, eadk_color_black, eadk_color_white);
      eadk_timing_msleep(2000);
  }
}

/*---------------------------------------------------------------------------*/

void go_to(){
  eadk_display_push_rect_uniform((eadk_rect_t){0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT}, eadk_color_white);
  eadk_display_draw_string("Go to line n:", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press exe to finish", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press back to quit", (eadk_point_t){0, 220}, false, eadk_color_black, eadk_color_white);

  int line_number = input_int((eadk_point_t){0, 30}, true, true);
  if (line_number == 0) {
      return;
  }

  scroll_offset = (line_number - 1) * 16; 
  
  draw_text(eadk_external_data, (eadk_point_t){0, -scroll_offset}, eadk_color_black, eadk_color_white);

}

/*-------------------------------------------------------------------------*/

void reader() {
    eadk_rect_t full_screen_rect = {0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT};
  eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white);
  eadk_display_draw_string("Loading...", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
  eadk_timing_msleep(500);
  eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white);

  eadk_display_draw_string("The calculator has issues with third-party\napplications. Save your Python script before\nlaunching the app to avoid data loss. If it\ncrashes, reinstall the app.", (eadk_point_t){0, 0}, false, eadk_color_orange, eadk_color_white);


  eadk_display_draw_string("Press home to quit", (eadk_point_t){0, 80}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press shift to search", (eadk_point_t){0, 100}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press alpha to go to line n", (eadk_point_t){0, 120}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press up/down to scroll", (eadk_point_t){0, 140}, false, eadk_color_black, eadk_color_white);
  eadk_display_draw_string("Press left/right to scroll 10 lines", (eadk_point_t){0, 160}, false, eadk_color_black, eadk_color_white);

  eadk_display_draw_string("Press OK to start", (eadk_point_t){0, 200}, false, eadk_color_black, eadk_color_white);

  while (true){
    eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan();
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_ok) || eadk_keyboard_key_down(keyboard_state, eadk_key_home)) {
      eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white);
      break;
    }
  }

  bool refresh_screen = true;

  while (true) {
    eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan();
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_home)) {
      break; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_down)) {
      scroll_offset += 16; 
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_up)) {
      scroll_offset -= 16;
      if (scroll_offset < 0) {
        scroll_offset = 0; 
      }
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_right)) {
      scroll_offset += 160; 
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_left)) {
      scroll_offset -= 160;
      if (scroll_offset < 0) {
        scroll_offset = 0; 
      }
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_shift)) {
      search(); 
      refresh_screen = true; 
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_alpha)) {
      go_to(); 
      refresh_screen = true; 
    }

    if (refresh_screen) {
      eadk_display_push_rect_uniform((eadk_rect_t){0, 18, 320, 222}, eadk_color_white); 

      if (eadk_external_data != NULL && eadk_external_data_size > 0) {
        draw_text(eadk_external_data, (eadk_point_t){0, -scroll_offset}, eadk_color_black, eadk_color_white);

        eadk_rect_t titre = {0, 0, 320, 18};
        eadk_display_push_rect_uniform(titre, eadk_color_orange);
      
        eadk_display_draw_string("TXT-READER", (eadk_point_t){125, 3}, false, eadk_color_white, eadk_color_orange);
        
      } else {
        eadk_display_draw_string("No external data", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
      }
      refresh_screen = false; 
    }

    eadk_timing_msleep(100); 
  }
}

/*---------------------------------------------------------------------------------------------------*/

void main(int argc, char * argv[]) {
  int count = 0;
  char buf[128];
  bool partial_draw = false, redraw = true;
  int cursor_pos = 0;
  const int ATOM_NUMS = sizeof(atomsdefs) / sizeof(struct AtomDef);
  for (;;) {
    /*---------------------------------------------------------------------------------------------------*/
    if (cursor_pos == 5) {
      eadk_keyboard_state_t state = eadk_keyboard_scan();
      if (eadk_keyboard_key_down(state, eadk_key_nine)) {
        count++;
      }
      else {
        count = 0;
      }

      if (count == 5) {
        count = 0;
        reader();
      }
    }
    /*---------------------------------------------------------------------------------------------------*/
    if (redraw) {
      if (partial_draw) {
        for(int i = 0; i < ATOM_NUMS; i++) {
          drawAtom(i);
        }
        partial_draw = false;
        draw_rectangle(41, 0, 169, 57, eadk_color_white);
        draw_rectangle(0, 180, EADK_SCREEN_WIDTH, 14, eadk_color_white);
      } else {
        draw_rectangle(0, 0, EADK_SCREEN_WIDTH, LCD_HEIGHT_PX, eadk_color_white);
        for(int i = 0; i < ATOM_NUMS; i++) {
          drawAtom(i);
        }
      }
      // draw_string_small(0, 198, eadk_color_black, eadk_color_white, "Copier dans le presse papier");
      // draw_string_small(0, 210, eadk_color_black, eadk_color_white, "OK: tout, P:protons, N:nucleons, M:mass, E:khi");
      if (atomsdefs[cursor_pos].y >= 7) {
        stroke_rectangle(6 + atomsdefs[cursor_pos].x * 17, 15 + atomsdefs[cursor_pos].y * 17, 18, 18, 0x000000);
        stroke_rectangle(7 + atomsdefs[cursor_pos].x * 17, 16 + atomsdefs[cursor_pos].y * 17, 16, 16, 0x000000);
      } else {
        stroke_rectangle(6 + atomsdefs[cursor_pos].x * 17, 6 + atomsdefs[cursor_pos].y * 17, 18, 18, 0x000000);
        stroke_rectangle(7 + atomsdefs[cursor_pos].x * 17, 7 + atomsdefs[cursor_pos].y * 17, 16, 16, 0x000000);
      }
      
      draw_rectangle(48,  99, 2, 61, rgb24to16(0x525552));
      draw_rectangle(48, 141, 9,  2, rgb24to16(0x525552));
      draw_rectangle(48, 158, 9,  2, rgb24to16(0x525552));

      draw_string(73, 23, atomsdefs[cursor_pos].symbol);
      draw_string_small(110, 27, eadk_color_black, eadk_color_white, atomsdefs[cursor_pos].name);
      sprintf(buf, "%d", atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num);
      draw_string_small(50, 18, eadk_color_black, eadk_color_white, buf);
      sprintf(buf, "%d", atomsdefs[cursor_pos].num);
      draw_string_small(50, 31, eadk_color_black, eadk_color_white, buf);
      /*
      sprintf(buf, "M : %f", atomsdefs[cursor_pos].mass);
      draw_string_small(0, 180, eadk_color_black, eadk_color_white, buf);  
      sprintf(buf, "khi : %f", atomsdefs[cursor_pos].electroneg);
      draw_string_small(160, 180, eadk_color_black, eadk_color_white, buf);
      */
    }
    redraw = false;
    int32_t timeout = 1000;
    eadk_event_t event = eadk_event_get(&timeout);
    if (event == eadk_event_left && cursor_pos > 0) {
      cursor_pos--;
      redraw = partial_draw = true;
    } else if (event == eadk_event_right && cursor_pos < ATOM_NUMS - 1) {
      cursor_pos++;
      redraw = partial_draw = true;
    } else if (event == eadk_event_up) {
      uint8_t curr_x = atomsdefs[cursor_pos].x;
      uint8_t curr_y = atomsdefs[cursor_pos].y;
      bool updated = false;

      if (curr_y > 0 && curr_y <= 9) {
        for(uint8_t i = 0; i < ATOM_NUMS; i++) {
          if (atomsdefs[i].x == curr_x && atomsdefs[i].y == curr_y - 1) {
            cursor_pos = i;
            redraw = partial_draw = true;
          }
        }
      }
    } else if (event == eadk_event_down) {
      uint8_t curr_x = atomsdefs[cursor_pos].x;
      uint8_t curr_y = atomsdefs[cursor_pos].y;
      bool updated = false;

      if (curr_y >= 0 && curr_y < 9) {
        for (uint8_t i = 0; i < ATOM_NUMS; i++) {
          if (atomsdefs[i].x == curr_x && atomsdefs[i].y == curr_y + 1) {
            cursor_pos = i;
            redraw = partial_draw = true;
            break;
          }
        }
      }
    } else if (event == eadk_event_ok || event == eadk_event_exe) {
      sprintf(buf, "%s,%d,%d,%f,%f", atomsdefs[cursor_pos].name, atomsdefs[cursor_pos].num, atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num, atomsdefs[cursor_pos].mass, atomsdefs[cursor_pos].electroneg);
      copy(buf);
      redraw = partial_draw = true;
    } else if (event == eadk_event_left_parenthesis) {
      sprintf(buf, "%d", atomsdefs[cursor_pos].num);
      copy(buf);
      redraw = partial_draw = true;
    } else if (event == eadk_event_eight) {
      sprintf(buf, "%d", atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num);
      copy(buf);
      redraw = partial_draw = true;
    } else if (event == eadk_event_seven) {
      sprintf(buf, "%f", atomsdefs[cursor_pos].mass);
      copy(buf);
      redraw = partial_draw = true;
    } else if (event == eadk_event_comma) {
      sprintf(buf, "%f", atomsdefs[cursor_pos].electroneg);
      copy(buf);
      redraw = partial_draw = true;
    }
  }
}
