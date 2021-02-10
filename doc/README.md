# FeenoX docs

This directory contains the FeenoX documentation.
Everything is created out of the source files which are a mixture of Bash, M4 and Markdown.

Execute (and see) `make.sh`

# Syntax Highlight

Execute `syntax-kate.sh` to get a katepart XML file with the syntax highlight rules that can be used for Kate and for Pandoc. Also, `syntax-tex.sh` can create the language definition for LaTeX's listings package (which can in turn be used by Pandoc).


<https://docs.kde.org/stable5/en/applications/katepart/highlight.html>

# Manual page

From `man-pages(7)`:

 > For  commands,  this  shows the syntax of the command and its arguments (including options); boldface is used
 > for as-is text and italics are used to indicate replaceable arguments.  Brackets ([]) surround optional argu‐
 > ments,  vertical  bars (|) separate choices, and ellipses (...) can be repeated.  For functions, it shows any
 > required data declarations or #include directives, followed by the function declaration.
 >
 > Filenames (whether pathnames, or references to header files) are always in italics.
 >
 > Complete commands should, if long, be written as an indented line on their own, with a blank line before and after the command, for example
 >
 >     man 7 man-pages
 >
 > If  the  command  is short, then it can be included inline in the text, in italic format, for example, man 7 man-pages.  In
 > this case, it may be worth using nonbreaking spaces ("\ ") at suitable places in the command.  Command  options  should  be
 > written in italics (e.g., -l).
 
 > Expressions,  if  not  written  on a separate indented line, should be specified in italics.  Again, the use of nonbreaking
 >  spaces may be appropriate if the expression is inlined with normal text.
 >
 >  When showing example shell sessions, user input should be formatted in bold, for example
 >
 >      $ date
 >      Thu Jul  7 13:01:27 CEST 2016
 >
 >  Any reference to another man page should be written with the name in bold, always followed by the section number, formatted
 >  in Roman (normal) font, without any separating spaces (e.g., intro(2)).
 >
 >  Starting with release 2.59, man-pages follows American spelling conventions (previously, there was a random mix of  British
 >  and American spellings); please write all new pages and patches according to these conventions.
 >
 >   Aside from the well-known spelling differences, there are a few other subtleties to watch for:
 >
 >         *  American  English  tends to use the forms "backward", "upward", "toward", and so on rather than the British forms "back‐
 >            wards", "upwards", "towards", and so on.
