/* -------------------------------------------------------------
file: intencode.cc
   Functions to encode numbers using  fixed length coding, elias gama 
coding and elias delta coding.

Warning: The word of the output should have a zero.

---------------------------------------------------------------*/

#ifndef _PREFIXINMEM_H
#define  _PREFIXINMEM_H


#include <stdio.h>
#include <stdlib.h>

#define MORE_THAN_256MBRAM


extern unsigned char _DeltaDecode[256];
extern   unsigned char _TamDelta[];
extern   unsigned int _Delta[];
extern   unsigned int _Gama[];
extern   unsigned char _TamGama[];
extern   unsigned char _GamaDecode[256];
extern   unsigned char _UnaryDecode[256];
extern   unsigned char _startbit; 
/********************************************************************
  
                   General functions

******************************************************************/





inline void InitStartBit(unsigned char x) { _startbit = x; }




inline void MostraBits(unsigned int n) {
  
  unsigned x;
  
  for(x = 0x80000000; x>0; x = x >> 1) {
    
    if(n & x) printf("1"); else printf("0");
  }
}




#define _IncPos(curpos,fim,nextstart)  \
    if(curpos == fim) { \
      curpos = nextstart; \
    } \
    else { \
      curpos++; \
    }

#define _RawCopyBits(code,len,output,btat) { \
  register unsigned int c; \
  *output = (*output| ( code >> btat)); \
  c = btat + len; \
  if( c < 32) { \
    btat = c; \
  } \
  else { \
    output++; \
     if(c > 32) { \
        if (btat) { \
          *output = code << (32 - btat); \
        } \
        else { \
          *output = 0; \
        } \
      btat = c - 32; \
     } \
     else { \
      *output = 0; \
      btat = 0; \
  } \
 } \
}


 void RawCopyBits(unsigned int &code, unsigned char &len, unsigned int *output,unsigned char &btat) ;


/* ----------------------------------------------------
   Macro to copy the "len" bits of code to the outbut buffer
  that starts at btat bit position.

  code    data to be tranferred
  len     number of bits to be transferred
  output  pointer to the output buffer
  btat    bit position where the data should start at the output buffer

----------------------------------------------------------------*/

inline void  _CopyBits(unsigned int code,
		       unsigned int len,
		       unsigned int *&output,
		       unsigned char &btat, 
		       unsigned int *&fim, 
		       unsigned int *nextstart) { 
  register unsigned int c; 
  *output = ((*output)| ( code >> btat)); 
  c = btat + len; 
  if( c < 32) { 
    btat = c; 
  } 
  else { 
    _IncPos(output,fim,nextstart); 
    if (btat){ 
      *output = code << (32 - btat); 
    } 
    else { 
      *output = 0; 
    } 
    btat = c - 32; 
    if(output == nextstart) {
      fim = NULL; 
      btat += _startbit;
      if(btat < 32 ) {
	*output = (*output) >>_startbit;
      }
      else if (btat == 32) {
        *output = (*output) >>_startbit;
	output ++;
	*output = 0;
	btat = 0;
      }
      else {
	c = *output;
	*output = (*output) >> _startbit;
	output ++;
	btat -= 32;
	*output = c << (32 - _startbit);
      }
    }
  } 
} 



/* ********************************************************

                 GAMA CODING IMPLEMENTATION

***********************************************************/

/*-----------------------------------------------------------------

  Encodes a number using elias GAMA
 n      - number to be coded
 output - 32 bits pointer to the output buffer 
 btat   - bit position to encode the number at the current output position
 fim    - last word of the current block 
 nextstart - word where the nextblock starts
---------------------------------------------------------------- */


