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

/*
 * types handling
 */

#include <stdlib.h>
#include <stdio.h>
#include "fbvmspec.h"
#include "fbcc.h"

int local_var_size;
SYM *local_var_sym;

BLOCK *block_decl;
extern FILE *ccout;


int Spec_Parse(int *spec_storage1,SYM **spec_sym,LIST *spec)
{
    int a,spec_storage;
    int spec_type,spec_sign;

    spec_sign=0;
    spec_type=0;
    spec_storage=STORAGE_DEFAULT;

    while (spec!=NULL) {
        a=hd_tag(spec);
        switch(a) {
            case QUALIF_CONST:
            case QUALIF_VOLATILE:
                break;

            case TYPE_SIGNED:
            case TYPE_UNSIGNED:
                if (spec_sign==0) {
                    spec_sign=a;
                } else if (spec_sign!=a) 
                    Error("'unsigned' to 'signed' incompatibles");
                break;

            case TYPE_VOID:
            case TYPE_CHAR:
            case TYPE_SHORT:
            case TYPE_INT:
                if ((spec_type==TYPE_INT && a==TYPE_SHORT) ||
                        (spec_type==TYPE_SHORT && a==TYPE_INT)) {
                    spec_type=TYPE_SHORT;
                } else if (spec_type!=0) {
                    Error("Invalid types");
                } else {
                    spec_type=a;
                }
                break;

            case TYPE_TYPEDEF_IDENT:
                spec=tl(spec);
                if (spec_type!=0) Error("Invalid types");
                spec_type=TYPE_TYPEDEF_IDENT;
                *spec_sym=hd_sym(spec);
                break;

            case TYPE_STRUCT:
            case TYPE_UNION:
            case TYPE_ENUM:
                spec=tl(spec);
                *spec_sym=hd_sym(spec);
                if (spec_type!=0) Error("Invalid types");
                spec_type=a;
                break;

            case STORAGE_AUTO:
            case STORAGE_REGISTER:
            case STORAGE_STATIC:
            case STORAGE_EXTERN:
            case STORAGE_TYPEDEF:
                if (spec_storage!=STORAGE_DEFAULT) 
                    Error("storage error");
                spec_storage=a;
                break;

            default:
                Error("Invalid type");
        }
        spec=tl(spec);
    }
    switch(spec_type) {
        case TYPE_CHAR:
            if (spec_sign==TYPE_UNSIGNED) spec_type=TYPE_UCHAR;
            break;
        case TYPE_SHORT:
            if (spec_sign==TYPE_UNSIGNED) spec_type=TYPE_USHORT;
            break;
        case 0:
        case TYPE_INT:
            if (spec_sign==TYPE_UNSIGNED) spec_type=TYPE_UINT;
            else spec_type=TYPE_INT;
            break;
        default:
            if (spec_sign!=0) 
                Error("'unsigned' to 'signed' error");
            break;
    }

    *spec_storage1=spec_storage;
    return spec_type;
}



LIST *Type_VarList(LIST *spec,LIST *decls)
{
    LIST *name,*decl_list,*decl,*vardef,*vardef_list,*type,*init;
    int spec_storage,spec_type;
    SYM *spec_sym;

    spec_type=Spec_Parse(&spec_storage,&spec_sym,spec);
    vardef_list=NULL;
    decl_list=decls;

    while (decl_list!=NULL) {
        decl=hd_list(decl_list);
        name=list_dup(hd_list(decl));
        decl=tl(decl);
        decl_list=tl(decl_list);
        init=list_dup(hd_list(decl_list));
        decl_list=tl(decl_list);
        switch(spec_type) {
            case TYPE_STRUCT:
            case TYPE_UNION:
            case TYPE_ENUM:
                type=append(list_dup(decl),mk_tag(spec_type,mk_sym(spec_sym,NULL)));
                break;
            case TYPE_TYPEDEF_IDENT:
                type=append(list_dup(decl),list_dup(tl(spec_sym->list)));
                break;
            default:
                type=append(list_dup(decl),mk_tag(spec_type,NULL));
        }

        vardef=mk_list(name,
                mk_tag(spec_storage,
                    mk_list(type,
                        mk_list(init,NULL))));

        vardef_list=append(vardef_list,vardef);
    }
    /*
       fprintf(ccout,"Variables: ");
       List_Print(vardef_list);
       fprintf(ccout,"\n");
     */
    list_free(spec);
    list_free(decls);
    return vardef_list;
}



int Type_Size(LIST *type)
{
    int tag,dim,size;
    SYM *s;

    tag=hd_tag(type);
    switch(tag) {
        case TYPE_VOID:
            return -1;
        case TYPE_CHAR:
        case TYPE_UCHAR:
            return VM_CHAR_SIZE;
        case TYPE_SHORT:
        case TYPE_USHORT:
            return VM_SHORT_SIZE;
        case TYPE_INT:
        case TYPE_UINT:
            return VM_INT_SIZE;
        case TYPE_POINTER:
            return VM_POINTER_SIZE;

        case TYPE_ARRAY:
            dim=hd_int(tl(type));
            if (dim==-1) return -1;
            size=Type_Size(tl(tl(type)));
            if (size==-1) return -1;
            else return dim * size;
        case TYPE_ENUM:
            return VM_INT_SIZE;
        case TYPE_UNION:
        case TYPE_STRUCT:
            s=hd_sym(tl(type));
            if (hd_int(tl(s->list))!=-1) return hd_int(tl(tl(s->list)));
            else return -1;

        case TYPE_FUNC:
            return -1;
        default:
            Error_Internal("type size error %x",tag);
            return -1;
    }
}



