Collection of C++ classes for sound synthesis.
* Process is done sample by sample, not by blocks.
* Most of it is samplerate-agnostic.
* Optimized for simplicity, not for performance.

# REQUIREMENTS
* cmath, time.h (just for seed initialization).
* At least C++17: for "inline static" member variables.


# BASED ON...
* simple effects (filters, delay, oscillators...) are straightforward implementations of the well-known algorithms
* Virtual Analog (VA) filters taken from *The Art of VA Filter Design* by **Vadim Zavalishin**


# NAMING CONVENTIONS:
x: obvious input

y: obvious output

n: integer counting variable

N: integer counting limit

b: impulse bool variable (a "click")

dy: y increment (angular_frequency / TWOPI)

st: semitones

m: MIDI (sometimes)

q: filter quality

r: filter resonance

g: VA filters frequency constant (as in Zavalishin's book)


# DESIGN CHOICES:
* All members are public

* ... but no variable should be modified directly from outside.

* No object relies on implicit constructors (neither on implicit copy constructors, so for every object there is a copy constructor manually defined). All constructors, except copy constructors, are marked "explicit". Copy constructors do copy only variables relevant to internal computation, omitting variables that are defined from scratch on each sample update. Also, they guarantee no unnecessary recalculations, but random initializations will be repeated, so the copy may be not exact.

* For any object that has a "operator()", it updates the state of the object. In general, methods that modify the parameters of the object should be called before "operator()", and all data of the object should be accessed after it. If some method is to be called after "operator()" its name ends in "_post", and if some data is to be read before "operator()" is called, its name ends in "_prev". Accessing data not marked "_prev" before "operator()" has ever been called has undefined behavior.

* If a variable has appended underscore then has both getter and setter defined. Those are methods taking the name of the variable, without underscore, and the setter might be of "_prev" type.

* A variable has getter if and only if it has setter.

* Templates are not used.

* The only used data types are: bool, int, double, Double2 (just double[2] with multiplication and sum overloads).