inline void BlockGamaEncode(register unsigned int n, 
		      unsigned int *&output,
		      unsigned  char &btat, unsigned int  *&fim,
		      unsigned  int *nextstart) {
register  unsigned int len;
register  unsigned int code;

   if( n == 0) { 
     printf("tentativa de codificar o valor 0\n"); 
     exit(1); 
   } 

 if (n<64) { 
   code=_Gama[n];
   len = _TamGama[n];
 }
  else if(n < 0x80 ) { // n <2^7
      code = (0xFC000000 | ((n-0x40) << (32-13) ));    // 1111 1110 0000 00
      len = 13;
    }
    else if (n < 0x100 ) { // n <2^8
      code = (0xFE000000 | ((n-0x80) << (32-15) ));   
      len = 15;
    }
    else if (n < 0x200 ) { // n <2^9
      code = (0xFF000000 | ((n-0x100) << (32-17) ));  
      len = 17;
    }
 else if (n < 0x400 ) { // n <2^10
      code = (0xFF800000 | ((n-0x200) << (32-19) ));  
      len = 19;
    }
 else if (n < 0x800 ) { // n <2^11
      code = (0xFFC00000 | ((n-0x400) << (32-21) ));  
      len = 21;
    }
 else if (n < 0x1000 ) { // n <2^12
      code = (0xFFE00000 | ((n-0x800) << (32-23) ));  
      len = 23;
    }
 else if (n < 0x2000 ) { // n <2^13
      code = (0xFFF00000 | ((n-0x1000) << (32-25) ));  
      len = 25;
    }
else if (n < 0x4000 ) { // n <2^14
      code = (0xFFF80000 | ((n-0x2000) << (32-27) ));  
      len = 27;
    }
else if (n < 0x8000 ) { // n <2^15
      code = (0xFFFC0000 | ((n-0x4000) << (32-29) ));  
      len = 29;
    }
else if (n < 0x10000 ) { // n <2^15
      code = (0xFFFE0000 | ((n-0x8000) << (32-31) ));  
      len = 31;
    }
else if (n < 0x20000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFF0000;  
      len = 17;
      _CopyBits(code,len,output,btat,fim,nextstart);
     code = (n-0x10000) << (32-16);
      len = 16;
}
else if (n < 0x40000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFF8000;  
      len = 18;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x20000) << (32-17);
      len = 17;
}
else if (n < 0x80000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFC000;  
      len = 19;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x40000) << (32-18);
      len = 18;
}
else if (n < 0x100000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFE000;  
      len = 20;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x80000) << (32-19);
      len = 19;
}
else if (n < 0x200000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFF000;  
      len = 21;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x100000) << (32-20);
      len = 20;
}
else if (n < 0x400000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFF800;  
      len = 22;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x200000) << (32-21);
      len = 21;
}
else if (n < 0x800000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFC00;  
      len = 23;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x400000) << (32-22);
      len = 22;
}
else if (n < 0x1000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFE00;  
      len = 24;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x800000) << (32-23);
      len = 23;
}
else if (n < 0x2000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFF00;  
      len = 25;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x1000000) << (32-24);
      len = 24;
}
else if (n < 0x4000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFF80;  
      len = 26;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x2000000) << (32-25);
      len = 25;
}
else if (n < 0x8000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFFC0;  
      len = 27;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x4000000) << (32-26);
      len = 26;
}
else if (n < 0x10000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFFE0;  
      len = 28;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x8000000) << (32-27);
      len = 27;
}
else if (n < 0x20000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFFF0;  
      len = 29;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x10000000) << (32-28);
      len = 28;
}
else if (n < 0x40000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFFF8;  
      len = 30;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x20000000) << (32-29);
      len = 29;
}
else if (n < 0x80000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xFFFFFFFC;  
      len = 31;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x40000000) << (32-30);
      len = 30;
} 
else { fprintf(stderr,"Encode: maximum gama code number is 0x7FFFFFFF %d %d\n", n, btat);
 exit(1);
}

    _CopyBits(code,len,output,btat, fim, nextstart);
}





/* --------------------------------------------------------
   Auxiliar function to  GAMA and DELTA coding

exp    - numero de bits a ser decodificados
btat   - bit atual no dado  a ser decodificado 
p      - posicao do buffer que esta sendo decodificada
result - recebe o resultado da decodificacao

-------------------------------------------------------*/

inline void Block_aux_decode_bin(unsigned int exp,
		   unsigned  char &btat,
		   unsigned int *&p, unsigned int &result, 
		   unsigned int *&fim,
		   unsigned int *nextstart) { 
unsigned int t;
 result=0x1<<exp; 
  t = ( (*p<<btat) >> (32-exp)); 
 btat+= exp; 
 if (btat >31) {
   _IncPos(p,fim,nextstart); 
   if(btat == 32) { 
     btat = 0;
     if(p == nextstart) {
       fim = NULL;
       btat =_startbit;
     }
   }
   else {
     exp = btat-32;
     if(p == nextstart) {
       fim = NULL;
       btat =_startbit;
     }
     else {
       btat = 0;
     }
     t = t|( (*p<<btat) >> (32-exp)); 
     btat+= exp; 
     if(btat > 31) {
       p++;
       if(btat == 32) { 
	 btat = 0;
       }
       else {
	 btat -= 32;
	  t = t| (*p>>(32-btat));
       } 
     }
   }
 }
 result += t; 
}

