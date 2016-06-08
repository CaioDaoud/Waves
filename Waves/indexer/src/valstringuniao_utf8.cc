#include "../../libs-iw/indexer/include/valstringuniao_utf8.h"

char  _IsNum[256];
char  _Valid[256];
char  _Invalid[256];
unsigned char  _ConvertComAcentos[256];
unsigned char  _ConvertSemAcentos[256];
unsigned char _ShouldConvert;

unsigned char _UpperCase[256];

// Ativa vocabulario da Espanha:

#define VOC_ESPANHA



#ifndef ValidCh

 #define ValidCh(ch)  (isalnum(ch)) /* Teste de validacao	  */   
 #define InvalidCh(ch) (!ValidCh(ch))


#endif


void StartValid() {

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

void StartConvertComAcentos() {

  unsigned x;

  StartValid();
  for(x = 1; x < 65 ; x++) {
      _ConvertComAcentos[x] = x;
  }
  for(x = 65; x < 91; x++) {
    _ConvertComAcentos[x] = x+32;
  }
  for(; x < 128; x++) {
      _ConvertComAcentos[x] = x;
  }
  _ConvertComAcentos[45] = 45; 
  _ConvertComAcentos[128] = 231; // 0x80
  _ConvertComAcentos[129] = 0; 
  _ConvertComAcentos[130] = 0;
  _ConvertComAcentos[131] = 0;
  _ConvertComAcentos[132] = 0;
  _ConvertComAcentos[133] = 133; //0x85
  _ConvertComAcentos[134] = 0;
  _ConvertComAcentos[135] = 135; // 87
  _ConvertComAcentos[136] = 0;
  _ConvertComAcentos[137] = 0;
  _ConvertComAcentos[138] = 0; // 0x8A
  _ConvertComAcentos[139] = 0;
  _ConvertComAcentos[140] = 0;
  _ConvertComAcentos[141] = 0; 
  _ConvertComAcentos[142] = 0;
  _ConvertComAcentos[143] = 0; // 8F
  _ConvertComAcentos[144] = 0; // 90
  _ConvertComAcentos[145] = 145;
  _ConvertComAcentos[146] = 146; 
  _ConvertComAcentos[147] = 147;
  _ConvertComAcentos[148] = 148;
  _ConvertComAcentos[149] = 149; //95
  _ConvertComAcentos[150] = 150;
  _ConvertComAcentos[151] = 151;
  _ConvertComAcentos[152] = 152;
  _ConvertComAcentos[153] = 153;
  _ConvertComAcentos[154] = 154; // 9A
  _ConvertComAcentos[155] = 0; // 9B
  _ConvertComAcentos[156] = 0;
  _ConvertComAcentos[157] = 0;
  _ConvertComAcentos[158] = 0;
  _ConvertComAcentos[159] = 0; // 9F
  _ConvertComAcentos[160] = 160; // A0
  _ConvertComAcentos[161] = 161;
  _ConvertComAcentos[162] = 162;
  _ConvertComAcentos[163] = 163;
  _ConvertComAcentos[164] = 164;
  _ConvertComAcentos[165] = 165; //A5
  _ConvertComAcentos[166] = 166;
  _ConvertComAcentos[167] = 167;
  _ConvertComAcentos[168] = 168;
  _ConvertComAcentos[169] = 169;
  _ConvertComAcentos[170] = 170; //AA
  _ConvertComAcentos[171] = 171;
  _ConvertComAcentos[172] = 172;
  _ConvertComAcentos[173] = 173;
  _ConvertComAcentos[174] = 174;
  _ConvertComAcentos[175] = 175; //AF
  _ConvertComAcentos[176] = 176; //B0
  _ConvertComAcentos[177] = 177;
  _ConvertComAcentos[178] = 178;
  _ConvertComAcentos[179] = 179;
  _ConvertComAcentos[180] = 180;
  _ConvertComAcentos[181] = 181; //B5
  _ConvertComAcentos[182] = 182;
  _ConvertComAcentos[183] = 183;
  _ConvertComAcentos[184] = 184;
  _ConvertComAcentos[185] = 185;
  _ConvertComAcentos[186] = 186; //BA
  _ConvertComAcentos[187] = 187;
  _ConvertComAcentos[188] = 188;
  _ConvertComAcentos[189] = 189;
  _ConvertComAcentos[190] = 190;
  _ConvertComAcentos[191] = 191; //BF
  _ConvertComAcentos[192] = 224; //C0
  _ConvertComAcentos[193] = 225;
  _ConvertComAcentos[194] = 226;
  _ConvertComAcentos[195] = 227;
  _ConvertComAcentos[196] = 228;
  _ConvertComAcentos[197] = 229; //C5
  _ConvertComAcentos[198] = 198;
  _ConvertComAcentos[199] = 231;
  _ConvertComAcentos[200] = 232;
  _ConvertComAcentos[201] = 233;
  _ConvertComAcentos[202] = 234; // CA
  _ConvertComAcentos[203] = 235;
  _ConvertComAcentos[204] = 236;
  _ConvertComAcentos[205] = 237;
  _ConvertComAcentos[206] = 238;
  _ConvertComAcentos[207] = 239; //CF
  _ConvertComAcentos[208] = 208; //D0
  _ConvertComAcentos[209] = 241;
  _ConvertComAcentos[210] = 242;
  _ConvertComAcentos[211] = 243;
  _ConvertComAcentos[212] = 244;
  _ConvertComAcentos[213] = 245; // D5
  _ConvertComAcentos[214] = 246;
  _ConvertComAcentos[215] = 215;
  _ConvertComAcentos[216] = 216;
  _ConvertComAcentos[217] = 249;
  _ConvertComAcentos[218] = 250; //DA
  _ConvertComAcentos[219] = 251;
  _ConvertComAcentos[220] = 252;
  _ConvertComAcentos[221] = 253; // 253
  _ConvertComAcentos[222] = 222;
  _ConvertComAcentos[223] = 223; // DF
  _ConvertComAcentos[224] = 224; //E0
  _ConvertComAcentos[225] = 225;
  _ConvertComAcentos[226] = 226;
  _ConvertComAcentos[227] = 227;
  _ConvertComAcentos[228] = 228;
  _ConvertComAcentos[229] = 229; //E5
  _ConvertComAcentos[230] = 230;
  _ConvertComAcentos[231] = 231;
  _ConvertComAcentos[232] = 232;
  _ConvertComAcentos[233] = 233;
  _ConvertComAcentos[234] = 234; //EA
  _ConvertComAcentos[235] = 235;
  _ConvertComAcentos[236] = 236;
  _ConvertComAcentos[237] = 237;
  _ConvertComAcentos[238] = 238;
  _ConvertComAcentos[239] = 239; //EF
  _ConvertComAcentos[240] = 240; //F0
  _ConvertComAcentos[241] = 241;
  _ConvertComAcentos[242] = 242;
  _ConvertComAcentos[243] = 243;
  _ConvertComAcentos[244] = 244;
  _ConvertComAcentos[245] = 245; //F5
  _ConvertComAcentos[246] = 246;
  _ConvertComAcentos[247] = 247;
  _ConvertComAcentos[248] = 248;
  _ConvertComAcentos[249] = 249;
  _ConvertComAcentos[250] = 250; //FA
  _ConvertComAcentos[251] = 251;
  _ConvertComAcentos[252] = 252;
  _ConvertComAcentos[253] = 253;
  _ConvertComAcentos[254] = 254;
  _ConvertComAcentos[255] = 255;
  
  for(x = 0; x < 97; x++) {
    _UpperCase[x] = x; 
  }
  for(x = 97; x < 123; x++) {
    _UpperCase[x] = x-32; 
  }
 for(x = 123; x < 224; x++) {
    _UpperCase[x] = x; 
  }
 for(x = 224; x < 253; x++) {
   _UpperCase[x] = x-32; 
 }
}


void StartConvertSemAcentos() {
  
  unsigned x;
  
  StartValid();
  for(x = 1; x < 65 ; x++) {
    _ConvertSemAcentos[x] = x;
  }
  for(x = 65; x < 91; x++) {
    _ConvertSemAcentos[x] = x+32;
  }
  for(; x < 128; x++) {
    _ConvertSemAcentos[x] = x;
  }
  _ConvertSemAcentos[45] = 45; 
  _ConvertSemAcentos[128] = 99; // 0x80
  _ConvertSemAcentos[129] = 0; 
  _ConvertSemAcentos[130] = 0;
  _ConvertSemAcentos[131] = 0;
  _ConvertSemAcentos[132] = 0;
  _ConvertSemAcentos[133] = 32; //0x85
  _ConvertSemAcentos[134] = 0;
  _ConvertSemAcentos[135] = 32; // 87
  _ConvertSemAcentos[136] = 0;
  _ConvertSemAcentos[137] = 0;
  _ConvertSemAcentos[138] = 0; // 0x8A
  _ConvertSemAcentos[139] = 0;
  _ConvertSemAcentos[140] = 0;
  _ConvertSemAcentos[141] = 0; 
  _ConvertSemAcentos[142] = 0;
  _ConvertSemAcentos[143] = 0; // 8F
  _ConvertSemAcentos[144] = 0; // 90
  _ConvertSemAcentos[145] = 32;
  _ConvertSemAcentos[146] = 32;
  _ConvertSemAcentos[147] = 32;
  _ConvertSemAcentos[148] = 32;
  _ConvertSemAcentos[149] = 32; // 95
  _ConvertSemAcentos[150] = 32;
  _ConvertSemAcentos[151] = 32;
  _ConvertSemAcentos[152] = 32;
  _ConvertSemAcentos[153] = 32;
  _ConvertSemAcentos[154] = 32; // 9A
  _ConvertSemAcentos[155] = 0;
  _ConvertSemAcentos[156] = 0;
  _ConvertSemAcentos[157] = 0;
  _ConvertSemAcentos[158] = 0;
  _ConvertSemAcentos[159] = 0;  // 9F
  _ConvertSemAcentos[160] = 32; // A0
  _ConvertSemAcentos[161] = 161;
  _ConvertSemAcentos[162] = 162;
  _ConvertSemAcentos[163] = 163;
  _ConvertSemAcentos[164] = 164;
  _ConvertSemAcentos[165] = 165; //A5
  _ConvertSemAcentos[166] = 166;
  _ConvertSemAcentos[167] = 167;
  _ConvertSemAcentos[168] = 168;
  _ConvertSemAcentos[169] = 169;
  _ConvertSemAcentos[170] = 32; //AA
  _ConvertSemAcentos[171] = 171;
  _ConvertSemAcentos[172] = 172;
  _ConvertSemAcentos[173] = 173;
  _ConvertSemAcentos[174] = 174;
  _ConvertSemAcentos[175] = 175; //AF
  _ConvertSemAcentos[176] = 176; //B0
  _ConvertSemAcentos[177] = 177;
  _ConvertSemAcentos[178] = 178;
  _ConvertSemAcentos[179] = 179;
  _ConvertSemAcentos[180] = 180;
  _ConvertSemAcentos[181] = 181; //B5
  _ConvertSemAcentos[182] = 182;
  _ConvertSemAcentos[183] = 183;
  _ConvertSemAcentos[184] = 184;
  _ConvertSemAcentos[185] = 185;
  _ConvertSemAcentos[186] = 186; // BA
  _ConvertSemAcentos[187] = 187;
  _ConvertSemAcentos[188] = 188;
  _ConvertSemAcentos[189] = 189;
  _ConvertSemAcentos[190] = 190;
  _ConvertSemAcentos[191] = 191; //BF
  _ConvertSemAcentos[192] = 97;  // C0
  _ConvertSemAcentos[193] = 97;
  _ConvertSemAcentos[194] = 97;
  _ConvertSemAcentos[195] = 97;
  _ConvertSemAcentos[196] = 97;
  _ConvertSemAcentos[197] = 97; // C5
  _ConvertSemAcentos[198] = 198;
  _ConvertSemAcentos[199] = 99;
  _ConvertSemAcentos[200] = 101;
  _ConvertSemAcentos[201] = 101;
  _ConvertSemAcentos[202] = 101; // CA
  _ConvertSemAcentos[203] = 101;
  _ConvertSemAcentos[204] = 105;
  _ConvertSemAcentos[205] = 105;
  _ConvertSemAcentos[206] = 105;
  _ConvertSemAcentos[207] = 105; // CF
  _ConvertSemAcentos[208] = 208; // D0
#ifndef VOC_ESPANHA
 _ConvertSemAcentos[209] = 241;
#else
  _ConvertSemAcentos[209] = 110;
#endif
  _ConvertSemAcentos[210] = 111;
  _ConvertSemAcentos[211] = 111;
  _ConvertSemAcentos[212] = 111;
  _ConvertSemAcentos[213] = 111; // D5
  _ConvertSemAcentos[214] = 111;
  _ConvertSemAcentos[215] = 215;
  _ConvertSemAcentos[216] = 216;
  _ConvertSemAcentos[217] = 117;
  _ConvertSemAcentos[218] = 117; // DA
  _ConvertSemAcentos[219] = 117;
  _ConvertSemAcentos[220] = 117;
  _ConvertSemAcentos[221] = 121;
  _ConvertSemAcentos[222] = 122;
  _ConvertSemAcentos[223] = 223; //DF
  _ConvertSemAcentos[224] = 97 ; // E0
  _ConvertSemAcentos[225] = 97 ;
  _ConvertSemAcentos[226] = 97 ;
  _ConvertSemAcentos[227] = 97 ;
  _ConvertSemAcentos[228] = 97 ;
  _ConvertSemAcentos[229] = 97 ; // E5
  _ConvertSemAcentos[230] = 230;
  _ConvertSemAcentos[231] = 99 ;
  _ConvertSemAcentos[232] = 101;
  _ConvertSemAcentos[233] = 101;
  _ConvertSemAcentos[234] = 101; // EA
  _ConvertSemAcentos[235] = 101;
  _ConvertSemAcentos[236] = 105;
  _ConvertSemAcentos[237] = 105;
  _ConvertSemAcentos[238] = 105;
  _ConvertSemAcentos[239] = 105; // EF
  _ConvertSemAcentos[240] = 240; // F0
#ifdef VOC_ESPANHA
  _ConvertSemAcentos[241] = 241;
#else
  _ConvertSemAcentos[241] = 110;
#endif
  _ConvertSemAcentos[242] = 111;
  _ConvertSemAcentos[243] = 111;
  _ConvertSemAcentos[244] = 111;
  _ConvertSemAcentos[245] = 111; // F5
  _ConvertSemAcentos[246] = 111;
  _ConvertSemAcentos[247] = 247;
  _ConvertSemAcentos[248] = 248;
  _ConvertSemAcentos[249] = 117;
  _ConvertSemAcentos[250] = 117; // FA
  _ConvertSemAcentos[251] = 117;
  _ConvertSemAcentos[252] = 117;
  _ConvertSemAcentos[253] = 121;
  _ConvertSemAcentos[254] = 254;
  _ConvertSemAcentos[255] = 121; // FF

}

void ConverteBufferComAcentos(register unsigned char *ch, register unsigned char *fim) {

    while(ch < fim) {
      *ch = _ConvertComAcentos[*ch];
      ch++;
    }
}


//~ void ConverteBufferSemAcentos(register unsigned char *ch, register unsigned char *fim) {

    //~ while(ch < fim) {
      //~ *ch = _ConvertSemAcentos[*ch];
      //~ ch++;
    //~ }
//~ }

char ConverteBufferUTF8( char ch )
{
	switch( (int)ch ) {
		case -92:{ return 'a'; break; }
		case -93:{ return 'a'; break; }
		case -94:{ return 'a'; break; }
		case -95:{ return 'a'; break; }
		case -96:{ return 'a'; break; }
		case -124:{ return 'a'; break; }
		case -125:{ return 'a'; break; }
		case -126:{ return 'a'; break; }
		case -127:{ return 'a'; break; }
		case -128:{ return 'a'; break; }
		case -85:{ return 'e'; break; }
		case -86:{ return 'e'; break; }
		case -87:{ return 'e'; break; }
		case -88:{ return 'e'; break; }
		case -117:{ return 'e'; break; }
		case -118:{ return 'e'; break; }
		case -119:{ return 'e'; break; }
		case -120:{ return 'e'; break; }
		case -81:{ return 'i'; break; }
		case -82:{ return 'i'; break; }
		case -83:{ return 'i'; break; }
		case -84:{ return 'i'; break; }
		case -113:{ return 'i'; break; }
		case -114:{ return 'i'; break; }
		case -115:{ return 'i'; break; }
		case -116:{ return 'i'; break; }
		case -74:{ return 'o'; break; }
		case -75:{ return 'o'; break; }
		case -76:{ return 'o'; break; }
		case -77:{ return 'o'; break; }
		case -78:{ return 'o'; break; }
		case -106:{ return 'o'; break; }
		case -107:{ return 'o'; break; }
		case -108:{ return 'o'; break; }
		case -109:{ return 'o'; break; }
		case -110:{ return 'o'; break; }
		case -68:{ return 'u'; break; }
		case -69:{ return 'u'; break; }
		case -70:{ return 'u'; break; }
		case -71:{ return 'u'; break; }
		case -100:{ return 'u'; break; }
		case -101:{ return 'u'; break; }
		case -102:{ return 'u'; break; }
		case -103:{ return 'u'; break; }
		case -89:{ return 'c'; break; }
		case -121:{ return 'c'; break; }
		default: { return ch; }
	}
}

void ConverteBufferSemAcentos( char *ch, char *fim, char *novo ) {
    while(ch < fim) {
		
		if( *ch < 0 ) {
			if( *ch == -61 ) {
				ch++;
				*novo = ConverteBufferUTF8( *ch );
				novo++;
			}
		}
		else{
			*novo = _ConvertSemAcentos[*ch];
			novo++;
		}		
		
		
      	ch++;
    }
	*novo = '\0';
}

void ConverteBufferSemEspacos(register unsigned char *ch, register unsigned char *fim, char acento) {
    register unsigned char *ch2;
    
    if (acento){
        ch2 = ch;
        while(ch < fim) {
            *ch2 = _ConvertComAcentos[*ch];
            ch2++;
            if(*ch == ' ') {
                do {
                    ch++;
                } while(*ch == ' ');
            }
            else {
                ch++;
            }
        }
        *ch2 = 0;
    }else{
        ch2 = ch;
        while(ch < fim) {
            *ch2 = _ConvertSemAcentos[*ch];
            ch2++;
            if(*ch == ' ') {
                do {
                    ch++;
                } while(*ch == ' ');
            }
            else {
                ch++;
            }
        }
        *ch2 = 0;
    }
}


int isupperaccents(unsigned char c) {
  
  if(((c >= 65) && (c <= 90)) ||
     ( (c >= 192) && (c <= 214)) ||
     ( (c >= 216) && (c <= 221))) {
    return 1;
  }
  return 0;
}

unsigned char  charupperaccent(unsigned char c) {
  return _UpperCase[c];
}

void strupperaccent(unsigned char *str) {
  
  while(*str) {
    *str = _UpperCase[*str];
    str++;
  }
}
