# Implementation Spec

This spec details the modules, functions, and libraries we plan to use in our Nuggets game.

## Data structures (e.g., struct names and members)
We used the following Data Structures to represent the state of out game and the objects within it.

* Grid struct to keep track of grid (`grid_t`)
    * `rows`. Integer representing number of rows in the grid (`int`)
    * `cols`. Integer representing number of columns in the grid (`int`)
    * `grid`. 2D array of char[`numRows`][`numColumns`] holding data from map and randomly assigned gold piles.

* Player Struct that holds player's message addresses and other heuristic info (`player_t`)
    * `realName`. Name representing player shown at the end of the game (`char*`)
    * `playerGold`. Hold amount of gold collected by player (`int`)
    * `letter`. Holds the leter (avatar) associated with the player on the grid (`char`).
    * `playerX`. Hold player's x location (`int`)
    * `playerY`. Hold player's y location (`int`)
    * `playerGrid`. Hold a grid representing the map as known to player (`grid_t`)
    * `playerAddress`. Player address (`addr_t`)

* Gold struct to keep track of gold (amounts)
    * `totalGold` holds amnt of gold in each gold pile `int[numGoldPiles]`
    * `numPiles` holds total number of gold piles `int`
    * `remainingGold` holds the amount of gold left on the map `int`
    * `index` holds index into gold pile array `int` (Game terminates when this index is equal to totalNumerOfGoldPiles - 1 as at that point all gold will have been collected)

* Game state struct that contains all aspects of the game (grid, gold, players, etc.). (`gameState_t`)
    * `spectator`: Address of the spectator watching the game. Is initially a no_addr(), but is changed to the address of whatever new client connects as a spectator. (`addr_t`)
    * `allPlayers` is a player array (size 25) that holds pointers to all players in the game (`player_t[]`)
    * `gameGold` is a gold struct that holds information about the gold in the game. (`gold_t`)
    * `masterGrid`. Master grid (with all players and gold pieces visible). This will be what is shown to the SPECTATOR. (`grid_t`)
    * `numRows`: an integer to hold the number of rows on the map (`int`)
    * `numCols`: an integer to hold the number of columns on the map (`int`)

### Global variables
We used a global statew vairable to keep track of changing game state (this GameState object is passed to the message_loop function).

* **gameState** (`gameState_t`): A struct that contains all our game state information (See description further down). Is passed to handle_message() callback.

## Modules and associated function prototypes
The following is a list of the modules we plan to break out nuggets game into. 
### File module (libcs50)
We are using the file_readLines() and file_readLine() functions from the file.h in the libcs0 module.
<br/>

### Player module
The player module contains all functions and structs associated with `player_t` objects in the game.

### Function Protypes
```c=
addr_t player_getAddr(player_t* player)
int player_getX(player_t* player)
int player_getY(player_t* player)
grid_t* player_getGrid(player_t* player)
char* player_getRealName(player_t* player)
```

#### addr_t player_getAddr(player_t* player)
Returns the addr_t associated with a given player. Is used to compare players.

```c=
return player.playerAddress;
```

#### int player_getX(player_t* player)
Returns a given player's x coordinate on the grid.
```c=
return player.playerX;
```

#### int player_getY(player_t* player)
Returns a given player's y coordinate on the grid.
```c=
return player.playerY;
```

#### grid_t* player_getGrid(player_t* player)
Returns the grid as known to the player.
```c=
return player.playerGrid;
```

#### char* player_getRealName(player_t* player)
Returns the realName associated with a given player.
```c=
return player.realName;
```
<br/>

### Main / Driver module

### Module Variables
* **seed**: The seed number used for randomly placing gold piles onto the grid.
* **MaxNameLength** = 50: max number of chars in playerName
* **MaxPlayers** = 26: maximum number of players
* **NumPlayers** = 0: tells the current number of players in the game
* **GoldTotal** = 250: amount of gold in the game
* **GoldMinNumPiles** = 10: minimum number of gold piles
* **GoldMaxNumPiles** = 30: maximum number of gold piles

