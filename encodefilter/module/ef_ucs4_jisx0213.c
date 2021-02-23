/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#include "../src/ef_ucs4_jisx0213.h"

#include "../src/ef_jis_property.h"

#ifdef USE_ICONV

#include "ef_iconv.h"

#else

#include "table/ef_jisx0213_2000_1_to_ucs4.table"
#include "table/ef_jisx0213_2000_2_to_ucs4.table"

#include "table/ef_ucs4_to_jisx0213_2000_1.table"
#include "table/ef_ucs4_to_jisx0213_2000_2.table"

#endif

/* --- global functions --- */

#ifdef USE_ICONV

int ef_map_jisx0213_2000_1_to_ucs4(ef_char_t *ucs4, u_int16_t jis) {
  static iconv_t cd;
  u_char src[2];

  ICONV_OPEN(cd, "UTF-32BE", "EUC-JISX0213");

  src[1] = ((jis & 0x7f) | 0x80);
  src[0] = (((jis >> 8) & 0x7f) | 0x80);

  ICONV(cd, src, 2, ucs4->ch, 4);

  ucs4->size = 4;
  ucs4->cs = ISO10646_UCS4_1;
  ucs4->property = 0;

  return 1;
}

int ef_map_jisx0213_2000_2_to_ucs4(ef_char_t *ucs4, u_int16_t jis) {
  static iconv_t cd;
  u_char src[3];

  ICONV_OPEN(cd, "UTF-32BE", "EUC-JISX0213");

  src[2] = ((jis & 0x7f) | 0x80);
  src[1] = (((jis >> 8) & 0x7f) | 0x80);
  src[0] = 0x8f;

  ICONV(cd, src, 3, ucs4->ch, 4);

  ucs4->size = 4;
  ucs4->cs = ISO10646_UCS4_1;
  ucs4->property = 0;

  return 1;
}

int ef_map_ucs4_to_jisx0213_2000_1(ef_char_t *jis, u_int32_t ucs4_code) {
  static iconv_t cd;

  ICONV_OPEN(cd, "EUC-JISX0213", "UTF-32");

  ICONV(cd, (char*)&ucs4_code, 4, jis->ch, 2);

  jis->size = 2;
  jis->cs = JISX0213_2000_1;
  jis->property = ef_get_jisx0213_2000_1_property(jis->ch);

  jis->ch[0] &= 0x7f;
  jis->ch[1] &= 0x7f;

  return 1;
}

int ef_map_ucs4_to_jisx0213_2000_2(ef_char_t *jis, u_int32_t ucs4_code) {
  static iconv_t cd;

  ICONV_OPEN(cd, "EUC-JISX0213", "UTF-32");

  ICONV(cd, (char*)&ucs4_code, 4, jis->ch, 3);

  jis->ch[0] = jis->ch[1] & 0x7f;
  jis->ch[1] = jis->ch[2] & 0x7f;

  jis->size = 2;
  jis->cs = JISX0213_2000_2;
  jis->property = 0;

  return 1;
}

#else

int ef_map_jisx0213_2000_1_to_ucs4(ef_char_t *ucs4, u_int16_t jis) {
  u_int32_t c;

  if ((c = CONV_JISX0213_2000_1_TO_UCS4(jis))) {
    ef_int_to_bytes(ucs4->ch, 4, c);
    ucs4->size = 4;
    ucs4->cs = ISO10646_UCS4_1;
    ucs4->property = 0;

    return 1;
  }

  return 0;
}

int ef_map_jisx0213_2000_2_to_ucs4(ef_char_t *ucs4, u_int16_t jis) {
  u_int32_t c;

  if ((c = CONV_JISX0213_2000_2_TO_UCS4(jis))) {
    ef_int_to_bytes(ucs4->ch, 4, c);
    ucs4->size = 4;
    ucs4->cs = ISO10646_UCS4_1;
    ucs4->property = 0;

    return 1;
  }

  return 0;
}

int ef_map_ucs4_to_jisx0213_2000_1(ef_char_t *jis, u_int32_t ucs4_code) {
  u_int16_t c;

  if ((c = CONV_UCS4_TO_JISX0213_2000_1(ucs4_code))) {
    ef_int_to_bytes(jis->ch, 2, c);
    jis->size = 2;
    jis->cs = JISX0213_2000_1;
    jis->property = ef_get_jisx0213_2000_1_property(jis->ch);

    return 1;
  }

  return 0;
}

int ef_map_ucs4_to_jisx0213_2000_2(ef_char_t *jis, u_int32_t ucs4_code) {
  u_int16_t c;

  if ((c = CONV_UCS4_TO_JISX0213_2000_2(ucs4_code))) {
    ef_int_to_bytes(jis->ch, 2, c);
    jis->size = 2;
    jis->cs = JISX0213_2000_2;
    jis->property = 0;

    return 1;
  }

  return 0;
}

#endif
