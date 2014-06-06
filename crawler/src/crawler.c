/* ========================================================================== */
/* File: crawler.c - Tiny Search Engine web crawler
 *
 * Author: Kartikeya Menon
 * Date: April 2014
 *
 * Input: A seed URL, the directory for webpage file output, the desired crawl depth
 *
 * Command line options: None
 *
 * Output: A file for each webpage crawled in the given depth found from the seed url
 *
 * Error Conditions: If any of the inputs is invalid (0 =< depth =< 4) or malloc fails at any point
 *
 * Special Considerations: Compile with make
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>                           // printf

#include <curl/curl.h>                       // curl functionality

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

// ---------------- Local includes  e.g., "file.h"
#include "../util/common.h"                          // common functionality
#include "../util/web.h"                             // curl and html functionality
#include "../util/list.h"                            // webpage list functionality
#include "../util/hashtable.h"                       // hashtable functionality
#include "../util/utils.h"                           // utility stuffs



// ---------------- Constant definitions

// ---------------- Macro definitions
#define HELP "\nUsage: crawler [seedURL] [targetDirectory] [desiredDepth]\nWill search and download all of the webpages found in the seedURL (continuing to those webpages as well depending on the supplied depth) and output a file per webpage found, named 1, 2, 3, ... n to the supplied targetDirectory.\n\n"
#define EXIT_FAILURE 1
#define BUFFER_LENGTH 9

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/* ========================================================================== */


/* This is getting to be a hassle and cluttering up main, so deal with argument checking in this function. */

void validateArguments(int argc, char* argv[]) {

    struct stat directory;
    
    
    if ((argc > 1) && (!strcmp(argv[1], "--help"))) {
        printf(HELP);
        exit (0); // For UNIX $? not a failure to exit on --help
    }
    
    // First see if user fed wrong arguments
    if (argc != 4) {
        fprintf(stderr, "Error, incorrect arguments. \nUsage: ./crawler [url] [target Directory] [Crawl depth] \n");
        exit (EXIT_FAILURE);
    }
    
    // Check for out-of-domain webpages
    if (!strstr(argv[1], URL_PREFIX)) {
        fprintf(stderr, "Please only crawl *.dartmouth.edu domains.\n");
        exit (EXIT_FAILURE);
    }
    
    // Depth must be 0<d<4
    if ((argv[3][1]) || (argv[3][0] > '4') || (argv[3][0] < '0')) {
        fprintf(stderr, "Error. Crawl depth must be between 0 and 4.\n");
        exit (EXIT_FAILURE);
    }
    
    // See if directory is good and writeable
    if (stat(argv[2], &directory) != 0) {
        fprintf(stderr, "Error. The supplied directory was not found. Please enter a valid directory.\n");
        exit (EXIT_FAILURE);
    }
    
}

/*
 Function that takes the name of the target directory, the name of the file (a number between 0 and however many webpages are crawled) and a WebPage struct from which to take data and write to the file in question.
 Input: Target directory char, file name char, WebPage struct
 Output: Int indicating success (0) or not (1)
 */

int writePage(char *targetDirectory, char *file, WebPage *page) {
    
    int fileNameLen = strlen(file);
    
    char *fileName = (char *)calloc(strlen(targetDirectory) + fileNameLen + 1 + 1, sizeof(char)); // one for null, one for "/"
    //char *fileName = calloc(strlen(targetDirectory + fileNameLen) + 1, sizeof(char));
    
    if (!fileName) {
        fprintf(stderr, "File name calloc failed\n");
        return 1;
    }
    
    strcpy(fileName, targetDirectory);
    strcat(fileName, "/");
    strcat(fileName, file);
    FILE *fp;
    fp = fopen(fileName, "a+");
    
    
    if (!fp) {
        fprintf(stderr, "File supplied to write function does not exist or supplied directory unwriteable. \n");
        return 1;
    }
    
    fprintf(fp, "%s\n", page->url);
    fprintf(fp, "%d\n", page->depth);
    fprintf(fp, "%s\n", page->html);
    fclose(fp);
    
    free(fileName);
    return 0;
}


