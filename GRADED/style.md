## CS50 Code style and quality

Grade: 13/15

Summary:  Overall pretty good, with some messy spots.
The following feedback is meant to highlight some of my general observations;
look for specific comments with keyword "STYLE" or "GRADER" in your Makefile and source-code files.

* `make` builds all and cleans all; no warnings.  very nice.
* good use of the log module.
* Some areas of code (esp grid) are missing comments.
* visibility code is horrendously long and complex... and repeated in two functions; it should be refactored and 'folded' to bring common cases together, reducing overall code.
* You use malloc when it is not necessary; e.g., to create a message string, it would be sufficient to define one local variable `char message[message_MaxBytes]` and sprintf into that string.  Such a string is (by definition) big enough for any message in the game.  No malloc/free needed.
* Some calls to malloc/calloc are not testing for NULL response.
* Define named constants for all the grid characters (like walls, spots, and gold) rather than sprinkling code with character constants.  A named constant will make the code more readable, and more adaptable if there is a later need to change characters.
* Avoid peeking and poking into the 'struct' of other modules, avoiding the getter/setter API; see how 'grid' uses 'player' for example.

Recall the *[CS50 coding standards](https://www.cs.dartmouth.edu/~cs50/Labs/CodingStyle.html)*.

Recall the [style rubric](https://github.com/cs50spring2021/nuggets-info/blob/main/rubric.md#code-style-and-quality-15), which referred to the following general expectations:

* code should be well-organized with sensible filenames and subdirectories
* brief `README.md` in each directory
* clear Makefile for clean and build
* clear and consistent style
* clear code design, functional decomposition, naming
* clear code logic and flow
* at most one global variable in each program
* good choice of, and use of, data structures
* good modularity (strong cohesion, loose coupling)
* good in-code documentation (comments)
* code is consistent with documentation (IMPLEMENTATION.md)
* defensive programming (error checking, malloc, etc.)
* no compiler warnings
* no memory leaks/errors caused by student code
* no output to stdout other than what is required to play the game
* handles errors gracefully
