# bgp_simulator
Project work of Protocolo Processing course

## Compilation:
g++ -o as_full as.cpp as_bgp_listen.cpp as_bgp_send.cpp client_bgp_listen.cpp client_bgp_send.cpp routing_table.cpp -Wno-write-strings -std=c++11 -lpthread

## Sample code:
1. Copy as_configs.sample and as_links.csv.sample to as_configs and as_links.csv
2. Create sub dir e.g as_1, as_2
3. Copy as_full and sample files to every sub dirs.
4. Change as_configs with port corresponding to an AS
5. Change as_links.csv with ports corresponding to ASes connected to the AS
