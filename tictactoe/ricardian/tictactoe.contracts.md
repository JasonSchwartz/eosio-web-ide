<h1 class="contract"> create </h1>

Stub for create action's ricardian contractrameters

Input parameters:

* `challenger` (name of the party being invited to play tictactoe game)

* `host` (name of the party that is requesting a new came to be played between host and challanger. )


### Intent
INTENT. The intention of the author is to create a record of a new tictactoe game to be played between host (invoker and signer of the contract) and challenger.  The challenger does not neet to confirm the game for it to be signed.  This action does not gurentee that a game will be played, but simply records that a host has challenged a player (challenger) to a game. 

Host is responsible for signing and invoking the contract.  This contract is created without the consent of challenger.

The contract owner, is responsible for paying the RAM fees for the storage of the resulting information in state.

### Term
TERM. This Contract stores the resulting information in state in the game table, which is scoped by the contract owner (tictactoe).


<h1 class="contract"> close </h1>

Stub for close action's ricardian contractrameters

Input parameters:

* `challenger` (name of the party that was invited to play tictactoe game by host.)

* `host` (name of the party that requested a new came to be played between host and challanger.)


### Intent
INTENT. The intention of the author is to terminate (close) a previously created (create) game of tictactoe that was recorded between the host and challenger.  This claus may be invoked place regardless of whether host and challenger actually played a tictactoe game and it's only prerequiste is that a create clause was previously declared between host and challenger.  This clause may be invoked and authorized by either host or challenger.  

This code will erase the record of a game being played between host and challenger from state.

The contract owner, is responsible for paying the RAM fees for the storage of the resulting information in state.

### Term
TERM. This Contract stores the erases information from state that was previously stored in the game table, scoped by the contract owner (tictactoe).