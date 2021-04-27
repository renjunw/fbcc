/*
 *  FBCC - A simple C compiler.
 * 
 *  Copyright (c) 1996 Fabrice Bellard
 *
 *  Contact addresses:
 *  mail: Fabrice Bellard, 451 chemin du mas de Matour, 34790 Grabels, France
 *  email: bellard@email.enst.fr
 *  url: http://www.enst.fr/~bellard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdlib.h>
#include <stdio.h>
#include "fbvmspec.h"
#include "fbcc.h"
#include "compiler.h"
#include "config.h"


char *vm_instr_str[]=
{
    "zero",
    "ld_b", 
    "ld_ub",
    "ld_w",
    "ld_uw",
    "ld_i",

    "st_b",
    "st_w",
    "st_i",

    "add_i",
    "sub_i",
    "mul_i",
    "div_i",
    "div_ui",
    "mod_i",
    "mod_ui",
    "neg_i",

    "cmplt_i",
    "cmple_i",
    "cmpge_i",
    "cmpgt_i",
    "cmpeq_i",
    "cmpne_i",

    "cmplt_ui",
    "cmple_ui",
    "cmpge_ui",
    "cmpgt_ui",

    "and_i",
    "or_i",
    "xor_i",
    "not_i",
    "shl_i",
    "shr_i",
    "shr_ui",

    /* conversions */
    "cvt_i_b",
    "cvt_i_ub",
    "cvt_i_w",
    "cvt_i_uw",

    "cvt_b_i",
    "cvt_w_i",


    "li_i",
    "libp_i",

    "jeq_i",
    "jne_i",
    "switch_i",

    "jmp",

    "jsr",
    "rts",

    "dup",
    "pop",
    "addsp",

    "libcall",
    NULL,
};

const char cc_lib_str[] = 
".module\n"
" \n"
".text\n"
"  li_i _vars\n"
"       li_i _getvars\n"
"       jsr 4\n"
"       pop\n"
"       \n"
"       li_i _argv\n"
"       ld_i\n"
"       li_i _argc\n"
"       ld_i\n"
"       li_i avl_main\n"
"       jsr 8\n"
"       li_i avl_exit\n"
"       jsr 4\n"
"       \n"
"/* variables globales */\n"
"\n"
".data\n"
" .align 4\n"
"_vars:\n"
"stdin:\n"
" .globl stdin\n"
" .int 0\n"
"stdout:\n"
" .globl stdout\n"
" .int 0\n"
"stderr:\n"
" .globl stderr\n"
" .int 0 \n"
"_argc:\n"
" .int 0\n"
"_argv:\n"
" .int 0\n"
"\n"
"\n"
"\n"
"/* librairie */\n"
".text\n"
"\n"
"_getvars:\n"
" libcall 0\n"
" rts\n"
"\n"
"avl_malloc:\n"
" .globl avl_malloc\n"
" libcall 1\n"
" rts\n"
"\n"
"avl_free:\n"
" .globl avl_free\n"
" libcall 2\n"
" rts\n"
"\n"
" \n"
"avl_putc:\n"
" .globl avl_putc\n"
" libcall 3\n"
" rts\n"
"\n"
"avl_getc:\n"
" .globl avl_getc\n"
" libcall 4\n"
" rts\n"
"\n"
"avl_delay_ms:\n"
" .globl avl_delay_ms\n"
" libcall 5\n"
" rts\n"
"\n"
"avl_delay_ns:\n"
" .globl avl_delay_ns\n"
" libcall 6\n"
" rts\n"
"\n"
"avl_read32:\n"
" .globl avl_read32\n"
" libcall 7\n"
" rts\n"
"\n"
"avl_read16:\n"
" .globl avl_read16\n"
" libcall 8\n"
" rts\n"
"\n"
"avl_read8:\n"
" .globl avl_read8\n"
" libcall 9\n"
" rts\n"
"\n"
"avl_write32:\n"
" .globl avl_write32\n"
" libcall 10\n"
" rts\n"
"\n"
"avl_write16:\n"
" .globl avl_write16\n"
" libcall 11\n"
" rts\n"
"\n"
"avl_write8:\n"
" .globl avl_write8\n"
" libcall 12\n"
" rts\n"
" \n"
"avl_print_num:\n"
" .globl avl_print_num\n"
" libcall 13\n"
" rts\n"
" \n"
"avl_memset:\n"
" .globl avl_memset\n"
" libcall 14\n"
" rts\n"
"\n"
"avl_memcpy:\n"
" .globl avl_memcpy\n"
" libcall 15\n"
" rts\n"
"\n"
"avl_memcmp:\n"
" .globl avl_memcmp\n"
" libcall 16\n"
" rts\n"
"\n"
"avl_print_str:\n"
" .globl avl_print_str\n"
" libcall 17\n"
" rts\n"
" \n"
"avl_exit:\n"
" .globl avl_exit\n"
" libcall 18\n"
" rts\n"
"\n"
"avl_get_wakeup_info:\n"
" .globl avl_get_wakeup_info\n"
" libcall 19\n"
" rts\n"
"\n"
"avl_try_to_recover_from_standby:\n"
" .globl avl_try_to_recover_from_standby\n"
" libcall 20\n"
" rts\n"
"\n"
"avl_ddr_setup_handle:\n"
" .globl avl_ddr_setup_handle\n"
" libcall 21\n"
" rts\n"
"\n"
"\n"
"\n";

