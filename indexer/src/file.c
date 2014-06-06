/* ========================================================================== */
/* File: file.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer
 *
 * Author: Ira Ray Jenkins - jenkins@cs.dartmouth.edu
 * Date: Tue Apr 29, 2014
 *
 * These functions are provided for the students to simplify the manipulation
 * of files and directories.
 */
/* ========================================================================== */
#define _BSD_SOURCE                          // scandir,
                                             // students shouldn't take advantage
                                             // of the bsd extensions, but
                                             // it makes it easier to test file
                                             // types

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h>                          // free, calloc
#include <string.h>                          // strlen, strcpy
#include <dirent.h>                          // scandir, alphasort

#include <sys/stat.h>                        // stat functionality

// ---------------- Local includes  e.g., "file.h"
#include "file.h"                             // file/dir functionality

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes
static int SelectFile(const struct dirent *entry);

// ---------------- Public functions

int IsDir(const char *path)
{
    struct stat statbuf;                     // directory stat buffer

    // make sure we have a path
    if(!path) return 0;

    // if we can stat it, and it's a directory
    return (!stat(path, &statbuf) && S_ISDIR(statbuf.st_mode));
}

int GetFilenamesInDir(const char *dir, char ***filenames)
{
    struct dirent **namelist;                // list of dir entries from scandir
    int num_ents;                            // number of regular files found
    int status;                              // return status

    // make sure we have a directory path to work with
    if(!dir) { return -1; }

    // scan the directory for regular files
    status = num_ents = scandir(dir, &namelist, SelectFile, alphasort);
    if(num_ents < 0) {                       // something went wrong scanning
        return -1;
    }

    // allocate the filenames array
    *filenames = calloc(num_ents, sizeof(char*));
    if(!*filenames) {
        status = -1;
        goto cleanup;                        // sorry Dijkstra
    }

    // copy all the filenames and free the dirents
    for(int i = 0; i < num_ents; i++) {
        // allocate a filename large enough to contain the name
        (*filenames)[i] = calloc(strlen(namelist[i]->d_name) + 1, sizeof(char));
        if(!(*filenames)[i]) {
            status = -1;
            goto cleanup;                    // sorry Dijkstra
        }

        // copy the filename
        strcpy((*filenames)[i], namelist[i]->d_name);
    }

cleanup:
    if(namelist) {
        for(int i = 0; i < num_ents; i++) {
            if(namelist[i])
                free(namelist[i]);
        }
        free(namelist);
    }

    return status;
}

/*
 * SelectFile - test if a directory entry is a file
 * @entry: directory entry to test
 *
 * Returns non-zero if the directory entry is a regular file; otherwise, 0.
 *
 * This function uses the BSD dirent extensions, d_type and  DT_REG. Posix
 * doesn't promise that dirent will contain a type, but BSD does. Using this
 * avoids a separate call to stat.
 *
 * Should have no use outside of this file, thus declared static.
 */
static int SelectFile(const struct dirent *entry)
{
    return(entry->d_type == DT_REG);  // BSD ONLY
}
