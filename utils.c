#include "utils.h"

char *lx2str(uint64_t n){
  static char b[20];
  if(n==0) return "0";
  uint32_t i=0;
  for(uint64_t ncpy=n; ncpy; ncpy>>=1)i ++;

  i = (i+3)/4;

  b[i+1]='\0';
  for(uint32_t j=0; j<i; j++){
    char a = (n>>i)&0xf;
    b[i] = a;
  }

  return b;
}

uint64_t catn_x64(char *restrict b, uint64_t cap, uint64_t n){
  if(n == 0){
    b[2]='0';
    return 1;
  }

  uint64_t nc=0;
  for(uint64_t ncpy = n ;ncpy ; ncpy >>= 1) nc++;
  // one char [0-9][a-z] ~ 4 bits

  nc >>= 2;

  // buffer not large enough to print
  if(nc + 2 >= cap ) return 0;

  for(int i = nc; i>=0 ;i--){
    // ça fonctionne alors pas touche
    char a = (n>>(4*(nc - i -1)))&0xf;
    2[b+i] = a + ((a>9)?'a'-10:'0');
  }
  //puts("exit with nc\n");
  //exit(nc);
  return nc;
}