### Function Protypes
```c=
main(int argc, char* argv[])
void parseArgs(int argc, char* argv[], int* seedNum)
gameState_t* game_init(FILE* mapFile)
bool handleMessage(void* arg, const addr_t from, const char* message)
void onKey(char* tokens[], addr_t* from_address)
void onSpectate(addr_t* from_address)
void onPlay(addr_t* from_address)
char* convertMasterGridToString()
char* convertPlayerGridToString(player_t* player)
bool isGameEnded(void)
char* formattedLeaderboard(void)
int countRemainingGold(void)
void sendGoldToSpectaor(addr_t* spectator)
player_t* findPlayerByKey(addr_t address)
void gold_init(gold_t* gold, int numPiles)
void gold_distribute(grid_t* Grid, gold_t* Gold)
bool checkPointContainsPlayer(int x, int y)
void getPlayerSpawnPos(grid_t* grid, int* x, int* y)
```

#### main(int argc, char* argv[])
Begins execution of the server. From here, we load the maps into the master grid, initialize the server via the message module, and setup our message loop callback functions. On termination of the message_loop() function, the message_done() function is called and the server program exits with code 1.
```c=
int seedNum =  0;
parse the arguments
Seed the random number generator
open mapFile from argv[1]
initialize gameStat
initialize message module and get port
if port is 0
    exit(1)
start message loop with gameState as the arg and handleMessage
end message loop
```

#### void parseArgs(int argc, char* argv[], int* seedNum)
Parses arguments. Makes sure that given map file is valid and saves seed value to a given address.
```c=
exit(1) if argc != 3:
if can convert argv[2] into an integer:
    set *seedNum to integer value of argv[2]
else
    print error message about giving a valid seed number and quit

if can open file at argv[1]:
    close file
    return
else
    print error message about being unable to open file
    exit(1)
```

#### gameState_t* game_init(FILE* mapFile)
Initializes the gamestate struct for later use
```c=
if mapFile is NULL
    return NULL
else
    Allocate space for the gamestate struct
    Allocate space for the gameState.allPlayers array
    Allocate space for the gameState.gameGold struct
    set gameState.numRows
    set gameState.numCols
    Initialize gameGrid.masterGrid
    Set numPiles to a random number between 10 and 30
    Initialize gold
    distribute gold
```


#### bool handleMessage(void* arg, const addr_t from, const char* message)
Initial message handler. Called every time the server recieves a new message. Dispatches new messages to clients and updates game state accordingly.

Handle Message is responsible for:
* On PLAY Message
    * If new player add to players array with address and new avatar and gold starting at 0
    * Respond with OK and the letter associated with the player (index into allPlayers converted to a-z via ascii addition)
    * If index into allPlayers > 26, return error
* On SPECTATE message
    * Replace what's in current spectator obejct with new address
    * Make spectator struct with Addr_t
* On KEY message
    * Move players if relevant and update gameState object
* On INVALID message
    * Return a ERROR message to the client

**Pseudocode**
```c=
    Split message into tokens
        in the case of a "PLAY" message
            use onPlay(from_address)
                
        in the case of a "SPECTATE" message
            use onSpectate(from_address)
        
        in the case of a "KEY" message
            onKey(tokens, from_address)
            
        if tokens[0] is none of these keys
            send "ERROR malformed message" to from_address
    
    // After handling a message, check if game is at a terminal state and if so return true to end message_loop
    if (isGameEnded())
        // Send quit messages to players
        send "QUIT GAME OVER" + formattedLeaderboard() to all players
        // Send quit messages to spectators
        sent "QUIT GAME OVER" + formattedLeaderBoard() to spectator
        
        // Return true and end message loop
        return true;
        
    // Otherwise, display the grid to each player and update the gold amnts
    calculate visibility for all players and update their internal grid
        
        // Convert player grid to string
        playerGridAsString = covertPlayerGridToString(player);
        
        // Send DISPLAY \n playerGridAsString
        message_send(from_address, playerGridAsString);
        
        // Send gold data to player
        sendGoldToPlayer(player)
    
    // Update spectator
    masterGridAsString = convertMasterGridToString()
    message_send(gameState.spectator, masterGridAsString)
    sendGoldToSpectator(gameState.spectator)
    
    
```
#### void onKey(char* tokens[], addr_t* from_address)
onKey is a callback that handles the KEY message from the client.
```c=
// Get player associated with key
player_t currentPlayer = findPlayerByKey(from_address);

// Move player in gameState object
char k = tokens[1]
if grid_canMove(gameState.masterGrid, current_player, k):
    grid_movePlayer(gameState, current_player, x, y)
```

#### void onSpectate(addr_t* from_address)
onSpectate is a callback that handles when the SPECTATE message is sent to the server. Kicks out previous spectator (initial spectator is message_noAddr) with a QUIT message and replaces them with a new spectator address.
```c=
message_send(gameState.spectator, "QUIT a new spectator has entered")
gameState.spectator = from_address
```

