----------------------
Command Line Arguments
----------------------

---------------------
1) Dining Philosopher
---------------------
./dining [Argument 1]
* Argument 1 must be an integer greater than 0, or less than or equal to 15 

Example - ./dining 15

This is an implemention of the classic "Dining Philosophers" problem where 
each philosopher can eat from either sides respective of their placement 
in the table. 

---------------------
2) Drinking Philosopher
---------------------
./drinking [Argument 1]
* Argument 1 must be a file with n by n matrix entries

Example - ./drinking 5phil

This is a twist to the classic "Dining Philosophers" problem. Instead of 
having a circular queue (table) of philosophers eating, this problem entails
a 2D matrix where each philosopher can grab a bottle adjacent to them to drink.
This required creating a lock on each bottle and handling concurrency everytime
a philospher grabs a bottle.

----------------------
Environment
----------------------
Ubuntu 12.04 LTS