/*-----------------------------------------------------------------
  Decode Elias GAMA compressed numbers.

 p - pointer to the buffer to be decoded
 btat - bit position in the buffer where de decoding should start

 returns the decoded number
-----------------------------------------------------------------*/

inline unsigned int BlockGamaDecode(unsigned int  *&p, unsigned char &btat,
		   unsigned int *&fim,
		   unsigned int *nextstart) { 

  unsigned int result;
  unsigned c,c2;
 
  c = (*p<<btat)>>24;
  result = _GamaDecode[c];
  if(result< 16) {
    c2 = btat +_TamGama[result];
    if( c2 < 32) {
      btat = c2;
      return result;
    }
    else if (c2 == 32) {
      _IncPos(p,fim,nextstart);
      if(p == nextstart) {
	fim = NULL;
	btat = _startbit;
      }
      else {
	btat = 0;
      }
      return  result;
    }
  }
  {
    unsigned int t,tam_bin;
    t = _UnaryDecode[c];
    tam_bin = 0;
    while (t > 8) {
      tam_bin +=8;
      btat += 8;
      if(btat>31) { 
	_IncPos(p,fim,nextstart); 
	if(p == nextstart) {
	  fim = NULL;
	  btat = _startbit;
	}
	else {
	  btat=0;
	} 
      }
    UltimoElemento:
      c = (*p<<btat)>>24;
      t = _UnaryDecode[c];
    }
    c2 = btat + t;
    if( c2 < 32) {
      btat = c2;
      tam_bin += t;
      tam_bin --;
      Block_aux_decode_bin(tam_bin,btat,p,result,fim,nextstart); // 2^(tam_bin-1) + binario 
      //  btat = c2;
      return result;
    }
    else {
      _IncPos(p,fim,nextstart);
      if(c2 == 32) { 
	if(p == nextstart) {
	  fim = NULL;
	  btat = _startbit;
	}
	else {
	  btat =0; 
	}
	tam_bin += t;
	tam_bin --;
	Block_aux_decode_bin(tam_bin,btat,p,result,fim,nextstart); // 2^(tam_bin-1) + binario 
	return result; 
      }
      if(p == nextstart) {
	fim = NULL;
	btat=_startbit;
      }
      else {
	btat = 0;
      }
	t --;
	c = c | ( ((*p) << btat)) >> (24 +t);
	if( _UnaryDecode[c] < 9 ) {
	btat += (_UnaryDecode[c]-t);
	/*
	  To be used only if you are planning to use more than 256 MB of
	  RAM
	*/
#ifdef  MORE_THAN_256MBRAM
	if(btat > 31) {
	  p++;
	  btat = 0;
          t --;
          c = c | ( (*p) >> (24 +t));
	  if( _UnaryDecode[c] < 9 ) {
	    btat += (_UnaryDecode[c]-t);
	    tam_bin += _UnaryDecode[c];
	    tam_bin --;
	    Block_aux_decode_bin(tam_bin,btat,p,result,fim,nextstart); 
	    return result;
	  }
	  else {
	    btat += 8-t;
	    tam_bin+= 8;
	    goto UltimoElemento;
	  }
	}
	/* END OF PORTION EXCLUSIVE FOR MORE THAN 256 MB of RAM */
#endif
	
	tam_bin += _UnaryDecode[c];
	tam_bin --;
	Block_aux_decode_bin(tam_bin,btat,p,result,fim,nextstart); // 2^(tam_bin-1) + binario 
	return result;
	}
	else {
	  btat += 8-t;
	  tam_bin+= 8;
	  goto UltimoElemento;
	}
    }
  }
}




/******************************************************************

                 FIXED LENGTH CODING				    
  
******************************************************************/

/* ------------------------------------------------------
   Decodifica numeros codificados em binarios de "tam" bits

tam    - numero de bits a ser decodificados
btat   - bit atual no dado  a ser decodificado 
p      - posicao do buffer que esta sendo decodificada
result - recebe o resultado da decodificacao
----------------------------------------------------------*/

