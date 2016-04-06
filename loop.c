#include <stdio.h>
#include <stdlib.h>

#include "lib/jieba.h"
#include <string.h>

void CutDemo() {
  printf("CutDemo:\n");
  static const char* DICT_PATH = "./dict/jieba.dict.utf8";
  static const char* HMM_PATH = "./dict/hmm_model.utf8";
  static const char* USER_DICT = "./dict/user.dict.utf8";
  // init will take a few seconds to load dicts.
  Jieba handle = NewJieba(DICT_PATH, HMM_PATH, USER_DICT);

  /* const char* s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。"; */
  /* const char* s = "南京市长江大桥"; */
  char *s;
  int n;
  while (scanf ("%s", &s) == 1) {
    if (s == "EOF") {
      break;
    } else {
      size_t len = strlen(s);
      CJiebaWord* words = Cut(handle, s, len);
      CJiebaWord* x;
      for (x = words; x && x->word; x++) {
        printf("%*.*s/", x->len, x->len, x->word);
      }
      FreeWords(words);
    }
  }
  FreeJieba(handle);
}

void ExtractDemo() {
  printf("ExtractDemo:\n");
  static const char* DICT_PATH = "./dict/jieba.dict.utf8";
  static const char* HMM_PATH = "./dict/hmm_model.utf8";
  static const char* IDF_PATH = "./dict/idf.utf8";
  static const char* STOP_WORDS_PATH = "./dict/stop_words.utf8";
  static const char* USER_DICT = "./dict/user.dict.utf8";

  // init will take a few seconds to load dicts.
  Extractor handle = NewExtractor(DICT_PATH,
        HMM_PATH,
        IDF_PATH,
        STOP_WORDS_PATH,
        USER_DICT);

  const char* s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。";
  size_t top_n = 5;
  CJiebaWord* words = Extract(handle, s, strlen(s), top_n);
  CJiebaWord* x;
  for (x = words; x && x->word; x++) {
    printf("%*.*s\n", x->len, x->len, x->word);
  }
  FreeWords(words);
  FreeExtractor(handle);
}

int main(int argc, char** argv) {
  CutDemo();
  /* ExtractDemo(); */
  return 0;
}