int Type_Align(LIST *type)
{
    int tag;
    tag=hd_tag(type);
    switch(tag) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
            return 1;
        case TYPE_SHORT:
        case TYPE_USHORT:
            return 2;
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_POINTER:
            return 4;


        default:
            return 4;
    }
}


int Type_Compare(LIST *type1,LIST *type2)
{
    int t1,t2;

    t1=hd_tag(type1);
    t2=hd_tag(type2);
    if (t1 != t2) return 0;
    switch (t1) {
        case TYPE_POINTER:
            return Type_Compare(tl(type1),tl(type2));
        case TYPE_ARRAY:
        case TYPE_FUNC:
            /* non fait */
            return 1;
        case TYPE_ENUM:
        case TYPE_UNION:
        case TYPE_STRUCT:
            if ( hd_sym(tl(type1)) != hd_sym(tl(type2)) ) return 0;
            else return 1;
        default:
            return 1;
    }
}



int Type_IsInteger(LIST *t)
{
    switch (hd_tag(t)) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_ENUM:
            return 1;
        default:
            return 0;
    }
}



int Type_IsArith(LIST *t)
{
    return Type_IsInteger(t);
}



LIST *Type_IsPointer(LIST *t)
{
    if (hd_tag(t)==TYPE_POINTER) {
        return tl(t);
    } else if (hd_tag(t)==TYPE_ARRAY) {
        return tl(tl(t));
    } else {
        return NULL;
    } 
}


int sym_table_num=100;

int Sym_NewTable(void)
{
    sym_table_num++;
    return sym_table_num;
}




LIST *Type_ArrayDeclare(LIST *type,LIST *expr)
{
    int tag,t,size;

    if (expr==NULL) {
        size=-1;
    } else {
        t=hd_tag(hd_list(expr));
        tag=hd_tag(tl(expr));
        if (tag!=EXPR_INT) 
            Error("Array declared error");
        size=hd_int(tl(tl(expr)));
        if (size<=0) 
            Error("Array declared error");

    }
    return mk_tag(TYPE_ARRAY,mk_int(size,type));
}


LIST *Type_FuncDeclare(int func_type,LIST *var_list)
{
    LIST *var_name,*var_type,*vars,*var_list1;
    int var_storage;

    if (func_type==FUNC_OLD) {
        return mk_tag(TYPE_FUNC,mk_tag(FUNC_OLD,mk_list(var_list,NULL)));
    }


    var_type=hd_list(tl(tl(var_list)));
    if (hd_tag(var_type) == TYPE_VOID) {
        if (func_type == FUNC_ELLIPSIS) 
            Error("Function declaration error");
        if ( tl(tl(tl(tl(var_list)))) != NULL ) 
            Error("Function declaration error");
        return mk_tag(TYPE_FUNC,mk_tag(FUNC_NEW,mk_list(NULL,NULL)));
    }

    vars=var_list;
    var_list1=NULL;
    while (vars!=NULL) {
        var_name=list_dup(hd_list(vars));
        vars=tl(vars);
        var_storage=hd_tag(vars);
        vars=tl(vars);
        var_type=hd_list(vars);
        vars=tl(tl(vars));

        if (var_storage!=STORAGE_DEFAULT && var_storage != STORAGE_REGISTER) 
            Error("Function declaration error");



        if (hd_tag(var_type) == TYPE_ARRAY) {
            var_type=mk_tag(TYPE_POINTER,list_dup(tl(tl(var_type))));
        } else if (hd_tag(var_type) == TYPE_FUNC) {
            var_type=mk_tag(TYPE_POINTER,list_dup(var_type));
        } else {
            var_type=list_dup(var_type);
        }

        if (Type_Size(var_type) == -1) 
            Error("parameters isn't compatible");

        var_list1=append(var_list1,
                mk_list(var_name,mk_tag(var_storage,mk_list(var_type,mk_list(NULL,NULL)))));
    }
    list_free(var_list);

    return mk_tag(TYPE_FUNC,mk_tag(func_type,mk_list(var_list1,NULL)));
}



LIST *Struct_Declare(char *str,int type,LIST *var_list)
{
    int sym_table;
    LIST *vars,*var_type;
    char *var_name;
    int tag,var_align,var_size,struct_size,struct_align;
    SYM *s;
    char buf[SYM_LENMAX];

    struct_size=0;
    struct_align=0;
    sym_table=0;

    if (type!=TYPE_ENUM) {
        sym_table=Sym_NewTable();
        vars=var_list;
        while (vars!=NULL) {
            var_name=hd_str(hd_list(vars));
            s=Sym_Search(var_name,sym_table);
            if (s!=NULL) Error("Variable '%s' of struct/union undeclared",var_name);
            var_type=list_dup(hd_list(tl(tl(vars))));
            var_size=Type_Size(var_type);
            if (var_size==-1) Error("variable '%s' of struct/union undeclared",var_name);
            var_align=Type_Align(var_type);
            if (type==TYPE_UNION) {
                /* structure union */
                Sym_New(var_name,sym_table,mk_list(var_type,mk_int(0,NULL)));

                if (var_align>struct_align) struct_align=var_align;
                if (var_size>struct_size) struct_size=var_size;
            } else {
                /* structure struct */

                /* alignement */
                if (var_align>struct_align) struct_align=var_align;
                struct_size=(struct_size+var_align-1) & ~(var_align-1);

                Sym_New(var_name,sym_table,mk_list(var_type,mk_int(struct_size,NULL)));

                /* taille */
                struct_size+=var_size;
            }

            vars=tl(tl(tl(tl(vars))));
        }
    }

    if (str==NULL) {
        Sym_NewName(buf);
        str=buf;
    }

    s=Sym_Search(str,TABLE_STRUCT);
    if (s!=NULL && s->block==block_current) {
        tag=hd_tag(s->list);
        if (tag!=type) Error("struct/union/enum '%s' type error",str);
        if ( hd_int(tl(s->list)) != -1) Error("Redefinition the struct/union/enum '%s'",str);
        list_free(s->list);
    } else {
        s=Sym_New(str,TABLE_STRUCT,NULL);
    }
    s->list=mk_tag(type,mk_int(sym_table,mk_int(struct_size,mk_int(struct_align,NULL))));
    return mk_tag(type,mk_sym(s,NULL));
}



