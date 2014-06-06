/* ========================================================================== */
/* File: web.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * Author: Ira Ray Jenkins - jenkins@cs.dartmouth.edu
 * Date: Sun Apr 20, 2014
 *
 * These functions are provided for the students to simplify the html parsing
 * and web retrieval.
 */
/* ========================================================================== */
#define _GNU_SOURCE                          // strncasecmp, strdup
                                             // students shouldn't take advantage
                                             // of the gnu extensions, but
                                             // parsing html without is a pain

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>                           // fprintf, stderr
#include <stdlib.h>                          // free, calloc, realloc
#include <ctype.h>                           // tolower, isspace
#include <string.h>                          // strcasestr, strchr, strrchr,
                                             // strncasecmp, memset, strlen,
                                             // memcpy, strnlen, strncpy,
                                             // strcmp, strdup, strndup, strcat,
                                             // strpbrk

#include <curl/curl.h>                       // curl functionality

// ---------------- Local includes  e.g., "file.h"
#include "web.h"                             // web functionality
#include "common.h"                          // common functionality

// ---------------- Constant definitions

// ---------------- Macro definitions
#define NUM_EXTS 3                           // size of EXTS array

// ---------------- Structures/Types
// students shouldn't need to use this at all
struct URL {
    char* scheme;                            // http://
    char* user;                              // username:password@
    char* host;                              // www.example.com
    char* path;                              // /path/to/file.html
    char* query;                             // ?name1=val1&name2=val2
    char* fragment;                          // #top
};

// ---------------- Private variables
// students shouldn't need to use this at all'
const char* EXTS[] = {                       // valid extensions
    "html",
    "jsp",
    "php"
};

// ---------------- Private prototypes
static char *RemoveDotSegments(char *input);
static void RemoveWhitespace(char* str);
static int ParseURL(char* str, struct URL* url);
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

// ---------------- Public functions
/*
 * GetWebPage - curl's page->url into page->html
 *
 * Assumptions:
 *     1. page has been allocated
 *     2. page->url contains the url to curl
 *     3. page->html is NULL at call time
 *
 * Pseudocode:
 *     1. check for valid page
 *     2. allocate buffer to page->html, set page->html_len to 0
 *     3. setup curl
 *     4. curl the page->url
 *     5. check return status
 *     6. cleanup
 */
int GetWebPage(WebPage* page)
{
    int status = 1;                          // return value
    CURL* curl_handle;
    CURLcode res;                            // curl response code

    // check page
    if(!page) { return 0; }

    // allocate space for the html
    page->html = calloc(1, sizeof(char));
    page->html_len = 0;

    // init curl session
    curl_handle = curl_easy_init();

    // specify url
    curl_easy_setopt(curl_handle, CURLOPT_URL, page->url);

    // send all data to this function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    // pass page struct to callback function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)page);

    // add a user agent just in case servers need it
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* Add any additional options here */

    // get the page
    res = curl_easy_perform(curl_handle);

    // check response code
    if(res != CURLE_OK) {
        status = 0;
    }

    // cleanup curl stuff
    curl_easy_cleanup(curl_handle);

    return status;
}

/*
 * GetNextURL - get the next url from html[pos] into result
 *
 * Assumptions:
 *     1. html and base_url are allocated and valid
 *     2. pos = 0 on initial call
 *     3. result is NULL and will be allocated internally
 *
 * Pseudocode:
 *     1. check arguments
 *     2. if pos = 0: remove whitespace from html
 *     3. find hyperlink starting tags "<a" or "<A"
 *     4. find next href attribute "href="
 *     5. find next end tag ">"
 *     6. check that href comes before end tag
 *     7. deal with quoted and unquoted urls
 *     8. determine if url is absolute
 *     9. fixup relative links
 *    10. create new character buffer for result and return it
 */
