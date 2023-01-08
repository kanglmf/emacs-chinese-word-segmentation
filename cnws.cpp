// tested compilers:
// general  g++:        g++ -Icppjieba/deps/limonp/include -Icppjieba/include -O3 -o cnws cnws.cpp
// debian   g++-12:     [-std=c++17]
// debian   clang++-14: -std=c++17
// cygwin   g++-11:     [-std=c++17]
// cygwin   clang++-8:  -std=c++17
// termux   clang++-15: [-std=c++11 | -std=c++14]

#include <iostream>
#include <csignal>
#include <cwchar>

#include "cppjieba/DictTrie.hpp"
#include "cppjieba/FullSegment.hpp"
#include "cppjieba/HMMModel.hpp"
#include "cppjieba/HMMSegment.hpp"
#include "cppjieba/Jieba.hpp"
#include "cppjieba/KeywordExtractor.hpp"
#include "cppjieba/MixSegment.hpp"
#include "cppjieba/MPSegment.hpp"
#include "cppjieba/PosTagger.hpp"
#include "cppjieba/PreFilter.hpp"
#include "cppjieba/QuerySegment.hpp"
#include "cppjieba/SegmentBase.hpp"
#include "cppjieba/SegmentTagged.hpp"
#include "cppjieba/TextRankExtractor.hpp"
#include "cppjieba/Trie.hpp"
#include "cppjieba/Unicode.hpp"

using namespace std;

char* convert_char (const wchar_t* str) {
  char *ret;
  size_t len;
  len = MB_CUR_MAX * (wcslen (str) + 1);
  ret = (char *) malloc (len);
  wcstombs (ret, str, len);
  ret[len - 1] = '\0';
  return ret;
}

int main(int argc, char** argv) {
  signal(SIGINT, exit);

  if (argc < 6) {
    fprintf(stderr, "Usage:\n\
  %s <DICT_PATH> <HMM_PATH> <USER_DICT_PATH> <IDF_PATH> <STOP_WORD_PATH>\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  const char* const DICT_PATH       = argv[1];
  const char* const HMM_PATH        = argv[2];
  const char* const USER_DICT_PATH  = argv[3];
  const char* const IDF_PATH        = argv[4];
  const char* const STOP_WORD_PATH  = argv[5];

  cppjieba::Jieba jieba(
		  DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH
		  );
  vector<string> words;
  vector<cppjieba::Word> jiebawords;
  string s;

  int WIDE_CHAR_SIZE = 3;
  cout << "Chinese word segmentation" << endl;
  cout << "=========================" << endl;

  while (getline (cin, s)) {
    if (s[s.size() - 1] == '\r') s.erase(s.size() - 1);
    if (s == "EOF") {
      break;
    } else {
      jieba.Cut(s, words, true);
      cout << words << ": ";
      for (unsigned int i = 0; i < words.size(); i++) {
        cout << words[i].length() / WIDE_CHAR_SIZE << " ";
      }
      cout << endl;
    }
  }
  return EXIT_SUCCESS;
}