LIST *Struct_Use(char *str,int type)
{
    SYM *s;
    int tag;

    s=Sym_Search(str,TABLE_STRUCT);
    if (s!=NULL) {
        tag=hd_tag(s->list);
        if (tag!=type) 
            Error("struct/union/enum type error"); 
    } else {
        s=Sym_New(str,TABLE_STRUCT,
                mk_tag(type,mk_int(-1,NULL)));
    }
    return mk_tag(type,mk_sym(s,NULL));
}





LIST *var_init_after;

void Var_Init1(LIST *type1,LIST *expr_list,int first_init)
{
    int t1,val;
    LIST *type2,*elem_type,*expr,*buf,*elem_expr;
    int i,size,buf_size,elem_nb,elem_size;	 
    char bufstr[SYM_LENMAX];

    t1=hd_tag(type1);

    switch(t1) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_ENUM:
            if (hd_tag(expr_list) != INIT_EXPR) Error("Expression error");
            expr=tl(expr_list);
            type2=hd_list(expr);
            if (!Type_IsInteger(type2)) Error("type isn't interger");

            if (hd_tag(tl(expr)) != EXPR_INT)
                Error("Expression constante error");
            val=hd_int(tl(tl(expr)));
            switch(t1) {
                case TYPE_CHAR:
                case TYPE_UCHAR:
                    fprintf(ccout," .byte %d\n",val & 0xFF);
                    break;
                case TYPE_SHORT:
                case TYPE_USHORT:
                    fprintf(ccout," .short %d\n",val & 0xFFFF);
                    break;
                case TYPE_INT:
                case TYPE_UINT:
                case TYPE_ENUM:
                    fprintf(ccout," .int %d\n",val);
                    break;
            }
            break;

        case TYPE_POINTER:

            if (hd_tag(expr_list) != INIT_EXPR) Error("Expression error");
            expr=tl(expr_list);
            type2=hd_list(expr);

            if (hd_tag(tl(expr)) == EXPR_STR) {
                Sym_NewName(bufstr);
                fprintf(ccout," .int %s\n",bufstr);
                var_init_after=append(var_init_after,
                        mk_str(bufstr,
                            mk_list(mk_tag(TYPE_ARRAY,mk_int(-1,mk_tag(TYPE_CHAR,NULL))),
                                mk_list(list_dup(expr_list),NULL))));
            } else if (hd_tag(tl(expr)) == EXPR_INT) {
                val=hd_int(tl(tl(expr)));
                fprintf(ccout," .int %d\n",val);
            } else {
                Error("Type isn't initialized");
            }
            break;

        case TYPE_ARRAY:
            size=hd_int(tl(type1));
            if (size==-1 && !first_init) Error("array size error");
            elem_type=tl(tl(type1));
            elem_size=Type_Size(elem_type);
            if (elem_size == -1) Error("Array size error");
            elem_nb=0;


            if (hd_tag(expr_list) == INIT_EXPR) {
                expr=tl(expr_list);
                type2=hd_list(expr);
                if (hd_tag(tl(expr))==EXPR_STR && hd_tag(elem_type)==TYPE_CHAR) {

                    buf=tl(tl(expr));
                    while (buf!=NULL) {
                        buf_size=buf->data.buf_size;
                        for(i=0;i<buf_size;i++) {
                            if (size != -1 && elem_nb>=size) 
                                Error("the size more than array's elements");
                            fprintf(ccout," .byte %d\n",buf->str[i]);
                            elem_nb++;
                        }
                        buf=tl(buf);
                    }
                    if (size == -1) {
                        fprintf(ccout," .byte 0\n");
                        elem_nb++;
                    }
                } else {
                    Error("Constant error");
                }
            } else {


                expr_list=tl(expr_list);
                while (expr_list != NULL) {
                    if (size != -1 && elem_nb>=size) 
                        Error("size error");
                    elem_expr=hd_list(expr_list);
                    Var_Init1(elem_type,elem_expr,0);
                    expr_list=tl(expr_list);
                    elem_nb++;
                }
            }

            if (size!=-1 && elem_nb < size) {
                fprintf(ccout," .zero %d\n",(size-elem_nb)*elem_size);
            }


            if (size==-1) put_int(tl(type1),elem_nb);
            break;

        default:
            Error("Initialization of type error");
            break;
    }

}

void Var_Init(LIST *type1,LIST *expr_list)
{
    LIST *vars;
    var_init_after=NULL;
    Var_Init1(type1,expr_list,1);

    vars=var_init_after;
    while (vars!=NULL) {
        fprintf(ccout,"%s:\n",hd_str(vars));
        Var_Init1(hd_list(tl(vars)),hd_list(tl(tl(vars))),1);
        vars=tl(tl(tl(vars)));
    }
    list_free(var_init_after);
}




