#ifndef MINIMIZER_H
#define MINIMIZER_H

typedef struct TokenMinimizer TokenMinimizer;

TokenMinimizer* token_minimizer_create(void);
char* token_minimizer_process(TokenMinimizer* minimizer, const char* token, const char* pattern);
void token_minimizer_free(TokenMinimizer* minimizer);

#endif
