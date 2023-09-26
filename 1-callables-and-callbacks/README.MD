# Callables and Callbacks

- [Callables and Callbacks](#callables-and-callbacks)
  - [Formal definition of a Callable](#formal-definition-of-a-callable)
  - [Why do we need Callbacks?](#why-do-we-need-callbacks)
  - [Some sample use cases](#some-sample-use-cases)
    - [A. `Predicates of Algorithms in the C and C++ Standard Library`](#a-predicates-of-algorithms-in-the-c-and-c-standard-library)
    - [B. `Event-Driven GUI frameworks`](#b-event-driven-gui-frameworks)
      - [`GTK`](#gtk)
      - [`wxWidgets`](#wxwidgets)
      - [`QT`](#qt)
    - [C. `Multithreading`](#c-multithreading)
    - [D. `std::variant` and `std::visit`](#d-stdvariant-and-stdvisit)
    - [E. `Simulating Polymorphism and Dynamic Dispatch`](#e-simulating-polymorphism-and-dynamic-dispatch)
  - [How do we achieve callbacks?](#how-do-we-achieve-callbacks)
    - [Function Pointers](#function-pointers)
      - [`Function Pointer Declaration:`](#function-pointer-declaration)
      - [`Pointer to a member function`](#pointer-to-a-member-function)
    - [`std::function`](#stdfunction)
      - [`std::function object/type notation`](#stdfunction-objecttype-notation)
      - [`Callback call notation`](#callback-call-notation)
      - [`Using std::function`](#using-stdfunction)
        - [***1. `Function pointers and pointers to member functions`***](#1-function-pointers-and-pointers-to-member-functions)
        - [***2. `Lambda expressions`***](#2-lambda-expressions)
        - [***3. `std::bind` expressions***](#3-stdbind-expressions)
        - [***4. `Function objects`***](#4-function-objects)
    - [Using Templated Callback Type](#using-templated-callback-type)
      - [Using `C++ Generic programming`, `concepts` and `std::function`](#using-c-generic-programming-concepts-and-stdfunction)
  - [Addendum: Casting Function pointer and Dangerous Callables - Warning !!!](#addendum-casting-function-pointer-and-dangerous-callables---warning-)
  - [Conclusion](#conclusion)
  - [Section files](#section-files)
  - [Summary](#summary)
  - [Resources and More Reading](#resources-and-more-reading)


`Callables` are code blocks that can be called or "invoked" to perform a task. They may or may not be passed data. They are usually executed using the call operator (i.e. `operator()`).
## Formal definition of a Callable
A `Callable` type is a type for which the `INVOKE` operation (used by, e.g., `std::function`, `std::bind`, and `std::thread::thread`) is applicable. ([cppreference](https://en.cppreference.com/w/cpp/named_req/Callable))

Note you can also invoke them using std::invoke or the std::invoke_r\<return_type\> 

For the vast majority of us, we would be dealing with functions, lambdas and function objects ("functors"). These can be used to modularize our code. 


Callables used as arguments to other functions are called `Callbacks`.


## Why do we need Callbacks?
They add a form of dynamic behaviour to our programs. It can emulate runtime polymorphism in a way without the use of virtual tables. This is used in several places.

It allows us to write generic code that is independent of the logic of the called function and can be reused with different callbacks. It is also used in Event-driven programs to notify the program that certain events have taken place. This is a form of static/compile time flexibility.

Library designers use this to allow the user of the library to decide what to do with certain information or actions. Thus enabling dynamic runtime behaviour.

## Some sample use cases
### A. `Predicates of Algorithms in the C and C++ Standard Library`
Using the `sort` function as a case study. C and C++ have standard sorting algorithms `qsort` in `<stdlib>` C header file (or `<cstdlib>` for C++) and the `std::sort` in C++ `<algorithm>`.

```cpp
//C++ qsort
void qsort( void *ptr, std::size_t count, std::size_t size, /*compare-pred*/* comp );
void qsort( void *ptr, std::size_t count, std::size_t size, /*c-compare-pred*/* comp );

extern "C++" using /* compare-pred */ = int(const void*, const void*);
extern "C" using /* c-compare-pred */ = int(const void*, const void*);

//C qsort
void qsort( void *ptr, size_t count, size_t size, int (*comp)(const void *, const void *) );


//C++ sort using Metaprogramming
template< class RandomIt, class Compare >
void sort( RandomIt first, RandomIt last, Compare comp );

template< class RandomIt, class Compare >
constexpr void sort( RandomIt first, RandomIt last, Compare comp );

//C++ sort used with a comp callback
```

`qsort()` sorts a given array pointed to by `ptr` in ascending order. The array contains `count` elements of `size` bytes. The function pointed to by `comp`` is used for object comparison. This function pointer parameter allows you to determine how the elements are compared. Normally this is the Less than "<" operator.

For the C++ `std::sort()` if you don't supply the `comp` argument, it uses the operator<. C++ also defines standard callbacks `std::less<T>`, `std::greater<T>` etc. But the requirement is that the comparison function object is that it returns ​true if the first argument is less than (i.e. is ordered before) the second. 
The signature of the comparison function should be equivalent to the following: (or can be non-const if it is a value parameter)
```c++
 bool cmp(const Type1 &a, const Type2 &b);
 ```

Note that there are multiple overloads of the C++ sort, even one that allows you to set the "execution" policy, this allows parallelized operations on containers. The beauty of C++!


The C++ Standard library uses this concept a lot in the form of a predicates ("Pred"). You can check out `cppreference.com` for the (template) functions contained in the `<algorithm>` and `<numeric>` headers. Along with Generic programming and  Metaprogramming with Templates, they give C++ a powerful ability to manipulate containers.


```c++

#include <algorithm>
#include <functional>
#include <array>
#include <iostream>
#include <string_view>
 
int main()
{
    std::array<int, 10> numbers = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
 
    auto print = [&numbers](std::string_view const rem) {
        for (auto number : numbers) {
            std::cout << number << ' ';
        }
        std::cout << ": " << rem << '\n';
    };
 
    std::sort(numbers.begin(), numbers.end());
    print("sorted with the default operator<");
 
    std::sort(numbers.begin(), numbers.end(), std::greater<int>());
    print("sorted with the standard library compare function object, std::greater<int>()");
 
    struct {
        bool operator()(int a, int b) const { return a < b; }
    } customLess;
    std::sort(numbers.begin(), numbers.end(), customLess);
    print("sorted with a custom function object,CustomLess");
 
    std::sort(numbers.begin(), numbers.end(), [](int a, int b) {
        return a > b;
    });
    print("sorted with a lambda expression implemenation of greater-than");
}


```
`Output`
```txt
0 1 2 3 4 5 6 7 8 9 : sorted with the default operator<
9 8 7 6 5 4 3 2 1 0 : sorted with the standard library compare function object, std::greater<int>()
0 1 2 3 4 5 6 7 8 9 : sorted with a custom function object, CustomLess
9 8 7 6 5 4 3 2 1 0 : sorted with a lambda expression implemenation of greater-than

```

### B. `Event-Driven GUI frameworks`
Callbacks are heavily used in many GUIs and by library authors in their framework code. This allows library users to create custom actions to be executed when an event occurs, for example when you click on a button. This is known as Event Driven programming. 
In event-driven GUI frameworks, user interactions (such as clicking a button or typing in a text box) are treated as events. These events trigger specific event handlers or callbacks, allowing developers to define how the application responds to each event. The application sits in a loop waiting for events initiated by the user or from another source (such as a window needing to be refreshed or a socket connection), and then it dispatches the event to an appropriate function that handles it. 
Due to this inherent characteristic, callables and callbacks are used to execute an action when an event occurs.  

Below are a few examples from my biased choice of GUI frameworks:

#### `GTK`
```cpp
/*
This is a simple "Hello World" program using the GTK+ library.
GTK+ is primarily a C library, gtkmm is it's C++ flavour.
*/
#include  <gtk/gtk.h>

//Call back function
void hello (GtkWidget *widget, gpointer data ) {
  g_print ("Hello World\n");
}

int main( int argc, char* argv[])
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window;
  GtkWidget *button

  /* creates a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  /* Sets the border width of the window. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Creates a new button with the label "Hello World". */
  button = gtk_button_new_with_label ("Hello World");

  /*This is where we pass the callback using a GTK wrapper G_CALLBACK()*/ 
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (hello), NULL);

  /*The button is stacked into window container*/
  gtk_container_add (GTK_CONTAINER (window), button);
  /* The final step is to display this newly created widget. */
  gtk_widget_show (button);
  /* and the window */
  gtk_widget_show (window);

  /* All GTK applications must have a gtk_main(). Control ends here
  * and waits for an event to occur (like a key press or
  * mouse event). */
  gtk_main ();
  return 0;
}
```
For the other frameworks, I will just show code how a callback is created and used:

#### `wxWidgets`
```cpp

//Callbacks
void MyFrame::OnAbout(wxCommandEvent& event) {
  wxString msg;
  msg.Printf(wxT(“Hello and welcome to %s”), wxVERSION_STRING);
  wxMessageBox(msg, wxT(“About Window”), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnQuit(wxCommandEvent& event) {
  // Destroy the frame
  Close();
}

//Statically routing events using a macro
// Event table for MyFrame
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
END_EVENT_TABLE()

//Analog code: Dynamically routing events
frame->Connect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnQuit) );
frame->Connect( wxID_ABOUT,wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnAbout) );
```

#### `QT`
```cpp
#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QPushButton *button = new QPushButton("Quit");
  QObject::connect(button, SIGNAL(clicked()), &app, SLOT(quit()));button->show();
  return app.exec();
}
```

Another C++ GUI framework that is gaining popularity is the [Dear ImGUI](https://github.com/ocornut/imgui) (You can check out its pitch and where it is best suited). It is an Immediate mode GUI. QT and wxWidgets are Event Driven Retained mode GUIs.

This section just illustrates how similar many GUI frameworks are. Asides from the boilerplate code for them, they all make use of callbacks to dispatch and handle different user and system events.

### C. `Multithreading`
Multithreading allows concurrent execution of different parts of a program for maximum utilization of the CPU.

To support this functionality, it uses callbacks to dispatch routines.

Concepts like Multithreading, Concurrency, and Parallelism are quite complex and are by themselves all rabbit holes that deserve separate articles. A top-level simple way of distinguishing them is that Concurrency and Parallelism are concepts that describe the way processes run. They are either sequential(one after another), concurrent(able to make progress "at the same time" or in overlapping periods although not necessarily at the same instant), or parallel(they happen simultaneously). Multithreading is a technique that can be used to enable concurrency(for example if the threads interact), and parallelism (if the threads do not interact). 

***<details><summary><code>Rabbit Hole</code></summary>***
Note that multithreading can occur on a concurrent or noncurrent system (eg a non-multicore computer) because the operating system can simulate this by switching between the different tasks quickly and intelligently (Task scheduling, Time sharing slicing etc). So in this situation, the tasks can be said to have occurred concurrently but not in parallel. 
A useful guide is that concurrency existed before multicore/multiprocessor machines due to various clever techniques by the task scheduler, but parallel computing is impossible in a (single core) single processor as only one computation can occur at any instant (during any single clock cycle.) Concurrent computing consists of process lifetimes overlapping but execution *need not* happen at the same instant.
Unfortunately, this is the end of the rabbit hole. It is an interesting area to check out as one or all the techniques may be useful in your software project. 
Concurrent and parallel systems have their quirks and interesting problems you have to consider and solve to avoid spectacular crashes, data corruption, and race conditions among others.

`Resources for Further reading`

1. [`Concurrent_computing - Wikipedia`](https://en.m.wikipedia.org/wiki/Concurrent_computing)

2. CppNow:
    - [`Bryce Adelstein - C++ Standard Parallelism`](https://youtu.be/H8HplZtVGT0)
    - [`Fedor Pikus - Concurrency in C++ part 1`](https://youtu.be/ywJ4cq67-uc)
    - [`Fedor Pikus - Concurrency in C++ part 1`](https://youtu.be/R0V4xJ9HZpA)
    - [`Slides`](https://github.com/boostcon)

3. CppCon
    - [`Rainer Grimm - Concurrency Patterns`](https://youtu.be/A3DQxZCtKqo)
4. Meeting Cpp
    - [`Rainer Grimm - Concurrency Patterns`](https://youtu.be/OSZ7XtU1Oh4)
5. [`What is the difference between concurrency and Multithreading - StackOverflow`](https://stackoverflow.com/questions/35100102/what-is-the-differene-between-concurrency-and-multithreading)
</details>
<hr>
<br>
Below is a simple program utilizing C++ <em>std::thread</em> functionality to split routines into different threads. The routines are passed around as callables(callbacks).

```cpp
#include <iostream>
#include <chrono>
#include <thread>

//Callback
void threadfn(int value) {
  std::cout << "We are inside a thread t1\nLong process for t1 about to start:\n";
  std::this_thread::sleep_for(std::chrono::seconds(2)); //Simulating a long process e.g. establishing a network connection.
  std::cout << "Long process for t1 over\n";
  std::cout << "Value: " << value * 2 << '\n';
}

/*
In reality, the standard console output stream is a resource we may not want to multithread.
*/

int main()
{
  int localvalue = 20;
  std::thread t1 {threadfn, localvalue};

  std::thread t2 {
    [&]() {
      std::cout << "***This is another thread using a lambda function***\n";
      std::cout << "***Value: " << ++localvalue << " ***\n"; //capture list allows this
      std::cout << "****End of t2****\n";
    }
  };

  t1.join();
  t2.join();
}
```
A possible output can be below:
```text
$g++ test.cpp && ./a.out
We are inside a thread t1
Long process for t1 about to start:
***This is another thread using a lambda function***
***Value: 21 ***
****End of t2****
Long process for t1 over
Value: 40
```

### D. `std::variant` and `std::visit`
Now, this is an interesting part. If you do not know about `std::variant` (C++ new disjointed unions or [sum type](https://en.wikipedia.org/wiki/Sum_type)), [here](https://www.cppstories.com/2018/06/variant/) is a useful article talking about `std:variant`, the overload visitor pattern,  and some of its applications. I may have an article coming up on this too. 
To summarize, `std::visit` needs a function object that has overloads that accept all possible `std::variant` alternatives. It’s okay if the overloads do not exactly match since the compiler will use normal overload resolution.

The easiest way is to use a generic lambda:

```cpp
//Generic Lambda
//Only condition is that std::ostream& operator<< for the type should be overloaded
//You can use Concepts to specify this requirement
auto PrintVisitor = [](const auto& t) { std::cout << t << '\n'; };
std::variant<int, float, std::string> intFloatStr { "Hello" };
std::visit(PrintVisitor, intFloatStr);


//Here is a specification using the a concept clause
template <typename T>
concept Printable = requires(T t) { std::cout << t; };

auto PrintVisitor = [](const Printable auto & t){ std::cout << t << '\n'; }; 


//Below is a test program to prove this
#include <concepts>   
#include <iostream>
#include <string>
#include <variant> 

struct IntDoublePrint {
    int i{0};
    double d{0};

    IntDoublePrint(){};
    IntDoublePrint(int ii, double dd) : i{ii}, d{dd} {};

    friend std::ostream& operator<<(std::ostream& os, const IntDoublePrint& t) {
        return std::cout << t.i << "  " << t.d;
    };
};

int main() {
    auto PrintVisitor = [](const Printable auto& t) { std::cout << t << '\n'; };

    // auto PrintVisitor = [](const auto & t){ std::cout << t << '\n'; }; //Pure
    // generic lambda, std::ostream constrained

    // std::variant<int, float, std::string, IntDoublePrint> intFloatStr { "Hello" };
    std::variant<int, float, std::string, IntDoublePrint> intFloatStr{IntDoublePrint(1, 2)};
    std::visit(PrintVisitor, intFloatStr);
    intFloatStr = "Hello";
    std::visit(PrintVisitor, intFloatStr);

    // Test that the std::ostream& operator<< works
    IntDoublePrint printval;
    std::cout << printval << '\n';
}
```
Output:
```txt
1  2
Hello
0  0
```
[Play with this in Compiler Explorer!](https://godbolt.org/z/ox51a7f6Y)

       

Using a custom function object that has overloads for each `std::variant` alternative:

```cpp
struct MultiplyVisitor
{
    float mFactor;
    MultiplyVisitor(float factor) : mFactor(factor) { }
    void operator()(int& i) const {
        i *= static_cast<int>(mFactor);
    }
    void operator()(float& f) const {
        f *= mFactor;
    }
    void operator()(std::string& ) const {
        // nothing to do here...
        std::cout << "Called MultiplyVisitor String:" << mFactor << '\n';
    }
};

//Mode of Use

std::variant<int, float> intFloat { 20.4f };
std::visit(MultiplyVisitor(0.5f), intFloat); //returns 10.2

```
This is nice but you have to pay attention to scope problems, it is verbose and not very flexible. Here we had to use a struct member variable to store the multiplying factor. This value was passed into the function object constructor. This is actually an advantage of function objects over functions. The struct member variables can be used to store states to be accessed later.


Using the Overload Visitor Pattern, you can write several custom lambdas for all matching types within the `std::variant`. It creates a function object inheriting all their operators and `std::visit` calls the corresponding one as needed (Using the visit pattern).

```cpp
// overload pattern
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::variant<int, float, std::string> intFloatString2 { "Hello" };

//The pattern allows this:
std::visit(overloaded{
    [](int& i) { i*= 2; std::cout << i; },
    [](float& f) { f*= 2.0f;  std::cout << f;},
    [](std::string& s) { s = s + s; std::cout << s; }
}, intFloatString2);
```
Output
```txt
HelloHello
```
If you haven't come across this pattern before, it might be difficult to understand it on your first glance. This version is a culmination of new features of C++ standard up until C++20.
You can find out more about `std::variant`, and how and why the overload pattern works in this [cppstories article](https://www.cppstories.com/2018/06/variant/).

This section just demonstrates the use of callables in `std::variant`.


### E. `Simulating Polymorphism and Dynamic Dispatch`
Have you ever wondered how large projects (eg the Linux kernel, Git etc) were written in C without OOP? They use a handful of "pure" techniques like structs data members filled with callbacks using function pointers etc.

***<details><summary><code>Rabbit Hole</code></summary>***

[Check this section for a description of some techniques in simulating polymorphism.](./simulating-polymorphism.md)

For a C library that takes this to another level, check out [`GObject`](https://docs.gtk.org/gobject/concepts.html) used by GTK, GDK, Pango and other GNOME projects.
</details>

## How do we achieve callbacks?

Throughout this article, I have made hints at predicates in the form of lambdas and function pointers. But what are the other standard ways functions are passed to other functions? I have also just shown them being passed but I haven't described how you declare a function that would receive another function. A function that takes another function as an argument is called a Higher Order Function.

Callbacks can be achieved in several ways, as of C++20, here are a few standard ways:
1. Function pointers
2. Lambda expressions
3. Function objects
4. Bind expressions
5. `std::function` objects
6. Class Data members?


### Function Pointers
When I just started learning C++, I discovered quickly that you can have a pointer to almost anything. Why would I need a pointer to a function? 
The answer lies in how code execution works. In simple terms, when your program is executing and it gets to a function call, this is simply a jump statement to a block of memory containing some code (if the function has parameters, this is also passed) and an address to return the result of the computation ( Check these [here](https://stackoverflow.com/questions/23981391/how-exactly-does-the-callstack-work/61157110#61157110) for a more  technical and correct explanation, [or this](https://zhu45.org/posts/2017/Jul/30/understanding-how-function-call-works/)). A function name identifies this address (a reference) and function pointers can store the starting address.

With this knowledge, it is easy to see how we can emulate this behaviour as software engineers, by explicitly passing these blocks to other functions to execute code sections.

Function pointers are the purest form of callables.
***<details><summary>Useful Info</summary>***
I'll be using the following to represent possible parameters and parameter types.
```c++
function_name([parameter_type_1] [, parameter_type_2 ...])
```
Of course, I could have opted for:
```c++
function_name(parameter_types...)
```

But this alludes to parameter unpacking which is a concept in C++. This might be confusing and misleading. The chosen style is more obvious that is not valid C++ code
</details>

#### `Function Pointer Declaration:`

***`A function pointer type`***
```c++
//Note: not valid C++ code, just a representation
return_type (*) ([parameter_type_1] [, parameter_type_2 ...])

//For example
int (*) (int) // e.g int foo(int x) {return x;}
```

***`A named function pointer`***
```c++
return_type (* name) ([parameter_type_1] [, parameter_type_2 ...])

//For example
int (*f_int_t) (int) = &foo // Function pointer pointing to int foo(int x) {return x;}
```

***`typedef`***
```c++

typedef int (*f_int_t) (int);

//f_int_t is now a type: function pointer taking an int and returning an int

f_int_t foo_p = &foo;  //foo_p is the function pointer variable
```

***`using` construct***
```c++
//Better still ( and easier to read)
using f_int_t = int(*)(int);
```

`Declaring a Higher order function taking in a function pointer callback`
```c++
int higher_order_fn(int x, int(*callback)(int));

//analog
int higher_order_fn(int x, f_int_t callback); //f_int_t = int(*)(int)
```

`Invoking the callback within a function`
```c++
int higher_order_fn(int x, int (*callback)(int))
{
    return x + callback(x); // function pointer "callback" called using argument x
}
// analog
int higher_order_fn (int x, f_int_t callback)
{
    return x + callback(x);
}
```

***Note:*** As of C++17, a call like f(...) can be written as std::invoke(f, ...) which also handles the pointer to member case. This provides a uniform interface for calling callbacks.
C++ compilers allows the free function names to be passed directly to a function pointer callback parameter without passing a reference as the name is already a reference. 
```c++
int add8(int x) {return x + 8;}
//passed 
higher_order_fn(5, add8);
```

#### `Pointer to a member function`
A more complex version of function pointer is a pointer to a member function. A pointer to a member function of some class `T` requires an object of type `T` to operate on.

***`Pointer to a member function notation`***
```c++
// can have more or less parameters
return_type (T::*)([parameter_type_1] [, parameter_type_2 ...])

// e.g. a pointer to T::foo with one int parameter and returning an int 
int (T::*) (int)

//Named pointer to member function
return_type (T::* name)([parameter_type_1] [, parameter_type_2 ...])
```
Just as before, you can get a named type using `typedef` or `using` (please prefer `using` when creating named interfaces)

```c++
// taking int returning int is:
typedef int (T::* f_T_int_t) (int x); 
//using notation
using f_T_int_t = int(T::*)(int x);


// T_foo_p is a pointer to member function of type T taking int and returning int
// Its value is initialized by an address of a named class member function "foo"
int (T::* T_foo_p)(int) = &T::foo;

// which can also be written using the type alias defined above:
f_T_int_t T_foo_p = &T::foo;

```

`Declaring Higher Order Function taking in a member function pointer`

```c++
// T_higher_fn has a callback named "callback" of type pointer to a member function of T
// where the callback returns int taking int as its argument
// also needs an object of type T
int T_higher_fn (int x, T const &t, int (T::*callback)(int));
// can equivalently declared using the typedef above:
int T_higher_fn (int x, T const &t, f_T_int_t callback);
```
`Callback call notation`

To invoke a pointer to a member function of T, you need an object of type T and member access operation on the dereferenced pointer.


```c++
int T_higher_fn (int x, T const &t, int (T::*callback)(int))
{
    return x + (t.*callback)(x); // member function pointer "callback" invoked for object t using argument x
}
// analog
int T_higher_fn (int x, T const &t, f_T_int_t callback)
{
    return x + (t.*callback)(x); 
}


//if the object is passed as a pointer/ or onject address
int T_higher_fn2 (int x, T const * t, int (T::*callback)(int))
{
    if (!t) return x;
    // member function pointer "callback" called for object *t using argument x
    return x + ((*t).*callback)(x); 
}
// or equivalent:
int T_higher_fn2 (int x, T const * t, f_T_int_t callback)
{
    if (!t) return x;
    return x + (t->*callback)(x); 
}
```

`Callback use notation and compatible types`

```c++
T t{2}; // aggregate initialization
int a = 5;
int b = T_higher_fn(a, t, &T::foo); // call T_higher_fn with pointer to foo as its callback
```

***Notes***
*Default parameters won’t work for functions called through function pointers. Default parameters are resolved at compile-time (that is, if you don’t supply an argument for a defaulted parameter, the compiler substitutes one in for you when the code is compiled). However, function pointers are resolved at run-time. Consequently, default parameters can not be resolved when making a function call with a function pointer. You’ll explicitly have to pass in values for any defaulted parameters in this case.*

*[Functions are not first class citizens in C and C++](https://stackoverflow.com/questions/26559758/function-to-function-pointer-decay). They are code, not data and they cannot be copied, passed as arguments to functions or returned by functions (but all these can happen to pointers to functions). This is why a function name is treated like a pointer to that function and not like the function body itself. The possibility to use a function (name) instead of a pointer to the function is just a courtesy the language makes to the programmer, and not a "decay". The same for arrays: they are not copied, not passed as function arguments and not returned by functions. The address of their first element is used instead (copied, passed as function argument or returned by functions). This is why the array name can be used instead of the address of its first element and again, this is just a way to write less (and less obfuscated) code.*


### `std::function`
`std::function` objects are a magic polymorphic function wrapper to store, copy or invoke callables. They are found in the header `<functional>`.
To disambiguate the word "magic", 
`std::function` is a *`type erasure`* object. That means it erases the details of how some operations happen, and provides a uniform runtime interface to them. For `std::function`, the primary operations are copy/move, destruction, and 'invocation' with operator() -- the 'function like call operator'.
`lambdas` and `std::function` added more functional programming capability to C++ by having a "first class" callable type that can be passed around as data.


#### `std::function object/type notation`
```c++
std::function<return_type(parameter_types...)>

//declaration in C++ standard
template< class R, class... Args >
class function<R(Args...)>;

// e.g. given a declared function foo:
std::function<int(int)> stdf_foo = &foo;
// or T::foo:
std::function<int(const T&, int)> stdf_T_foo = &T::foo;
```

#### `Callback call notation`
The class `std::function` has `operator()` defined which can be used to invoke its target.

```c++
int stdf_higher_fn (int x, std::function<int(int)> callback)
{
    return x + callback(x); // std::function "callback" called
}
// or 
int stdf_T_higher_fn (int x, T const &t, std::function<int(T const &, int)> callback)
{
    return x + callback(t, x); // std::function "callback" called using t and x
}
```

#### `Using std::function`
Since `std::function` is a wrapper, it very generic and can be passed function pointers or pointers to member function. These can be implicitly converted into an `std::function` object.

`A list of types that can be passed to an std::function parameter`

##### ***1. `Function pointers and pointers to member functions`***
```c++ 
//Given
int add2 (int x) { return 2+x; }

//using stdf_higher_fn definition above
int a = 3;
int b = stdf_higher_fn(a, &add2);   //passed a function pointer
// b = 8 = ( 3 + (2+3) )

T t{7}; // aggregate initialization
int b = stdf_T_higher_fn(a, t, &T::add2);  //passed a pointer to a member function
// b = 11 = ( 2 + (7+2) )
```
For complicated `std::function` parameters, you may need to wrap the function pointer reference with a `std::function()` constructor . I noticed this in clang 16 and GCC 13.1 with variadic `std::function` parameters eg.
```c++
//Functions
double quadratic(double x, double a, double b, double c) {return a *  std::pow(x, 2) + b * std::pow(x,1) + c *std::pow(x,0); }
double mag_vector(double i, double j, double k) {return std::sqrt(std::pow(i,2) + std::pow(j,2) + std::pow(k,2));};

//scale_args
template<typename... T>
double scale_args(double scale, std::function<double(T...)> fn, T... args ) 
{
  return fn(scale * args...); //scaled using fold expression
}

//in Use:
std::cout << scale_args(5.0, std::function(quadratic), 4.0, 3.0, 4.0, 6.0); //6430 
std::cout << scale_args(5.0, std::function(mag_vector), 3.0, 4.0, 6.0);     //39.0512
```

##### ***2. `Lambda expressions`***

An unnamed closure from a lambda expression can be stored in a std::function object:
```c++
int a = 2;
int c = 3;
int b = stdf_higher_fn(a, [c](int x) -> int { return 7+c*x; });
// b == 15 ==  a + (7+c*a) == 2 + (7+ 3 × 2)
```
Named closures are supported as well. 
You can think of lambdas as convenient anonymous functors (function objects). Another way of looking at lambdas is that they are `std::function` literals, just like `int` has the literal form e.g. `123`, an `std::function` object can have an unnamed lambda closure as a literal.
e.g. 
```c++
std::function<int(int)> add2 = [](int num){return num + 2;} ;
```
`std::function` objects formed from lambdas will also store the state of the lambda (i.e. the capture policy). 
```c++
int value = 10;
std::function<double(double)> add2value = [&](double num){return num + value + 2;};
std::cout << scale_args(5.0, add2value, 4.0);  //32 (using the definition of scale_args above)
```
These are called Stateful lambdas and is very similar to function objects that can have states. For example below is a simple illustration of how capture lists may work:

```c++
#include <iostream>

int main()
{
  int x{1}, y{2};

  //variable x is captured by value, y by reference in this IIILE (inlined, immediately, invoked lambda expr)
  [x, &y]() mutable {x = 10, y = 20;} ();
  std::cout << "x = " << x << ", y = " << y << std::endl; // x = 1, y =  20

  //Functor analog of IIILE
  {
    struct F
    {
      int x;
      int &y;


      F(int x, int &y) : x(x), y(y) {}

      void operator() () {
        x = 10;
        y = 20;
      };

      F f(x, y);
      f();
    }
  }  //end of  functor IIILE
  std::cout << "x = " << x << ", y = " << y << std::endl; // x = 1, y =  20
}
```
This ability to capture external values is one of the most powerful features of lambdas in C++.


##### ***3. `std::bind` expressions***

The result of a `std::bind` expression can be passed to `std::function`. 
`std::bind` is a standard function object that acts as Functional adaptor taking an already existing callable as input and returns a new function object with one or more of the arguments of the passed callable bound or rearranged. It takes the callable as a first argument and then the parameters of the callable as other arguments. C++ provides the  `std::placeholders` objects and `std::cref`, `std::ref` functions as utility for the `std::bind`. `std::bind` and the C++ newer version ([`std::bind_front` & `std::bind_back`](https://en.cppreference.com/w/cpp/utility/functional/bind_front)) allows for [*partial function application* and/or *currying*](https://www.modernescpp.com/index.php/partial-function-application) in C++.

***`std::bind` passed as an `std::function` argument:***
```c++
//demonstrating Partial Application
using namespace std::placeholders;  //for _1, _2 etc
double triangle_area(double b, double h) { return 0.5 * b * h; }
double trapezium_area(double a, double b, double h) { return 0.5* h *(a + b);}
double rectangle_area(double l, double b){ return l * b;}

double uniform_prism(double length, double breadth, double depth, const std::function<double(double, double)>& shape_area){
  return depth * shape_area(length, breadth);
}

//Function was made with Rectangles in mind, but we want to use Trapezium and triangle prisms
std::cout << "\nVol of Cubiod: " << uniform_prism(2.2, 4.3, 6.5, rectangle_area);  //Order matches  = std::bind(rectangle_area, _1, _2)
std::cout << "\nVol of Cube: " << uniform_prism(2.2, 4.3, 6.5, std::bind(rectangle_area, _1, _1));  //Fixed one argument, square is equal 
std::cout << "\nVol of Triangle prism: " << uniform_prism(2.2, 4.3, 6.5, std::bind(triangle_area, _2, _1)); //swap dimensions
std::cout << "\nVol of Trapeziodal prism: " << uniform_prism(2.2, 4.3, 6.5, std::bind(trapezium_area, _1, 2.6, _2)); //adapt dimensions, b=2.6
```
Output:
```txt

Vol of Cubiod: 61.49
Vol of Cube: 31.46
Vol of Triangle prism: 30.745
Vol of Trapeziodal prism: 67.08

```
***objects can be bound as the object for the invocation of pointer to member functions:***
```c++
T t;
std::cout << uniform_prism(2.2, 4.3, 6.5 std::bind(&T::triangle_area, t, _2, _1)); //30.745, assuming triangle_area is contained in T


//Using std::bind with STL algorithms
std::vector<double> input_vec = {1,2,3,4,5};
std::transform(input_vec.begin(), input_vec.end(), input_vec.begin(), std::bind((double (*)(double, int))std::pow, _1, 7)); //cast to correct overload
//input_vec = 1, 128, 2187, 16384, 78125

//Using a Lambda
input_vec = {1,2,3,4,5};
std::transform(input_vec.begin(), input_vec.end(), input_vec.begin(), [](double val)->double {return std::pow(val, 7);}); //More readable?
//input_vec = 1, 128, 2187, 16384, 78125

```
The placeholders positions always refer to the arguments passed to the outer function.

***<details><summary><code>Rabbit Hole</code></summary>***
More resources on std::bind
* [C++: Binding a new Function](https://www.youtube.com/watch?v=CQHQmqL5M3M) provides good information on std::bind.
* [C++ Reference](https://en.cppreference.com/w/cpp/utility/functional/bind)
* [Partial Function Application](https://www.modernescpp.com/index.php/partial-function-application)

`std::bind` was introduced before lambdas, drawing inspiration from `boost::bind` (Just like many C++ features). With the introduction of lambdas, they are becoming less and less useful in most cases and you should generally [prefer lambdas for its readability and inlining ability](https://www.reddit.com/r/cpp_questions/comments/92jxn9/need_an_in_depth_explanation_of_why_youd_use/). Lambdas can also be used for partial function application as demonstrated above. 

Here `std::bind` is used in code that might come from a GUI Library. It takes in a member function pointer, calls it with placeholders and passes the generated function object to an `std::function` parameter.

```c++
class EventDispatcher {
public:
    void addHandler(std::function<void (int, float)> handler);
};


class EventSink {
public:

    EventSink(EventDispatcher & Dispatcher) {
        Dispatcher.addHandler(std::bind(&EventSink::handler, this, std::placeholders::_1, std::placeholders::_2));
    }

    void handler(int i, float f) { ... }
};
```
*Remember that member function pointer to need a `this` pointer in order to be called.* You can provide `this` through a reference or pointer to an instantiated object.  Non-static member function have `this` as their first parameter though it is hidden. That's why the above bind expression works.

When using <code>std::ref</code> and <code>std::cref</code> with <code>std::bind</code>, you have to note the difference between <em>bind time</em> and <em>invocation time</em>. Binding occurs during runtime and when a variable is bound to a function object, it is as if a literal of the content of the variable is passed. If the variable changes before the function object is invoked, this change will not be captured. To capture this change, use <code>std::cref()</code> when binding the varible.
</details>


##### ***4. `Function objects`***

Objects of classes having a proper operator() overload can be stored inside a std::function object, as well.
```c++
struct Meow
{
  int y = 0;
  Meow(int y_) : y(y_) {}
  int operator()(int x) { return y * x; }
};
int a = 11;
int b = stdf_foobar(a, Meow{8});
// b = 99 = 11 + ( 8 * 11 )
```

A Class/Struct's data member is technically a callable as for each data member in a class, there exists an implicit "getter-function" even if it is not defined by the programmer. A wrapper like `std::function` (callable object) can be assigned this "getter-function" which is simply the data member name.
```c++
#include <functional>
#include <iostream>
class MyType 
{
  public:
  MyType(int num) : data(num) {}

  int data;
};

int main()
{
  std::function<int(const MyType*)> GetDataPtr = &MyType::data;
  std::function<int(const MyType&)> GetDataRef = &MyType::data;

  MyType t(11);

  std::cout << GetDataPtr(&t) << ""\n;  //Returns 11
  std::cout << GetDataPtr(t) << ""\n;  //Returns 11
}
```


### Using Templated Callback Type
This makes the higher order function to accept more generic types of callbacks, adding extra compile-time polymorphism. You can use it along with `std::function`.
For example, the higher order function can take more types of callbacks and can do different things depending on the callback. This is especially useful to library designers specifying the possible callbacks that can be used in a particular higher order function. 

**Note:** 
Templates are a compile-time feature and are a design tool for compile-time polymorphism. 
If runtime dynamic behaviour is to be achieved through callbacks, templates will help but they won't induce runtime dynamics. 

Below is a code to generalize a callable with 1 parameter and non void return type.
```c++
template<class T>
void stdf_transform(std::vector<T> v, std::function<T(T)> fp)
{
  for ( auto& i : v)
  {
    i = fp(i);  //Callback is used to modify vector
  }
}
```
An even more generic syntax uses a plain template argument that is to be deduced later on:
```c++
template<class F>
void transform_every_int_templ(std::vector<int> v, F f)
{
  for (auto& i : v) { i = f(i); }
}
```

But you might have noticed that this isn't type-safe and descriptive enough. How do you ensure type safety? How else do we define that `f` should be a callable asides naming it "Predicate", "Compare", "Func" etc ?

####  Using `C++ Generic programming`, `concepts` and `std::function`
To provide a safe way to specify callback requirements, you can utilize std::function and concepts. Library authors can define concepts and requirements for callable arguments in Higher-order functions. If you are just concerned with the parameters and the return type, `std::function` might be just enough as we have seen in previous examples. But to enable several types or several callables whose arguments would be deduced within the higher function and processed. You can combine generics with concepts to define some base requirements.


For example:
```c++
//1
template <typename Func>
requires std::invocable<Func&, int>
int higher_order_func(Func fn, int val)
{
    return fn(val);
}

//2 (Analog)
int higher_order_func2(std::invocable<int> auto fn, int val) {
    return fn(val);
}

//Analog
int higher_order_func3(std::function<int(int)> fn, int val) {
  return fn(val);
}
```
Notes:
1. In `(1)` and `(2)` , the unnamed concept defined ensures that `typename Func` is an invocable type and can take in an `int` as an argument (and return an `int` due to the function higher order function return type). C++ rules for Implicit conversion still hold e.g. you can pass in a `double(*)(double)` and it would work.
```c++
double test_fn(double val) {
    return val * 4;
}

int main() {
  higher_order_func(test_fn, 5); //valid
}
```
2. For the `std::function` version, you have to define the full function signature. The same C++ implicit conversion rules apply. 

For callables, `std::function` is generally safer and easier to use, but `concepts` can be powerful tools in specifying requirements and constraints. If you are just concerned about specifying the parameter types (and amount) and the return type, `std::function` may be all you need. `concepts`, on the other hand, can allow you to specify so many more constraints. 
For example, in a Calculator GUI code concepts can be used in order to specify that functions passed into this must be capable of handling `std::complex<T>`. Below defines two callables specifications.
```c++
//Named concept defining integral and floating point numbers
template <typename T>
concept Number = (std::integral<T> || std::floating_point<T>) 
                 && !std::same_as<T, bool>
                 && !std::same_as<T, char>
                 && !std::same_as<T, unsigned char>                 
                 && !std::same_as<T, char8_t>
                 && !std::same_as<T, char16_t>
                 && !std::same_as<T, char32_t>
                 && !std::same_as<T, wchar_t>;

//concept expressed succintly through syntax and semantic constraints
//This defines callables that creates a complex number using 2 numbers
template <typename Func, typename T>
concept ComplexCreateFunc = Number<T> && requires(Func& fn, T d, T d2){
    {fn(d, d2)} -> std::same_as<std::complex<T>>;
};

//Concept defining Unary operators on a complex number,
template <typename Func, typename T>
concept ComplexNumberUnaryOp = Number<T> && requires(Func& fn, std::complex<T> inumber){
    {fn(inumber)} -> std::same_as<std::complex<T>>;   
    //or {std::invoke(fn, inumber)} -> std::same_as<std::complex<T>>;
    // to allow any valid callable.
};


//templated functions
template <typename T>
std::complex<T> complex_calculator(ComplexCreateFunc<T> auto& fn, T arg1, T arg2) {
    return fn(arg1, arg2);
}

template <typename T>
std::complex<T> complex_calculator(ComplexNumberUnaryOp<T> auto& fn, std::complex<T> inumber) {
    return fn(inumber);
}

// templated ComplexCreate callback
template <typename T>
std::complex<T> complex_reactance(const T inductive_react,
                                  const T capacitive_react) {
    return std::complex<T>{0, inductive_react - capacitive_react};
}

//main
int main () {
    std::complex<double> cmplx{4.0,3.9};
    //Below code will not compile because, char is not a "Number". 
    //(Doesn't satisfy its constraint)
    //std::cout << '\n' << complex_calculator(std::polar<char>, 'c', 'c');

    // Lambda to wrap std::polar for matching the concept
    // Note that this wrapper was only needed in MSVC, in GCC and Clang I passed 
    // std::polar<double> directly. Explanation needed!
    auto polar_wrapper = [](auto r, auto theta) -> std::complex<decltype(r)> {
        return std::polar(r, theta);
    };

    //calls valid overloads
    //automatic type deduction
    std::cout << '\n' << complex_calculator(polar_wrapper, 3.4, 3.5);
    std::cout << '\n' << complex_calculator(complex_reactance<double>, 3.4, 5.0)
    std::cout << '\n' << complex_calculator(std::exp<double>, cmplx);
}
```
```txt
(-3.18395,-1.19266)
(0,-1.6)
(-39.6346,-37.5508)
```

I created a demo program that demonstrates specifying concepts with callables. It is very simple C++ code and easy to follow. ➡️ [`canvas_drawer.cpp`](./src/canvas_drawer.cpp) and a summary of most of the code segments discussed is here ➡️ [`callables_demo.cpp`](./src/callables_demo.cpp)

For an in-depth overview of why `concepts` were created, how they work and how to use them in functions and classes check out [Sandor Dargo's Blog](https://www.sandordargo.com/blog/2021/02/10/cpp-concepts-motivations)

## Addendum: Casting Function pointer and Dangerous Callables - Warning !!!
C++ allows the casting of function pointers either using C-style `(Type)` casts. C++ `static_cast<T>` would throw an error if the types are incompatible, so using this offers more type safety. Due to this feature, dangerous casts can occur and in many cases, cause undefined behaviour. [Here](https://www.ibm.com/docs/en/rdfi/9.6.0?topic=program-casting-pointers) is an IBM article describing how function pointers can be cast in their ILE C/C++ compiler and [Emscripten Porting Guidelines - Function Pointer Issues](https://emscripten.org/docs/porting/guidelines/function_pointer_issues.html). This [StackOverflow thread](https://stackoverflow.com/questions/559581/casting-a-function-pointer-to-another-type) talks about the C++ Standard's specification on function pointer casting.

Usually, function pointers can be cast into `void(*)(void)` and back to their original state as a means of type erasure to allow a very dynamic interface. It is recommended to use C++ Metaprogramming and the `std::function` wrapper.

***<details><summary><code>Rabbit Hole</code></summary>***
There is a lot of type erased code in the Vulkan API. For example, <code>VkStructureType</code> is basically an enum of the various possible object types. This value is passed to <code>sType</code> memeber of the Object's CreateInfo struct. The CreateInfo struct is later passed into an Object builder function (<code>vkCreateXXXXXKHR()</code>) that uses the member field's data to create the object. This is as opposed to having long and confusing function signature. These specific type info passed around can then be used to ensure type safety within the function.
</details>
<hr>
<br>

Here is an example of a function pointer cast misuse. This code compiles (with or without warnings) on some major compilers and can lead to program crashes at runtime. Out-of-bound data access, data leaks and junk data.

```c++
int valhalla(int val, int halla)
{
    val += halla; 
    return val;   
}

//Demonstrating function cast
using f_int_t     = int(*)(int);
using f_ints_t_refs = int(*)(int&, int&);

f_int_t bad_cast        =  (f_int_t) & valhalla;       //dangerous cast
f_ints_t_refs evil_cast =  (f_ints_t_refs) & valhalla; //dangerous cast

int hel{10};
std::cout << bad_cast(hel) << '\n' ;           //some random jibberish
std::cout << evil_cast(hel, hel) << '\n' ;     //some random jibberish
```
```text
Output:
-911668460
-2021817400
```
In `bad_cast`, based on the definition of `valhalla`, parameter `halla` is never supplied, the memory address for halla might contain junk which is used for the operation. (Making `halla` a default parameter eg `int valhalla(int val, int halla = 10)`) doesn't solve the issue as default parameters are resolved at compile time and casts are resolved at runtime. A safer way to be able to call with less parameters is to use `std::bind` expressions combined with `std::function`.
e.g. 
```c++
std::function<int(int)> good_cast = std::bind(valhalla, std::placeholders::_1, 10); //pass the default here
std::cout << good_cast(hel);  //20
```
In `evil_cast`, we are accessing the parameters as if by reference meanwhile they are unknown `rvalue` memory and can contain junk which is used for the operation.
To solve this we can use `std::bind` again here:
```c++
std::function<int(int&, int&)> correct_cast = std::bind(valhalla, std::placeholders::_1, std::placeholders::_2);
std::cout << correct_cast(hel, hel) << ", " << hel;   // 20, 10
```
Now at compile time, an lvalue is bound to the callable "correct_cast" using placeholders until it is invoked with `hel`. Note that hel is still not modified as the underlying function receives `hel` by value.


## Conclusion
We explored callables, callbacks and higher-order functions. We also looked at examples of callbacks and their use in everyday C++ code and popular frameworks.
Then we treated the various methods of achieving callbacks, demonstrating the effectiveness of `std::function`, lambdas and functors.
Finally, we checked out a way of enforcing type safety using either concepts or the `std::function` wrapper and some useful ways of avoid function casting errors using `std::bind`.

## Section files
- [`callables_demo.cpp`](./src/callables_demo.cpp) or [`Callables demo on Compiler explorer`](https://godbolt.org/z/9Ks1Ecqrc)
- [`canvas_drawer.cpp`](./src/canvas_drawer.cpp) or [`Canvas Drawer on Compiler explorer`](https://godbolt.org/z/6n4nKPqfv)


## Summary
Items discussed in this section:
1. Formal definitions of Callables and Callbacks.
2. Why do we need Callbacks? (Raison d'etre). Application areas: 
   - Predicates in Algorithms
   - GUI frameworks
   - Multithreading
   - Visit pattern by `std::visit` in `std::variant`
   - Simulating polymorphism and Dynamic dispatch.
3. How to achieve callbacks
     - Function pointers
       - Pointer to member function
     - `std::function` - takes in function pointer,  `lambdas`, `std::bind` expressions, function objects.
     - Templated callback type
     - C++ Generic programming, concepts and `std::function`
4. Addendum: Casting Function pointer and Dangerous Callables - Warning !!!



## Resources and More Reading
1. [`Callback Functions in C++ - Stackoverflow`](https://stackoverflow.com/questions/2298242/callback-functions-in-c) - The remarkable answer given by `Pixelchemist` on stackoverflow
2. [`How do Function pointers in C work - Stackoverflow`](https://stackoverflow.com/questions/840501/how-do-function-pointers-in-c-work)
3. [`Function Pointers Tutorial - cprogramming.com`](https://www.cprogramming.com/tutorial/function-pointers.html)
4. [`C Algorithms - cppreference`](https://en.cppreference.com/w/c/algorithm)
5. [`C++ Algorithms - cppreference`](https://en.cppreference.com/w/cpp/algorithm)
6. [`C++ QSort Algorithm`](https://en.cppreference.com/w/cpp/algorithm/sort)
7. [`Function Pointers- LearnCpp`](https://www.learncpp.com/cpp-tutorial/function-pointers/)
8. [`Lambdas and how they work`](https://irkos.org/cpp/callable/)
9.  [`cpp concepts, 4 ways to use them`](https://www.sandordargo.com/blog/2021/02/17/cpp-concepts-4-ways-to-use-them)
10. [`Sandor Dargo's concept series`](https://www.sandordargo.com/blog/2021/02/10/cpp-concepts-motivations)
11. [`Concept Callables`](https://www.cppstories.com/2021/concepts-callables/)

