# A Tiny Blockchain
My work for BigRed//Hacks 2017.

## Thoughts
The graph should maintain 2-regularity (in the spirit of "tiny"). Each node should store a list of other active nodes. If it loses connection with one of its neighbors, it should attempt to connect with another neighbor. Two-regularity is enough to propogate all information through the networkf. Evidently, this information should include IPs of active nodes, blocks (obviously), and, perhaps, pending transactions. I still have to devise a system for collecting pending transactions, a system for chunking the collected pending teransactions into blocks, and a system for verifying the pending blocks to add them to the chain.
- Each node can generate information by sending it to each of its ~2 neighbors, and each node can propogate received information by sending it to its ~1 ignorant neighbor (if the neighbor is not ignorant, then it has already propogated the information)
- Best to develop a protocol robust to the general graph, not the tinychain network

## To Do
A node shouldn't announce its presence in the peer-to-peer network until it is successfully listening for transactions. Right now, a node announces its presence immediately after discovering the peer-to-peer network.

## Goal
I want a distributed database that sets a reasonable set of rules for adding to the database.

## Deploy Notes
If I had access to my remote server, I would be serving IPs (and ports) instead of just ports. Because I am doing everything locally, IP is irrelevant.

## Problems
I couldn't use socket bidirectionally because the client read while loop ran infinitely (this is what I think - eliminating the loop fixed the hang issue).

## Demo
I made a [list](http://bingweb.binghamton.edu/~thoelze1/nodes) of active nodes to show how discovery might work (if I had had access to a remote machine). I spent a lot of time trying to download this list in standard C, to no avail. I read about [http requests](http://bingweb.binghamton.edu/~thoelze1/nodes), and then looked at implementations [here](https://stackoverflow.com/questions/11208299/http-get-request-using-c-without-libcurl) and [here](https://gist.github.com/nolim1t/126991).

# Debugging Resources
I also used [this post](https://stackoverflow.com/questions/2773396/whats-the-content-length-field-in-http-header), [this post](https://stackoverflow.com/questions/5681206/tcp-nodelay-not-found-on-ubuntu) while trying to figure out how to send an http get request. [This post](http://www.thegeekstuff.com/2011/12/c-socket-programming/?utm_source=feedburner) helped me with the basics of sockets, and [this tutorial](http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html) was very useful, while [this question](http://www.cplusplus.com/forum/unices/115321/) and [this question](https://stackoverflow.com/questions/26297068/c-socket-connection-refused-at-the-second-try) helped me identify problems (I think?).

## Starting Points
- [Blockchain for currency in python](http://ecomunsing.com/build-your-own-blockchain)
- [How do Bitcoin clients find eachother?](https://bitcoin.stackexchange.com/questions/3536/how-do-bitcoin-clients-find-each-other)
- [How do new Bitcoin nodes know the IPs of existing nodes?](https://www.quora.com/In-a-blockchain-network-how-do-new-nodes-know-the-IP-addresses-of-other-nodes-to-connect-to)
