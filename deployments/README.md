#

```mermaid
graph TD;
  %% client([client])-. Ingress-managed <br> load balancer .->ingress[Ingress];
  %% ingress-->|routing rule|service[Service];

  subgraph namespace
  %% ingress;
  %% gps-->|tcp:4245|fortytwo[fortytwo]
  %% time-->message_bus

  rw0-->|rx:4278|fortytwo
  fortytwo[fortytwo]-->|tx:4277|rw0
  %% rw0-->|uart:2|uart-bus

  rw1-->|rx:4378|fortytwo
  fortytwo[fortytwo]-->|tx:4377|rw1
  %% rw1-->|uart:3|uart-bus

  rw2-->|rx:4478|fortytwo
  fortytwo[fortytwo]-->|tx:4477|rw2
  %% rw2-->|uart:4|uart-bus

  torquer-->|rx:4279|fortytwo[fortytwo]
  thruster-->|rx:4280|fortytwo[fortytwo]
  %% thruster-->|uart:29|uart-bus

  fortytwo[fortytwo]-->|tx:4245|gps
  gps-sim-->|s|gps-app-->cfs
  css-sim-->|s|css-app-->cfs
  gps-->|uart:1|uart-bus

  subgraph gps
    gps-sim
  end

  subgraph cfs
    gps-app;
  end

  fortytwo[fortytwo]-->|tx:4277|css
  %% css-->|64|i2c:2
  
  fortytwo[fortytwo]-->|tx:4279|mag

  fortytwo[fortytwo]-->|tx:9999|truth42sim
  truth42sim-->|tx:5111|yamcs

  fortytwo[fortytwo]-->|tx:4284|fss

  fortytwo[fortytwo]-->|tx:4281|imu

  fortytwo[fortytwo]-->|tx:4282|star-tracker

  fortytwo[fortytwo]-->|tx:4283|eps

  radio-->|ci:5010|nos_fsw
  radio-->|to:5011|nos_fsw
  radio-->|radio:5015|nos_fsw

  yamcs-->|tc:6010|cryptolib-->radio

  nos_fsw-->|command:5014|radio

  radio-->|tm:6011|yamcs
  yamcs-->|tc:8010|radio

  %% camsim-->|60|i2c:2
  %% camsim-->|0|spi

  nos_engine_server-->|tcp:12001|time-->gps
  stdio-terminal-->|udp:5555|x
  udp-terminal-->|udp:5556|x
  %% sample-sim-->|uart:16|uart-bus

  %% actuators
  %% sensors

  subgraph time_sync
    time

end


  subgraph reaction_wheels
    rw0;
    rw1;
    rw2;
  end

  subgraph actuators
    reaction_wheels
    thruster;
    torquer;
  end

  subgraph ground_segment
    yamcs;
    terminals;
    cryptolib;
  end

  subgraph physics_engine
    fortytwo
  end

  subgraph terminals
    stdio-terminal;
    udp-terminal;
  end

  subgraph fsw
    actuators
    mag
    mag
    fss
    css
    camsim
    eps
    gps
    star-tracker
    imu
    thruster
    torquer
    radio
    sample-sim
    uart-bus
    nos_fsw
  end

  subgraph space_segment
    physics_engine
    fsw
    truth42sim
  end
  %% subgraph sensors
  %%   start-tracker
  %% end

  end

  classDef plain fill:#ddd,stroke:#fff,stroke-width:4px,color:#000;
  classDef k8s fill:#326ce5,stroke:#fff,stroke-width:4px,color:#fff;
  classDef cluster fill:#fff,stroke:#bbb,stroke-width:2px,color:#326ce5;
  classDef rw fill:#326ce5,stroke:#fff,stroke-width:4px,color:#fff;

  class fortytwo,gps,stdio-terminal,udp-terminal,sample-sim,eps,torquer,thruster,camsim,yamcs,cryptolib,css,mag,fss,imu,eps,star-tracker k8s;

  class truth42sim k8s;
  class nos_engine_server k8s;
  class nos_fsw k8s;

  class rw0,rw1,rw2 rw;

  class client plain;
  class cluster cluster;
```

## docker ps

