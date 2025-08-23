here’s a quick list of parsing edge-cases and behaviors we still need to cover. Please review and incorporate them into our tokenizer/AST builder:

export a=”  1.  2. 3 “

$a ⇒ should split  1 2 3

“$a” ⇒ should not split

$HOME”hello” ⇒ check bash

ls $kclvklv ⇒ don’t send empty expanded tookens to execution

check and check

<< "HOME” ⇒ this doesn’t work but heredoc works perfectly when cat is used with it cat << “HOME”

cat << hello"” ⇒ test it in bash

delimeter of heredoc should not expand

echo -  hello ⇒ should print: - hello

Syntax Errors

ls | ⇒ should print and error 

cmd > ⇒ should print ; bash: syntax error near unexpected token `newline'

 if you have a redirection not followed by a word it’s an error .
