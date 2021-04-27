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
#include "fbcc.h"



LIST *Expr_CEval(LIST *e);

int Expr_IsConst(LIST *e)
{
    return hd_tag(tl(e)) == EXPR_INT;
}

int Expr_ConstVal(LIST *e)
{
    return hd_int(tl(tl(e)));
}


LIST *Expr_ConstInteger1(LIST *type,int val)
{
    return mk_list(type,mk_tag(EXPR_INT,mk_int(val,NULL)));
}

LIST *Expr_CBinary(LIST *expr)
{
    int op,val1,val2,val;
    LIST *ec1,*ec2,*type;

    ec1=hd_list(tl(tl(expr)));
    ec2=hd_list(tl(tl(tl(expr))));
    op=hd_tag(tl(expr));
    type=list_dup(hd_list(expr));

    ec1=Expr_CEval(ec1);
    ec2=Expr_CEval(ec2);

    if (op != EXPR_ASSIGN && Expr_IsConst(ec1) && Expr_IsConst(ec2) ) {
        val1=Expr_ConstVal(ec1);
        val2=Expr_ConstVal(ec2);

        switch(op) {
            case EXPR_ADD:
                val=val1+val2;
                break;
            case EXPR_SUB:
                val=val1-val2;
                break;
            case EXPR_MUL:
                if (hd_tag(type)==TYPE_UINT) val=(unsigned int)val1*(unsigned int)val2;
                else val=val1*val2;
                break;
            case EXPR_DIV:
                if (val2==0) Error("Divison error");
                if (hd_tag(type)==TYPE_UINT) val=(unsigned int)val1/(unsigned int)val2;
                else val=val1*val2;
                break;
            case EXPR_MOD:
                if (val2==0) Error("Divison error");
                if (hd_tag(type)==TYPE_UINT) val=(unsigned int)val1%(unsigned int)val2;
                else val=val1 % val2;
                break;
            case EXPR_AND:
                val = (unsigned int)val1 & (unsigned int)val2;
                break;
            case EXPR_OR:
                val = (unsigned int)val1 | (unsigned int)val2;
                break;
            case EXPR_XOR:
                val = (unsigned int)val1 ^ (unsigned int)val2;
                break;
            case EXPR_SHR:
                val = (unsigned int)val1 >> (unsigned int)val2;
                break; 
            case EXPR_SHL:
                val = (unsigned int)val1 << (unsigned int)val2;
                break;
            default:
                val=0;
        }
        list_free(ec1);
        list_free(ec2);
        return Expr_ConstInteger1(type,val);
    } else {
        return mk_list(type,mk_tag(op,mk_list(ec1,mk_list(ec2,NULL))));
    }
}


LIST *Expr_CUnary(LIST *expr)
{
    int op,val1,val;
    LIST *ec1,*type;

    ec1=hd_list(tl(tl(expr)));
    op=hd_tag(tl(expr));
    type=list_dup(hd_list(expr));

    ec1=Expr_CEval(ec1);

    if (op != EXPR_INDIR && Expr_IsConst(ec1)) {
        val1=Expr_ConstVal(ec1);
        switch (op) {
            case EXPR_NEG:
                val=-val1;
                break;
            default:

                val=0;
        }
        list_free(ec1);
        return Expr_ConstInteger1(type,val);
    } else {
        return mk_list(type,mk_tag(op,mk_list(ec1,NULL)));
    }
}



LIST *Expr_CCast(LIST *expr)
{
    int val;
    LIST *ec1,*type;

    ec1=hd_list(tl(tl(expr)));
    type=list_dup(hd_list(expr));

    ec1=Expr_CEval(ec1);

    if (Expr_IsConst(ec1)) {
        val=Expr_ConstVal(ec1);
        switch(hd_tag(type)) {
            case TYPE_CHAR:
                val=(char)val;
                break;
            case TYPE_UCHAR:
                val=(unsigned char)val;
                break;
            case TYPE_SHORT:
                val=(short)val;
                break;
            case TYPE_USHORT:
                val=(unsigned short)val;
                break;
        }
        list_free(ec1);
        return Expr_ConstInteger1(type,val);
    } else {
        return mk_list(type,mk_tag(EXPR_CAST,mk_list(ec1,NULL)));
    }
}

LIST *Expr_CEval(LIST *expr)
{
    int op;

    op=hd_tag(tl(expr));

    switch(op) {
        case EXPR_CAST:
            return Expr_CCast(expr);
        case EXPR_NEG:
        case EXPR_INDIR:
            return Expr_CUnary(expr);
        case EXPR_ADD:
        case EXPR_MUL:
        case EXPR_SUB:
        case EXPR_DIV:
        case EXPR_ASSIGN:
        case EXPR_MOD:
        case EXPR_AND:
        case EXPR_OR:
        case EXPR_XOR:
        case EXPR_SHR:
        case EXPR_SHL:
            return Expr_CBinary(expr);
        default:
            return list_dup(expr);
    }
}


LIST *Expr_ConstEval(LIST *expr)
{
    LIST *res;



    res=Expr_CEval(expr);
    list_free(expr);



    return res;
}


