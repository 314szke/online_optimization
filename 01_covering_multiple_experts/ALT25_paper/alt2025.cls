\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{alt2025}[2025/01/01 v1.0 ALT 2025 Proceedings]
\newcommand{\todo}{\color{red}}

\@ifundefined{ifanonsubmission}{
  \newif\ifanonsubmission
  \anonsubmissionfalse
}{}

\@ifundefined{iffinalsubmission}{
  \newif\iffinalsubmission
  \finalsubmissionfalse
}{}

\DeclareOption{anon}{\anonsubmissiontrue}
\DeclareOption{final}{\finalsubmissiontrue}
\ProcessOptions\relax

\LoadClass[pmlr]{jmlr}

\jmlrvolume{vol {XXX}}
\jmlryear{2025}
\jmlrproceedings{PMLR}{Proceedings of Machine Learning Research}

\ifanonsubmission
 \newcommand{\altauthor}[1]{}
 \author{author names withheld}
 \editor{Under Review for ALT 2025}
 \jmlrworkshop{36th International Conference on Algorithmic Learning Theory}
 \renewcommand{\acks}[1]{}
\else
 \newcommand{\altauthor}[1]{\author{#1}}
 \iffinalsubmission
  \editors{Gautam Kamath and Po-Ling Loh}
  \jmlrworkshop{36th International Conference on Algorithmic Learning Theory}
 \fi
\fi



