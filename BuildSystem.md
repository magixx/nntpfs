Build System

# Introduction #

A lot of people will scoff at the system I used for compiling the application, however for my defense I will explain the rationale behind this.

To explain the solution in short: I am using a preprocessor to combine all source files into a single c++ source which is then compiled. The syntax is is inspired from Pascal/Delphi which I used for a long time before switching to C.

# Details #

Under this approach, the .h/.cpp pair is combined in a single file and the two parts are delimited by the "interface" and "implementation" keywords. The "uses" keyword is used instead of #include.

The preprocessor determines all dependencies and combines all used sources in the appropriate order. There are two places where the "uses" keyword should be used: in the beginning of the interface, when the local type definitions of the module refer to types defined by other modules, or at the beginning of the implementation when local functions use types and functions declared in other modules.

I considered using namespaces for separating the modules but that did more harm than good so I abandoned the approach.

The reasons I used this system are the following:
  * At least under some circumstances it saves compile time (signigicant time is wasted by the compiler parsing headers for each source, if you are not using PCH that is; this is especially meaningfull for the standard headers in c++)
  * Under this approach, global optimizations are done by default (this seems to increase compilation time though)
  * Some of the features of C are actually work-arounds for the separation of headers and actual soruces. Consider the #ifndef _HEADER\_DEF, #define #endif present in all ".h" files and the "external" definitions._

# Conclusions #

The features I introduced with this preprocessor are nothing new, they existed in other languages for many years now, and it seems to be also the direction for the newer languages (consider D, C#, Java). However I prefer to stick to C/C++ for performance reasons and I only eliminated what I personally considered to be some nuissances.

I hope that will not discourage any potential developers from contributing to this project. If needed, we can modify the preprocessor to generate the old-style header/source pairs.