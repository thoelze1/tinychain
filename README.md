# A Tiny Blockchain
My work for BigRed//Hacks 2017.

## Goal
I want a distributed database that sets a reasonable set of rules for adding to the database.

## Deploy Notes
If I had access to my remote server, I would be serving IPs (and ports) instead of just ports. Because I am doing everything locally, IP is irrelevant.

## Problems
I couldn't use socket bidirectionally because the client read while loop ran infinitely (this is what I think - eliminating the loop fixed the hang issue).