int GetNextURL(char *html, int pos, char *base_url, char **result)
{
    int bad_link;                            // is this link ill formatted?
    int relative;                            // is this link relative?
    char delim;                              // url delimiter: ' ', ''', '"'
    char *lnk;                               // hyperlink tags
    char *href;                              // href in a tag
    char *end;                               // end of hyperlink tag or url
    char *ptr;                               // absolute vs. relative

    // make sure we have text and base url
    if(!html || !base_url) return -1;

    // condense html, makes parsing easier
    if(pos == 0)
        RemoveWhitespace(html);

    // parse for hyperlinks
    do {
        relative = 0;                        // assume absolute link
        bad_link = 0;                        // assume valid link

        // find tag "<a" or "<A""
        lnk = strcasestr(&html[pos], "<a");

        // no more links on this page
        if(!lnk) { result = NULL; return -1; }


        // find next href after hyperlink tag
        href = strcasestr(lnk, "href=");

        // no more links on this page
        if(!href) { result = NULL; return -1; }

        // find end of hyperlink tag
        end = strchr(lnk, '>');

        // if the href we have is outside the current tag, continue
        if(end && (end < href)) {
            bad_link = 1; pos+=2; continue;
        }

        // move href to beginning of url
        href+=5;

        // something went wrong, just continue
        if(!href) { bad_link=1; pos+=2; continue; }

        // is the url quoted?
        if(*href == '\'' || *href == '"') {  // yes, href="url" or href='url'
            delim = *(href++);               // remember delimiter
            end = strchr(href, delim);       // find next of same delimiter
        }
        else {                               // no, href=url
            end = strchr(href, '>');         // hope: <a ... href=url>
                                             // since we've stripped whitespace
                                             // this could mangle things like:
                                             // <a ... href=url name=val>
        }

        // if we don't know where to end the url, continue
        if(!end) {
            bad_link = 1; pos += 2; continue;
        }

        // have a link now
        if(*href == '#') {                   // internal reference
            bad_link = 1; pos += 2; continue;
        }

        // is the url absolute, i.e, ':' must preceede any '/', '?', or '#'
        ptr = strpbrk(href, ":/?#");
        if(!ptr || *ptr != ':') { relative = 1; }
        else if(strncasecmp(href, "http", 4)) { // absolute, but not http(s)
                bad_link = 1; pos += 2; continue;
        }
    } while(bad_link);                       // keep parsing

    // have a good link now
    if(relative) {                           // need to fixup relative links
        // create new buffer
        *result = calloc(end-href+2+strlen(base_url), sizeof(char));

        if(!*result) { return -2; }          // check memory

        // add the base url
        strcat(*result, base_url);

        // we may need to add '/'
        if(base_url[strlen(base_url)-1] != '/' && *href != '/') {
            (*result)[strlen(base_url)] = '/';
        }

        // add the relative url
        strncat(*result, href, end - href);
    } else {
        // create new buffer
        *result = calloc(end-href+1, sizeof(char));

        if(!*result) { return -2; }          // check memory

        // copy over absolute url
        strncpy(*result, href, end - href);
    }

    // return position at the end of the url
    return end - html;
}

/*
 * NormalizeURL - normalize the url according to RFC 3986 chapter 3
 *
 * Assumptions:
 *     1. url has been allocated
 *     2. url is an absolute url
 *
 * Pseudocode:
 *     1. check arguments
 *     2. try to parse url
 *     3. check any file extensions
 *     4. clear the url
 *     5. remove dot segments and lowercase scheme and host
 *     6. put the url back together
 */
