/**
\file valstringuniao.h
\brief Converte os caracteres da base em caracteres alfanum�ricos.
*/
#ifndef VALSTRINGUNIAO_H
#define VALSTRINGUNIAO_H


#include<ctype.h>
#ifndef ValidCh

 #define ValidCh(ch)  (isalnum(ch))
 #define InvalidCh(ch) (!ValidCh(ch))

#endif

extern char  _IsNum[256];
extern char  _Valid[256];
extern char  _Invalid[256];
extern unsigned char  convertWithAccent[256];
extern unsigned char  convertWithoutAccent[256];
extern unsigned char _ShouldConvert;

void startConvertWithoutAccent();
void startValid();

#endif



