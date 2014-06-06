#ifndef QUERY
#define QUERY

#include "../../util/header.h"
#include "../../util/genHashtable.h"
#include "../../util/genlist.h"

List *retrieveQuery(int len, char *input);
void validateArgs(int argc, char* argv[]);
int countDocNodes(WordNode *wordNode);
int wordArraySize(char **list);
DocumentNode *copyDocNode(DocumentNode *node);
void freeDocNodeList(DocumentNode **list);
DocumentNode **indexLookUp(char **wordList, HashTable *index);
DocumentNode **rankQuery(DocumentNode **linkedList, int n);
int charCount(FILE *fp);
char *findQueryURL(DocumentNode* docNode, char *crawledDir);
int countWords(char* buffer);
int findDocListSize(DocumentNode **list);
DocumentNode **combineArrays(DocumentNode **combined, DocumentNode **input);
void freeWordList(List *list);

#endif