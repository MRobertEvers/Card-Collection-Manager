# Card-Collection-Manager
Program to manage cards. Built for Magic The Gathering (MtG) collection management. Build Decks, Track Cards and more.

Features
========
Collections maintain lists of each card you put in it. Adding cards to a collection is simplified by the convenient search function that maintains a 
lightweight database of all possible cards. Card auto-completion enabled fast additions and removals for efficient collection editting. 
Collections can also be assigned as subsets of another. A collection may represent all cards you own. You can then make a deck that is a subset of those cards. Cards that
you don't own may be included as 'virtual' cards or marked a 'wants'.  

Cards can have tags and features assigned to them. Acquisition date, quality, etc.. can be tracked on a per-card basis.  

Anytime a card or collection is changed, that is tracked in the history. This is a nice way to see changes and decisions over-time about deck-building or sales.

Details
=======
Architecture based on Client-Server. Client written in C++ utilizing the wxWidgets library. Server written in C++.  
Server maintains a buffer of all MtG cards for fast searching. Card data is cached on first use for faster data manipulation. Utilizes MtGJSON as its source of card data. 
Collection history automatically tracked for convenience. Card details (such as acquisition date, quality, foil, etc) are maintained for each card.
Collections saved in common, readable format. Additionally, a history file is maintained for historical tracking of a collection. 
Cards are maintained as an identifying string that includes the set and the name. Additional meta-data is included as a list of tags or key-value pairs.


Notes
=====
Client originally implemented in WPF, C#, and C++/CLI. This is now deprecated but is maintained in a separate repository.