void Global_Declare(char *var_name,int var_storage,LIST *var_type)
{
    int tag,var1_storage;
    LIST *var1_type;
    SYM *s;

    s=Sym_Search(var_name,TABLE_VAR);

    if (s!=NULL) {

        tag=hd_tag(s->list);
        if (tag!=SYM_VAR) Error("Identifier error",var_name);
        var1_storage=hd_tag(tl(s->list));
        var1_type=hd_list(tl(tl(s->list)));
        if (var1_storage!=STORAGE_EXTERN && var_storage!=STORAGE_EXTERN) 
            Error("Redefined function '%s'",var_name);
        if (!Type_Compare(var_type,var1_type)) 
            Error("Redefined function type '%s'",var_name);
        if (var1_storage==STORAGE_EXTERN && var_storage!=STORAGE_EXTERN) {

            list_free(s->list);
        }
    } else {
        s=Sym_New(var_name,TABLE_VAR,NULL);
    }	 

    s->list=mk_tag(SYM_VAR,mk_tag(var_storage,mk_list(var_type,
                    mk_list(mk_tag(VAR_DATA,NULL),NULL))));
}



void Gen_VarInit(char *var_name,LIST *type,LIST *init)
{
    int var_size,var_align;

    fprintf(ccout,".data\n");

    if (init==NULL) {
        var_size=Type_Size(type);
        if (var_size==-1) Error("variable global type compatible");
        var_align=Type_Align(type);
        fprintf(ccout," /* variable un-initializing: %s */\n",var_name);
        fprintf(ccout," .align %d\n",var_align);
        fprintf(ccout,"%s:\n",var_name);
        fprintf(ccout," .zero %d\n",var_size);
    } else {
        var_align=Type_Align(type);
        fprintf(ccout," /* Variable initialize: %s */\n",var_name);
        fprintf(ccout," .align %d\n",var_align);
        fprintf(ccout,"%s:\n",var_name);

        Var_Init(type,init);
    }
}


void Local_Static(char *var_name,int var_storage,LIST *var_type,LIST *var_init)
{

    Sym_New(var_name,TABLE_VAR,
            mk_tag(SYM_VAR,mk_tag(var_storage,mk_list(var_type,
                        mk_list(mk_tag(VAR_DATA,NULL),NULL)))));

    Gen_VarInit(var_name,var_type,var_init);
    fprintf(ccout, " .text\n");
}




void Local_Declare(char *var_name,int var_storage,LIST *var_type,LIST *var_init)
{
    int var_offset,var_size,var_align;
    LIST *expr;

    var_size=Type_Size(var_type);
    var_align=Type_Align(var_type);
    if (var_size==-1) Error("variable locale '%s' undeclared",var_name);

    /* alignement */
    block_decl->local_var_offset=(block_decl->local_var_offset+(var_align-1)) & ~(var_align-1);
    /* allocation */
    var_offset=block_decl->local_var_offset;
    block_decl->local_var_offset=block_decl->local_var_offset+var_size;
    if (block_decl->local_var_offset>local_var_size) 
        local_var_size=block_decl->local_var_offset;

    fprintf(ccout,"/* variable locale '%s' stack offset=%d */\n",var_name,var_offset);

    Sym_New(var_name,TABLE_VAR,mk_tag(SYM_VAR,mk_tag(var_storage,
                    mk_list(var_type,
                        mk_list(mk_tag(VAR_STACK,mk_int(var_offset,NULL)),NULL)))));


    if (var_init != NULL) {
        if (hd_tag(var_init) != INIT_EXPR)
            Error("Non-initialization variables locales");

        expr=Expr_Assign(Expr_Ident(var_name),list_dup(tl(var_init)));
        Gen_InstrExpr(expr);
    }
}


void Var_Declare(LIST *var_list)
{
    char *var_name;
    LIST *vars,*var_type,*var_init;
    int var_storage;
    SYM *s;

    vars=var_list;
    while (vars!=NULL) {
        var_name=hd_str(hd_list(vars));
        var_storage=hd_tag(tl(vars));
        var_type=list_dup(hd_list(tl(tl(vars)))); 
        var_init=hd_list(tl(tl(tl(vars))));

        if (var_storage == STORAGE_TYPEDEF) { 

            if (var_init!=NULL) Error("'typedef' error");
            Sym_Create(var_name,TABLE_VAR,mk_tag(TYPE_TYPEDEF_IDENT,
                        var_type));
        } else if (block_current == block_global) {

            if (var_storage == STORAGE_REGISTER || 
                    var_storage == STORAGE_AUTO) 
                Error("unknown storage of variable");


            if (hd_tag(var_type)==TYPE_FUNC) var_storage=STORAGE_EXTERN;

            Global_Declare(var_name,var_storage,var_type);

            if (var_storage != STORAGE_EXTERN) {
                Gen_VarInit(var_name,var_type,var_init);
                if (var_storage != STORAGE_STATIC) { 
                    fprintf(ccout," .globl %s\n",var_name);
                }
            }

        } else {

            s=Sym_Search(var_name,TABLE_VAR);
            if (s!=NULL) {
                if (s->block==block_decl) Error("Redefinition symbol '%s'",var_name);
            }
            if (var_storage == STORAGE_STATIC) {
                Local_Static(var_name,var_storage,var_type,var_init);
            } else {
                Local_Declare(var_name,var_storage,var_type,var_init);
            }
        }
        vars=tl(tl(tl(tl(vars))));
    }
}



