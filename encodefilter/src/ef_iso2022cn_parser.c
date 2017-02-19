/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#include "ef_iso2022cn_parser.h"

#include <stdio.h> /* NULL */

#include "ef_iso2022_parser.h"

/* --- static functions --- */

static void iso2022cn_parser_init(ef_parser_t *parser) {
  ef_iso2022_parser_t *iso2022_parser;

  ef_parser_init(parser);

  iso2022_parser = (ef_iso2022_parser_t*)parser;

  iso2022_parser->g0 = US_ASCII;
  iso2022_parser->g1 = UNKNOWN_CS; /* CNS11643_1 or GB2312 */
  iso2022_parser->g2 = UNKNOWN_CS; /* CNS11643_2 */
  iso2022_parser->g3 = UNKNOWN_CS;

  iso2022_parser->gl = &iso2022_parser->g0;
  iso2022_parser->gr = NULL;

  iso2022_parser->non_iso2022_cs = UNKNOWN_CS;

  iso2022_parser->is_single_shifted = 0;
}

/* --- global functions --- */

ef_parser_t *ef_iso2022cn_parser_new(void) {
  ef_iso2022_parser_t *iso2022_parser;

  if ((iso2022_parser = ef_iso2022_parser_new()) == NULL) {
    return NULL;
  }

  iso2022cn_parser_init((ef_parser_t*)iso2022_parser);

  /* override */
  iso2022_parser->parser.init = iso2022cn_parser_init;

  return (ef_parser_t*)iso2022_parser;
}
