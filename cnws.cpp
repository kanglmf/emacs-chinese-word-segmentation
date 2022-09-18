// g++ -Ideps/limonp/include -Iinclude -o FILE $0

#include <iostream>
// #include "cppjieba/Jieba.hpp"

#include <csignal>
#include <cwchar>

// requires C++ 2017
#include <filesystem>
namespace fs = std::filesystem;

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

  fs::path dir (argv[1]);

  fs::path file_jieba ("jieba.dict.utf8");
  fs::path file_hmm_model ("hmm_model.utf8");
  fs::path file_user ("user.dict.utf8");
  fs::path file_idf ("idf.utf8");
  fs::path file_stop_words ("stop_words.utf8");

  fs::path path_jieba = dir / file_jieba;
  fs::path path_hmm_model = dir / file_hmm_model;
  fs::path path_user = dir / file_user;
  fs::path path_idf = dir / file_idf;
  fs::path path_stop_words = dir / file_stop_words;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
  const char* const DICT_PATH = convert_char (path_jieba.c_str());
  const char* const HMM_PATH = convert_char (path_hmm_model.c_str());
  const char* const USER_DICT_PATH = convert_char (path_user.c_str());
  const char* const IDF_PATH = convert_char (path_idf.c_str());
  const char* const STOP_WORD_PATH = convert_char (path_stop_words.c_str());
#else
  const char* const DICT_PATH = path_jieba.c_str();
  const char* const HMM_PATH = path_hmm_model.c_str();
  const char* const USER_DICT_PATH = path_user.c_str();
  const char* const IDF_PATH = path_idf.c_str();
  const char* const STOP_WORD_PATH = path_stop_words.c_str();
#endif

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
