
# emacs-chinese-word-segmentation

Emacs 中的基于 cjieba (<https://github.com/yanyiwu/cjieba>) 的中文分词工具。

Simple Chinese word segmentation library for Emacs based on cjieba
(<https://github.com/yanyiwu/cjieba>).  This library uses cjieba
(commit 91c233d) with simple modification.

## Compile the C program

To compile the Chinese word segmentation program, do:

```
cd /path/to/this-library
make
```

This will generate the chinese-word-segmentation executable.

## Usage example

```
(add-to-list 'load-path "/path/to/this-library")
(setq cns-prog "/path/to/this-library/chinese-word-segmentation")
(setq cns-dict-directory "/path/to/this-library/dict")
(setq cns-recent-segmentation-limit 20) ; default is 10
(setq cns-debug nil) ; disable debug output, default is t
(require 'cns nil t)
(when (featurep 'cns)
  (add-hook 'find-file-hook 'cns-auto-enable))
```

To turn on this minor mode, type: M-x cns-mode RET.  You can also turn
on global-cns-mode if you like.

## Key bindings

This minor mode changes the following key bindings:

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

Please see the comments in cns.el for more detail.