void Func_Declare(LIST *spec,LIST *decl,LIST *var_list)
{
    LIST *func,*func_type,*params,*param_type,*l;
    char *func_name,*param_name;
    int func_storage,func_style,param_storage,param_size,param_offset;
    int stack_offset;
    SYM *s;
    char buf[SYM_LENMAX];

    func=Type_VarList(spec,mk_list(decl,mk_list(NULL,NULL)));

    func_name=hd_str(hd_list(func));
    func_storage=hd_tag(tl(func));
    func_type=list_dup(hd_list(tl(tl(func))));

    if (hd_tag(func_type) != TYPE_FUNC)
        Error("Declared symbol isn't a function");
    if (func_storage != STORAGE_STATIC && func_storage != STORAGE_DEFAULT)
        Error("Function '%s' is incorrected",func_name);

    func_style=hd_tag(tl(func_type));

    if (var_list != NULL && func_style != FUNC_OLD) 
        Error("Function declared error");



    Global_Declare(func_name,func_storage,func_type);

    Block_Enter(BLOCK_DECL);
    block_decl=block_current;
    block_function=block_current;


    params=var_list;
    while (params!=NULL) {
        l=hd_list(params);
        param_name=hd_str(l);
        params=tl(params);
        param_storage=hd_tag(params);
        params=tl(params);
        param_type=list_dup(hd_list(params));
        params=tl(tl(params));
        Sym_Create(param_name,TABLE_VAR,
                mk_tag(SYM_VAR,mk_tag(param_storage,mk_list(param_type,
                            mk_list(NULL,NULL)))));
    }
    list_free(var_list);


    fprintf(ccout, ".text\n");
    fprintf(ccout," /* definition function %s */\n",func_name);
    fprintf(ccout,"%s:\n",func_name);
    if (func_storage != STORAGE_STATIC) fprintf(ccout," .globl %s\n",func_name);

    Sym_NewName(buf);
    local_var_sym=Sym_New(buf,TABLE_VAR,NULL);
    Gen_LI(addsp,0,local_var_sym);


    block_decl->local_var_offset=VM_LOCAL_START;
    local_var_size=VM_LOCAL_START;


    params=hd_list(tl(tl(func_type)));

    stack_offset=VM_LOCAL_PARAM_END;

    while (params!=NULL) {
        l=hd_list(params);
        if (l==NULL) Error("parameters error");
        param_name=hd_str(l);
        params=tl(params);
        param_storage=hd_tag(params);
        params=tl(params);
        param_type=list_dup(hd_list(params));
        params=tl(tl(params));


        s=NULL;
        if (param_type == NULL) {
            s=Sym_Search(param_name,TABLE_VAR);
            if (s != NULL && s->block == block_current) {
                l=tl(tl(tl(s->list)));
                param_type=hd_list(tl(tl(s->list)));
            } else {
                s=NULL;
            }
            if (param_type==NULL) param_type=mk_tag(TYPE_INT,NULL);
        }



        param_size=Type_Size(param_type);
        if (param_size>VM_STACK_ALIGN) 
            Error("Type error for function: too many arguments");

#if VM_LITTLE_ENDIAN==0
        param_offset=stack_offset- param_size;
#else
        param_offset=stack_offset- VM_STACK_ALIGN;
#endif
        stack_offset=stack_offset - VM_STACK_ALIGN;

        if (s==NULL) {
            Sym_Create(param_name,TABLE_VAR,
                    mk_tag(SYM_VAR,mk_tag(param_storage,mk_list(param_type,
                                mk_list(mk_tag(VAR_STACK,mk_int(param_offset,NULL)),NULL)))));
        } else {
            l=tl(tl(tl(s->list)));
            put_list(l,mk_tag(VAR_STACK,mk_int(param_offset,NULL)));
        }
    }


    s=block_current->sym_first;
    while (s!=NULL) {
        if (s->list != NULL && hd_tag(s->list) == SYM_VAR) {
            l=hd_list(tl(tl(tl(s->list))));
            if (l==NULL) Error("Passed arguments error '%s'",s->str);
        }
        s=s->block_next;
    }

    list_free(func);
}


void Func_End(void)
{
    Gen_LI(li_i,0,NULL);
    Gen_Code(rts);

    local_var_size=(local_var_size+VM_STACK_ALIGN-1) & ~(VM_STACK_ALIGN-1);
    fprintf(ccout," .equ %s,%d\n",local_var_sym->str,local_var_size-VM_LOCAL_START);
    fprintf(ccout,"/* end of function */\n");
    Block_Leave();
    block_decl=block_current;
}






void Expr_CheckLValue(LIST *e)
{
    int tag,t;
    tag=hd_tag(tl(e));
    t=hd_tag(hd_list(e));

    switch (tag) {
        case EXPR_IDENT:
        case EXPR_INDIR:
            if (t==TYPE_ARRAY) Error("Left Value error"); 
            break;

        default:
            Error("Left Value error");
    }

}



LIST *Expr_Cast(LIST *new_type,LIST *e1)
{
    if ( Type_Compare(new_type,hd_list(e1)) ) {
        list_free(new_type);
        return e1;
    } else {
        return mk_list(new_type,mk_tag(EXPR_CAST,mk_list(e1,NULL)));
    }
}


LIST *Expr_CastAssign(LIST *type1,LIST *e2)
{
    LIST *type2,*tp1,*tp2;

    type2=hd_list(e2);

    if ( (Type_IsArith(type1) && Type_IsArith(type2)) ) { 

    } else if ( (tp1=Type_IsPointer(type1)) && (tp2=Type_IsPointer(type2)) ) {

        if ( hd_tag(tp1)==TYPE_VOID || hd_tag(tp2)==TYPE_VOID ) {

        } else {
            if ( !Type_Compare(tp1,tp2) ) 
                Error("Types error");
        }
    } else if (Type_IsPointer(type1) && hd_tag(tl(e2))==EXPR_INT) {
        if ( Type_IsInteger(hd_list(e2)) && hd_int(tl(tl(e2))) == 0 ) {

        } else {
            Error("Type error");
        }
    } else {
        if ( !Type_Compare(type1,type2) )
            Error("Types isn't compatibles");


        return e2;
    }

    return Expr_Cast(list_dup(type1),e2);
}