inline unsigned int BinaryDecode(unsigned int tam,
				  unsigned int *p) {
 return ( (*p) >> (32-tam)); 
}


/* -----------------------------------------------------------
   Codifica numeros em binario usando "tam" bits


tam    - numero de bits a ser decodificados
btat   - bit atual no dado  a ser decodificado 
output - posicao do buffer que esta sendo decodificada
n      - numero a ser codificado
 fim    - last word of the current block 
 nextstart - word where the nextblock starts
------------------------------------------------------*/

inline void BinaryEncode( unsigned int code, unsigned int len,
			  unsigned int *&output,
			  unsigned  char &btat) { 
  register unsigned int c;
  code = code << (32 - len);
  *output = (*output| ( code >> btat)); 
  c = btat + len; 
  if( c < 32) { 
    btat = c; 
  } 
  else { 
    output ++;
    if (btat){ 
      *output = code << (32 - btat); 
    } 
    else { 
      *output = 0; 
    } 
    btat = c - 32; 
  } 
}


/* ----------------------------------------------
 Uses btat = 0;
*/

inline void BinaryReEncode( unsigned int n, unsigned int len,
			  unsigned int *output) { 

 register unsigned int code = n;

 code = code << (32 -len);
 *output = (*output)& (0xFFFFFFFF >> len);
 *output = *output|  code; 
}




/* -----------------------------------------------------------

code     - numero a ser codificado
len    - numero de bits a ser decodificados
output - posicao do buffer que esta sendo decodificada

Codifies code always starting at the first bit of the *output
and without change the remaining bits of the *output
------------------------------------------------------*/

inline void BinaryEncodeNextBlock( unsigned int code, unsigned int len,
			  unsigned int *output) { 

  code = (code << (32 - len)) | (0xFFFFFFFF >> len);
  *output = *output | ( 0xFFFFFFFF << (32 - len));
  *output = *output & code;
}


/* ********************************************************

                 DELTA CODING IMPLEMENTATION

***********************************************************/


/*-----------------------------------------------------------------

  Encodes a number using elias DELTA
 n      - number to be coded
 output - 32 bits pointer to the output buffer 
 btat   - bit position to encode the number at the current output position
 fim    - last word of the current block 
 nextstart - word where the nextblock starts
---------------------------------------------------------------- */


inline void BlockDeltaEncode(register unsigned int n, 
			unsigned int *&output,
			unsigned  char &btat,
			unsigned int *&fim,
			unsigned int *nextstart) {
register  unsigned int len;
register  unsigned int code;

 if (n<64) { 
   code=_Delta[n];
   len = _TamDelta[n];
 }
  else if(n < 0x80 ) { // n <2^7
      code = (0xD8000000 | ((n-0x40) << (32-11) ));    // 1111 1110 0000 00
      len = 11;
    }
    else if (n < 0x100 ) { // n <2^8
      code = (0xE0000000 | ((n-0x80) << (32-14) ));   
      len = 14;
    }
    else if (n < 0x200 ) { // n <2^9
      code = (0xE2000000 | ((n-0x100) << (32-15) ));  
      len = 15;
    }
 else if (n < 0x400 ) { // n <2^10
      code = (0xE4000000 | ((n-0x200) << (32-16) ));  
      len = 16;
    }
 else if (n < 0x800 ) { // n <2^11
      code = (0xE6000000 | ((n-0x400) << (32-17) ));  
      len = 17;
    }
 else if (n < 0x1000 ) { // n <2^12
      code = (0xE8000000 | ((n-0x800) << (32-18) ));  
      len = 18;
    }
 else if (n < 0x2000 ) { // n <2^13
      code = (0xEA000000 | ((n-0x1000) << (32-19) ));  
      len = 19;
    }
else if (n < 0x4000 ) { // n <2^14
      code = (0xEC000000 | ((n-0x2000) << (32-20) ));  
      len = 20;
    }
else if (n < 0x8000 ) { // n <2^15
      code = (0xEE000000 | ((n-0x4000) << (32-21) ));  
      len = 21;
    }
else if (n < 0x10000 ) { // n <2^15
      code = (0xF0000000 | ((n-0x8000) << (32-24) ));  
      len = 24;
    }
else if (n < 0x20000 ) { // n <2^16 
      code = (0xF0800000| ((n-0x10000) << (32-25) ));  
      len = 25;
}
else if (n < 0x40000 ) {
 code = (0xF1000000| ((n-0x20000) << (32-26) )); 
 len = 26;
}
else if (n < 0x80000 ) { // n <2^16 
  code = (0xF1800000| ((n-0x40000) << (32-27) )); 
  len = 27;
}
else if (n < 0x100000 ) { // n <2^16
  code = (0xF2000000| ((n-0x80000) << (32-28) )); 
  len = 28;
}
else if (n < 0x200000 ) { // n <2^16 
 code = (0xF2800000| ((n-0x100000) << (32-29) )); 
  len = 29;
}
else if (n < 0x400000 ) { // n <2^16 
 code = (0xF3000000| ((n-0x200000) << (32-30) )); 
  len = 30;
}
else if (n < 0x800000 ) { // n <2^16 
 code = (0xF3800000| ((n-0x400000) << (32-31) )); 
  len = 31;
}
else if (n < 0x1000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF4000000;  
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x800000) << (32-23);
      len = 23;
}
else if (n < 0x2000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF4800000;  
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x1000000) << (32-24);
      len = 24;
}
else if (n < 0x4000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF5000000;  
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x2000000) << (32-25);
      len = 25;
}
else if (n < 0x8000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF5800000;
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x4000000) << (32-26);
      len = 26;
}
else if (n < 0x10000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF6000000;
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x8000000) << (32-27);
      len = 27;
}
else if (n < 0x20000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF6800000;  
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x10000000) << (32-28);
      len = 28;
}
else if (n < 0x40000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF7000000;
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x20000000) << (32-29);
      len = 29;
}
else if (n < 0x80000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF7800000;
      len = 9;
      _CopyBits(code,len,output,btat, fim, nextstart);
     code = (n-0x40000000) << (32-30);
      len = 30;
}
else { fprintf(stderr,"Encode: maximum gama code number is 0x7FFFFFFF %d %d\n", n, btat);
 exit(1);
}

    _CopyBits(code,len,output,btat, fim, nextstart);
}



