/* ========================================================================== */
/* File: web.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer
 *
 * This file contains the web/html processing functionality.
 *
 */
/* ========================================================================== */
#ifndef WEB_H
#define WEB_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

/*
 * GetNextWord - returns the next word from doc[pos] into word
 * @doc: pointer to an html document
 * @pos: current position in the document
 * @word: a pointer to a c-style string, used to pass the word back out
 *
 * Returns the current position searched so far in doc; otherwise, returns < 0;
 * The doc should be a valid character buffer. The pos argument should be 0 on the
 * initial call. The word argument should be a NULL pointer. On successful parse
 * of the html in doc, word will contain a newly allocated character buffer;
 * may be NULL on failed return. The caller is responsible for free'ing this
 * memory.
 *
 * Usage example: (retrieve all words in a page)
 * int pos = 0;
 * char *word;
 * char *doc = "<ruh>Vox clamantis <roh> 3.0 < 5.0 /> in deserto.<raggy>";
 *
 * while((pos = GetNextWord(doc, pos, &word)) > 0) {
 *     // do something with word
 *     free(word);
 * }
 */
int GetNextWord(const char *doc, int pos, char **word);

/*
 * NormalizeWord - lowercases all the alphabetic characters in word
 * @word: the character buffer to normalize
 *
 * Word is modified in-place, with all uppercase letters lowered.
 *
 * Usage example:
 * char *str = "HELLO WORLD!";
 * NormalizeWord(str);
 * // str should now be "hello world!"
 */
void NormalizeWord(char *word);

#endif // WEB_H
