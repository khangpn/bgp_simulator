# bgp_simulator
Project work of Protocolo Processing course

## Compilation:
g++ -o as_full as.cpp as_bgp_listen.cpp as_bgp_send.cpp client_bgp_listen.cpp client_bgp_send.cpp routing_table.cpp -Wno-write-strings -std=c++11 -lpthread

## Sample code:
1. Copy as_configs.sample and as_links.csv.sample to as_configs and as_links.csv.
2. Create sub dir e.g as_1, as_2
3. Copy as_full and sample files to every sub dirs.
4. Change as_configs with port corresponding to an AS
5. Change client_configs with port for the AS' clients communication.
6. Change neighbours.csv with ports corresponding to ASes directly connected to the AS

##  Config formats:

### as_configs
as_name,as_port,client_port

### neigbours.csv
as_name_1,as_port_1,client_port_1
as_name_2,as_port_2,client_port_2

### routing_table.csv
destination,next_hop,path_length,priority,ful_path