/*-----------------------------------------------------------------
  Decode Elias DELTA compressed numbers.

 p - pointer to the buffer to be decoded
 btat - bit position in the buffer where de decoding should start

 returns the decoded number
-----------------------------------------------------------------*/

inline unsigned int BlockDeltaDecode(unsigned int  *&p, unsigned char &btat,
				 unsigned int *&fim,
				 unsigned int *nextstart) {
  unsigned int result;
  register unsigned c,c2;
  //  unsigned int *ant;
 
  c = (*p<<btat)>>24;
  result = _DeltaDecode[c];
  if(result< 16) {
    c2 = btat +_TamDelta[result];
    if( c2 < 32) {
      btat = c2;
      return result;
    }
    else if (c2 == 32) {
      _IncPos(p,fim,nextstart);
      if(p == nextstart) {
	fim = NULL;
	btat = _startbit;
      }
      else {
	btat = 0;
      }
      return  result;
    }
  }
  c2 = BlockGamaDecode(p,btat,fim,nextstart);
  c2 --;
  Block_aux_decode_bin(c2,btat,p,result,fim,nextstart); // 2^(tam_bin-1) + binario 
  return result;
}


/*-----------------------------------------------------------------

  Encodes a number using elias GAMA
 n      - number to be coded
 output - 32 bits pointer to the output buffer 
 btat   - bit position to encode the number at the current output position
 fim    - last word of the current block 
 nextstart - word where the nextblock starts
---------------------------------------------------------------- */

