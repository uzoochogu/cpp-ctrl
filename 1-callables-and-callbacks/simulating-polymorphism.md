# Simulating Polymorphism and Dynamic Dispatch
Have you ever wondered how large projects (e.g. the Linux kernel, Git etc) were written in C without native Object Oriented Programming (OOP) support?  They use a handful of "pure" techniques like structs data members filled with function pointer callbacks and many others. Objects in OOP are similar to functions with associated data (attributes with a behaviour). They are instances of a class, containing its set of instance data fields and shares methods with all the other instances of the same class.

Structs with function pointers are quite similar to objects with vtables. Multiple structs with the same shared part can be cast to each other. If you have a field telling you the type of the struct in this shared part then you can use this to provide features of inheritance etc.



***<details><summary><code>Rabbit Hole</code></summary>***

## **How to simulate OOP?**
Some ideas on how to simulate different concepts in OOP are discussed below. I fould a [`very useful article`](https://www.state-machine.com/oop) by state-machine that also talks about this.

### 1. **`Abstraction`** 
Hiding away implementation details by giving a higher-level interface to a concept. Structs and functions operating on the structs can be used to abstract concepts away in C.

### 2.  **`Encapsulation and Data Hiding`**
The definition of Encapsulation may refer to two related yet distinct notions. Some authors define encapsulation namely, as the bundling of data with the functions that operate on that data (Main stream definition), while others define it as a language mechanism for restricting direct access to an object’s members (This might more accurately be called Data Hiding).
You can do this by having a header file as a public interface bundling the data and the interfacing functions and then an implementation file that enforces data hiding, especially the private variables necessary to provide the desired functionality. The private variables can be global variables within the implementation file so they can't be accessed by clients of the header file but only by the implemented functions within that implementation file. We can also compile the library to a shared runtime library so that the source code will not be visible.
<br> <br>
To bind an instance to its methods, you can pass a pointer to the specific struct instance containing the public data members for example to update the data fields. [This blog article](https://www.kristijorgji.com/blog/simulating-object-oriented-programming-oop-in-c/) by kristi jorgji explores this concept.
<br>

In a possible implementation, each struct has its file. The public interface (functions operating on the struct and public data members) is in the header `.h` file, while the private data members and methods are in the implementation `.c` file. This can easily work for getters and setters. We keep a pointer within the public struct that points to a private struct defined in the source file. The private struct contains the members you want to hide. A simple example is seen below:
```h
/* vehicle.h */
#ifndef VEHICLE_H
#define VEHICLE_H

typedef struct vehicle vehicle;

enum energy_source {
    petrol_ice = 0,
    diesel_ice = 1,
    human_power = 2,
    battery = 3,
    hydogen_fuel_cell = 4,
};

struct vehicle {
    // public members
    int colour;          /* RGB */
    void* private_data;  // Opaque pointer to hide details
};

/* Public  Interface  */
void vehicle_ctor(vehicle* const v, int colour, int wheels,
                  enum energy_source fuel_type);

void vehicle_dtor(vehicle* v);
int get_wheels(const vehicle* v);
void set_fuel_type(vehicle* v, const enum energy_source fuel_type);
enum energy_source get_fuel_type(const vehicle* v);

#endif /* VEHICLE_H */
```

```c
/* vehicle.c */
#include "vehicle.h"

#include <stdlib.h>

/* Private struct with hidden members */
typedef struct {
    int wheels;
    enum energy_source fuel_type;
} vehicle_private;

/* Implementation */

void vehicle_ctor(vehicle* const v, int colour, int wheels,
                  enum energy_source fuel_type) {
    v->colour = colour;
    v->private_data =
        malloc(sizeof(vehicle_private)); /* Allocate memory for private data */
    /* Initialize data members */
    ((vehicle_private*)v->private_data)->fuel_type = fuel_type;
    ((vehicle_private*)v->private_data)->wheels = wheels;
}

void vehicle_dtor(vehicle* v) {
    free(v->private_data);
    v->private_data = NULL;
}

int get_wheels(const vehicle* v) {
    return ((vehicle_private*)v->private_data)->wheels;
}

void set_fuel_type(vehicle* v, const enum energy_source fuel_type) {
    ((vehicle_private*)v->private_data)->fuel_type = fuel_type;
}

enum energy_source get_fuel_type(const vehicle* v) {
    return ((vehicle_private*)v->private_data)->fuel_type;
}
```

```c
/* client - main.c */
#include <stdio.h>

#include "vehicle.h"

void fuel_type_printer(const enum energy_source source) {
    switch (source) {
        case petrol_ice:
            printf("petrol fuel");
            break;
        case diesel_ice:
            printf("diesel fuel");
            break;
        case human_power:
            printf("human power as energy");
            break;
        case battery:
            printf("battery powered");
            break;
        case hydogen_fuel_cell:
            printf("petrol fuel");
            break;
        default:
            printf("Invalid fuel type!");
    }
}

int main() {
    vehicle car;
    vehicle_ctor(&car, 0xFFFFFF, 4, petrol_ice);
    printf("The wheels are %d in number\n", get_wheels(&car));

    set_fuel_type(&car, battery);
    printf("The fuel type is: ");
    fuel_type_printer(get_fuel_type(&car));

    vehicle_dtor(&car);
}
```
Output:
```txt
The wheels are 4 in number
The fuel type is: battery powered
```
This is similar to the Pimpl idiom - Pointer to Implementation in C++ 
   - Another technique is demonstrated in [this article](https://www.kristijorgji.com/blog/simulating-object-oriented-programming-oop-in-c/). The author stored function pointers to the "class" methods within the struct. Then binds an instance to a method by pasing it as a pointer. This enables calls like below:
```c
// main.c
 
#include <stdio.h>
#include "faker.h"
 
int main()
{
    Faker* faker = newFaker(18);
    printf("Random age %d\n", faker->age(faker));
    printf("Random address: %s\n", faker->address());
    printf("Random email: %s\n", faker->email());
}
```
This is another interesting technique.



### 3. **`Inheritance`** 
Many projects define an elegant way to simulate Inheritance without sacrificing extensibility. Below outlines the basic idea:
    
 - To "inherit", a new struct is created with the first member of the struct being the object to inherit from.
  
Below is a demonstration:
```cpp
//base class
struct vehicle {
   int wheels;
   int fuel_type;
}

//derived class
struct van {
   struct vehicle base;
   int cubic_size;
}
```
 - At the most basic level, you just use plain structs as objects and pass them around by pointers:
```cpp
struct CPU
{
    float frequency;
    int cores;
    int cache;
};

void cpu_simulate(struct CPU *cpu)
{
    /* do a little getter */
}


struct base
{
    /* base class members */
};

struct derived
{
    struct base super;
    /* derived class members */
};

struct derived d;
struct base *base_ptr = (struct base *)&d;  // upcast
struct derived *derived_ptr = (struct derived *)base_ptr;  // downcast
```

### 4. **`Polymorphism`** 
(Subtyping, Subtype polymorphism, or Inclusion polymorphism). To simulate virtual functions and subtype polymorphism, you can use function pointers and optionally function pointer tables, also known as virtual tables or vtables.

```cpp
#include <stdio.h>
// Inheritance test
struct base;  // forward declaration
struct base_vtable {
    int (*getter)(struct base *);            // Sample base getter
    void (*setter)(struct base *, int val);  // Sample base setter
};

struct base {
    struct base_vtable *vtable;

    /* base members */
};

int base_getter(struct base *b) { return b->vtable->getter(b); }

void base_setter(struct base *b, int val) { b->vtable->setter(b, val); }

struct derived1 {
    struct base super;
    /* derived1 members */
};

int derived1_getter(struct derived1 *d) {
    /* implementation of derived1's getter function */
    /* Do something different an return a value */
    puts("Derived 1 Getter was called");
    return 1;  // placeholder
}

void derived1_setter(struct derived1 *d, int val) {
    /* implementation of derived 1's setter function */
    puts("Derived 1 Setter was called");
}

/* global vtable for derived1 */
struct base_vtable derived1_vtable = {
    &derived1_getter, /* you might get a warning here about incompatible pointer
                         types */
    &derived1_setter /* you can ignore it, or perform a cast to get rid of it */
};

void derived1_init(struct derived1 *d) {
    d->super.vtable = &derived1_vtable;
    /* init base members d->super.foo */
    /* init derived1 members d->foo */
}

struct derived2 {
    struct base super;
    /* derived2 members */
};

int derived2_getter(struct derived2 *d) {
    /* implementation of derived2's getter function */
    /* Do something different an return a value */
    puts("Derived 2 Getter was called");
    return 1;  // placeholder
}

void derived2_setter(struct derived2 *d, int val) {
    /* implementation of derived2's setter function */
    puts("Derived 2 Setter was called");
}

struct base_vtable derived2_vtable = {&derived2_getter, &derived2_setter};

void derived2_init(struct derived2 *d) {
    d->super.vtable = &derived2_vtable;
    /* init base members d->super.foo */
    /* init derived1 members d->foo */
}

int main(void) {
    /* Demonstration of Polymorphism in C! */
    struct derived1 d1;
    derived1_init(&d1);

    struct derived2 d2;
    derived2_init(&d2);

    struct base *b1_ptr = (struct base *)&d1;
    struct base *b2_ptr = (struct base *)&d2;

    int val = base_getter(b1_ptr); /* calls derived1_getter */
    val = base_getter(b2_ptr);     /* calls derived2_getter */

    base_setter(b1_ptr, 42); /* calls derived1_setter */
    base_setter(b2_ptr, 42); /* calls derived2_setter */

    return 0;
}
```
```txt
Output
Derived 1 Getter was called
Derived 2 Getter was called
Derived 1 Setter was called
Derived 2 Setter was called
```
You can test the above code using a C compiler (C++ compiler will throw errors and will not compile as this goes against the coding standards)
[Link to Code in Compiler playground](https://godbolt.org/z/f7jorG3z8)
## Explanation
This is a demonstration of a simple way to do polymorphism in C. We utilized function pointers and structs. We created a struct containing function pointers. These function pointers will act as the virtual functions of the base class. I used a getter and setter here to demonstrate typical class member functions. This would act as the virtual table for the base type. The ***base class*** struct now takes in a data member as a pointer to this base vtable struct. The base getter and setter are now defined to act through this pointer by taking in a pointer to the base type which already contains a pointer to the `vtable`, which also has function pointers that can take in a pointer to the base (in the case of the getter) and an extra argument(in the case of the setter).

Then we have the ***derived class*** structs here, defined as having their ***first*** data member as the object of the base struct. This would be useful for ***inheriting*** some of the functionality and enabling a form of safe casting. You can now define the derived getters and setters (called ***Overriding*** in C++). They should take in at least a pointer to the derived class (matching the base getter and setter signature). After this, you define and initialize a "base vtable" object with references to the derived class getter and setter. This variable will act as the vtable of the derived type. A ***derived_init*** function was also defined. This function simply sets the `vtable` data member of the base (which is composed in the derived struct) to the address of this defined ***base vtable*** object. 
In the main function, we declare a derived object, use the derived_init function and then demonstrate polymorphism by casting to the base class and calling the right getter and setter when the base getter or setter is called on a casted value.

Note that this is a rudimentary way of simulating polymorphism. Pointer casts are not very safe but this is quite safe as long as the base class is the first member of the derived class. I encourage you to play around with this construct and make more sophisticated variations. 



## Other notes:
***Multiple inheritance*** is much harder - in that case, to cast between base classes other than the first, you need to manually adjust your pointers based on the proper offsets, which is really tricky and error-prone. (Since we are simulating inheritance by placing the instance of the base structs in the derived struct and we are casting to different address spaces).

Another (tricky) thing you can do is change the dynamic type of an object at runtime! You just reassign it to a new vtable pointer. You can even selectively change some of the virtual functions while keeping others, creating new hybrid types. Just be careful to create a new vtable instead of modifying the global vtable, otherwise, you'll accidentally affect all objects of a given type. Code excerpt obtained from [`Object-orientation in C - Stackoverflow.`](https://stackoverflow.com/questions/415452/object-orientation-in-c)



<hr>

For a C library that takes this to another level, check out [`GObject`](https://docs.gtk.org/gobject/concepts.html) used by Pango, GDK, GTK and many other GNOME projects.


### Useful Resources
* [`Object-orientation in C - Stackoverflow.`](https://stackoverflow.com/questions/415452/object-orientation-in-c)
* [`Object Oriented programming in C`](https://www.state-machine.com/oop)
* [`Simulating OOP in C - Kristi jorg`](https://www.kristijorgji.com/blog/simulating-object-oriented-programming-oop-in-c/)
* [`GObject`](https://docs.gtk.org/gobject/concepts.html)

</details>