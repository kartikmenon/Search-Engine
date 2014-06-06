/* ========================================================================== */
/* File: file.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer
 *
 * This file contains utility functions for dealing with files and directories.
 * These functions are not likely to be very portable, if at all. They are
 * known to run properly on Linux and BSD systems.
 *
 */
/* ========================================================================== */
#ifndef FILE_H
#define FILE_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros


/*
 * IsDir - determine if path is a directory
 * @path: path to check
 *
 * Returns non-zero if path is a directory; otherwise, 0.
 *
 * Usage example:
 * if(IsDir(".")) {
 *     // "." is a directory
 * }
 */
int IsDir(const char *path);

/*
 * GetFilenamesInDir - populates a list of filenames found in the directory dir
 * @dir: directory path to search
 * @filenames: a pointer to an array of c-style strings
 *
 * Returns the number of filenames found; otherwise, a negative number indicates
 * failure. The filenames array should not be preallocated, but will be created
 * by this function. It is the caller's responsibility to free the filenames and
 * the array containing them.
 *
 * Usage example:
 * char **filenames = NULL;
 * int num_files = 0;
 *
 * num_files = GetFilenamesInDir(".", &filenames);
 * if(num_files < 0) {
 *     // failed to get any filenames (various reasons)
 * } else {
 *     for(int i = 0; i < num_files; i++) {
 *         printf("File: %s\n", filenames[i]);
 *         free(filenames[i]);
 *     }
 *     free(filenames);
 * }
 */
int GetFilenamesInDir(const char *dir, char ***filenames);

#endif // FILE_H
