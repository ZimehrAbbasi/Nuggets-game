# CS50 Design review
## Team of 3: pine

Speakers: Alphonso, Zimehr, Amittai

* can't change network protocol
* player to server: only `PLAY` and `KEY`
* spectator to server: only `SPECTATE` and `KEY` 
* consider representing grid as array of char
* consider representing what player 'knows' as a grid
* pseudocode
	* there is no set of spectators; only one
	* outer loop is actually just message_loop
* is testing automated or scripted in some way?
* division of work - vague

### Total (27/36)

We grade this section out of 36 points.

#### Presentation (8/12 points)

"In 10 minutes you must present an *overview* of your design decisions and how your group will divide the work; the design document provides more detail. Be prepared to answer questions and receive feedback."

* (4/4) Good overview of the design.
* (3/4) Decomposition and data structures.
* (1/4) Plan for division of the work.

#### Document (19/24 points)

"Your design document (written in Markdown) shall describe the major design decisions, plan for testing, and the roles of each member of your group."

**Server (19):**

* (4/4) User interface
* (3/4) Inputs and outputs
	* can't change the network spec!
* (3/4) Functional decomposition into functions/modules
* (3/4) Major data structures
* (3/4) High-level pseudo code (plain English-like language) for logic/algorithmic flow
	* shoudl have been deeper, another level deeper
* (3/4) Testing plan, including unit tests, integration tests, system tests
	* unclear *how* you will do these tests