inline void RawGamaEncode(register unsigned int n, 
		      unsigned int *&output,
		      unsigned  char &btat) {
register  unsigned int len;
register  unsigned int code;

 if (n<64) { 
   code=_Gama[n];
   len = _TamGama[n];
 }
  else if(n < 0x80 ) { // n <2^7
    code = (0xFC000000 | ((n-0x40) << (32-13) ));    // 1111 1110 0000 00
    len = 13;
  }
 else if (n < 0x100 ) { // n <2^8
   code = (0xFE000000 | ((n-0x80) << (32-15) ));   
   len = 15;
 }
 else if (n < 0x200 ) { // n <2^9
   code = (0xFF000000 | ((n-0x100) << (32-17) ));  
   len = 17;
 }
 else if (n < 0x400 ) { // n <2^10
   code = (0xFF800000 | ((n-0x200) << (32-19) ));  
   len = 19;
 }
 else if (n < 0x800 ) { // n <2^11
   code = (0xFFC00000 | ((n-0x400) << (32-21) ));  
   len = 21;
 }
 else if (n < 0x1000 ) { // n <2^12
   code = (0xFFE00000 | ((n-0x800) << (32-23) ));  
   len = 23;
 }
 else if (n < 0x2000 ) { // n <2^13
   code = (0xFFF00000 | ((n-0x1000) << (32-25) ));  
   len = 25;
 }
 else if (n < 0x4000 ) { // n <2^14
   code = (0xFFF80000 | ((n-0x2000) << (32-27) ));  
   len = 27;
 }
 else if (n < 0x8000 ) { // n <2^15
   code = (0xFFFC0000 | ((n-0x4000) << (32-29) ));  
   len = 29;
 }
 else if (n < 0x10000 ) { // n <2^15
   code = (0xFFFE0000 | ((n-0x8000) << (32-31) ));  
   len = 31;
 }
 else if (n < 0x20000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFF0000;  
   len = 17;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x10000) << (32-16);
   len = 16;
 }
 else if (n < 0x40000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFF8000;  
   len = 18;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x20000) << (32-17);
   len = 17;
 }
 else if (n < 0x80000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFC000;  
   len = 19;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x40000) << (32-18);
   len = 18;
 }
 else if (n < 0x100000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFE000;  
   len = 20;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x80000) << (32-19);
   len = 19;
 }
 else if (n < 0x200000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFF000;  
   len = 21;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x100000) << (32-20);
   len = 20;
 }
 else if (n < 0x400000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFF800;  
   len = 22;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x200000) << (32-21);
   len = 21;
 }
 else if (n < 0x800000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFC00;  
   len = 23;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x400000) << (32-22);
   len = 22;
 }
 else if (n < 0x1000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFE00;  
   len = 24;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x800000) << (32-23);
   len = 23;
 }
 else if (n < 0x2000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFF00;  
   len = 25;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x1000000) << (32-24);
   len = 24;
 }
 else if (n < 0x4000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFF80;  
   len = 26;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x2000000) << (32-25);
   len = 25;
 }
 else if (n < 0x8000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFFC0;  
   len = 27;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x4000000) << (32-26);
   len = 26;
 }
 else if (n < 0x10000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFFE0;  
   len = 28;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x8000000) << (32-27);
   len = 27;
}
 else if (n < 0x20000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFFF0;  
   len = 29;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x10000000) << (32-28);
   len = 28;
 }
 else if (n < 0x40000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFFF8;  
   len = 30;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x20000000) << (32-29);
   len = 29;
 }
 else if (n < 0x80000000 ) { // n <2^16 // It does not fit in 32 bits
   code = 0xFFFFFFFC;  
   len = 31;
   _RawCopyBits(code,len,output,btat);
   code = (n-0x40000000) << (32-30);
   len = 30;
 }
else { fprintf(stderr,"Encode: maximum gama code number is 0x7FFFFFFF %d %d\n", n, btat);
 exit(1);
 }
 
 _RawCopyBits(code,len,output,btat);
}

 
/* --------------------------------------------------------
   Auxiliar function to  GAMA and DELTA coding

exp    - numero de bits a ser decodificados
btat   - bit atual no dado  a ser decodificado 
p      - posicao do buffer que esta sendo decodificada
result - recebe o resultado da decodificacao

-------------------------------------------------------*/

inline void Rawaux_decode_bin(unsigned int exp,
		   unsigned  char &btat,
		   unsigned int *&p, unsigned int &result) { 
unsigned int t;
 result=0x1<<exp; 
  t = ( (*p<<btat) >> (32-exp)); 
 btat+= exp; 
 if (btat >31) { 
   p++;
   if(btat == 32) { 
     btat = 0;
   }
   else {
     btat -=32; 
     t = t|(*p>>(32-btat)); 
   }
  } 
 result += t; 
		}

/*-----------------------------------------------------------------
  Decode Elias GAMA compressed numbers.

 p - pointer to the buffer to be decoded
 btat - bit position in the buffer where de decoding should start

 returns the decoded number
-----------------------------------------------------------------*/

