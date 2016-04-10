#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>

#include "lib/jieba.h"

char* getstr () {
  wchar_t *str;
  char *ret;
  wint_t i = 1;
  size_t j;
  /* assuming a sentence contains at most 200 wide characters */
  size_t WCHAR_MAX_SIZE = sizeof (wchar_t) * 200;

  str = (wchar_t *) malloc (WCHAR_MAX_SIZE);
  while ('\n' != (str[i - 1] = getwchar ())) {
    if (str[i - 1] == WEOF) break;
    i ++;
  }
  str[i - 1] = L'\0';
  j = sizeof (wchar_t) * (wcslen (str) + 1);
  ret = (char *) malloc (j);
  wcstombs (ret, str, j);
  ret[j - 1] = '\0';
  free (str);
  return ret;
}

void cut_input (char *DICT_PATH, char *HMM_PATH, char *USER_DICT) {
  char *s;
  size_t wchar_width = sizeof (wchar_t) - 1;

  Jieba handle = NewJieba (DICT_PATH, HMM_PATH, USER_DICT);
  setlocale (LC_ALL, "");
  printf ("Chinese word segmentation\n=========================\n\n");

  while (s = getstr ()) {
    if (strcmp (s, "EOF") == 0) {
      free (s);
      printf ("bye\n");
      break;
    } else {
      size_t len = strlen (s);
      CJiebaWord* words = Cut (handle, s, len);
      CJiebaWord* x;
      for (x = words; x && x->word; x++) {
        /* printf ("%*.*s/", x->len, x->len, x->word); */
        printf ("%d ", x->len / wchar_width);
      }
      printf ("\n");
      free (s);
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
