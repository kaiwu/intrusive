# Intrusive Containers

Intrusive containers are critical in high performance computing, and their usage needs greater attention
than regular containers, boost has its [intrusive implementations](https://www.boost.org/doc/libs/1_77_0/doc/html/intrusive.html),
and it turns out that [libuv](https://github.com/libuv/libuv/blob/v1.x/src/queue.h) also
has an hidden intrusive circular queue, however their usages are somewhat cryptic.

This demo tries to make libuv's intrusive queue explicit, in light of its design, it provides also
an intrusive binary search tree.