/*
 * Promotion entiere
 */ 
LIST *Expr_PromoteInteger(LIST *expr)
{
    switch (hd_tag(hd_list(expr))) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_ENUM:
            return Expr_Cast(mk_tag(TYPE_INT,NULL),expr);
        default:
            return expr;
    }
}



void Expr_BinaryCast(LIST **res1,LIST **res2,LIST *e1,LIST *e2)
{
    LIST *t1,*t2,*t;
    e1=Expr_PromoteInteger(e1);
    e2=Expr_PromoteInteger(e2);

    t1=hd_list(e1);
    t2=hd_list(e2);

    if (hd_tag(t1)==TYPE_UINT || hd_tag(t2)==TYPE_UINT) {
        t=mk_tag(TYPE_UINT,NULL);
    } else {
        t=mk_tag(TYPE_INT,NULL);
    }

    *res1=Expr_Cast(list_dup(t),e1);
    *res2=Expr_Cast(list_dup(t),e2);
    list_free(t);
}



LIST *Expr_BinaryInteger(int op,LIST *e1,LIST *e2)
{
    LIST *res1,*res2;

    if ( !Type_IsInteger(hd_list(e1)) || !Type_IsInteger(hd_list(e2)) ) 
        Error("Operand type error");

    Expr_BinaryCast(&res1,&res2,e1,e2);

    return mk_list(list_dup(hd_list(res1)),
            mk_tag(op,mk_list(res1,mk_list(res2,NULL))));
}


LIST *Expr_BinaryArith(int op,LIST *e1,LIST *e2)
{
    LIST *res1,*res2;

    if ( !Type_IsArith(hd_list(e1)) || !Type_IsArith(hd_list(e2)) ) 
        Error("operand algo error");

    Expr_BinaryCast(&res1,&res2,e1,e2);

    return mk_list(list_dup(hd_list(res1)),
            mk_tag(op,mk_list(res1,mk_list(res2,NULL))));
}


LIST *Expr_ConstInteger(int c)
{
    return mk_list(mk_tag(TYPE_INT,NULL),mk_tag(EXPR_INT,mk_int(c,NULL)));
}



LIST *Expr_List(LIST *expr_list)
{
    LIST *expr;
    if (tl(expr_list)==NULL) {
        expr=hd_list(expr_list);
        expr_list->data.list=NULL;
        list_free(expr_list);
        return expr;
    } else {
        expr=expr_list;
        while (tl(expr)!=NULL) expr=tl(expr);
        return mk_list(list_dup(hd_list(hd_list(expr))),mk_tag(EXPR_LIST,expr_list));
    }
}



LIST *Expr_AddSub(int op,LIST *e1,LIST *e2)
{
    LIST *type1,*type2,*tp1,*tp2,*expr,*t;
    int size;

    type1=hd_list(e1);
    type2=hd_list(e2);

    tp1=Type_IsPointer(type1);
    tp2=Type_IsPointer(type2);

    if (tp1 || tp2) {
        if (tp1 && tp2) {

            if (op==EXPR_ADD) Error("Addition 2 pointers");
            if (!Type_Compare(tp1,tp2)) 
                Error("sub operation error");
            size=Type_Size(tp1);
            if (size==-1) Error("Size error");
            expr=mk_list(list_dup(type1),
                    mk_tag(op,mk_list(e1,mk_list(e2,NULL))));
            expr=Expr_Cast(mk_tag(TYPE_INT,NULL),expr);
            if (size!=1) {
                expr=Expr_Binary(EXPR_DIV,expr,Expr_ConstInteger(size));
            }
            return expr;
        } else {

            if (tp2) {
                t=e1; e1=e2; e2=t;
                t=tp1; tp1=tp2; tp2=t;
                t=type1; type1=type2; type2=t;
            }
            if (!Type_IsInteger(type2)) Error("sub operands type error");

            size=Type_Size(tp1);
            if (size==-1) Error("size error");
            if (size!=1) {
                e2=Expr_Binary(EXPR_MUL,e2,Expr_ConstInteger(size));
            }
            e2=Expr_Cast(mk_tag(TYPE_POINTER,mk_tag(TYPE_VOID,NULL)),e2);
            return mk_list(list_dup(type1),
                    mk_tag(op,mk_list(e1,mk_list(e2,NULL))));
        }
    } else {
        return Expr_BinaryArith(op,e1,e2);
    }
}


LIST *Expr_BinaryShift(int op,LIST *e1,LIST *e2)
{
    LIST *type1;

    if ( !Type_IsInteger(hd_list(e1)) || !Type_IsInteger(hd_list(e2)) ) 
        Error("Type error of shift operation");

    e1=Expr_PromoteInteger(e1);
    type1=hd_list(e1);
    e2=Expr_Cast(mk_tag(TYPE_INT,NULL),Expr_PromoteInteger(e2));

    return mk_list(list_dup(type1),
            mk_tag(op,mk_list(e1,mk_list(e2,NULL))));
}


