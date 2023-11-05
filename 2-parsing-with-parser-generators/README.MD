# Parsing with Parser Generators
Parsing is needed in most of software development. We receive input text input from users and many times we need to run some analysis on the text to extract some useful information. With the advent of LLMs(large language models) that have the capability of NLP (Natural Language processing) this can be easily done, but this is computationally expensive. Besides this, programs may need a deterministic interpretation involving portable and fast computation to understand input. Due to this, we are usually caught writing one parser or the other. For very simple cases, Regex expressions can be used to match patterns. But regex loses it's advantages when the language is more complex to parse usually when it involves recursive, branching grammar etc. (More information on this later).

When do we just match and when do we parse?
Parsing tests whether a text conforms to a grammar and also turns a correct text to a parse tree. It also allows us to return useful information from a text. So it doesn't match, we also extract useful information.

A language, at least in computing, can very simply be described as a set of string. It can be a finite set or an infinite set. But most useful languages are infinite. Computer science likes to talk about languages as "formal langauages", the "formal" means ["defineable using the tools of computer science and mathematics."](http://redsymbol.net/articles/svg-markup-chomsky/) 
 
Noam Chomsky categorized formal languages depending on its expressive power. He split them into about [four (4) categories](https://en.wikipedia.org/wiki/Chomsky_hierarchy). Two such categories are called "regular" and "context-free". The definitions are quite precise: a language is regular if there exists some finite state machine that accepts all strings in the language and rejects all strings outside of it. And a language is context-free if there exists some context-free grammar that accepts all strings in that language, and rejects those outside of it.

But then what is a regular language and a context free language? Definitions from [wikipedia](https://en.wikipedia.org/wiki/Regular_language) are a little cyclic in using regular expression (or rational) expression to define it. Generally, regular languages deal with straightforward, repetitive patterns, while context-free languages handle more complex, structured patterns or relationships.

How do we recognise represent languages?

Grammars:
Grammars are the language of languages. They are a formal method to describe a textual language. Since you can't define a finite set of acceptable sentences for infinite languages, grammars are usually a finite recipe that descibes all acceptable sentences. 


So how do we parse or extract useful information from text strings?
Regular languages are the simplest of the 4 categories and we can use regular expressions to validate and find patterns in text. They can match patterns but not more complex structures. Stictly speaking, regular expressions are a sequence of character that specfies a match pattern in text. But they have their limitations: For example it is difficult if not impossible to write a regular expression which will determine if a given document is valid HTML (due to things like tag nesting, escape characters, required attributes, et cetera). On the other hand, it's (relatively) trivial to write a parser for HTML.
Though some modern regex engines have support for parsing recursive structures, by definition regular expressions cannot parse(match) more complex patterns or non-regular languages.

Example of these are:
* HTML/XML - [open/close tags, required attributes, self contained tags](https://stackoverflow.com/questions/1732348/regex-match-open-tags-except-xhtml-self-contained-tags/1732454#1732454)
* Programming languages - open/close braces, parentheses in arithmetic expression.
* Order of operations in a mathematical expression

Note that most Regex engines in practice can parse more than regular languages and care parse larger subsets of CFGs and context sensitive grammars. [Tom Christiansen's](http://stackoverflow.com/a/4234491/471272) answer shows how Perl Regex was used to write a good parser for HTML but it is in fact, it seems to use REegex for lexing and glueing together using a some perl code. (HTML lexer or in his word's chuncker)

So in order to parse these complex patterns, you'd need the next level of formal grammars - context free grammars.

CFG are a strict super set of all regular languages. So we say they have more expressive power than regular languages.

Now, how does this relate to markup languages? Well, XML, and some XML markup languages like SVG and XHTML, are all context free. You cannot create a finite state machine that will correctly discriminate all possible XML (or SVG or XHTML) documents.

However, every context-free language has subsets that are regular.
---------------------------
Lexers, Parsers 
Parsers and lexers are fundamental components of the process of transforming human-readable text into structured data that computers can understand.
Lexers are also known as Tokenizers:
* A lexer, often referred to as a tokenizer, is the first stage of the language processing pipeline.
* Its primary function is to break down a stream of characters or text into individual units called tokens.
* Tokens represent fundamental language elements such as keywords, identifiers, literals, and operators.
* Lexers discard whitespace and comments and categorize characters into meaningful tokens.
* Lexers often generate a stream of tokens, which is the input for the subsequent parsing stage.
* Lexers are used in the analysis of programming languages, natural languages, and various data formats.

Parser:
A parser is the second stage in language processing, following the tokenization by the lexer.
* Its main task is to analyze the sequence of tokens and determine whether they adhere to the rules of a specified grammar.
* Parsers recognize the hierarchical structure of the language and create a structured representation, typically a tree or an abstract syntax tree (AST).
* An abstract syntax tree represents the syntactic structure of the input, facilitating further analysis or code generation.
* Parsers play a central role in the interpretation of programming languages, the evaluation of mathematical expressions, and the validation of data formats.
* They can be generated manually or using parser generators based on formal grammar specifications.

-----------
Parser Generators and Parser Libraries, Examples, Grammar types and define our focus
-----------
Parsers Library and Parser Generators
So we can see that we should write parsers when we need to parse more complicated languages (that is parsers that can understand more complicated grammars).

If you are creating a new programming language, you can write a custom built parser with good design principles and support for more features. But what if you are tasked to write parsers for some custom data formats, or domain specific languages, many times within your code? You could still try to write a custom built parser but doing more research on formal languages, grammars and parsing, you would see that there are many parsing techniques each with its own strength and weakness. Creating a custom parser without knowing some of these techniques might make your code very disjointed and hard to follow. Furthermore it might not be correct and may be hard to extend. So there are some libraries (or programs) in most General purpose languages for creating parsers. You have parser generators and parsing libraries.

Parser libraries are collections of function, classes and tools that you use to write custom parsers directly in your programming language of choice. They allow the expression of parse rules as an API and give you fine grained control over the parsing process, allowing you to implement custom cases. They can probably also be highly optimized for specific use cases.
Parser Generators take formal grammar and automatically generate parsing code for you. There are some very popular ones like Bison, Yacc, ANTLR. They are a sort of metaprogramming technique that involves a declarative style of writing parsers. They are very useful when you have well defined grammars especially for standardized domain specific languages. Because of this correctness is assured. You may lose a bit of fine-grained control but most parser generators give you a lot of settings and flags for some level of control. If you are especially skilled, you can even modify generated code to your taste. 

------------
Understanding Grammars
--------------

Types of grammars:
Context free Grammar:A context-free grammar is a grammar in which the left-hand side of each production rule consists of only a single nonterminal symbol. This restriction is non-trivial; not all languages can be generated by context-free grammars.

Regular grammars
In regular grammars, the left hand side is again only a single nonterminal symbol, but now the right-hand side is also restricted. The right side may be the empty string, or a single terminal symbol, or a single terminal symbol followed by a nonterminal symbol, but nothing else. (Sometimes a broader definition is used: one can allow longer strings of terminals or single nonterminals without anything else, making languages easier to denote while still defining the same class of languages.)

Recursive grammars
A recursive grammar is a grammar that contains production rules that are recursive. For example, a grammar for a context-free language is left-recursive if there exists a non-terminal symbol A that can be put through the production rules to produce a string with A as the leftmost symbol.An example of recursive grammar is a clause within a sentence separated by two commas


Terminal (or Terminal symbol): Terminals are strings written within quotes. They are meant to be used as they are. Nothing is hidden behind them.

Non-terminal(or Non-terminal symbol) - are used when we need a name to refer to something else. In BNF non-terminal names are written within <> but in ENBF, they are written just plainly.

The whole language is derived from a single non-terminal symbol. This is called the root symbol of the grammar. It is usually written as the first non-terminal in the BNF/ENBF grammar description.

------------
Grammar types:
------------

CFGs can be described using  EBNF and  PEG forms among others. ENBF describes CFG in terms of production  rules. Rules that when applied  can generate all possible legal phrases  in the languages.

PEG uses recognition rules which are rules that can be used to match valid phrases in the language.

When talking about grammars we must also consider the notation grammars can be represented in. A very popular representation method is EBNF - Extended Backus-Naur form. It is used to make a formal description of a formal language. It is actually an extension of the basic Backus-Naur metasyntax notation. Another variant of BNF is ABNF - Augmented Backus-Naur Form.


EBNF is a code that expresses the syntax of a formal language. An EBNF consists of terminal symbols and non-terminal production rules which are the restrictions governing how terminal symbols can be combined into a legal sequence. Examples of terminal symbols include alphanumeric characters, punctuation marks, and whitespace characters.
For more information check: https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form


The EBNF defines production rules where sequences of symbols are respectively assigned to a nonterminal:


After defining the syntax, it is your responsibility to defined the semantics.

Something written syntactically correctly in a language can be completely meaningless. And no text can be meaningful if its syntax is incorrect. When this occurs, we say a sentence is grammatically well-formed but semantically nonsensical 

A popular example is Noam chomsky's (https://en.wikipedia.org/wiki/Colorless_green_ideas_sleep_furiously)
1. Colorless green ideas sleep furiously. - Syntaxically correct but possibly meaningless
2. *Furiously sleep ideas green colorless. - syntaxically wrong. Cannot be meaningful

Here is a good BNF and ENBF playground:
https://bnfplayground.pauliankline.com/


PEG is a type of analytic formal grammar, i.e. it describes a formal language in terms of a set of rules for recognizing strings in the language. 
PEGs also look similar to context-free grammars (CFGs), but they have a different interpretation: the choice operator selects the first match in PEG, while it is ambiguous in CFG. This is closer to how string recognition tends to be done in practice, e.g. by a recursive descent parser.

Parsing Expression Grammaer Notation is a universal notation for parsing any grammar , including natural


For this tutorial we would be treating and comparing ANTLR4, Boost Spirit and CTPG.

Example of PEG grammar
Describing mathematical formulas that apply the basic five operations to non-negative
integers.
Expr    ← Sum
Sum     ← Product (('+' / '-') Product)*
Product ← Power (('*' / '/') Power)*
Power   ← Value ('^' Power)?
Value   ← [0-9]+ / '(' Expr ')'


Boost Spirit

https://www.boost.org/doc/libs/1_65_0/libs/spirit/doc/html/spirit/introduction.html

Boost Spirit is an object-oriented, recursive-descent parser and output generation library for C++. It allows you to write grammars and format descriptions using a format similar to Extended Backus Naur Form (EBNF)[2] directly in C++. These inline grammar specifications can mix freely with other C++ code and, thanks to the generative power of C++ templates, are immediately executable. In retrospect, conventional compiler-compilers or parser-generators have to perform an additional translation step from the source EBNF code to C or C++ code


CTPG - It is a parser generator (Compile time) and a parsing library? 
I think BOost.Spirit is as well.
-------------------


-------------- 
USEFUL
--------------
https://stackoverflow.com/questions/7378622/uses-of-writing-lexers-and-parsers-other-than-for-compilers
Lexers and parsers are good for computerized interpretation of anything that is a context-free language but not a regular language.

In more practical terms, this means that they're good for interpreting anything that has a defined structure but is beyond the capabilities of (or more difficult to do with) regex.

For instance, it is difficult if not impossible to write a regular expression which will determine if a given document is valid HTML (due to things like tag nesting, escape characters, required attributes, et cetera). On the other hand, it's (relatively) trivial to write a parser for HTML.

Similarly, you would probably not want to even try to write a regex to determine the order of operations in a mathematical expression. On the other hand, a parser can do it easily.

As for your question regarding individual lexers or parsers:

Neither is "necessary" for the other, or at all.

For instance, one could have human-readable words which translate directly to machine opcodes that would get lexed directly into machine code (this would essentially be a very basic "assembly language"). This would not require a parser.

One could also simply write programs in a way that already was expressed in machine-readable individual symbols and thus easy for a machine to parse - for instance, boolean algebra expressions that used only the symbols 0, 1, &, |, ~, (, and ). This would not require a lexer.

Or you could do without either - for instance, Brainfuck needs neither lexing nor parsing because it is simply a set of ordered instructions; the interpreter just maps symbols to things to do. Machine opcodes, similarly, do not require either.

Mostly, lexers and parsers are written to make things nicer and easier. It's nicer not to have to write everything in individual single-meaning glyphs. It's easier to be able to write out complex expressions in whatever way is convenient (say, with parentheses, (3+4)*2) than it is to force ourselves to write them in ways that machines work (say, RPN: 3 4 + 2 *).


Some definitions:
In the discipline of Parsing, you would see word like language (formal language, regular language, etc), grammar (Context-free, Context sensitive etc ) thrown about , here are a few definitions of them. 
--------------------




-----------
RESOURCES
-----------
A regular language is defined as a formal langauge that can be defined by a  regular (or rational) expression. (https://en.wikipedia.org/wiki/Regular_language). Strictly speaking, regular expressions are a sequence of character that specfies a match pattern in text.

(http://redsymbol.net/articles/svg-markup-chomsky/)
 Here, "formal" means "defineable using the tools of computer science and mathematics." 

 A language, in this context, is just a set of strings. That's all. It can be an infinite set, and in fact, all useful programming languages I know of are infinite.


 A language can be categorized depending on its expressive power. Two such categories are called "regular" and "context-free". The definitions are quite precise: a language is regular if there exists some finite state machine that accepts all strings in the language and rejects all strings outside of it. And a language is context-free if there exists some context-free grammar that accepts all strings in that language, and rejects those outside of it.

 You may be familiar with both finite state machines (FSMs) and context free grammars (CFGs). They are simply mathematical constructs that can be used to define sets of strings. In case you don't recall or never learned, an important fact is that not all languages can be described by a FSM. In other words, some languages are regular, and some are not. Similarly, some languages are context-free (can be described by a CFG), and are not. However, all regular languages are also context-free languages, but not vice versa.

 CFG are a strict super set of all regular languages. So we say they have more expressive power than regular languages.

 Now, how does this relate to markup languages? Well, XML, and some XML markup languages like SVG and XHTML, are all context free. You cannot create a finite state machine that will correctly discriminate all possible XML (or SVG or XHTML) documents.

However, every context-free language has subsets that are regular.


https://en.wikipedia.org/wiki/Formal_grammar
Grammars is a set of structural rules on how to form strings from an "alphabet " of a formal language that are valid according to a language syntax. It doesn't describe the meaning of the strings or what can be done with them.

Grammars are a set of rules (also called production rules). They are the language of languages. They are a formal method to describe a textual language. Since you can't define a finite set of acceptable sentences for infinite languages, grammars are usually a finite recipe that descibes all acceptable sentences. 


* Programming languages : C, Java, C++
* Domain-specific languages : BibTex
* Data format: log files, protocol data

Parsing tests whether a text conforms to a grammar and also turns a correct text to a parse tree.


Chomsky's Grammar Hierarchy
Type-0: Recursively Enumerable
 Rules: α -> β (unrestricted)
Type-1: Context-sensitive
● Rules: αAβ -> αγβ

Type-2: Context-free
● Rules: A -> γ
Type-3: Regular 
● Rules: A -> a and A -> aB




For instance, the Python lexical specification uses them.

In these grammars:

postfix * means "repeated 0 or more times"
postfix + means "repeated 1 or more times"
postfix ? means "0 or 1 times"
The definition of floating point literals in Python is a good example of combining several notations:

floatnumber   ::=  pointfloat | exponentfloat
pointfloat    ::=  [intpart] fraction | intpart "."
exponentfloat ::=  (intpart | pointfloat) exponent
intpart       ::=  digit+
fraction      ::=  "." digit+
exponent      ::=  ("e" | "E") ["+" | "-"] digit+


-----------------
RESOURCES LINKS
------------------

Resources:
https://www.freecodecamp.org/news/what-are-bnf-and-ebnf/

https://matt.might.net/articles/grammars-bnf-ebnf/

http://docs.python.org/3/reference/grammar.html

PEGN? https://pegn.dev/

(https://en.wikipedia.org/wiki/Comparison_of_parser_generators)

Comparing Context-free grammar PGs :
https://en.wikipedia.org/wiki/Comparison_of_parser_generators#Deterministic_context-free_languages



PEG:
https://en.wikipedia.org/wiki/Comparison_of_parser_generators#Parsing_expression_grammars,_deterministic_boolean_grammars


https://en.wikipedia.org/wiki/Parsing_expression_grammar

----------------------------






----------------------
OLD WRITE UP
----------------------

Talk about ENBF and PEG


Talk about Parsing
Parsing - Lexer and Parser

Types of Parsing 
LL, LR1

Writing your own parser?
When should you write a parser without a parser generator? Performance? Specialization e.g Compilers? Or to communicate intet 



When to use Regex, Parsers and Parser Gen (https://www.freecodecamp.org/news/beyond-regular-expressions-an-introduction-to-parsing-context-free-grammars-ee77bdab5a92/)
Along with regular expression, parsers are a useful tool for programmers.  
Originally Regular expressions is a method of validating and finding patterns in text. The kinds of patterns it can describe or detect are called regular languages which is the simplest of the formal languages in the [Chomsky hierarchy](https://en.wikipedia.org/wiki/Chomsky_hierarchy).
Though some modern regex engines have support for parsing recursive structures, by definition regular expressions cannot parse(match) more complex patterns or non-regular languages.

Example of these are:
* HTML/XML - [open/close tags, required attributes, self contained tags](https://stackoverflow.com/questions/1732348/regex-match-open-tags-except-xhtml-self-contained-tags/1732454#1732454)
* Programming languages - open/close braces, parentheses in arithmetic expression.
* Order of operations in a mathematical expression


(Most Regex engines in practice can parse more than regular languages and care parse larger subsets of CFGs and context sensitive grammars.) [Tom Christiansen's](http://stackoverflow.com/a/4234491/471272) answer shows how PCRE Regex was used to write a good parser for HTML but it is in fact, it seems to use REGEX for lexing and glueing together using a some perl code. (HTML lexer or in his word's chuncker)

So in order to parse these complex patterns, you'd need the next level of formal grammars - context free grammars.

CFGs can be described using  EXBNF and  PEG forms. ENBF describes CFG in terms of production  rules. Rules that when applied  can generate all possible legal phrases  in the languages.

PEG uses recognition rules which are rules that can be used to match valid phrases in the language.

Methods of writing a parser
1. Parser Generators - is a tool that generates the source code for a parser from an abstract definition of the parser, e.g Bison, ANTLR
   
2. Parsing Libraries - a library that allows the expression of parse rules as an API 

Parsing libraries are easier to understandm debug, maintain, and  customize.

For most cases you can construct a simple parser but in this talk, it can quickly become complicated and and difficult to extend (Citation needed!!!). Here comes Parser Generator. There are a sort of metaprogramming technique that involves a declarative style of writing parsers (Citation needed!!!). Using Parser generators, you define the structure of the language, thuis is called the Grammar and then you may add code to execute for different branches and configuration. This results in a more extensible parser and is useful when you find yourself needing to write one parser or the other from time to time.

A list of some common Parser generator: (Citation needed!!!) From  Wikipedia
ANTLR
BISON
Yacc
Boost Spirit
CTPG



The next section should introduce Testing

Then after this I can introduce Benchmarking

Then Fuzzing?

Useful Resources to Learn the Boost.Spirit Embedded DSL: (Qi, Karma and Lex)
https://www.boost.org/doc/libs/1_83_0/libs/spirit/doc/html/index.html

X3

Boost.Spirit Qi
https://www.boost.org/doc/libs/1_78_0/libs/spirit/doc/html/spirit/karma.html
https://www.boost.org/doc/libs/1_78_0/libs/spirit/doc/html/spirit/lex.html



## Project
MS Excel Value Function
Describe the MS Excel Value function
That we would create a stripped down number parser in 

1 using CTPG
1 using ANTLR4 with actions
1 using ANTLR4 with an AST
1 using Boost Spirit with Actions
1 using Boost Spirit with AST

(Possibly show a fully featured Value parser and test it too - Use Work parser)


As simple as a simple parser for MS Excel Value might sound, the scope of the parser can quickly grow out of hand.
Here are a few constraints:

Constraints on the Value Parser?
	1. Decimal and only group separator -> Period and ','
	2. Exponential value
	3. Sign, Positive and Negative is allowed too
	4. Only ASCII digits, No Korean, Chinese, Japanese
	5. No Time value
	6. No Date value
	7. Allow currency
	8. Allow percentages
	9. Allow accounting () for negation


	
Resources:
Ciere X3 Boost Spirit


# Using the Compile Time Parser Generator or (Parsing with Parser Generators)
To get started on learning the library, you can check the README section of [Piotr Winter's CTPG](https://github.com/peter-winter/ctpg) is quite comprehensive but this is a starter project showing how to parse numbers.

This project is a simple project that parses a string into a "NumberValue" much like Microsoft Excel NumberValue() function.

Excel's number value can parse Numbers formatted with group and decimal separators, Scientific notation, Date and/or time representations, currency, Percentages etc. As you can see it is quite robust. See [here]() for MS Excel's official documentation and [this]() for a useful guide on how the function works

This is part of a parser generator series, and I will implement this same task with ANTLR4 and Boost::Spirit and test it using GTest or Catch2, and benchmark with google benchmark.

This is by no means a comprehensive comparison between parser generators as different parser generators excel in different areas due to their method of parsing (LL,LR1, ...????) etc [Check the article Wikipedia](). This is just a useful guide to the current state of things.

Remember Ragel and the ["Cloudflare/Google zero team incident"](), parsers generators help programmers build custom parsers for any form of text. A parser is a piece of software that can extract meaning from a text input. Grammatical meaning comes from rules defining what makes a statement(tokens and how they are arranged).

# Lexers and Parsers.
In some of the parsers, you provide a grammar definition consisting of lexer and parser rules. 
Lexer rule defines the language tokens, they are the smallest unit of the language. A good example in the English language is "." or "Hat", they are tokens representing Punctuation marks and a Noun respectively. In C++, "*", "=", and "class" would be lexical tokens. 
Parser rules define how the tokens are combined to form a valid statement(or expression). This is similar to how "Run", "Walk", "Talk", and "." are valid words (or tokens) but a sentence such as "Run Walk Talk." is not a valid English sentence. In C++, a valid assignment would be:

********
Lexemes is one of the building blocks of language. A lexeme is a basic unit of meaning. In English it can range from one word like play to phrasal verbs like speak up or orthographic combination of words.
*********

```cpp
assignment: lvalue EQUALS_OP expression semicolon | lvalue EQUALS_OP lvalue semicolon
/* 
In words "An assignment consists of a lvalue on the left side of an equals operator (Funnily this is called an assignment operator in C++ and can be +=, -= *= etc) and the right side can be an expression or another lvalue, it must always end with a semicolon" 
*/

//eg 
i = 2 * 4;
i = j;
```

(Note that I am trying to be grammar agnostic but this grammar format I used here is to Extended Backus-Naur-Form (EBNF) grammar representation )
Other Grammar representations can be found [here]().

Note that you can build complex rules on top of both Lexer rules and Parser rules and have something similar to a recursive definition of rules. For example, the "expression" used above may have a parser rule definition like this:
```cpp
expression:  [lvalue|rvalue] binary_operator [lvalue|rvalue] | [lvalue|rvalue] unary_operator |  [lvalue|rvalue] unary_operator
/* 
In words "An expression consists of a binary operator in between two lvalue/rvalues, or a unary operator beside an lvalue/rvalue whichever side of the lvalue/rvalue.
*/
```


The grammar definition for an "expression" in a complex language like C++ can be very complicated. The above is very simplified. 
and note that even "binary_operator" used above can be defined to include "+,-,*,/" etc. So the point is that grammar rules can get very complicated and strict (Just like most rules). I could have also defined a rule "value" that can be "value = lvalue| rvalue"  to simplify the above definition.



This is a nice segue from our treatise on [callables](../1-callables-and-callbacks/README.MD). CTPG uses functors that execute when a term is matched.


CTPG as a parser generator is unique as it uses a ".cpp" file as its grammar file and C++ as the description language. Other parser generators like ANTLR4 have a grammar file (.g4) extension and use the ENBF grammar description format. Because of this, it can seem a little complicated (C++ is already a complex language). It also uses a lot of compile-time features to allow the rules and context to be generated at compile time and the runtime is as fast as possible.
This is related to a Embedded Domain Specific Language

Boost.Spirit expects rules to be described using Parsing Expression Grammar (PEG). PEG is related to Extended Backus-Naur-Form (EBNF).


This project demonstrates how the library can be used in a scenario such as Excel's "Numbervalue" function. I expect that you might have gone through the tutorial specified [here](https://github.com/peter-winter/ctpg).


## `Implementing Numbervalue in CTPG`
This program will take in command line arguments and try to parse them,
it can also take in a text file and will try to parse each line and report its success state per line. 

There are 3 files



<hr>
In the next section, we would go through Test Driven Development and how we can test this numbervalue function so that it matches Excel's behaviour. 


