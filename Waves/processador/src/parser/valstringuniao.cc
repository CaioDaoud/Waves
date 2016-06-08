#include "valstringuniao.h"

char  _IsNum[256];
char  _Valid[256];
char  _Invalid[256];
unsigned char  convertWithAccent[256];
unsigned char  convertWithoutAccent[256];
unsigned char _ShouldConvert;
unsigned char _UpperCase[256];

#define VOC_ESPANHA
#ifndef ValidCh

 #define ValidCh(ch)  (isalnum(ch))
 #define InvalidCh(ch) (!ValidCh(ch))


#endif

void startValid() {

  unsigned x;

  for( x = 0 ; x < 128 ; x++) {
    _IsNum[x]=0;
    if( ValidCh(x) ) {
      _Valid[x] = 1;
      _Invalid[x] = 0;
    }
    else {
      _Valid[x] = 0;
      _Invalid[x] = 1;
    }
  }
  for( x = 128; x < 256; x++) {
    _IsNum[x]=0;
    _Valid[x] = 0;
    _Invalid[x] = 1;
  }
  _Valid[0]=_Invalid[0]=0;
  for(x = 48; x < 58; x++) {
    _IsNum[x] = 1;
  }
  _Valid[224] = 1;  _Invalid[224] = 0;
  _Valid[225] = 1;  _Invalid[225] = 0;
  _Valid[226] = 1;  _Invalid[226] = 0;
  _Valid[227] = 1;  _Invalid[227] = 0;
  _Valid[228] = 1;  _Invalid[228] = 0;
  _Valid[229] = 1;  _Invalid[229] = 0;
  _Valid[231] = 1;  _Invalid[231] = 0;
  _Valid[232] = 1;  _Invalid[232] = 0;
  _Valid[233] = 1;  _Invalid[233] = 0;
  _Valid[234] = 1;  _Invalid[234] = 0;
  _Valid[235] = 1;  _Invalid[235] = 0;
  _Valid[236] = 1;  _Invalid[236] = 0;
  _Valid[237] = 1;  _Invalid[237] = 0;
  _Valid[238] = 1;  _Invalid[238] = 0;
  _Valid[239] = 1;  _Invalid[239] = 0;
  _Valid[241] = 1;  _Invalid[241] = 0;
  _Valid[242] = 1;  _Invalid[242] = 0;
  _Valid[243] = 1;  _Invalid[243] = 0;
  _Valid[244] = 1;  _Invalid[244] = 0;
  _Valid[245] = 1;  _Invalid[245] = 0;
  _Valid[246] = 1;  _Invalid[246] = 0;
  _Valid[249] = 1;  _Invalid[249] = 0;
  _Valid[250] = 1;  _Invalid[250] = 0;
  _Valid[251] = 1;  _Invalid[251] = 0;
  _Valid[252] = 1;  _Invalid[252] = 0;
  _Valid[253] = 1;  _Invalid[253] = 0;
  _Valid[255] = 1;  _Invalid[255] = 0;
}