LIST *Expr_BinaryCompare(int op,LIST *e1,LIST *e2)
{
    LIST *res1,*res2,*tp1,*tp2,*t1,*t2,*t;

    t1=hd_list(e1);
    t2=hd_list(e2);

    tp1=Type_IsPointer(t1);
    tp2=Type_IsPointer(t2);

    if (tp1 && tp2) {
        if ( hd_tag(tp1)==TYPE_VOID || hd_tag(tp2)==TYPE_VOID ) {
        } else if (!Type_Compare(tp1,tp2)) { 

            Error("Comparasion of pointer types differents");
        }
        res1=e1;
        res2=e2;
    } else if ( tp1 || tp2 ) {
        if (tp2) {
            t=tp1; tp1=tp2; tp2=t;
            t=e1; e1=e2; e2=t;
            t=t1; t1=t2; t2=t;
        }
        if (hd_tag(tl(e2))==EXPR_INT && 
                Type_IsInteger(hd_list(e2)) && hd_int(tl(tl(e2))) == 0 ) {
            e2=Expr_Cast(mk_tag(TYPE_POINTER,mk_tag(TYPE_VOID,NULL)),e2);
            res1=e1;
            res2=e2;
        } else {
            Error("Compare type error");
        }
    } else if ( Type_IsArith(t1) && Type_IsArith(t2) ) { 
        /* comparaison de nombres */
        Expr_BinaryCast(&res1,&res2,e1,e2);
    } else {
        Error("Compare 2 object type is incompatibles");
    }
    return mk_list(mk_tag(TYPE_INT,NULL),
            mk_tag(op,mk_list(res1,mk_list(res2,NULL))));
}


LIST *Expr_LogicalInteger(LIST *expr)
{
    LIST *type;

    expr=Expr_PromoteInteger(expr);
    type=hd_list(expr);
    if (!Type_IsInteger(type) && !Type_IsPointer(type)) 
        Error("Type error");
    return expr;
}


LIST *Expr_BinaryLogical(int op,LIST *e1,LIST *e2)
{
    e1=Expr_LogicalInteger(e1);
    e2=Expr_LogicalInteger(e2);
    return mk_list(mk_tag(TYPE_INT,NULL),
            mk_tag(op,mk_list(e1,mk_list(e2,NULL))));
}

LIST *Expr_Binary(int op,LIST *e1,LIST *e2)
{
    switch (op) {
        case EXPR_ADD:
        case EXPR_SUB:
            return Expr_AddSub(op,e1,e2);

        case EXPR_MUL:
        case EXPR_DIV:
            return Expr_BinaryArith(op,e1,e2);

        case EXPR_MOD:
        case EXPR_AND:
        case EXPR_OR:
        case EXPR_XOR:
            return Expr_BinaryInteger(op,e1,e2);

        case EXPR_SHR:
        case EXPR_SHL:
            return Expr_BinaryShift(op,e1,e2);

        case EXPR_LT:
        case EXPR_LE:
        case EXPR_GT:
        case EXPR_GE:
        case EXPR_EQ:
        case EXPR_NE:
            return Expr_BinaryCompare(op,e1,e2);

        case EXPR_LOR:
        case EXPR_LAND:
            return Expr_BinaryLogical(op,e1,e2);

        default:
            Error_Internal("Error between binary operation %x",op);
            return NULL;
    }
}

LIST *Expr_Unary(int op,LIST *e1)
{
    LIST *type;

    type=hd_list(e1);
    switch (op) {
        case EXPR_NEG:
        case EXPR_PLUS:
            if (!Type_IsArith(type)) Error("Unary operation type error: '+' or '-'");
            e1=Expr_PromoteInteger(e1);
            if (op==EXPR_NEG) {
                return mk_list(list_dup(hd_list(e1)),mk_tag(op,mk_list(e1,NULL)));
            } else {
                return e1;
            }

        case EXPR_NOT:
            if (!Type_IsInteger(type)) Error("operation not error: '~'");
            e1=Expr_PromoteInteger(e1);
            return mk_list(list_dup(hd_list(e1)),mk_tag(op,mk_list(e1,NULL)));

        case EXPR_LNOT:
            return Expr_Binary(EXPR_EQ,e1,Expr_ConstInteger(0));

        default:
            Error("Type %x un-implemented",op);
            return NULL;
    }
}


LIST *Expr_Indir(LIST *e1)
{		 
    LIST *type1,*tp;

    type1=hd_list(e1);
    tp=Type_IsPointer(type1);
    if ( !tp ) 
        Error("'*' operation is error");
    return mk_list(list_dup(tp),mk_tag(EXPR_INDIR,mk_list(e1,NULL)));
}


LIST *Expr_Index(LIST *e1,LIST *e2)
{
    LIST *type1,*type2,*tp;

    type1=hd_list(e1);
    type2=hd_list(e2);
    tp=Type_IsPointer(type1);
    if ( !tp )
        Error("'[]' syntax error");
    if ( !Type_IsInteger(type2) ) 
        Error("type error");


    return Expr_Indir(Expr_Binary(EXPR_ADD,e1,e2));
}


LIST *Expr_Addr(LIST *e)
{
    LIST *t;
    Expr_CheckLValue(e);
    t=mk_tag(TYPE_POINTER,list_dup(hd_list(e)));
    return mk_list(t,mk_tag(EXPR_ADDR,mk_list(e,NULL)));
}



LIST *Expr_Field(LIST *e1,char *str)
{
    LIST *type1,*type2,*type3;
    int t1;
    SYM *s;
    int sym_table,offset;

    type1=hd_list(e1);
    t1=hd_tag(type1);
    if (t1 != TYPE_STRUCT && t1 != TYPE_UNION) 
        Error("Type 'struct' to 'union' error");

    s=hd_sym(tl(type1));
    sym_table=hd_int(tl(s->list));
    if (sym_table==-1) Error("'struct' to 'union'");

    s=Sym_Search(str,sym_table);
    if (s==NULL) Error("'struct' to 'union' undefined");
    offset=hd_int(tl(s->list));
    type3=mk_tag(TYPE_POINTER,list_dup(hd_list(s->list)));

    type2=mk_tag(TYPE_POINTER,mk_tag(TYPE_CHAR,NULL));

    return Expr_Indir(Expr_Cast(type3,
                Expr_Binary(EXPR_ADD,
                    Expr_Cast(type2,e1),
                    Expr_ConstInteger(offset))));
}

