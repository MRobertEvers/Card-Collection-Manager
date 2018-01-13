# Card-Collection-Manager
Program to manage cards. Built for Magic The Gathering (MtG) collection management. Build Decks, Track Cards and more.

Features
========
TODO: Note features.

Details
=======
Architecture based on Client-Server. Client written in C++ utilizing the wxWidgets library. Server written in C++.  
Server maintains a buffer of all MtG cards for fast searching. Card data is cached on first use for faster data manipulation. Utilizes MtGJSON as its source of card data. 
Collection history automatically tracked for convenience. Card details (such as acquisition date, quality, foil, etc) are maintained for each card.
Collections saved in common, readable format.  
TODO: More details.

Notes
=====
Client originally implemented in WPF, C#, and C++/CLI. This is now deprecated but is maintained in a separate repository.
