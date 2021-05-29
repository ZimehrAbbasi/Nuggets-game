# Design spec

## Index
* [User Interface](#User-interface)
* [Inputs and outputs](#Inputs-an-outputs)
* [Functional decomposition into functions/modules](#Functional-decomposition-int-functions/modules)
* [Major data structures](#Major-data-structures)
* [High-level pseudo code](#High-level-pseudo-code)
* [Testing plan](##Testing-plan,-including-unit-tests,-integration-tests,-system-tests)

## User interface

### Server Side
As described in the Requirements Spec, the server's only interface with the user is on the command-line; it must always have two arguments:
* **map:** A text file holding the string representation of the game map
* **[seed]** (_optional_): A seed integer to use when randomly placing gold.
```bash=
$ ./server [map] [seed]
```
#### Example Run
```bash=
./server map1.txt 1337
```

## Inputs and outputs

### Server Side 

#### Input: 
Inputs to the server will conform to the "Player to Server" protocols outlined in the Requirements spec.
Namely, the server recieves:

##### Player Joined Message
```bash=
PLAY [player name]
```

##### Player Key Pressed Message
```bash=
KEY [player name] [pressed key]
```

##### Player Quit Message
```bash=
QUIT [player name] [explanation]
```

##### Spectator Joined Message
```bash=
SPECTATE 
```

#### Output:
* **Server.log:** Server logs useful information outlined above to a log file if specified.


The Server sends to clients the following messages:
##### OK
When a player has successfully been added to the game, the server will return an `OK` message back to that client, with it's corresponding name in the `Letters` set.
```bash=
OK [A-Z]
```

##### Grid
To initialize the client's window, the server sends a message containing the number of rows and columns in the map, in the following form:
```bash=
GRID numRows numCols
```
##### Gold
To communicate that the client has just collected n gold nuggets, its purse  has p gold nuggets, and there remain r gold nuggets to be found.
```bash=
GOLD n p r
```

##### Display
When a new client joins, and at any time to existing clients, the server will send out a string representing the map as known to the client preceded by a `DISPLAY` header
```bash=
DISPLAY\n[map as known to client]
```

##### Quit
To communicate a reason for acquiting a player or spectator, or when all gold has been collected.
```bash=
QUIT explanation
```

##### Error
In response to a malformed message from a client, the server may return an `ERROR` message with an explanation of the malformation.
```bash=
ERROR [explanation]
```

## Functional decomposition into functions/modules
* main: Parses arguments and begins run of the server with given parameters
* parseMap: Reads the map file and stores it as a Grid/Map object in memory
* handleMessages: Receives messages, interprets them and calls the appropriate helper functions.
* send: Sends a message to a specific client
* sendAll: Sends a message to all clients simultaneously
* close: Closes all open files and frees all used memory

#### Helper Modules with useful methods
* Map.h
    * map_new: Create a new map / grid
    * calc_vis: Determines what is and isn't visible to a given player and tells what 
    * player_move: Updates the map string when a player moves and prevents players from making invalid moves


## Major data structures
* Spectator Set:
    * A Set of all spectator ip addresses [set of char*]

* Players Set (A set to hold the 25 possible players)
    * A set that labels each player any letter from A to Z and stores the player struct associated with that letter.

* Player Struct (Individual Player)
    * Player IP Address [String]
    * 2D int[][] representing seen grid items [int[][]]
    * X position of the player [int]
    * Y position of the player [int]
    * Gold collected [int] (When the player picks up gold, take piece from gold struct in map and add amount of gold [int] to player)

* Map struct (Hold state representing entire map and all unclaimed Gold)
    * Holds a string that saves a master state of the grid to send to players (updated when players move and join the game) [char*]
    * Hold a set of Gold structs [set: struct Gold] (When the set of gold structs is empty the game can end)

* Gold Struct (Individual Piece of Gold)
    * X coordinate of gold piece [int]
    * Y coordinate of gold piece [int]
    * Amount of gold [int]


## High-level pseudo code

```text=
(Initialization)
parse the command line, validate parameters, initialize other modules
load map object into memory

(Game loop)
while listening on port
    if new connection
        check message from new connection
        if playername add to player set
            update map with new player
        else add to spectator set
    else check other messages
        if other messages
            parse messages and run helper methods
            if all gold is collected
                game is no longer in session
                break
    send updated map to all players
```

## Testing plan, including unit tests, integration tests, system tests.

### Unit Tests 
* Testing the map module
    * Test write renders of grid to see that visibility is working
    * Test creating map with different seed values and see that gold is place correctly
* Testing the player and spectator module
    * See that movement is consistent and working (no running out of bounds and other similar errors)
    * See that visibility / DISPLAY method works well
* Testing the message relays
    * Make sure that messages aren't malformed (log messages to external file and compare with expected results) 
    * Make sure that messages don't get sent to incorrect clients (Messages meant for a client don't get sent to a spectator)
* Testing the parsing of the messages
    * Make sure correct messages are interpreted appropriately.
    * Make sure malformed messages do not stall the game.
    * Make sure server's outgoing communication is correct and correctly formatted.


### Integration Tests
* Testing if server runs properly
* Testing with incorrect arguments, i.e. invalid number of arguments, string in place of seed, invalid file etc.
* Test run of the game
    * See that game terminates when expected (when all gold is collected)
    * See that client / spectators are all recieving correct messages and are rendering maps correctly

### System Tests
* Testing if there are any memory leaks using valgrind.
    * Test modules separately. Make sure each module's data constructors and processors do not leak memory.
    * Test modules together. Make sure collective functionality does not leak memory.
* Testing for compiler warnings.
    * Run make clean and make all to test for compilation warnings / errors

## Division of Labor
* Zimehr: Grid module, player module, and associated unit tests.
* Alphonso: Driver module (Handle Message, onKey, onSpectate, onPlay, isGameEnded, etc.) and associated unit tests. Will write a bash script and log functions to show that data is passing through the "handleMessage()" and associate callback functions appropriately.
* Amittai: System and integration tests, including automation of memory tests and tests for collective functionality of the separate pieces of the system, handling Makefile dependencies, etc.
> so Amittai is only testing? no coding of the game itself?