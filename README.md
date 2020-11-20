# the-super-tiny-compiler
C++ implemention inspired by [the-super-tiny-compiler](https://github.com/jamiebuilds/the-super-tiny-compiler/blob/master/the-super-tiny-compiler.js)

### Target
We're going to compile some `lisp-like` function calls into some `C-like` function calls.

```
                   LISP                      C
 
    2 + 2          (add 2 2)                 add(2, 2)
    4 - 2          (subtract 4 2)            subtract(4, 2)
    2 + (4 - 2)    (add 2 (subtract 4 2))    add(2, subtract(4, 2))
```

### Output
I add some output to show how this `super-tiny-compiler` works

``` text
Compile (add 4 (subtract 23 8))
========= Tokens: ==========
paren (
name add
number 4
paren (
name subtract
number 23
number 8
paren )
paren )
========== End Of Tokens ==========

========== Walk ============
CallExpression start
NumberLiteral
CallExpression start
NumberLiteral
NumberLiteral
CallExpression end
CallExpression end
========== End of Walk ============

========== Traversal ============
Program
CallExpression
NumberLiteral
CallExpression
NumberLiteral
NumberLiteral
========== End of Traversal ============

========= Code generated! ====
add(4,subtract(23,8));

========= End of Code Generate ====
```