FILE *ccout  = NULL;
FILE *ccin = NULL;

extern void yyset_in  (FILE * in_str);
extern void yyset_out  (FILE * out_str);


/*only used in local fbcc.c file*/
__ExternC int as_hexdump(char *input, char *output)
{
    char *buffer = NULL;
    FILE *fp = NULL;
    unsigned size = 0, i;
    read_all_file(input, &buffer, &size);

    fp = fopen(output, "wb");

    if(!fp)
    {
        fprintf(stderr, "Cannot create file %s\r\n", output);
        return ERR_OPENFILE;
    }

    for(i = 0; i < size; i++)
    {
        if(i && (i % 16) == 0)
        {
            fprintf(fp, "\n");
        }
        fprintf(fp, "%02x", (unsigned char)buffer[i]);
    }
    if(buffer)
        free(buffer);
    if(fp)
        fclose(fp);
    return 0;
}

/*used in GeneratorRsaKey.cpp*/
__ExternC unsigned int avl_vm_calc_check_sum(unsigned int check_sum, unsigned char *data, unsigned int size)
{
    unsigned int i;
    for(i = 0; i < size; i++)
    {
        check_sum += data[i];
    }
    return check_sum;
}

int main(int argc,char *argv[])
{
    int ret;
    Sym_Init();

    char* in = argv[1];
    char* asfile = argv[2];
    char* ecb = argv[3];
    char* hex = argv[4];
    unsigned stack_size = 64 * 1024; //use default value.
    FILE* temp_asfile = NULL;
    FILE* temp_fd = NULL;
    char buffer[4096] = {0};
    char buffer2[4096] = {0};
    char* assembler_in = "temp_all_in_one_file.s";

    //printf(cc_lib_str);

    //TODO: fix cc_lib_str
    ccin = fopen(in, "rb");
    if(!ccin)
    {
        fprintf(stderr, "Can't open file %s\r\n", in);
        return ERR_ARGUMENT;
    }

    ccout = fopen(asfile, "wb");
    if(!ccout)
    {
        fprintf(stderr, "Can't create file %s\r\n", asfile);
        return ERR_ARGUMENT;
    }

    /*ret = fwrite(cc_lib_str, 1, strlen(cc_lib_str), ccout);

    if(ret != (int)strlen(cc_lib_str) || ferror(ccout))
    {
        fprintf(stderr, "cannot write file %s\r\n", asfile);
        return ERR_IO;
    }*/

    line_current=1;
    block_current=NULL;
    Block_Enter(BLOCK_GLOBAL);
    block_global=block_current;
    block_decl=block_current;

    yyset_in(ccin);
    yyset_out(ccout);

    //printf("/* Module startup */\n");
    //printf(".module\n\n");
    yyparse(); 

    /*	 printf("\n");
         Sym_Print(); */

    fclose(ccin);
    fclose(ccout);

    temp_fd = fopen("lib.s", "rb");
    if(!temp_fd)
    {
        fprintf(stderr, "Can't create file lib.s\n");
        return ERR_ARGUMENT;
    }


    temp_asfile = fopen("temp_all_in_one_file.s", "wb+");
    if(!temp_asfile)
    {
        fprintf(stderr, "Can't create file temp_all_in_one_file\n");
        return ERR_ARGUMENT;
    }

    while(fread(buffer, 1, 1, temp_fd)){
        fwrite(buffer, 1, 1, temp_asfile);
    }
    
    fclose(temp_fd);
    fclose(temp_asfile);

    temp_fd = fopen(asfile, "rb");
    if(!temp_fd)
    {
        fprintf(stderr, "Can't create file %s\r\n", asfile);
        return ERR_ARGUMENT;
    }

    temp_asfile = fopen("temp_all_in_one_file.s", "ab+");
    if(!temp_asfile)
    {
        fprintf(stderr, "Can't create file temp_all_in_one_file\n");
        return ERR_ARGUMENT;
    }

    while(fread(buffer2, 1, 1, temp_fd)){
        fwrite(buffer2, 1, 1, temp_asfile);
    }

    fclose(temp_fd);
    fclose(temp_asfile);

    as_assembler(assembler_in, ecb, stack_size, 0);
    
    //as_hexdump(ecb, hex);

    return 0;
}