LIST *Expr_Assign(LIST *e1,LIST *e2)
{
    Expr_CheckLValue(e1);
    e2=Expr_CastAssign(hd_list(e1),e2);
    return mk_list(list_dup(hd_list(e1)),
            mk_tag(EXPR_ASSIGN,mk_list(e1,mk_list(e2,NULL))));
}



LIST *Expr_AssignOp(int op,LIST *e1,LIST *e2)
{
    int tag1;
    LIST *expr_list,*expr,*e3;
    char *var_name;
    char buf[SYM_LENMAX];

    tag1=hd_tag(tl(e1));
    expr_list=NULL;

    if (tag1!=EXPR_IDENT) {
        Sym_NewName(buf);
        var_name=buf;
        e3=Expr_Addr(e1);
        Local_Declare(var_name,STORAGE_DEFAULT,
                list_dup(hd_list(e3)),mk_tag(INIT_EXPR,e3));
        expr=Expr_Assign(Expr_Indir(Expr_Ident(var_name)),
                Expr_Binary(op,Expr_Indir(Expr_Ident(var_name)),e2));
    } else {
        var_name=hd_sym(tl(tl(e1)))->str;
        expr=Expr_Assign(Expr_Ident(var_name),
                Expr_Binary(op,Expr_Ident(var_name),e2));
    }
    list_free(e1);
    return expr;
}


LIST *Expr_Cond(LIST *e,LIST *e1,LIST *e2)
{
    LIST *res1,*res2,*t1,*t2;

    e=Expr_LogicalInteger(e);
    t1=hd_list(e1);
    t2=hd_list(e2);

    if (Type_IsArith(t1) && Type_IsArith(t2)) {			
        Expr_BinaryCast(&res1,&res2,e1,e2);
    } else if (!Type_Compare(t1,t2)) {
        Error("Types incompatibles");
    } else {
        res1=e1;
        res2=e2;
    }

    return mk_list(list_dup(hd_list(res1)),mk_tag(EXPR_COND,
                mk_list(e,mk_list(res1,mk_list(res2,NULL)))));
}




LIST *Expr_Call(LIST *e1,LIST *params1)
{
    LIST *type1,*func_params,*params,*func_rettype,*func_param_type;
    LIST *param_expr,*expr,*param_type,*param_list;
    int t1,func_type,params_nb;

    type1=hd_list(e1);
    t1=hd_tag(type1);


    if ( ! (t1==TYPE_FUNC || 
                (t1==TYPE_POINTER && hd_tag(tl(type1))==TYPE_FUNC) ) ) 
        Error("expression function error");

    if (t1==TYPE_POINTER) type1=tl(type1);


    func_type=hd_tag(tl(type1));
    func_params=hd_list(tl(tl(type1)));
    func_rettype=list_dup(tl(tl(tl(type1))));

    param_list=NULL;
    params=params1;
    params_nb=0;

    while (params!=NULL) {
        param_expr=list_dup(hd_list(params));
        params=tl(params);
        params_nb++;

        if (func_type==FUNC_NEW && func_params==NULL)
            Error("function parameters error");

        if ( (func_type==FUNC_ELLIPSIS && func_params==NULL) ||
                (func_type==FUNC_OLD) ) {

            param_type=hd_list(param_expr);
            if (Type_IsInteger(param_type)) {
                expr=Expr_PromoteInteger(param_expr);
            } else if (hd_tag(param_type) == TYPE_ARRAY) {
                func_param_type=mk_tag(TYPE_POINTER,tl(tl(list_dup(param_type))));
                expr=Expr_CastAssign(func_param_type,param_expr);
                list_free(func_param_type);
            } else {
                expr=param_expr;
            }
        } else {

            func_param_type=hd_list(tl(tl(func_params)));
            func_params=tl(tl(tl(tl(func_params))));

            expr=Expr_CastAssign(func_param_type,param_expr);
        }
        param_list=append(param_list,mk_list(expr,NULL));
    }
    if (func_type!=FUNC_OLD && func_params!=NULL) Error("parameter error");

    list_free(params1);

    return mk_list(func_rettype,mk_tag(EXPR_CALL,mk_list(e1,mk_int(params_nb,param_list))));
}



LIST *Expr_Ident(char *str)
{
    SYM *s;
    LIST *var,*type;
    int tag;

    s=Sym_Search(str,TABLE_VAR);
    if (s==NULL) Error("Variable '%s' undeclared",str);

    var=s->list;
    tag=hd_tag(var);
    switch(tag) {
        case SYM_VAR:
            type=list_dup(hd_list(tl(tl(var))));
            return mk_list(type,mk_tag(EXPR_IDENT,mk_sym(s,NULL))); 
        case SYM_ENUM_CONST:
            return Expr_ConstInteger(hd_int(tl(s->list)));
        default:
            Error("'%s': Variable error",str);
            return NULL;
    }
}



LIST *Expr_Sizeof(LIST *e,int is_expr)
{
    LIST *type;
    int size;

    if (is_expr) {
        type=hd_list(e);
    } else {
        type=e;
    }

    size=Type_Size(type);
    if (size==-1) Error("sizeof: type incompatible");

    list_free(e);

    return Expr_ConstInteger(size);
}