inline unsigned int RawGamaDecode(unsigned int  *&p, unsigned char &btat) { 

  unsigned int result;
  unsigned c,c2;
  unsigned int *ant;
 
  c = (*p<<btat)>>24;
  result = _GamaDecode[c];
  if(result< 16) {
    c2 = btat +_TamGama[result];
    if( c2 < 32) {
      btat = c2;
      return result;
    }
    else {
      ant = p;
      p++;
      if(c2 == 32) { 
	btat =0; 
	return result; 
      }
      c2 = c;
      c = c | ((*p)>>(56-btat));
      result = _GamaDecode[c];
      if(result<16) {
	btat = (btat+_TamGama[result])-32;
	return result;
      }
      else {
        p = ant;
	c = c2;
	goto DecodeLargeNum;
      }
    }
    return result;
  }
  else {
DecodeLargeNum:
    unsigned int t,tam_bin;
   t = _UnaryDecode[c];
   tam_bin = 0;
   while (t > 8) {
     tam_bin +=8;
     btat += 8;
     if(btat>31) { 
       p++;
       btat=0; 
     }
   UltimoElemento:
     c = (*p<<btat)>>24;
     t = _UnaryDecode[c];
   }
   c2 = btat + t;
   if( c2 < 32) {
     btat = c2;
     tam_bin += t;
     tam_bin --;
     Rawaux_decode_bin(tam_bin,btat,p,result); // 2^(tam_bin-1) + binario 
     //  btat = c2;
     return result;
   }
   else {
     p++;
     if(c2 == 32) { 
       btat =0; 
       tam_bin += t;
       tam_bin --;
       Rawaux_decode_bin(tam_bin,btat,p,result); // 2^(tam_bin-1) + binario 
       return result; 
     }
     c = c | ((*p)>>(56-btat));
     t = _UnaryDecode[c];
    
     if( t < 9 ) {
       btat = ( btat + t )-32;
       tam_bin += t;
       tam_bin --;
       Rawaux_decode_bin(tam_bin,btat,p,result); // 2^(tam_bin-1) + binario 
       return result;
     }
     else {
       btat = ( btat -(32- 8));
       tam_bin+= 8;
       goto UltimoElemento;
     }
   }
  }
}


/* ********************************************************

                 DELTA CODING IMPLEMENTATION

***********************************************************/



/*-----------------------------------------------------------------

  Encodes a number using elias DELTA
 n      - number to be coded
 output - 32 bits pointer to the output buffer 
 btat   - bit position to encode the number at the current output position
 fim    - last word of the current block 
 nextstart - word where the nextblock starts
---------------------------------------------------------------- */


