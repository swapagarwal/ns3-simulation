# Effect of buffer size on TCP and UDP flows


### Topology

       H1 ---+      +--- H4
             |      |
       H2 ---R1 -- R2--- H5
             |      |
       H3 ---+      +--- H6

A Dumbbell topology with two routers R1 and R2 connected by a (10 Mbps,100 ms) link.

Each of the routers is connected to 3 hosts
i.e., H1, H2 and H3 are connected to R1,
and H4, H5 and H6 are connected to R2.
All the hosts are attached to the routers with (100 Mbps, 10ms) links.

Both the routers use drop-tail queues with a equal queue size set according to bandwidth-delay product.

Packet size is 1.5KB.

### Simulation

Initially, 4 TCP New Reno flows are started.

After a while, 2 CBR over UDP flows (each with 20 Mbps) are started.

Rate of one UDP flow is increased upto 100 Mbps which impacts the throughput of the TCP flows and the other UDP flow.

The buffer size is varied in the range of 10 packets to 800 packets and the above is repeated to find out the impact of buffer size on the fair share of bandwidth.
