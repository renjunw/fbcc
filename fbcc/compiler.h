#ifndef __COMPILER_H
#define __COMPILER_H


#if defined(__cplusplus)

#ifndef __ExternC
#define __ExternC extern "C"
#endif //__ExternC

#ifndef __EXTERNC_BEGIN
#define __EXTERNC_BEGIN extern "C" {
#endif //__EXTERNC_BEGIN

#ifndef __EXTERNC_END
#define __EXTERNC_END    }
#endif //__EXTERNC_END

#else   //__cplusplus

#ifndef __ExternC
#define __ExternC extern
#endif //__ExternC

#ifndef __EXTERNC_BEGIN
#define __EXTERNC_BEGIN
#endif //__EXTERNC_BEGIN

#ifndef __EXTERNC_END
#define __EXTERNC_END    
#endif //__EXTERNC_END

#endif //__cplusplus

__EXTERNC_BEGIN


#define AVL_ECB_MAGIC      0x12345678
#define AVL_SIG_MAGIC      0xce587978
#define AVL_IMG_MAGIC      0x90abcdef


extern int as_assembler(char *input, char *output, int stack_size, int verbose);
extern int read_all_file(char *filename, char** buffer, unsigned *size);
extern char * avl_vm_get_filename(char *ori, const char *ext);
extern int avl_vm_compiling(char *in, char *asfile, char *ecb, char *hex, unsigned stack_size);
extern unsigned int avl_vm_calc_check_sum(unsigned int checksum, unsigned char *data, unsigned int size);


__EXTERNC_END

#endif //__COMPILER_H


