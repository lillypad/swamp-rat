// Swamp RAT Stub Defines

#ifndef TCP_PORT_MAX
#define TCP_PORT_MAX 65535
#endif

#ifndef TCP_PORT_MIN
#define TCP_PORT_MIN 1
#endif

#ifndef MAX_DOMAIN_LEN
#define MAX_DOMAIN_LEN 63
#endif

/* #ifndef BOOL_DEFINED */
/* typedef enum{false, true} bool; */
/* #define BOOL_DEFINED */
/* #endif */

#ifndef CONFIG_DEFINED
typedef struct{
  int xor_key;
  char host[MAX_DOMAIN_LEN];
  int host_port;
} config;
#define CONFIG_DEFINED
#endif