#### void onPlay(addr_t* from_address)
OnPlay is a callback that handles the case when a PLAY message is sent to the server. Creates a new player instance and handles errors as neccesary.
```c=
if NumPlayers < MaxPlayers - 1
    Create new player obejct
    Create / Allocate empty player grid 
    Fill grid with empty spaces
    set player_grid to new grid
    Choose a random (non taken) x and y location for player
    Use getPlayerSpawnPos() to set the initial x and y coordinates of the playe
    Add New Player to array
    Send data to new player
    
    Increment numPlayers
else
    send QUIT "Cannot join game" to from_address
```

#### char* convertMasterGridToString()
Converts the player grid into a sendable string and adds player locations to the string
```c=
Create new temp grid

Loop over every coordinate in the grid
        Loop over every player in the game
            If there is a player at this (visible) point, replace # or . with letter
            Otherwise (else) just put what's in the normal grid

Convert newly created tempGrid to a string
```


#### char* convertPlayerGridToString(player_t* player)
Converts the player grid into a sendable string and adds player locations to the string
```c=

playerGrid = player.playerGrid
Loop over every coordinate in the grid.
        Loop over every character in the game
            If there is a player at this (visible) point, replace # or . with letter
            Otherwise just put what's in the normal grid

Convert newly created tempGrid to a string
```

#### bool isGameEnded()
Returns a boolean stating whether or not the game is over (whether all gold has been collected)
```c=
int currentGoldIndex = gameState.gameGold.index
int numGoldPiles = gameState.gameGold.numPiles

if currentGoldIndex > numGoldPiles - 1:
    return true
else
    return false
```

#### char* formattedLeaderboard()
Returns a formatted leaderboard of all players and the amount of gold they collected.
```c=
create a string finalMessage
for player in gameState.allPlayers
    finalMessage += player.character + " " + player.playerGold + " " + player.realName + "\n"

return finalMessage
```

#### int countRemainingGold()
Uses the the gameState's `gold` field to calculate the remaining amount of gold
```c=
return remainingGold
```

#### void sendGoldToPlayer(int amount_collected, player_t* player)
Sends the GOLD message to the specified player.
```c=
remainingGold = calcRemainingGold();
message_send(player.playerAddress, "GOLD " + amount_collected + " " + player.playerGold + " " + remainingGold)
```

#### void sendGoldToSpectaor(addr_t* spectator)
Sends the GOLD message to the spectator
```c=
message_send(spectator, "GOLD " + 0 + " " + 0 + " " + countRemainingGold)
```


#### player_t* findPlayerByKey(addr_t address)
Returns the player associated with a given addr_t object
```c=
for player in gameState.allPlayers
    if player.playerAddress == address
        return player

// Return NULL if no player has given address
return NULL
```

