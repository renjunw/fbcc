#ifndef __CONFIG_H
#define __CONFIG_H
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#define NSIGN_VERSION "1.0.2"

#if defined(WIN32) || defined(WINDOWS)
#include <windows.h>
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")
  #define PATHCHAR '\\'
  #define DELETEFILE(f)  DeleteFile(f)
  #define STRCASECMP(s1, s2)  stricmp(s1, s2)
  #define SNPRINTF   _snprintf
  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned int uint32_t;
  typedef char int8_t;
  typedef short int16_t;
  typedef int int32_t;
  #define OS_CALLBACK __stdcall
#else
  #include <arpa/inet.h>
  #include <unistd.h>
	#define PATHCHAR '/'
  #define SNPRINTF   snprintf
	#define DELETEFILE(f)  unlink(f)
	#define STRCASECMP(s1, s2) strcasecmp(s1, s2)
	#define OS_CALLBACK 

#endif //WIN32

#define GRK_DEF_BOOTROM_VERSION    0
#define GRK_INVALID_HEAP_BASE      0xFFFFFFFF
#define GRK_INVALID_HEAP_SIZE      0xFFFFFFFF
#define GRK_INVALID_VALUE          0xFFFFFFFF
#define GRK_INVALID_CA_ID          0
#define GRK_DEFAULT_FEATURE_FLAGS  0

#define GRK_SIGNINFO_SIZE            (32)

#define GRK_SIGNINFO_SIZE_V2         (13 * sizeof(uint32_t))


#define  AVL_ROMCODE_DEF_MEMBASE   0x00000000
#define  AVL_ROMCODE_DEF_MEMSIZE   (128 * 1024 * 1024)



#define   AVL_ROMCODE_SIGN_FLAGS_VERSION_MASK   0x000000FF

#define   AVL_ROMCODE_FLAGS_VERSION_LIBRA1          0x00
#define   AVL_ROMCODE_FLAGS_VERSION_LIBRA_ECO       0x01
#define   AVL_ROMCODE_FLAGS_VERSION_LIBRA2          0x02

#define   AVL_ROMCODE_SIGN_FLAGS_RANDOM_LOAD     (1 << 0)
#define   AVL_ROMCODE_SIGN_FLAGS_SHA256          (1 << 1)
#define   AVL_ROMCODE_SIGN_FLAGS_ENCRYPT         (1 << 2)
#define   AVL_ROMCODE_SIGN_FLAGS_3DES            (1 << 3)  //Default AES
#define   AVL_ROMCODE_SIGN_FLAGS_2ND_SIGN        (1 << 4)
#define   AVL_ROMCODE_SIGN_FLAGS_RANDOM_KB       (1 << 5)  //Default aligned to 1MB
#define   AVL_ROMCODE_SIGN_FLAGS_LOADABLE        (1 << 6)

#define AVL_ROMCODE_DEF_RANDOM_LOADING_RANGE      32


#define MAXTEMPBUF 512
#define BYTEPERLINE 8
#define ECBSIZE     36

#define HASH_SHA1_LEN 20
#define HASH_SHA256_LEN 32

#define CIPHER_LEN  1024
#define SIGN_DATALEN    256
#define SIGN_LEN    ((SIGN_DATALEN) + (GRK_SIGNINFO_SIZE))
/*
#define LITERAL_PLATFILE_TEXT "file"
#define WATCHDATAUSBKEYPRO_TEXT "ukey_watchdata_pro"

#define MINGHUAUSBKEYPRO_TEXT "ukey_minghua_pro"
*/

#define DEF_PRIKEY_FILENAME "privkey188.pem"
#define DEF_BIN_FILENAME  "vmlinux.bin"
#define DEF_SIGN_FILENAME "kernel.bin"
#define DEF_ECB_FILENAME "appecb.bin"
#define DEF_PUBKEY_FILENAME "pubkey.h"
#define DEF_GENPRIKEY_FILENAME "new_prikey.pem"
#define DEF_PUBLIC_KEY_VARIABLE "PublicKey"
#define DEF_PUBLICFILE_PEM      "publicKey_new.pem"
#define CMD_SIGN_TEXT    "sign"
#define CMD_SIGN_ELF_TEXT    "sign_elf"
#define CMD_GENKEY_TEXT  "genkey"
#define CMD_WRUKEY_TEXT "wrukey"
#define CMD_VM_TEXT     "ecb"