void startConvertWithoutAccent() {
  
  unsigned x;
  
  startValid();
  for(x = 0; x < 65 ; x++) {
    convertWithoutAccent[x] = x;
  }
  for(x = 65; x < 91; x++) {
    convertWithoutAccent[x] = x+32;
  }
  for(; x < 128; x++) {
    convertWithoutAccent[x] = x;
  }
  convertWithoutAccent[45] = 45; 
  convertWithoutAccent[128] = 99; 
  convertWithoutAccent[129] = 32; 
  convertWithoutAccent[130] = 32;
  convertWithoutAccent[131] = 32;
  convertWithoutAccent[132] = 32;
  convertWithoutAccent[133] = 32; 
  convertWithoutAccent[134] = 32;
  convertWithoutAccent[135] = 32; 
  convertWithoutAccent[136] = 32;
  convertWithoutAccent[137] = 32;
  convertWithoutAccent[138] = 32; 
  convertWithoutAccent[139] = 32;
  convertWithoutAccent[140] = 32;
  convertWithoutAccent[141] = 32; 
  convertWithoutAccent[142] = 32;
  convertWithoutAccent[143] = 32; 
  convertWithoutAccent[144] = 32; 
  convertWithoutAccent[145] = 32;
  convertWithoutAccent[146] = 32;
  convertWithoutAccent[147] = 32;
  convertWithoutAccent[148] = 32;
  convertWithoutAccent[149] = 32; 
  convertWithoutAccent[150] = 32;
  convertWithoutAccent[151] = 32;
  convertWithoutAccent[152] = 32;
  convertWithoutAccent[153] = 32;
  convertWithoutAccent[154] = 32; 
  convertWithoutAccent[155] = 32;
  convertWithoutAccent[156] = 32;
  convertWithoutAccent[157] = 32;
  convertWithoutAccent[158] = 32;
  convertWithoutAccent[159] = 32;  
  convertWithoutAccent[160] = 32; 
  convertWithoutAccent[161] = 161;
  convertWithoutAccent[162] = 162;
  convertWithoutAccent[163] = 163;
  convertWithoutAccent[164] = 164;
  convertWithoutAccent[165] = 165; 
  convertWithoutAccent[166] = 166;
  convertWithoutAccent[167] = 167;
  convertWithoutAccent[168] = 168;
  convertWithoutAccent[169] = 169;
  convertWithoutAccent[170] = 32; 
  convertWithoutAccent[171] = 171;
  convertWithoutAccent[172] = 172;
  convertWithoutAccent[173] = 173;
  convertWithoutAccent[174] = 174;
  convertWithoutAccent[175] = 175; 
  convertWithoutAccent[176] = 176; 
  convertWithoutAccent[177] = 177;
  convertWithoutAccent[178] = 178;
  convertWithoutAccent[179] = 179;
  convertWithoutAccent[180] = 180;
  convertWithoutAccent[181] = 181; 
  convertWithoutAccent[182] = 182;
  convertWithoutAccent[183] = 183;
  convertWithoutAccent[184] = 184;
  convertWithoutAccent[185] = 185;
  convertWithoutAccent[186] = 186; 
  convertWithoutAccent[187] = 187;
  convertWithoutAccent[188] = 188;
  convertWithoutAccent[189] = 189;
  convertWithoutAccent[190] = 190;
  convertWithoutAccent[191] = 191; 
  convertWithoutAccent[192] = 97;  
  convertWithoutAccent[193] = 97;
  convertWithoutAccent[194] = 97;
  convertWithoutAccent[195] = 97;
  convertWithoutAccent[196] = 97;
  convertWithoutAccent[197] = 97; 
  convertWithoutAccent[198] = 198;
  convertWithoutAccent[199] = 99;
  convertWithoutAccent[200] = 101;
  convertWithoutAccent[201] = 101;
  convertWithoutAccent[202] = 101; 
  convertWithoutAccent[203] = 101;
  convertWithoutAccent[204] = 105;
  convertWithoutAccent[205] = 105;
  convertWithoutAccent[206] = 105;
  convertWithoutAccent[207] = 105; 
  convertWithoutAccent[208] = 208;
#ifndef VOC_ESPANHA
 convertWithoutAccent[209] = 241;
#else
  convertWithoutAccent[209] = 110;
#endif
  convertWithoutAccent[210] = 111;
  convertWithoutAccent[211] = 111;
  convertWithoutAccent[212] = 111;
  convertWithoutAccent[213] = 111; 
  convertWithoutAccent[214] = 111;
  convertWithoutAccent[215] = 215;
  convertWithoutAccent[216] = 216;
  convertWithoutAccent[217] = 117;
  convertWithoutAccent[218] = 117; 
  convertWithoutAccent[219] = 117;
  convertWithoutAccent[220] = 117;
  convertWithoutAccent[221] = 121;
  convertWithoutAccent[222] = 122;
  convertWithoutAccent[223] = 223; 
  convertWithoutAccent[224] = 97 ; 
  convertWithoutAccent[225] = 97 ;
  convertWithoutAccent[226] = 97 ;
  convertWithoutAccent[227] = 97 ;
  convertWithoutAccent[228] = 97 ;
  convertWithoutAccent[229] = 97 ; 
  convertWithoutAccent[230] = 230;
  convertWithoutAccent[231] = 99 ;
  convertWithoutAccent[232] = 101;
  convertWithoutAccent[233] = 101;
  convertWithoutAccent[234] = 101; 
  convertWithoutAccent[235] = 101;
  convertWithoutAccent[236] = 105;
  convertWithoutAccent[237] = 105;
  convertWithoutAccent[238] = 105;
  convertWithoutAccent[239] = 105; 
  convertWithoutAccent[240] = 240; 
#ifdef VOC_ESPANHA
  convertWithoutAccent[241] = 241;
#else
  convertWithoutAccent[241] = 110;
#endif
  convertWithoutAccent[242] = 111;
  convertWithoutAccent[243] = 111;
  convertWithoutAccent[244] = 111;
  convertWithoutAccent[245] = 111; 
  convertWithoutAccent[246] = 111;
  convertWithoutAccent[247] = 247;
  convertWithoutAccent[248] = 248;
  convertWithoutAccent[249] = 117;
  convertWithoutAccent[250] = 117; 
  convertWithoutAccent[251] = 117;
  convertWithoutAccent[252] = 117;
  convertWithoutAccent[253] = 121;
  convertWithoutAccent[254] = 254;
  convertWithoutAccent[255] = 121; 

}
