/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#ifndef __VT_MOTIF_DATA_H__
#define __VT_MOTIF_DATA_H__

/*
 * '.' highlight
 * ':' fg
 * '#' shade
 * ' ' bg
 */

static char *arrow_up_src[] = {"     .     ", "    ..#    ", "    ..#    ", "   ..:##   ",
                               "   ..:##   ", "  ..:::##  ", "  ..:::##  ", " ..:::::## ",
                               " ..:::::## ", "..#########", ".##########"};

static char *arrow_down_src[] = {"...........", ".........##", " ..:::::## ", " ..:::::## ",
                                 "  ..:::##  ", "  ..:::##  ", "   ..:##   ", "   ..:##   ",
                                 "    .##    ", "    .##    ", "     #     "};

static char *arrow_up_pressed_src[] = {"     #     ", "    ##.    ", "    ##.    ", "   ##:..   ",
                                       "   ##:..   ", "  ##:::..  ", "  ##:::..  ", " ##:::::.. ",
                                       " ##:::::.. ", "##.........", "#.........."};

static char *arrow_down_pressed_src[] = {"###########", "#########..", " ##:::::.. ", " ##:::::.. ",
                                         "  ##:::..  ", "  ##:::..  ", "   ##:..   ", "   ##:..   ",
                                         "    #..    ", "    #..    ", "     .     "};

#endif
