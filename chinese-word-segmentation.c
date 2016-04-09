#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/jieba.h"

char* getstr () {
  /* https://segmentfault.com/a/1190000000360944 */
  char* str;
  char* _str;
  int i = 1;
  str = (char*) malloc (sizeof (char) * (i + 1));
  while ('\n' != (str[i - 1] = getchar ())) {
    i ++;
    _str = (char*) malloc (strlen (str) + 1);
    str[i - 1] = '\0';
    strcpy (_str,  str);
    free (str);
    str = (char*) malloc (sizeof (char) * (i + 1));
    if (NULL == str) {
      free (_str);
      printf ("No enough memory!");
      return NULL;
    }
    strcpy (str,  _str);
    free (_str);
  }
  str[i - 1] = '\0';
  return str;
}

void cut_input (char *DICT_PATH, char *HMM_PATH, char *USER_DICT) {
  char *s;

  Jieba handle = NewJieba (DICT_PATH, HMM_PATH, USER_DICT);
  printf ("Chinese word segmentation\n=========================\n\n");

  while (s = getstr ()) {
    if (strcmp (s, "EOF") == 0) {
      printf ("bye\n");
      break;
    } else {
      size_t len = strlen (s);
      CJiebaWord* words = Cut (handle, s, len);
      CJiebaWord* x;
      for (x = words; x && x->word; x++) {
        /* printf ("%*.*s/", x->len, x->len, x->word); */
        printf ("%d ", x->len / 3);
      }
      printf ("\n");
      FreeWords (words);
    }
  }
  FreeJieba (handle);
}

int main (int argc, char** argv) {
  char *DICT_PATH;
  char *HMM_PATH;
  char *USER_DICT;
  int opt = 0;

  while ((opt = getopt (argc, argv, "j:h:u:")) != -1) {
    switch (opt) {
    case 'j':
      DICT_PATH = (char *) malloc (sizeof (char) * (strlen (optarg) + 1));
      strcpy (DICT_PATH, optarg);
      break;
    case 'h':
      HMM_PATH = (char *) malloc (sizeof (char) * (strlen (optarg) + 1));
      strcpy (HMM_PATH, optarg);
      break;
    case 'u':
      USER_DICT = (char *) malloc (sizeof (char) * (strlen (optarg) + 1));
      strcpy (USER_DICT, optarg);
      break;
    case '?':
      printf ("Unknow option：%c/n", optopt);
      break;
    }
  }

  cut_input (DICT_PATH, HMM_PATH, USER_DICT);
  free (DICT_PATH);
  free (HMM_PATH);
  free (USER_DICT);

  return EXIT_SUCCESS;
}
