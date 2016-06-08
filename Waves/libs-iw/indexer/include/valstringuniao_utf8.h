#ifndef VALSTRINGUNIAO_H
#define VALSTRINGUNIAO_H

#include<ctype.h>
#ifndef ValidCh

 #define ValidCh(ch)  (isalnum(ch)) /* Teste de validacao	  */   
 #define InvalidCh(ch) (!ValidCh(ch))

#endif

extern char  _IsNum[256];
extern char  _Valid[256];
extern char  _Invalid[256];
extern unsigned char  _ConvertComAcentos[256];
extern unsigned char  _ConvertSemAcentos[256];
extern unsigned char _ShouldConvert;

void ConverteBufferSemEspacos(register unsigned char *ch, register unsigned char *fim, char acento);
void StartConvertSemAcentos();
void StartConvertComAcentos();
void StartValid();
void ConverteBufferSemAcentos( char *ch, char *fim , char *novo);
void ConverteBufferComAcentos(register unsigned char *ch, register unsigned char *fim);
int isupperaccents(unsigned char c);

char ConverteBufferUTF8( char ch );
void strupperaccent(unsigned char *str);

unsigned char  charupperaccent(unsigned char c);

#endif