#define AVL_ROMCODE_IHEX_STEP_SIZE 0x20
#define AVL_ROMCODE_IHEX_SEGMENT_SIZE 0x10000
#define AVL_ROMCODE_IHEX_SEGMENT_MASK ((AVL_ROMCODE_IHEX_SEGMENT_SIZE) - 1)

#ifndef AVL_ROMCODE_LOADABLE_MAX_DESC_SIZE
#define AVL_ROMCODE_LOADABLE_MAX_DESC_SIZE  8
#endif //AVL_ROMCODE_LOADABLE_MAX_DESC_SIZE


#define ECB_NONE         "noecb"
#define ECB_DEF          "def"
#define ECB_APP          "app"
#define DEF_DECRYPT_DEVICE WATCHDATAUSBKEYPRO
#define USB_KEY_BASE_NAME "ukey"

#define DEF_DECRYPT_
#define CMD_SIGN     1
#define CMD_GENKEY   2
#define CMD_WRUKEY   3
#define CMD_VM       4
#define CMD_ELF_SIGN 5


#define DEF_KEYLEN  2048
#define DEF_EXPONENT 65537
#define DEF_KEYINDEX 0
#define DEF_ENTRY     0x00000000
#define DEF_BOOTENTRY 0x00000000
#define DEF_LINKADDR 0x0000

#define ASSERT_FREE(m) {if(m) delete m; m = NULL;}


#define ERR_OK            0

#define ERR_GENERATE_KEY -2020
#define ERR_ARGUMENT -2021
#define ERR_OPENFILE -2022
#define ERR_HASH     -2023
#define ERR_IO       -2024
#define ERR_READPRIVATEKEY -2025
#define ERR_ALLOC  -2026
#define ERR_PRIVATE_ENCRYPT -2027
#define ERR_GETECB  -2028
#define ERR_WRITEPRIVATE -2029
#define ERR_DECRYPT      -2030
#define ERR_FILESIZE     -2031
#define ERR_NUMBER       -2032
#define ERR_TIMEFORMAT   -2033
#define ERR_GMTIME       -2034
#define ERR_NOERROR      -2035
#define ERR_CMD          -2036
#define ERR_BINARY       -2037
#define ERR_SIGNFILE     -2038
#define ERR_WRITEPUBLIC  -2039
#define ERR_FAILED   -2040
#define ERR_CREATEDIR -2041
#define ERR_SELECTROOT -2042
#define ERR_WRITEFILE  -2043
#define ERR_ENVELOPE   -2044
#define ERR_SIGNATURE  -2045
#define ERR_KEY        -2046
#define ERR_GETRANDOM  -2047
#define ERR_EXTAUTHENTICATE -2048
#define ERR_ERASEDIR  -2049
#define ERR_ERASEFILE -2050
#define ERR_READFILE  -2051
#define ERR_SELECTDIR -2052
#define ERR_ACTIVATEFILE -2053
#define ERR_SELECTFILE -2054
#define ERR_USBKEY  -2055
#define ERR_CANCEL  -2056
#define ERR_ARGSCOMBO -2057
#define ERR_DEVNOTFOUND -2058
#define ERR_UNKNOWN -2059
#define ERR_ARGPRIVATE -2060
#define ERR_MEM         -2061
#define ERR_ECB         -2062
#define ERR_UNALIGNED   -2063
#define ERR_UNSUPPORTED -2064
#define ERR_INVALID_SIGN -2065
#define ERR_HEADER       -2066
#define ERR_VERSION      -2067
#define ERR_KEYIDX       -2068

#define AVL_VM_DEF_STACK_SIZE  1024

extern int Str2UL(const char * cp, uint32_t *val);


#endif //__CONFIG_H


