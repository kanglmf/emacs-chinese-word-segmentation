#include <stdio.h>
#include <stdlib.h>

#include "lib/jieba.h"
#include <string.h>
#include <sys/types.h>

#define CJIEBA_WCHAR_SIZE 3
#define MAX_SENTENCE_LEN  100

// const char* DICT_PATH = "./dict/jieba.dict.utf8";
// const char* HMM_PATH = "./dict/hmm_model.utf8";
// const char* USER_DICT = "./dict/user.dict.utf8";
// const char* IDF_PATH = "./dict/idf.utf8";
// const char* STOP_WORDS_PATH = "./dict/stop_words.utf8";

Jieba jieba_load_handle (int argc, char **argv) {
    if (argc < 6) {
        fprintf (stderr, "Usage:\n\
  %s <DICT> <HMM> <USER_DICT> <IDF> <STOP_WORD>\n",
                 argv[0]);
        exit (EXIT_FAILURE);
    }

    const char *DICT_PATH      = argv[1];
    const char *HMM_PATH       = argv[2];
    const char *USER_DICT_PATH = argv[3];
    const char *IDF_PATH       = argv[4];
    const char *STOP_WORD_PATH = argv[5];

    Jieba handle = NewJieba (DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH,
                             STOP_WORD_PATH);
    return handle;
}

char *jieba_cut (Jieba handle, char *s) {
    size_t      len = strlen (s);
    CJiebaWord *x;
    CJiebaWord *words       = Cut (handle, s, len);
    size_t      size        = MAX_SENTENCE_LEN * CJIEBA_WCHAR_SIZE + 1;
    char       *seg_len_str = malloc (size);
    char        str[4];

    strcpy (seg_len_str, s);
    strcat (seg_len_str, ": ");
    for (x = words; x && x->word; x++) {
        sprintf (str, "%zd ", x->len / CJIEBA_WCHAR_SIZE);
        strcat (seg_len_str, str);
    }
    strcat (seg_len_str, "\n");

    FreeWords (words);
    return seg_len_str;
}
