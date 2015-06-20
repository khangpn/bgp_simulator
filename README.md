# bgp_simulator
Project work of Protocolo Processing course. We create a network simulation for BGP and clients in ASes communications. In our demonstration, we designed a network topology with 10 nodes (ASes).

## Features:
1. Every ASes can discover the whole network.
2. Whenever any nodes in the network are ON or OFF, the whole network should be notified and update.
3. Simulate client communication using IPv4 and packets are sent by using BGP routing decision.
4. Routing decision can be modified by administrator.
5. Network topology can be designed flexibly by editing config files.

## Compilation:
`g++ -o as_full as.cpp as_bgp_listen.cpp as_bgp_send.cpp client_bgp_listen.cpp client_bgp_send.cpp routing_table.cpp -Wno-write-strings -std=c++11 -lpthread`

## Setup an AS nodes:
1. Copy as_configs.sample, demo_configs.sample and neighbours.csv.sample to as_configs and neighbours.csv.
2. Create sub dir e.g as_1, as_2... each of them is for a node.
3. Copy as_full, as_configs and neighbours.csv to every sub dirs.
4. Edit as_configs with as_port for AS' communications, change client_port for the AS' clients communication.
5. Edit neighbours.csv with as_port and client_port corresponding to ASes directly connected to the AS.
6. routing_table.csv will be generated automatically by the script.
7. Copy demo_configs to a node which you want to use to simulate  client communication. So the node will frequently send a client message to the as_name in the demo_configs

##  Config formats:

### as_configs
Contains info of current node.

`as_name,as_port,client_port`

### neigbours.csv
Contains info of neighbours connected directly to the current node.

`as_name_1,as_port_1,client_port_1

as_name_2,as_port_2,client_port_2`

### routing_table.csv
Contains routing_items which are routing info to any nodes in the network.

`destination,next_hop,path_length,priority,ful_path`

### demo_config
Contains as_name for the current node to send client message to frequently.
as_name

## Start
Go to every nodes and start it up by the command:

`./as_full`

## Simulation
While the simulation is running, we can directly edit routing_tables.csv to change priority value of a path to see the routing decision changed. The node should be restarted (cancel the script and start again) for the changes in routing_table.csv get effected.

Editting demo_configs to change as_name can make the current node to send client message to another node. This does NOT need to be restarted.

## Demonstration
`demo.zip` contains the demonstration in the presentation. It contains 10 nodes. You can open it and start all nodes on to see the demo. 