int main(int argc, char* argv[]) {
    
    // Need some things to crawl
    int currentDepth;
    int maxDepth;
    char *targetDirectory;
    char *seedURL;
    int fileNameInt = 1;
    char nameBuffer[BUFFER_LENGTH]; // number of websites is 1.9 bill = 10^9. Safe to say this won't be seg-faulting.
    
    validateArguments(argc, argv);
    // Define them
    maxDepth = atoi(argv[3]);
    targetDirectory = argv[2];
    seedURL = argv[1];
    
    // Get my hashtable to put URLS in
    HashTable *hashTable;
    hashTable = initHash();
    
    // My list to store WebPage structs
    List *webPageList;
    webPageList = initList();
    
    currentDepth = 0;

    
    
    // init curl
    curl_global_init(CURL_GLOBAL_ALL);

    // setup seed page, remember to free this later.
    WebPage *seed = (struct WebPage*)calloc(1, sizeof(WebPage));
    
    if (!seed) {
        fprintf(stderr, "Initial memory allocation for seedURL failed.\n");
        return 1;
    }
    
    // Make seed WebPage and add it's url to the hash table, checking everything
    // the check dead calls GetWebPage, so no need to do that. HTML for seed URL already in seed->url
    
    seed->url = (char *)calloc((strlen(seedURL) + 1), sizeof(char));
    
    if (!(seed->url)) {
        fprintf(stderr, "Seed->url calloc failed.\n");
        return 1;
    }
    strncpy(seed->url, seedURL, strlen(seedURL));

    seed->depth = currentDepth;
    
    int checkHashSeed, checkNorm;
    
    checkNorm = NormalizeURL(seed->url);
    if (!checkNorm) {
        free(seed->url);
        free(seed);
        fprintf(stderr, "Your seed url was not valid.\n");
        return 1;
    }
    
    if (!GetWebPage(seed)) {
        if (seed->html) {
            free(seed->html);
        }
        free(seed->url);
        free(seed);
        fprintf(stderr, "Seed URL could not be reached.\n");
        return 1;
    }
    
    checkHashSeed = hashInsert(hashTable, seed->url);
    if (!checkHashSeed) {
        free(seed->url);
        free(seed);
        fprintf(stderr, "Initial seed Hash failed.\n");
        return 1;
    }
    
    append(webPageList, seed);
    printf("[crawler]: Crawling - %s\n", seed->url);

    if (0 == maxDepth) {
        //CODE FOR PRINTING
        sprintf(nameBuffer, "%d", fileNameInt);
        if (writePage(targetDirectory, nameBuffer, seed)) {
            fprintf(stderr, "Page write to directory failed.\n");
            return 1;
        }

    }
    
    else {
        sprintf(nameBuffer, "%d", fileNameInt);
    // Output to webPageDirectory for the seed url
        if (writePage(targetDirectory, nameBuffer, seed)) {
            fprintf(stderr, "Page write to directory failed.\n");
            return 1;
        }

        int pos, checkHashInsert;
        
        while (!isEmpty(webPageList)) {
            pos = 0;
            char *result = NULL; // FREE THIS
            while (((pos = GetNextURL(webPageList->head->page->html, pos, webPageList->head->page->url, &result)) > 0) && (maxDepth > webPageList->head->page->depth)) {
                
                currentDepth = webPageList->head->page->depth + 1;
                /* POSSIBLE SILLY ARGS? */
                WebPage *new = (WebPage *) calloc(1, sizeof(WebPage));
                
                MALLOC_CHECK(stderr, new);
                
                new->html = NULL;
                
                /* POSSIBLE SILLY ARGS? */
                new->url = (char *)calloc((strlen(result) + 1), sizeof(char));
                
                MALLOC_CHECK(stderr, new->url);
                strncpy(new->url, result, strlen(result) + 1);
                
                new->depth = currentDepth;

                if (!NormalizeURL(new->url)) {
                    if (result) {
                        free(result);
                        result = NULL;
                    }
                    free(new->url);
                    free(new);
                    continue; // skip this iteration and don't add new link to linked list
                    
                }
                
                
                // Check for out-of-domain webpages
                if (!strstr(new->url, "http://old-www.cs.dartmouth.edu/~cs50/tse/")) {
                    if (result) {
                        free(result);
                        result = NULL;
                    }
                    free(new->url);
                    free(new);
                    continue;
                }
                
                // Don't look at URL if it's already been seen or its depth is too high
                if (hashLookUp(hashTable, new->url)) {
                    if (result) {
                        free(result);
                        result = NULL;
                    }
                    free(new->url);
                    free(new);
                    continue;
                }

                
                if (!GetWebPage(new)) {
                    if (result) {
                        free(result);
                        result = NULL;
                    }
                    free(new->url);
                    free(new->html);
                    free(new);
                    continue; // skip this iteration and don't add new link to linked list
                }
                
                checkHashInsert = hashInsert(hashTable, new->url);
                if (!checkHashInsert) { // This will not be run if URL already in hashList (continue from above)
                    fprintf(stderr, "Hash Insert for URL failed.\n");
                    return 1;
                }
                
                //printf("url im looking at: %s\n", new->url);

                if (append(webPageList, new)) {
                    fprintf(stderr, "Could not append url to linked list.\n");
                    return 1;
                }

                

                if (result) {
                    free(result);
                    result = NULL;
                }

            }
            //CODE FOR PRINTING
            if (result) {
                free(result);
            }
            sprintf(nameBuffer, "%d", fileNameInt);
            if (writePage(targetDirectory, nameBuffer, webPageList->head->page)) {
                fprintf(stderr, "Page write to directory failed.\n");
                return 1;
            }
            fileNameInt++;
            pop(webPageList);
            
        }
    }
    
    // cleanup curl
    curl_global_cleanup();
    
    freeList(webPageList);
    freeHash(hashTable);
    

    return 0;
}


