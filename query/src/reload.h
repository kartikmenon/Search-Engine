#ifndef RELOAD
#define RELOAD

#include "../../util/header.h"
#include "../../util/genHashtable.h"
#include "../../util/genlist.h"

char *makeFilePath(char *directory, char *fileName);
char *LoadDocIndex(char *fileName);
int GetDocID(char *fileName);
DocumentNode *DNode(int docID, int freq);
WordNode *WNode(DocumentNode *docNode, char *word);
int reloadIndexHash(char *word, int docID, int freq, HashTable *index);
HashTable *ReadFile(char *filePath);
void freeDocNode(WordNode *wordNode);
void freeWordNode(GenHashTableNode *loopNode);
void freeIndexTest(HashTable *index);

#endif 