
# emacs-chinese-word-segmentation

基于[结巴分词](https://github.com/yanyiwu/cppjieba)或
[HanLP](https://github.com/hankcs/HanLP) 的 Emacs 中文分词工具，实现了
以中文词语为单位的移动和编辑。支持 Linux、Cygwin、Windows 和
Android/Termux 平台。目前支持两种实现方式：（1）进程调用：在每个 Emacs
进程中分别启动分词软件；（2）网络连接：通过 Emacs 网络进程连接到分词服
务器。两种实现方式的简单比较如下：

| 实现方式 | 可选分词软件 | 优点                                           | 不足                                              |
|----------|--------------|------------------------------------------------|---------------------------------------------------|
| 进程调用 | 结巴         | 外部依赖少                                     | 每个 Emacs 进程都需要单独启动分词软件，内存占用高 |
| 网络连接 | 结巴、HanLP  | 1. 启动等待时间短<br>2. 可以多人共享分词服务器 | 过多 Emacs 客户端连接可能导致分词速度下降         |


在 Windows 平台中，**进程调用**方式是通过调用 WSL 或 Cygwin 进程实现的
（需要在 WSL 或 Cygwin 中编译）。

根据有限测试，使用 HanLP 可能出现首次分词卡顿，在分词请求频率过高时也
可能出现分词卡顿（如一直按住 <kbd>M-f</kbd>，执行 `cns-forward-word`）。
由于 HanLP 使用 pyTorch，因此建议优先使用 GPU 提高分词性能。

## 原理

将光标所在的中文句子发送到外部程序**实时**进行中文分词，分词结果返回给
Emacs，只用到分词后的每个词组的长度（如 `["中文", "分词"]: 2 2` 只保留
`2 2`）。实际上任何输出该格式（仅需要以空格分隔的词组长度列表）分词结
果的中文分词软件都可以配合本软件包使用，只需设置好
`cns-process-shell-command`。

## 安装和运行

### 1. 进程调用方式（结巴分词）

需要先安装 C++ 编译器（g++ 或 clang++）和 make：

```sh
git clone $this_repo
cd $this_repo
git submodule update --init --recursive
# using g++:
make
# or using clang++:
env CXX=clang++ make
```

编译成功后将生成 `cnws` 可执行文件（Cygwin 平台为 `cnws.exe`）。

### 2. 网络连接方式（结巴分词）

需要先安装 C/C++ 编译器（ gcc/g++ 或 clang/clang++）、make 和 binutils：

```sh
git clone $this_repo
cd $this_repo
git submodule update --init --recursive
# using gcc/g++:
make -f Makefile.server.jieba
# or using clang/clang++:
env CC=clang CXX=clang++ make -f Makefile.server.jieba
```

编译成功后将生成 `cnws-server-jieba` 可执行文件。运行分词服务：

```sh
cnws-server-jieba \
    cjieba/dict/jieba.dict.utf8 \
    cjieba/dict/hmm_model.utf8 \
    cjieba/dict/user.dict.utf8 \
    cjieba/dict/idf.utf8 \
    cjieba/dict/stop_words.utf8 \
    9999
```

其中 `9999` 为端口，可随意更改，但需要注意在 Emacs 中设置相同端口。

### 3. 网络连接方式（HanLP）

首先安装[HanLP](https://github.com/hankcs/HanLP)，再运行分词服务：

```sh
pip install hanlp
git clone $this_repo
cd $this_repo
python $this_repo/cnws_server_hanlp.py --host localhost --port 9999
```

注意：

1. 如果希望其他设备能访问分词服务器，需要修改 `--host` 参数为
   `0.0.0.0`，并开启相应的端口。
2. 首次运行 HanLP 将下载官网提供的预训练模型，若后续不希望再次联网检查
   模型更新，可以在运行 `cnws_server_hanlp.py` 前设置环境变量：

```sh
export TRANSFORMERS_OFFLINE=1
```

## 使用示例

### 1. 进程调用方式（结巴分词）

```elisp
(add-to-list 'load-path "$this_repo")
(setq cns-prog "$this_repo/cnws")
(setq cns-dict-directory "$this_repo/cppjieba/dict")

;; 若使用其它中文分词软件, 需要设置 cns-process-shell-command:
;; (setq cns-process-shell-command "word_segmentation_program arg1 arg2...")

;; Windows 平台（指官方 windows 版 Emacs）请先检查或设置
;; cns-cmdproxy-shell-path，默认优先选择 WSL 环境，因为编译后分词软件
;; 的运行速度在 WSL 中比 Cygwin 速度快，若选择使用 Cygwin 平台作为编译
;; 环境，需要手工设置好 cns-cmdproxy-shell-path。
;;
;; (setq cns-cmdproxy-shell-path "wsl.exe bash")
;; 或
;; (setq cns-cmdproxy-shell-path "C:/cygwin64/bin/bash.exe")

(setq cns-recent-segmentation-limit 20) ; default is 10
(setq cns-debug nil) ; disable debug output, default is t
(require 'cns nil t)
(when (featurep 'cns)
  (add-hook 'find-file-hook 'cns-auto-enable))
```

### 2. 网络连接方式（结巴分词、HanLP）

```elisp
(setq cns-client-host "127.0.0.1")
(setq cns-client-port 9999)

(setq cns-recent-segmentation-limit 20) ; default is 10
(setq cns-debug nil) ; disable debug output, default is t
(require 'cns nil t)
(when (featurep 'cns)
  (add-hook 'find-file-hook 'cns-auto-enable))
```


手动开启 minor mode：<kbd>M-x</kbd> `cns-mode` <kbd>RET</kbd>，或者开
启 `global-cns-mode`.

## 按键绑定

```
+---------------+----------------------+--------------------------+
| key binding   | default command      | cns-mode command         |
+---------------+----------------------+--------------------------+
| M-b           | (backward-word)      | (cns-backward-word)      |
| M-f           | (forward-word)       | (cns-forward-word)       |
| C-<backspace> | (backward-kill-word) | (cns-backward-kill-word) |
| M-DEL         | (backward-kill-word) | (cns-backward-kill-word) |
| C-<delete>    | (kill-word)          | (cns-kill-word)          |
| M-d           | (kill-word)          | (cns-kill-word)          |
| M-t           | (transpose-words)    | (cns-transpose-words)    |
+---------------+----------------------+--------------------------+
```

详见 `cns.el` 中的注释。

## 致谢

感谢优秀的开源中文分词项目：[结巴分词](https://github.com/yanyiwu/cppjieba)和[HanLP](https://github.com/hankcs/HanLP)。