inline void RawDeltaEncode(register unsigned int n, 
			unsigned int *&output,
			unsigned  char &btat) {
register  unsigned int len;
register  unsigned int code;

 if (n<64) { 
   code=_Delta[n];
   len = _TamDelta[n];
 }
  else if(n < 0x80 ) { // n <2^7
      code = (0xD8000000 | ((n-0x40) << (32-11) ));    // 1111 1110 0000 00
      len = 11;
    }
    else if (n < 0x100 ) { // n <2^8
      code = (0xE0000000 | ((n-0x80) << (32-14) ));   
      len = 14;
    }
    else if (n < 0x200 ) { // n <2^9
      code = (0xE2000000 | ((n-0x100) << (32-15) ));  
      len = 15;
    }
 else if (n < 0x400 ) { // n <2^10
      code = (0xE4000000 | ((n-0x200) << (32-16) ));  
      len = 16;
    }
 else if (n < 0x800 ) { // n <2^11
      code = (0xE6000000 | ((n-0x400) << (32-17) ));  
      len = 17;
    }
 else if (n < 0x1000 ) { // n <2^12
      code = (0xE8000000 | ((n-0x800) << (32-18) ));  
      len = 18;
    }
 else if (n < 0x2000 ) { // n <2^13
      code = (0xEA000000 | ((n-0x1000) << (32-19) ));  
      len = 19;
    }
else if (n < 0x4000 ) { // n <2^14
      code = (0xEC000000 | ((n-0x2000) << (32-20) ));  
      len = 20;
    }
else if (n < 0x8000 ) { // n <2^15
      code = (0xEE000000 | ((n-0x4000) << (32-21) ));  
      len = 21;
    }
else if (n < 0x10000 ) { // n <2^15
      code = (0xF0000000 | ((n-0x8000) << (32-24) ));  
      len = 24;
    }
else if (n < 0x20000 ) { // n <2^16 
      code = (0xF0800000| ((n-0x10000) << (32-25) ));  
      len = 25;
}
else if (n < 0x40000 ) {
 code = (0xF1000000| ((n-0x20000) << (32-26) )); 
 len = 26;
}
else if (n < 0x80000 ) { // n <2^16 
  code = (0xF1800000| ((n-0x40000) << (32-27) )); 
  len = 27;
}
else if (n < 0x100000 ) { // n <2^16
  code = (0xF2000000| ((n-0x80000) << (32-28) )); 
  len = 28;
}
else if (n < 0x200000 ) { // n <2^16 
  code = (0xF2800000| ((n-0x100000) << (32-29) )); 
  len = 29;
}
else if (n < 0x400000 ) { // n <2^16 
  code = (0xF3000000| ((n-0x200000) << (32-30) )); 
  len = 30;
}
else if (n < 0x800000 ) { // n <2^16 
  code = (0xF3800000| ((n-0x400000) << (32-31) )); 
  len = 31;
}
else if (n < 0x1000000 ) { // n <2^16 // It does not fit in 32 bits
  code = 0xF4000000;  
  len = 9;
  _RawCopyBits(code,len,output,btat);
  code = (n-0x800000) << (32-23);
  len = 23;
}
 else if (n < 0x2000000 ) { // n <2^16 // It does not fit in 32 bits
      code = 0xF4800000;  
      len = 9;
      _RawCopyBits(code,len,output,btat);
      code = (n-0x1000000) << (32-24);
      len = 24;
}
else if (n < 0x4000000 ) { // n <2^16 // It does not fit in 32 bits
  code = 0xF5000000;  
  len = 9;
  _RawCopyBits(code,len,output,btat);
  code = (n-0x2000000) << (32-25);
  len = 25;
}
else if (n < 0x8000000 ) { // n <2^16 // It does not fit in 32 bits
  code = 0xF5800000;
  len = 9;
  _RawCopyBits(code,len,output,btat);
  code = (n-0x4000000) << (32-26);
  len = 26;
}
else if (n < 0x10000000 ) { // n <2^16 // It does not fit in 32 bits
  code = 0xF6000000;
  len = 9;
  _RawCopyBits(code,len,output,btat);
  code = (n-0x8000000) << (32-27);
  len = 27;
}
else if (n < 0x20000000 ) { // n <2^16 // It does not fit in 32 bits
  code = 0xF6800000;  
  len = 9;
  _RawCopyBits(code,len,output,btat);
  code = (n-0x10000000) << (32-28);
  len = 28;
}
else if (n < 0x40000000 ) { // n <2^16 // It does not fit in 32 bits
  code = 0xF7000000;
  len = 9;
  _RawCopyBits(code,len,output,btat);
  code = (n-0x20000000) << (32-29);
  len = 29;
}
else if (n < 0x80000000 ) { // n <2^16 // It does not fit in 32 bits
  code = 0xF7800000;
  len = 9;
  _RawCopyBits(code,len,output,btat);
  code = (n-0x40000000) << (32-30);
  len = 30;
}
else { fprintf(stderr,"Encode: maximum gama code number is 0x7FFFFFFF %u %u\n", n, btat);
 exit(1);
}
 _RawCopyBits(code,len,output,btat);
}




/*-----------------------------------------------------------------
  Decode Elias DELTA compressed numbers.

 p - pointer to the buffer to be decoded
 btat - bit position in the buffer where de decoding should start

 returns the decoded number
-----------------------------------------------------------------*/

inline unsigned int RawDeltaDecode(unsigned int  *&p, unsigned char &btat) {
  unsigned int result;
  register unsigned c,c2;
  unsigned int *ant;
  
  c = (*p<<btat)>>24;
  result = _DeltaDecode[c];
  if(result< 16) {
    c2 = btat +_TamDelta[result];
    if( c2 < 32) {
      btat = c2;
      return result;
    }
    else {
      ant = p;
      p++;
      if(c2 == 32) { 
	btat =0; 
	return result; 
      }
      c = c | ((*p)>>(56-btat));
      result = _DeltaDecode[c];
      if(result<16) {
	btat = (btat+_TamDelta[result])-32;
	return result;
      }
      else {
	p = ant;
      }
    }
  }
  c2 = RawGamaDecode(p,btat);
  c2 --;
  Rawaux_decode_bin(c2,btat,p,result); // 2^(tam_bin-1) + binario 
  return result;
}


#endif
