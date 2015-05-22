String vs. char &#42;

# Introduction #

In terms of programming preferences I stand somewhere in-between C and C++. I prefer using C for efficiency, but I do not want to re-implement everything that is already contained in the C++ libraries.

Let me tell you one story that justifies my current position:

I once looked through some GTK sources and I found something surprising. The authors were defining structs that contained pointers to functions. In case you don't know it that is one of the ways of implementing the virtual functions in C++, so what the authors were doing, they were writing C++ in C syntax. But why punish yourself in this way? Really.

What I am doing is the exact opposite. My sources always have the .cpp extension even when I do not intent to use the c++ features. I use classes to encapsulate functionality. I barely ever use templates except for the ones defined in STL. I almost never use the iostreams, simply because scanf, printf are more powerful, however i dislike the way scanf and printf handle the variable numbers of parameters. It was an elegant solution at the time, but it goes really against what compilers do nowadays.

I consider the programming language just a means for you to express what is the job you want done. The compiler does not have to translate what you say into machine code line-by-line or word-by-word. Maybe it was so in the beginnig, but not anymore.

Consider the C version:
The variable number of parameters of functions is used almost exclusively for scanf and printf functions. The implementation is very much hardware-related and it relies on the way parameters were pushed on the stack. The syntax is very nice for the programmer once he gets used to it and it offers a lot of power in a very compact sentance. At the same time it's bad for performance because the format string has to be parsed every time.

On the other hand, in the C++ implementation:
The syntax is actually worse for the programmer. He has to open and close quotes for each parameter and when you are reading the code it's harder to see the actual sentence displayed because it's interrupted everywhere by operators and parameters.

But enough about this... to get to the actual issues

# Implementation #

In this code I have mixed STL strings and char&#42;. I'm not proud of it, but it was the best I could do at the moment.

I prefer char&#42; for reasons of efficiency. But the application is quite large and having to handle all the allocation deallocation and ownership of pointers manually would have been too much of a burden. So I had to use strings.

However, when doing profiling I noticed a significant time was spent in string operators so I had to do something about it. Indeed a large speed-up was obtained by switching to char &#42;.

In short, I used char &#42; in the following situations in critical code:
  * Characters were appended one by one at the end of a string (the STL library is supposed to handle this well, but apparently it doesn't)
  * A lot of indexing into the string was performed (for some reson this is slower, possibly bounds checking)
  * There was no question of the ownership of the string: a char&#42; or char[.md](.md) is used locally in a function and its lifetime is limited by the scope of that function.

You will notice in some cases that i passed strings to functions as references. Although when passing by value the actual contents of the string are not replicated normally I wanted to avoid calling the copy constructor and incrementing/decrementing the reference count.

# Conclusions #

I hope someone will read these pages so I did not write them for nothing, and I would love to get some feedback on the issues discussed here.