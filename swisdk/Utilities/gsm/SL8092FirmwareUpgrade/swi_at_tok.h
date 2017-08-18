/**
 *
 * @ingroup swi_at_channel.h
 *
 * @file
 * Header file for AT command execution functions
 *
 * @author
 * Copyright: © 2015 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#ifndef AT_TOK_H
#define AT_TOK_H 1

int at_tok_start(char **p_cur);
int at_tok_nextint(char **p_cur, int *p_out);
int at_tok_nexthexint(char **p_cur, int *p_out);

int at_tok_nextbool(char **p_cur, char *p_out);
int at_tok_nextstr(char **p_cur, char **out);

int at_tok_index_int(char **p_cur, int *p_out, int index);

int at_tok_hasmore(char **p_cur);
int at_tok_next_unsignedlong(char **p_cur, unsigned long *p_out, int base);

#endif