int NormalizeURL(char *url) {
    int status = 1;
    int valid_ext;
    struct URL tmp;                          // url struct for parsing
    char *dot;
    char *slash;

    // initialize struct
    tmp.scheme = NULL;
    tmp.user = NULL;
    tmp.host = NULL;
    tmp.path = NULL;
    tmp.query = NULL;
    tmp.fragment = NULL;

    // test url
    if(!url) { return 0; }

    // try to parse the url
    if(!ParseURL(url, &tmp)) {
        status = 0;
        goto cleanup;                        // sorry Dijkstra
    }

    // check file extension
    if(tmp.path) {                           // have a path
        dot = strrchr(tmp.path, '.');
        slash = strrchr(tmp.path, '/');

        if(dot && slash && dot > slash) {    // /path/to/file.ext
            dot++;                           // consume '.'

            if(strlen(dot) > 0) {
                // check all valid extensions
                valid_ext = 0;
                for(int i = 0; i < NUM_EXTS; i++) {
                    if(!strncasecmp(dot, EXTS[i], strlen(EXTS[i]))) {
                        valid_ext = 1; break;
                    }
                }

                // bad extension
                if(!valid_ext) {
                    status = 0;
                    goto cleanup;            // sorry Dijkstra
                }
            }
        }

    }

    // clear url
    memset(url, '\0', strlen(url)-1);

    // put normalized url back together
    if(tmp.scheme) {                         // scheme
        strcat(url, tmp.scheme);
    }

    if(tmp.user) {                           // user
        strcat(url, tmp.user);
    }

    if(tmp.host) {                           // host
        strcat(url, tmp.host);
    }

    if(tmp.path) {                           // path
        // remove . and .. segments
        char *p = RemoveDotSegments(tmp.path);

        if(p) {
            strcat(url, p);

            free(p);
        }
    }

    if(tmp.query) {                          // query
        strcat(url, tmp.query);
    }

    if(tmp.fragment) {                       // fragment
        strcat(url, tmp.fragment);
    }

cleanup:                                     // cleanup memory
    if(tmp.scheme)
        free(tmp.scheme);

    if(tmp.user)
        free(tmp.user);

    if(tmp.host)
        free(tmp.host);

    if(tmp.path)
        free(tmp.path);

    if(tmp.query)
        free(tmp.query);

    if(tmp.fragment)
        free(tmp.fragment);

    return status;
}

// ---------------- Public functions
/*
 * ParseURL - attempts to parse str into a URL struct
 * @str: absolute url to parse
 * @url: struct containing parts of a url
 *
 * Expects str to be an absolute url. Returns 0 if str cannot be
 * successfully parsed; otherwise, returns 1.
 *
 * From RFC 3986 chapter 3:
 *
 * The following are two example URIs and their component parts:
 *
 *       foo://example.com:8042/over/there?name=ferret#nose
 *       \_/   \______________/\_________/ \_________/ \__/
 *        |           |            |            |        |
 *     scheme     authority       path        query   fragment
 *        |   _____________________|__
 *       / \ /                        \
 *       urn:example:animal:ferret:nose
 *
 * Should have no use outside of this file, thus declared static.
 */
