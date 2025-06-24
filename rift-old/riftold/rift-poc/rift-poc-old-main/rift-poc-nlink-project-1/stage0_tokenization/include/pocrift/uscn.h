#ifndef USCN_H
#define USCN_H

typedef struct USCN_Normalizer USCN_Normalizer;

USCN_Normalizer* uscn_create(void);
char* uscn_normalize(USCN_Normalizer* normalizer, const char* input);
void uscn_free(USCN_Normalizer* normalizer);

#endif
