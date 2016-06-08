#ifndef AUXFUNCTIONS_H_
#define AUXFUNCTIONS_H_

#include "structures/result.h"
#include "parser/parser.h"
#include <algorithm> 

#define UTI_SCORE 1
#define BM25_SCORE 2
#define VETORIAL_SCORE 3


typedef struct Term {
  int term_id;
  int tf;
  float idf;
  float min_score;
  float max_score;
  float mil_score;
} Term;

void insereNoRanking(const int SIZE_, int *pos, float *threshold, const result r_ , vector<result> *candidate_results );
bool resultComparator(const result& x, const result& y);
bool resultDocIdComparator(const result& x, const result &y);
float calcScore(const float &pre, const float &idf, const float &freq, const float &norma, int index_type);

PostingList** getPostingLists(Index *index, vector<Term> &terms, int number_of_terms);
PostingList** getPostingLists(Index *index, Index *sIndex, vector<Term> &terms, int number_of_terms);
PostingList** getPostingLists(Index *index, Index *sIndex, Index *tIndex, vector<Term> &terms, int number_of_terms);
PostingList** secGetPostingLists(Index *index, vector<Term> &terms, int number_of_terms);
void sortPostings(PostingList** mplists,  int mplistsSize);
float calcLimiar(vector<Term> &terms, PostingList **plists, int &number_of_terms, int limiarType);
void popdown(PostingList **plists, int i, int num);
void sortPostingsCS(PostingList** mplists, PostingList** mSplists, int mplistsSize);
void sortPostingsCS(PostingList** mplists, PostingList** mSplists, PostingList** mTplists, int mplistsSize);
void popdownCS(PostingList **plists, PostingList **splists, int i, int num);
void popdownCS(PostingList **plists, PostingList **splists, PostingList **tplists, int i, int num);
unsigned int movepointer(PostingList *plist, unsigned int did);

#endif