static int ParseURL(char* str, struct URL* url)
{
    int host_len;                            // length of host segment
    char *scheme_end;                        // scheme end point, : or :/ or ://
    char *user_end;                          // end of user info, @
    char *host_beg;                          // beginning of host, : or @
    char *host_end;                          // end of host, scheme:host/
    char *host_e;                            // end of host, / or end of url
    char *path_end;                          // end of path, ? or # or end of url
    char *query_beg;                         // end of query, # or end of url
    char *frag_beg;                          // end of fragment, end of url

    // make sure we have a str and url struct
    if(!str || !url)
        return 0;

    // make sure absolute url, i.e., ':' must preceede any '/', '?', or '#'
    scheme_end = strpbrk(str, ":/?#");
    if(!scheme_end || *scheme_end != ':')
        return 0;

    scheme_end++;                            // consume ':'

    // do we have scheme:<path> or scheme:<host><path>
    if(!strncmp(scheme_end, "//", 2)) {     // have host
        scheme_end += 2;                    // consume "//"
    }

    // allocate url scheme
    url->scheme = calloc(scheme_end - str + 1, sizeof(char));
    if(!url->scheme) {
        return 0;
    }

    // lowercase it
    for(char *ptr = str; ptr < scheme_end; ptr++) {
        url->scheme[ptr - str] = tolower(*ptr);
    }


    // get user information, anything between scheme and first '@'
    user_end = strchr(scheme_end, '@');

    if(user_end) {                           // have user info
        user_end++;                          // consume '@'

        // allocate user
        url->user = calloc(user_end - scheme_end + 1, sizeof(char));
        if(!url->user) {
            return 0;
        }

        // copy user info
        strncpy(url->user, scheme_end, user_end - scheme_end);
    }

    // get host information
    host_end = strchr(scheme_end, '/');

    if(!host_end && !user_end) {             // scheme@host
        host_len = strlen(str) - (scheme_end - str);
        host_beg = scheme_end;
        host_e = &str[strlen(str)];
    } else if(!host_end && user_end) {       // scheme:user@host
        host_len = strlen(str) - (user_end - str);
        host_beg = user_end;
        host_e = &str[strlen(str)];
    } else if(host_end && !user_end) {       // scheme:host/path...
        host_len = host_end - scheme_end + 1;
        host_beg = scheme_end;
        host_e = host_end;
    } else {                                 // scheme:user@host/path...
        host_len = host_end - user_end + 1;
        host_beg = user_end;
        host_e = host_end;
    }

    // allocate host
    url->host = calloc(host_len + 1 , sizeof(char));
    if(!url->host) {
        return 0;
    }

    // lowercase it
    for(char *ptr = host_beg; ptr < host_e; ptr++) {
        url->host[ptr - host_beg] = tolower(*ptr);
    }

    // get path part, between host and query and/or fragment
    path_end = strpbrk(scheme_end, "?#");

    if(path_end) {                           // .../path? or .../path#
        url->path = calloc(path_end - host_e + 1, sizeof(char));
        if(!url->path) {
            return 0;
        }

        strncpy(url->path, host_e, path_end - host_e);
    } else {                                 // .../path
        url->path = calloc(&str[strlen(str)] - host_e + 1, sizeof(char));
        if(!url->path) {
            return 0;
        }

        strcpy(url->path, host_e);
    }

    // get fragment, anything after first '#'
    frag_beg = strchr(scheme_end, '#');

    if(frag_beg) {                           // have fragment
        url->fragment = calloc(&str[strlen(str)] - frag_beg + 1, sizeof(char));
        if(!url->fragment) {
            return 0;
        }

        strcpy(url->fragment, frag_beg);
    }

    // get query, anything after first '?' before any '#'
    query_beg = strchr(scheme_end, '?');

    if(query_beg && !frag_beg) {             // ...?name=value
        url->query = calloc(&str[strlen(str)] - query_beg + 1, sizeof(char));
        if(!url->query) {
            return 0;
        }

        strcpy(url->query, query_beg);
    } else if(query_beg && frag_beg) {       // ...?name=value#top
        url->query = calloc(frag_beg - query_beg + 1, sizeof(char));
        if(!url->query) {
            return 0;
        }

        strncpy(url->query, query_beg, frag_beg - query_beg);
    }

    return 1;                                // if we got this far, good
}

/*
 * RemoveDotSegments - removes . and .. segments from url paths
 * @input: the character buffer to cleanse
 *
 * Returns a newly allocated character buffer with . and .. segments removed
 * according to the algorithm in RFC 3986 section 5.2.4 "Remove Dot Segments".
 * See: http://www.ietf.org/rfc/rfc1738.txt
 *
 * Should have no use outside of this file, thus declared static.
 *
 * Implementation adapted for CS50 TSE Crawler from the cURL library:
 *
 * Copyright (c) 1996 - 2014, Daniel Stenberg, <daniel@haxx.se>.
 *
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall not
 * be used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization of the copyright holder.
 */
