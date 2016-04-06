all: chinese-word-segmentation
chinese-word-segmentation: libjieba.a
	gcc -o chinese-word-segmentation chinese-word-segmentation.c -L./ -ljieba -lstdc++ -lm
libjieba.a:
	g++ -o jieba.o -c -DLOGGING_LEVEL=LL_WARNING -I./deps/ lib/jieba.cpp
	ar rs libjieba.a jieba.o
clean:
	rm -f *.a *.o chinese-word-segmentation