```bash
docker ps | grep -v 'CONTAINER ID' |wc -l
24

```

```bash
docker ps

CONTAINER ID   IMAGE                     COMMAND                  CREATED              STATUS          PORTS                                                                                                NAMES
3a130fadaea9   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   41 seconds ago       Up 40 seconds                                                                                                        nos_time_driver
9661c0fa0ac8   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   49 seconds ago       Up 47 seconds                                                                                                        sc_1_torquer_sim
4ff3a142614a   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   50 seconds ago       Up 47 seconds                                                                                                        sc_1_thruster_sim
277b46405250   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   50 seconds ago       Up 48 seconds                                                                                                        sc_1_startrk_sim
44d1809986f5   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   50 seconds ago       Up 48 seconds                                                                                                        sc_1_sample_sim
dcde3f725275   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   51 seconds ago       Up 47 seconds   0.0.0.0:16010->6010/tcp, 0.0.0.0:16010->6010/udp, 0.0.0.0:16011->6011/tcp, 0.0.0.0:16011->6011/udp   sc_1_radio_sim
1e0a92b988c3   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   51 seconds ago       Up 48 seconds                                                                                                        sc_1_rw_sim2
eb3bc34fcc2f   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   51 seconds ago       Up 49 seconds                                                                                                        sc_1_rw_sim1
0c327e64ef4e   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   52 seconds ago       Up 50 seconds                                                                                                        sc_1_rw_sim0
9c6166335f7f   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   52 seconds ago       Up 50 seconds                                                                                                        sc_1_mag_sim
523b92a28e7f   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   53 seconds ago       Up 50 seconds                                                                                                        sc_1_imu_sim
82dfa03e9c62   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   53 seconds ago       Up 51 seconds                                                                                                        sc_1_gps_sim
af58b5d09af2   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   53 seconds ago       Up 51 seconds                                                                                                        sc_1_fss_sim
42fe85609195   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   54 seconds ago       Up 51 seconds                                                                                                        sc_1_eps_sim
ac7787f9cf7a   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   54 seconds ago       Up 51 seconds                                                                                                        sc_1_css_sim
5aedf4c0a619   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   56 seconds ago       Up 53 seconds                                                                                                        sc_1_truth42sim
0282c8fb48e2   ivvitc/nos3-64:20250217   "/usr/bin/nos_engine…"   57 seconds ago       Up 53 seconds                                                                                                        sc_1_nos_engine_server
14e98992d60b   ivvitc/nos3-64:20250217   "./support/standalone"   57 seconds ago       Up 54 seconds                                                                                                        sc_1_cryptolib
0c06e9b85d03   ivvitc/nos3-64:20250217   "/opt/neo/nos3/scrip…"   57 seconds ago       Up 53 seconds                                                                                                        sc_1_nos_fsw
1e84eafbabe1   ivvitc/nos3-64:20250217   "/opt/neo/nos3/scrip…"   58 seconds ago       Up 54 seconds                                                                                                        sc_1_onair
2365e959b0c6   ivvitc/nos3-64:20250217   "/home/neo/.nos3/42/…"   58 seconds ago       Up 55 seconds                                                                                                        sc_1_fortytwo
72c26bc3690a   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   59 seconds ago       Up 58 seconds                                                                                                        nos_udp_terminal
96c8948f8b01   ivvitc/nos3-64:20250217   "./nos3-single-simul…"   59 seconds ago       Up 58 seconds                                                                                                        nos_terminal
f78f9bda0e0a   ivvitc/nos3-64:20250217   "mvn -Dmaven.repo.lo…"   About a minute ago   Up 58 seconds   0.0.0.0:5012->5012/tcp, 0.0.0.0:8090->8090/tcp                                                       cosmos_openc3-operator_1

```

## docker network list

```bash

docker network list

NETWORK ID     NAME                DRIVER    SCOPE
2a772b648d00   bridge              bridge    local
5fe825f9aaa9   docker_quickstart   bridge    local
6652439bc0af   host                host      local
8cd5a6aac2de   none                null      local
6c6e8d8284e4   nos3_core           bridge    local
cca8286b3dd6   nos3_sc_1           bridge    local

```
