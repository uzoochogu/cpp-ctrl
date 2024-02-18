# `Parsers and Parser Generators`
- [`Parsers and Parser Generators`](#parsers-and-parser-generators)
	- [`When do we just match and when do we parse?`](#when-do-we-just-match-and-when-do-we-parse)
	- [`Language and Grammar`](#language-and-grammar)
	- [`Grammars`](#grammars)
	- [`Understanding Grammars`](#understanding-grammars)
	- [`Grammar Notation`](#grammar-notation)
		- [`ENBF (Extended Backus-Naur form)`](#enbf-extended-backus-naur-form)
		- [`PEG`](#peg)
		- [`Syntax and Semantics`](#syntax-and-semantics)
	- [`Parsing`](#parsing)
	- [`Lexers and Parsers `](#lexers-and-parsers-)
		- [Lexers (or Tokenizers):](#lexers-or-tokenizers)
		- [Parser:](#parser)
	- [`Parsers Library and Parser Generators`](#parsers-library-and-parser-generators)
	- [`Project`](#project)
		- [MS Excel Value Function](#ms-excel-value-function)
	- [`CTPG Implementation`](#ctpg-implementation)
	- [`Boost Spirit (X3) Implementation`](#boost-spirit-x3-implementation)
	- [`ANTLR4 Implementation`](#antlr4-implementation)
	- [`Resources`](#resources)
	- [`Further reading`](#further-reading)


Parsing is needed in most of software development. We receive text input from users and many times we need to run some analysis on the text to extract useful information. With the advent of LLMs(large language models) that have the capability of NLP (Natural Language processing) this can be easily done, but this is computationally expensive. Besides this, programs may need a deterministic interpretation involving portable and fast computation to understand input. Due to this, we are usually caught writing one parser or the other. For very simple cases, `regex` engines can be used to match patterns. But `regex` loses its advantages when the language is more complex to parse usually when it involves recursive, branching grammar etc. 

## `When do we just match and when do we parse?`
Parsing creates a computationally meaningful data structure from text and tests if it conforms to a particular grammar and returns useful information afterwards. On the other hand, matching involves determining whether a given input conforms to a specific pattern or set of patterns. So parsers don't just match, they also extract useful information. We parse when we need this information rather than a boolean true or false for a match or an extractive matching process. 

## `Language and Grammar`
A language, at least in computing, can very simply be described as a set of strings. It can be a finite set or an infinite set. But most useful languages are infinite. Computer science likes to talk about languages as "formal langauages", the "formal" means ["`defineable using the tools of computer science and mathematics`"](http://redsymbol.net/articles/svg-markup-chomsky/). 
 
Noam Chomsky categorized formal languages depending on its expressive power. He split them into about [`four (4) categories`](https://en.wikipedia.org/wiki/Chomsky_hierarchy), in reducing order of complexity they are: Recursively enumerable (Turing complete), Context-sensitive, Context-free, regular languages. 

The definitions are quite cyclic : a language is regular if there exists some finite state machine that accepts all strings in the language and rejects all strings outside of it. And a language is context-free if there exists some context-free grammar that accepts all strings in that language, and rejects those outside of it and so on.  So we can see that languages are defined in terms of a grammar.

## `Grammars`
We represent languages using grammars. Grammars are the language of languages. They are a formal method to describe a textual language. Since you can't define a finite set of acceptable sentences for infinite languages, grammars are usually a finite recipe that descibes all acceptable sentences i.e. production rules.

Since languages are represented using grammars, the classification extends to grammars:
`Unrestricted grammars` describe recursively enumerable turing complete languages, `context sensitive grammars` describe context sensitive languages, `context-free grammars` (CFG) describe context-free languages and `regular grammars` describe regular languages.
*`Context`* here means the surrounding information or conditions that influence the way a language is structured or rules for generating valid sentences in that language. Knowing this we can now differentiate between context-free and context-sensitive grammars.

In `context-free grammars(CFGs)`, the production rules for generating language constructs are independent of the surrounding syntax. There is no consideration of the broader context in which symbols appear. For example, "`A`" can be replaced by "`B`" or "`C`" regardless of its context.

`Context-sensitive grammars` on the other hand recognize that the generation of valid language constructs may depend on the context in which symbols appear. They take into account the symbols surrounding the particular symbol. For example `A` can be replaced by `B` only if it is preceded by `X` followed by `Y` where `X` and `Y` are specific symbols.

`Regular grammars` are finite state automatons and defines languages using a regular expression. In simpler terms, generally, regular languages deal with straightforward, repetitive patterns, while context-free languages handle more complex, structured patterns or relationships.


## `Understanding Grammars`

When talking about grammars, we often define them using `terminals`. `Terminals` represent the building block of a language and are the actual symbols in the generated string. They are "atomic" (not further decomposed within the grammar).

`Non-terminals` acts as placeholders or variables in a grammar, representing abstract structures that can be further expanded. They are usually denoted by symbols enclosed in angle brackets ("<>" in `BNF`, and plain symbols in `ENBF`).
Root symbol is the starting point for generating the strings in the language. They typically represent the highest level construct and is an initial non-terminal symbol that the generation process can start from. The whole grammar is derived from a single non-terminal symbol.

Based on this, `CFGs` consist of a non-terminal symbol on the left-hand-side that can be replaced by a sequence of symbols(terminals and/or non-terminals) on the right-hand-side. Unlike context-free grammars, where replacement is solely based on the current nonterminal symbol, context-sensitive grammars take into account both the left and right sides of a production rule. Specifically, the rules in a context-sensitive grammar can have conditions involving both terminals and non-terminals on the left and right sides. These conditions, or contexts, determine how a nonterminal symbol can be replaced based on what comes before and after it.

Regular grammars have more specific rules. They also have non-terminal on the `Left Hand side (LHS)` while the RHS is restricted to either an empty string, a single terminal symbol or a single terminal followed by a non-terminal symbol. Recursive grammars contain production rules  where nonterminal symbol can be replaced by a sequence that includes the same non-terminal symbol. This recursion allows the generation of structures that build upon themselves, enabling the description of more complex language constructs.

## `Grammar Notation`
When talking about grammars we must also consider the notation grammars can be represented in. We would consider two very propular notations `ENBF` and `PEG`. 
### `ENBF (Extended Backus-Naur form)`
ENBF describes `CFG` in terms of production rules i.e. rules that when applied can generate all possible legal phrases in the languages. ENBF is actually an extension of the basic Backus-Naur metasyntax notation. Another variant of BNF is ABNF - Augmented Backus-Naur Form.

It consists of terminal symbols and non-terminal production rules which are the restrictions governing how terminal symbols can be combined into a legal sequence. 
The [`BNF and ENBF playground`]( https://bnfplayground.pauliankline.com/) is a fun tool to play with different `ENBF` grammars.

### `PEG`
Parsing Expression Grammar  is a type of analytic formal grammar, i.e. it describes a formal language in terms of a set of rules for recognizing strings in the language. Unlike `CFGs`, `PEGs` cannot be ambiguous; a string has exactly one valid parse tree or none. It is conjectured that there exist context-free languages that cannot be recognized by a `PEG`, [but this is not yet proven.](https://en.wikipedia.org/wiki/Parsing_expression_grammar#cite_note-For04-1)

`PEG` uses recognition rules which are rules that can be used to match valid phrases in the language. As an example, the Compile Time Parser Generator by Piotr Winter generates an `LR(1)` parser from `PEG`. The one stands for the number of look ahead. Unlike other grammars that need a preprocessing step where it is compiled to a parsing code form, `PEG` can be executed directly.

### `Syntax and Semantics`
After defining the syntax, it is your responsibility to define the semantics.

Something written syntactically correctly in a language can be completely meaningless. And no text can be meaningful if its syntax is incorrect. When this occurs, we say a sentence is grammatically well-formed but semantically nonsensical.

Noam chomsky's has a famous [`illustration`](https://en.wikipedia.org/wiki/Colorless_green_ideas_sleep_furiously)
1. Colorless green ideas sleep furiously. - Syntaxically correct but possibly meaningless
2. *Furiously sleep ideas green colorless. - syntaxically wrong. Cannot be meaningful


## `Parsing`
So how do we parse or extract useful information from text strings?

Regular languages are the simplest of the 4 categories of languages and we can use regular expressions to validate and find patterns in text. They can match patterns but not more complex structures. Strictly speaking, regular expressions are a sequence of character that specifies a match pattern in text. But they have their limitations: For example it is difficult if not impossible to write a regular expression which will determine if a given document is valid HTML (due to things like tag nesting, escape characters, required attributes, et cetera). On the other hand, it's (relatively) trivial to write a parser for HTML.
Though some modern regex engines have support for parsing recursive structures, by definition regular expressions cannot parse(match) more complex patterns or non-regular languages.

Example of these are:
* HTML/XML - [open/close tags, required attributes, self contained tags](https://stackoverflow.com/questions/1732348/regex-match-open-tags-except-xhtml-self-contained-tags/1732454#1732454)
* Programming languages - open/close braces, nested and well formed parentheses in arithmetic expression.
* Order of operations in a mathematical expression

Note that most Regex engines in practice can parse more than regular languages and can parse larger subsets of `CFGs` and context sensitive grammars. [Tom Christiansen's](http://stackoverflow.com/a/4234491/471272) answer shows how Perl Regex was used to write a good parser for HTML but it is in fact, it seems to use regex for lexing and glueing together using a some perl code. (HTML lexer or in his words - chuncker)

So in order to parse these complex patterns, you'd need the next level of formal grammars - context free grammars.

`CFG` are a strict super set of all regular languages. So we say they have more expressive power than regular languages.

Now, how does this relate to markup languages? Well, XML, and some XML markup languages like SVG and XHTML, are all context free. You cannot create a finite state machine that will correctly discriminate all possible XML (or SVG or XHTML) documents.

However, every context-free language has subsets that are regular.

## `Lexers and Parsers `
In a more structured parsing pipeline, for example in compilers, you would have a lexer followed by a parser.
Parsers and lexers are fundamental components of the process of transforming human-readable text into structured data that computers can understand.
### Lexers (or Tokenizers):
* A lexer, often referred to as a tokenizer, is the first stage of the language processing pipeline.
* Its primary function is to break down a stream of characters or text into individual units called tokens.
* Tokens represent fundamental language elements such as keywords, identifiers, literals, and operators i.e. meaningful tokens
* Lexers often generate a stream of tokens, which is the input for the subsequent parsing stage.
* Lexers are used in the analysis of programming languages, natural languages, and various data formats.

### Parser:
A parser is the second stage in language processing, following the tokenization by the lexer.
* Its main task is to analyze the sequence of tokens and determine whether they adhere to the rules of a specified grammar.
* Parsers recognize the hierarchical structure of the language and create a structured representation, typically a tree or an abstract syntax tree (AST).
* An abstract syntax tree represents the syntactic structure of the input, facilitating further analysis or code generation.
* Parsers play a central role in the interpretation of programming languages, the evaluation of mathematical expressions, and the validation of data formats.
* They can be generated manually or using parser generators based on formal grammar specifications.

Note that for some use cases, neither is necessary for the other. For matching tasks, a lexer or tokenizer will do just fine, this is why many easy tasks are done with regex. Another good example is the translation of machine opcodes and human readable assembly. Assembly can just be lexed directly to machine code. But this would not work if you are trying to determine the order of operations of a mathematical expression. Here you would need a Parser.


## `Parsers Library and Parser Generators`
So we can see that we should write parsers when we need to parse more complicated languages (that is, parsers that can understand more complicated grammars).

If you are creating a new programming language, you can write a custom built parser with good design principles and support for more features. But what if you are tasked to write parsers for some custom data formats, or domain specific languages, many times within your code? You could still try to write a custom built parser. If you do more research on formal languages, grammars and parsing, you would see that there are many parsing techniques each with its own strength and weakness. Creating a custom parser without knowing some of these techniques might make your code very disjointed and hard to follow. Furthermore it might not be correct and may be hard to extend. So there are some libraries (or programs) in most General purpose languages for creating parsers. You have parser generators and parsing libraries.

Parser libraries are collections of function, classes and tools that you use to write custom parsers directly in your programming language of choice. They allow the expression of parse rules as an API and give you fine grained control over the parsing process, allowing you to implement custom cases. They can probably also be highly optimized for specific use cases.
Parser Generators take formal grammar and automatically generate parsing code for you. There are some very popular ones like Bison, Yacc, ANTLR. They are a sort of metaprogramming technique that involves a declarative style of writing parsers. They are very useful when you have well defined grammars especially for standardized domain specific languages. Because of this correctness is assured. You may lose a bit of fine-grained control but most parser generators give you a lot of settings and flags for some level of control. If you are especially skilled, you can even modify generated code to your taste. 

In C++, there is even a surge of compile-time parser generators that can generate parsers at compile time (e.g. `CTPG` by Piotr Winter, ). These libraries can even provide reflection capability (e.g. `YACP`) and macros for code generation (`Macro-rules`).

For this tutorial we would be treating and comparing `ANTLR4`, `Boost Spirit` and `CTPG`.

Though they make use of CFGs they are infact usually more powerful than this because they often support the ability for user-written code thus introducing limited amounts of context-sensitivity. (For example, upon encountering a variable declaration, user-written code could save the name and type of the variable into an external data structure, so that these could be checked against later variable references detected by the parser.)


`ANTLR4` (ANother Tool for Langauge Recognition) is a very powerful parser generator. It is a very mature tool and support several targets including Java, C#, JS, Python, C++ etc. A very useful resource on it is in a book called "The Definitive ANTLR4 Reference".

[`Boost Spirit`](https://www.boost.org/doc/libs/1_65_0/libs/spirit/doc/html/spirit/introduction.html) is primarily a parsing library, not a parser generator. It is an object-oriented, recursive-descent parser and output generation library for C++. It allows you to write grammars and format descriptions using a format similar to Extended Backus Naur Form (`EBNF`) directly in C++. So you can mix this with other  C++ code and generate immediately executable code. Boost Spirit has a family of libraries for this purpose: [`Qi - Header only Parser generator`](https://www.boost.org/doc/libs/1_84_0/libs/spirit/doc/html/spirit/qi/tutorials/quick_start.html), [`Karma- Output generator`](https://www.boost.org/doc/libs/1_84_0/libs/spirit/doc/html/spirit/karma/tutorials/quick_start.html) and [`Lex- Lexer generator`](https://www.boost.org/doc/libs/1_83_0/libs/spirit/doc/html/spirit/lex/lexer_introduction.html). With `Spirit.Lex`, you can create a standalone lexer that can add more features to an overall parser like backtracking. `Qi` can generator fully-working parser from a formal ENBF specification, you do not need Lex to generate the full feature parser. `Karma` can generate output inline with a defined `AST`. That said, rather than using this family of libraries, we would use Boost Spirit `X3` which combines some of their features and presents a more expressive API. It is a newer version by Ciere and is what I would use in our project.

`CTPG` seems to be midway between a parsing library and parser generator. It can generate a parser at compile-time by turning code specified in C++ into an LR1 table parser which can actually parse at compile time (if you do things correctly). 

`CTPG` and `Boost.Spirit`  uses a ".cpp" file as its grammar file and C++ as the description language while other parser generators like `ANTLR4` have a grammar file (.g4) extension and use the `ENBF` grammar description format. Because of this, they can seem a little complicated (C++ is already a complex language). `CTPG` uses a lot of compile-time features to allow the rules and context to be generated at compile time and the runtime is as fast as possible. Boost Spirit and `CTPG` create their own embedded Domain Specific Language.


## `Project`
### MS Excel Value Function
This project is a simple project that parses a string into a "Value" much like Microsoft Excel Value() function. The [Excel `VALUE` function](https://support.microsoft.com/en-us/office/value-function-257d0108-07dc-437d-ae1c-bc2d3953d8c2) converts text that appears in a recognized format (i.e. a number, date, or time format) into a numeric value. 

As simple as a parser for MS Excel Value function might feel, the scope of the parser can quickly grow out of hand. Value function can parse Numbers formatted with group and decimal separators, Scientific notation, Date and/or time representations, currency, Percentages etc. As you can see it is quite robust. So we would create a stripped down Value parser. Here are a few feature we would support:
1. Decimal and only group separator -> Period and ','
2. Scientific notations and exponents.
3. Signed numbers
4. ASCII digits, No Korean, Chinese, Japanese numerals
5. Time value
6. Date value
7. Date and Time Value
8. Allow currency
9. Allow percentages
10. Allow accounting brackets () for negation
11. Conform to only US Locale - This affects the date formats that are allowed
   
`Unsupported Features in Value`
1. Locale dependent date format system.
2. Support for Thai or other numeric digits in certain locales.
3. Decimal minutes and seconds in a time format.

We will implement this same task with ANTLR4 and Boost::Spirit, testing all using with GTest and/or Catch2, and benchmarking with google benchmark.

This is by no means a comprehensive comparison between different parser generators and parsing libraries. It serves as a demonstration of some of the concepts we have discussed in real world code. Different parser generators and parsing libraries excel in different areas due to their method of parsing (LL,LR(1), Recursive descent etc.) 

`Good Improvements to this parser`
1. Adding command line arguments support. cppopts and boost library are good libraries for creating CLIs in C++: This way you can pass the string that you want to be parsed or a text file containing lines that you want to be converted. Success state can now be reported per line either in an output manifest file or in the same file at the end of the line.
2. Modularized into functions and data structures so can be used as a library in other projects.
3. Provide support for all MS Excel features.


## `CTPG Implementation`
CTPG was written by Piotr Winter. It can generate lexers or use a custom one.
This is a nice segue from our treatise on [`callables`](../1-callables-and-callbacks/README.MD). CTPG uses functors that execute when a term is matched. The technique used in my implementation can be called action-based parsing.
This program will prompt the user for a string to be parsed and it returns a number representing the value or a parsing error.

Current Limitations:
* No support for decimal and scientific notation.
* No support for month space year pattern.
`Link to code`: [`value_ctpg`](.\src\value_ctpg.cpp)


## `Boost Spirit (X3) Implementation`
We also used an action based approach here. This is not ideal, you can create a proper AST which you can fetch data from. We would compare their performance too.

`Link to Actions-based code`: [`value_spirit`](.\src\value_spirit.cpp)



## `ANTLR4 Implementation`

In ANTLR4, you provide a grammar definition consisting of lexer and parser rules. 
Lexer rule defines the language tokens, they are the smallest unit of the language. A good example in the English language is "." or "Hat", they are tokens representing Punctuation marks and a Noun respectively. In C++, "*", "=", and "class" would be lexical tokens. 
Parser rules define how the tokens are combined to form a valid statement(or expression). This is similar to how "Run", "Walk", "Talk", and "." are valid words (or tokens) but a sentence such as "Run Walk Talk." is not a valid English sentence. 

Lexemes is one of the building blocks of language. A lexeme is a basic unit of meaning. In English it can range from one word like play to phrasal verbs like speak up or orthographic combination of words.
In C++, a valid assignment would be:
```cpp
assignment: lvalue EQUALS_OP expression semicolon | lvalue EQUALS_OP lvalue semicolon
/* 
In words "An assignment consists of a lvalue on the left side of an equals operator 
(this is called an assignment operator in C++ and can be +=, -= *= etc) and the right
side can be an expression or another lvalue, it must always end with a semicolon" 
*/

//e.g. 
i = 2 * 4;
i = j;
```

(Note that I am trying to be grammar agnostic but this grammar format I used here is to Extended Backus-Naur-Form (EBNF) grammar representation )

Note that you can build complex rules on top of both Lexer rules and Parser rules and have something similar to a recursive definition of rules. For example, the "expression" used above may have a parser rule definition like this:
```cpp
expression:  [lvalue|rvalue] binary_operator [lvalue|rvalue] | [lvalue|rvalue] unary_operator 
          |  [lvalue|rvalue] unary_operator
/* 
In words "An expression consists of a binary operator in between two lvalue/rvalues, 
or a unary operator beside an lvalue/rvalue whichever side of the lvalue/rvalue.
*/
```

The grammar definition for an "expression" in a complex language like C++ can be very complicated. The above is very simplified.

Note that even `binary_operator` used above can be defined to include `+,-,*,/` etc. So the point is that grammar rules can get very complicated and strict (Just like most rules). We could have also defined a rule "value" that can be "value = lvalue| rvalue"  to simplify the above definition.

`Link to code`: 


## `Resources`

[`Stack overflow - Writing lexers and parser other than for compilers`](https://stackoverflow.com/questions/7378622/uses-of-writing-lexers-and-parsers-other-than-for-compilers)

[`Freecodecamp - BNF and ENBF`](https://www.freecodecamp.org/news/what-are-bnf-and-ebnf/)

[`Grammars BNF and ENBF`](https://matt.might.net/articles/grammars-bnf-ebnf/)

[`Python Grammar`](http://docs.python.org/3/reference/grammar.html)


[`Comparison of Parser Generators`](https://en.wikipedia.org/wiki/Comparison_of_parser_generators)

[`Comparing Context-free grammar Parser Generators`](https://en.wikipedia.org/wiki/Comparison_of_parser_generators#Deterministic_context-free_languages)


[`Ciere X3 Boost Spirit PDF`](https://ciere.com/cppnow15/using_x3.pdf)

[`Boost Spirit X3`](https://www.boost.org/doc/libs/develop/libs/spirit/doc/x3/html/index.html)

[`Useful Resources to Learn the Boost.Spirit Embedded DSL: Qi, Karma and Lex`](https://www.boost.org/doc/libs/1_83_0/libs/spirit/doc/html/index.html)


[`Recomended book on the topic: Compilers: Principles, Techniques and Tools`](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools)



[`Expressive Compile Time Parsers - Alon Wolf - Meeting C++ 2023`](https://youtu.be/q6X9bKpAmnI?list=PL29gq9jumhdy15P2x2QwB8yZkwlBkwAru)



## `Further reading`
We would use this section when we are going over Test Driven Developmen. We can test our Value parser and ensure it matches our targeted functionality. We might also want to benchmark the various parsers comparing their speed. Another useful thing to be done is to fuzz the input to check for bugs and security vulnerabilities that can leak information. When available, they will be linked here.