#### void gold_init(gold_t* gold, int numPiles)
Initializes the gold struct with the total number of gold piles and allocates a random amount of gold into each pile [a "pile" is an index into the gameState's gameGold array].
```c=
set space for goldTotal array
for(int i = 0; i < numPiles; i++)
    goldTotal[i] = rand(1, GoldTotal - (numPiles - i - 1))
```

#### void gold_distribute(grid_t* Grid, gold_t* Gold)
Seeds the map with gold piles, distributing them randomly according to the number and value of gold given by the seed number.
```c
grid_t grid = gameState.gameGrid
i = 0;
int x, y;

while(i < numPiles)
    x = rand(1, Grid.numCols)
    y = rand(1, Grid.numRows)
    If coordinate is a dot, place gold there. Ignore otherwise
    increment i

```

#### bool checkPointContainsPlayer(int x, int y)
Returns whether there is a player at the given coordinate
```c=
for player in gameState.allPlayers:
    if player.player_x == x && player.player_y == y:
        return true

return false
```

#### void getPlayerSpawnPos(grid_t* grid, int* x, int* y)
Puts the x and y coordinates of a random (untaken) dot position, into the int pointers.
```c=
bool isLooking = true;
while isLooking
    int curr_x = random(numRows, numCols);
    int curr_y = random(numRows, numCols);

    Check if grid point is a dot on master Grid
        Check if Grid point is already taken by another player
            *x = curr_x
            *y = curr_y
            set isLooking to false;
```

## Grid Module

### Function Protypes
```c=
grid_t* grid_init(FILE* mapFile, int numRows, int numCols)
grid_toString(grid_t*)
bool grid_isWall(grid_t* master, int x, int y)
bool grid_isPlayer(game_t* gameState, grid_t* master, int x, int y)
bool grid_isGold(grid_t* master, int x, int y)
bool grid_isDot(grid_t* master, int x, int y)
bool grid_isPassage(grid_t* master, int x, int y)
bool grid_canMove(grid_t* master, player_t* player, char k)
void grid_movePlayer(game_state_t* gameState, player_t* player, int x, int y)
void grid_calcVisibility(game_state_t* gameState, player_t* player)
```

#### grid_t* grid_init(FILE* mapFile, int numRows, int numCols)
Given a function pointer to a map file and a game state, loads a map into the grid.
```c=

allocate space for grid struct
allocate space for each element in grid
int x = 0;
while readline from file
    for char in strlen(line)
        set grid[x][char] to line[char]
    x += 1;
    
return grid struct
```

#### grid_toString(`grid_t*`)
Converts a given grid object to a sendable (multi-line) string for sending over the network
```c=
allocate memory for string rep. of grid
for row in grid
    concatenate row to string with '\n' at the end.
```


#### bool grid_isWall(grid_t* master, int x, int y)
Returns whether the given coordinate is a wall or not.
```c=
if point in master grid is '|' or '-' or '+' or ' ':
    return true;
else
    return false;
```

#### bool grid_isPlayer(game_t* gameState, grid_t* master, int x, int y)
```c=
// Returns whether the given space is taken by a player.
for each player in gameState.allPlayers
    if player x matches and player y matches and position in master is '.':
        return true
return false
```

#### bool grid_isGold(grid_t* master, int x, int y)
Returns whether the given coordinate contains a gold piece or not
```c=
if master[y][x] is "*"
    return true
else
    return false
```

#### bool grid_isDot(grid_t* master, int x, int y)
Returns whether the given coordinate contains a dot or not
```c=
if master[y][x] is "."
    return true
else
    return false
```

#### bool grid_isPassage(grid_t* master, int x, int y)
Returns whether the point in the in the map is a passage.
```c=
if master[y][x] is '#'
    return true
else
    return false
```

#### bool grid_canMove(grid_t* master, player_t* player, char k)
Returns whether the player can move to a certain point
```c=
char letter = ...;
switch (letter) {
case 'A': if grid_isWall() returns true then return false and vice versa; break;
case 'B': ... code for letter 'B'; break;
case 'C': ... code for letter 'C'; break;
default:  ... code for letter not matching any case above.
}
```

#### void grid_movePlayer(game_state_t* gameState, player_t* player, int x, int y)
Moves the player to a specified position, updating the amount of gold in the player's purse / in the game if neccesary. Also updates master map with new player locations. (Assumes that grid_canMove was called prior and returned true. i.e. We know that we are moving to a valid point)
```c=
set master to gameState.grid
set gameGold to gameState.gold

if grid_isGold(x, y)
    set player[x] to x
    set player[y] to y
    
    set gameGold to gold_t from gameState
    set player.gold to gameGold[gameGold.index]
    subtract gameGold[gameGold.index] from gameGold.total
    increment gameGold.index
    
    get grid from gameState
    update grid[x][y] to '.'
    
    
else if grid_isPlayer(master, x, y)
    for otherPlayer in players in gameState
        if otherPlayer[x] == x and otherPlayer[y] == y
            set otherPlayer[x] to player[x]
            set otherPlayer[y] to player[y]
            get grid from gameState
            update grid[player[x], player[y]] to hold otherPlayer
    
    set player[x] to x
    set player[y] to y
    get grid from gameState
    update grid[x, y] to hold player

else
    get grid from gameState
    update grid[player[x], player[y]] to be empty
    update grid[x, y] to hold player
    set player[x] to x
    set player[y] to y
```

#### void grid_calcVisibility(game_state_t* gameState, player_t* player)

This function calculates the visibility of the player using the algorithm discussed in the design spec.

```c=
get master_grid
get player_grid
initiate slope, x_pred, y_pred, lower, upper, visibility
for x in cols
    for y in rows
        calculate slope
        set visibility to true
        if difference in y coordinates is greater than difference in x coordinates
            for y1 = min(y, player.y) to max(y, player.y)
                x_pred = corresponding x of y1 on the line
                set upper to ceil(x_pred);
                set lower to floor(x_pred);

                if master_grid[y1][upper] or master_grid[y1][lower] is a wall or passage
                    set visibility to false;
                    break;     
        else
            for x1 = min(x, player.x) to max(x, player.x)
                x_pred = corresponding y of x1 on the line
                set upper to ceil(y_pred);
                set lower to floor(y_pred);

                if master_grid[upper][x1] or master_grid[lower][x1] is a wall or passage
                    set visibility to false;
                    break;      
                
        if visibility is true
            player_grid[y][x] = master_grid[y][x];
        else
            if master_grid[y][x] is gold
                set player_grid[y][x] to a room space
        
```
<br/>

## Error handling and recovery
* The only user input into the server are the command line arguments passed at the beginning of execution. When command line arguments are parsed (in `parseArgs()`), we terminate the server with exit code 1 and display a message about why the arguments parsed are invalid (perhaps an invalid map file or  random seed was given).
* Malformed messages from clients are handled in the handle_message callback and its associated subroutines. When a malformed message is sent to the server, and `ERROR` message is returned to the sending client.

## Persistent storage (files, database, etc)
* We use the text files in the `maps` directory to store the maps that our server is able to run on. 
* We also make use ofthe `log` module to store the messages relayed to and from the server in a `server.out` file.


## Testing plan

Here is an implementation-specific testing plan.

### Unit Testing

Our implementation consists of four main points of interest:
* [grid](grid.c), offering a representation of and convenient interface with the `grid`, our in-application representation of the map.
* [player](server.c), offering a repsesentation of and interface with the clients connected to the server, masking details such as their network address, their position, their grid, and their general state of the universe in the game.
* [gold](server.c), a handy but simple representation of gold piles in the map that enables tracking the much gold that has been collected, and the amount remaining. 
* [gameState](server.c), a wrapper for all the different variables created and tracked in an instance of a game -- for instance, an array of all `players`, details about the `gold` left in the `grid`, the master `grid`, and the number of rows and columns.

Since everything is closely coupled, the correct functionality of the whole is dependent on the correct functionality of each component module. To this end:

1. We write a test program `gridtest.c` that interacts with the [grid](grid.c), loading a sample grid and printing it out its raw representation (as separate strings), testing random updates to the grid, and checking whether the utility functions for checking whether a point on the grid is a wall, pathway, player, and for calculating visibility all function correctly.
2. We write a conditionally-compiled test routine in `server.c` that test random updates to the game, e.g. spawning random `players`, assigning `gold` to players and printing the state of the game afterwards,  and checking that actions done in the game are reflected in the state of the game. This will help ensure that the `player`, `gold`, and `gameState` modules function as intended.

### Regression Testing

1. The server's collective functionality depends on multiple modules. For routine regression tests, we test the system on simple maps such as [small](../maps/small.txt) and [fewspots](../maps/fewspots.txt). This will enable us to quickly check for erratic functionality from the latest code updates without detracting from development work.
2. We also include reconstruction strategies in our Makefile such that building a target automatically reconstructs all its dependencies using `make -C "otherdirectory"`. This will help us to promptly detect if a change in one module breaks functionality in another module.

### Integration Tests
1. We use the chat [miniclient](../support/miniclient.c) to send controlled messages to the server and print out feedback from the server. This is an important step to making sure our server parses and interprets messages correctly, and responds as intended. We also intentionally test malformed or misleading messages (such as trying to run into a wall, manually set a player's position to dead space, etc.) to make sure the server handles all edge cases.
2. We write a Shell script, `runbots.sh`, which launches the `server`, the `botmode` driver, and adds a specified number of players and a spectator to watch the game. This will enable us to visualize different game scenarios and iron out any integration issues in our system.

### System Tests
1. Testing if there are any memory leaks using valgrind.
    * We write a Shell script, `gridtest_memcheck.sh`, which invokes our `grid` unit test and tracks it for memory leaks. This will help detect and fix any memory leaks in our `grid` module.
    * We write a Shell script, `server_memcheck.sh`, which invokes the server and tracks it for memory leaks as different players and spectators join and leave the game.
2. Testing for compiler warnings.
    * We write a Shell script, `test_make.sh`, which cleans all existing object files and executables, runs `make all`, displaying any compiler warnings and errors, then automatically cleans up again.


## Division of Labor
* Zimehr: Grid module, player module, and associated unit tests.
* Alphonso: Driver module (Handle Message, onKey, onSpectate, onPlay, isGameEnded, etc.) and associated unit tests. Will write a bash script and log functions to show that data is passing through the "handleMessage()" and associate callback functions appropriately.
* Amittai: System and integration tests, including automation of memory tests and tests for collective functionality of the separate pieces of the system, handling Makefile dependencies, etc.
