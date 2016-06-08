#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vocabulario.h"
#include "valstringuniao.h"
#define WORD_SIZE 30

class Parser{
public:
    
    int* parseQuery(const char *_query, Vocabulario *_vocabulary, int *_numberOfTerms);
    
};

#endif