static char *RemoveDotSegments(char *input)
{
    size_t in_len;                           // input length
    size_t copy_len;                         // copy length
    char *copy;                              // copy of input that is modified
    char *copyptr;                           // handle for free'ing copy'
    char *out;                               // output buffer
    char *outptr;                            // pointer to current write point

    if(!input || strlen(input) < 1)
        return NULL;

    // save lengths
    in_len = copy_len = strlen(input);

    // create output buffer
    out = outptr = calloc(in_len + 1, sizeof(char));
    if(!out)
        return NULL; /* out of memory */

    // copy the input buffer
    copy = copyptr = strdup(input);
    if(!copy) {
        free(out);
        return NULL;
    }

    // 2.  While the input buffer is not empty, loop as follows:
    do {
        // A. If the input buffer begins with a prefix of "../" or "./",
        //    then remove that prefix from the input buffer; otherwise,
        if(!strncmp("./", copy, 2)) {
            copy += 2;
            copy_len -= 2;
        }
        else if(!strncmp("../", copy, 3)) {
            copy += 3;
            copy_len -= 3;
        }

        // B. if the input buffer begins with a prefix of "/./" or "/.",
        //    where "." is a complete path segment, then replace that
        //    prefix with "/" in the input buffer; otherwise,
        else if(!strncmp("/./", copy, 3)) {
            copy += 2;
            copy_len -= 2;
        }
        else if(!strcmp("/.", copy)) {
            copy[1] = '/';
            copy++;
            copy_len -= 1;
        }

        // C. if the input buffer begins with a prefix of "/../" or "/..",
        //    where ".." is a complete path segment, then replace that
        //    prefix with "/" in the input buffer and remove the last
        //    segment and its preceding "/" (if any) from the output
        //    buffer; otherwise,
        else if(!strncmp("/../", copy, 4)) {
            copy += 3;
            copy_len -= 3;

            // remove the last segment
            while(outptr > out) {
                outptr--;
                if(*outptr == '/')
                    break;
            }
            *outptr = 0;
        }
        else if(!strcmp("/..", copy)) {
            copy[2] = '/';
            copy += 2;
            copy_len -= 2;

            // remove the last segment
            while(outptr > out) {
                outptr--;
                if(*outptr == '/')
                    break;
            }
            *outptr = 0;
        }

        // D. if the input buffer consists only of "." or "..", then remove
        //    that from the input buffer; otherwise, */
        else if(!strcmp(".", copy) || !strcmp("..", copy)) {
            *copy = 0;
        }

        // E. move the first path segment in the input buffer to the end of
        //    the output buffer, including the initial "/" character (if
        //    any) and any subsequent characters up to, but not including,
        //    the next "/" character or the end of the input buffer. */
        else {
            do {
                *outptr++ = *copy++;
                copy_len--;
            } while(*copy && (*copy != '/'));
            *outptr = '\0';
        }
    } while(*copy != '\0');                          // keep going

    // cleanup copy
    free(copyptr);

    return out;
}

/*
 * RemoveWhitespace - removes whitespace from str
 * @str: char buffer to modify
 *
 * Eliminates whitespace by shifting and condensing all the non-whitespace
 * toward the beginning of the buffer. This does not alter the size of the
 * buffer.
 *
 * Should have no use outside of this file, thus declared static.
 */
static void RemoveWhitespace(char* str)
{
    char *prev;                              // previous whitespace
    char *cur;                               // current non-whitespace

    // start at beginning of str
    cur = prev = str;

    do {
        while(isspace(*cur)) cur++;          // consume any whitespace
    } while ((*prev++ = *cur++));            // condense to front of str
}

/*
 * WriteMemoryCallback - curl callback for writing retrieved data
 *
 * For implementation details see:
 * http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTWRITEFUNCTION
 *
 * Should have no use outside of this file, thus declared static.
 */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    WebPage *page = (WebPage*)userp;

    page->html = realloc(page->html, page->html_len + realsize + 1);
    if(page->html == NULL) {
        return 0;
    }

    memcpy(&(page->html[page->html_len]), contents, realsize);
    page->html_len += realsize;
    page->html[page->html_len] = 0;

    return realsize;
}
