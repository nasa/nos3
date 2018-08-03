project(sim_server)

set(sim_server_config
    nos_engine_server_simulator_config.json
)    

install(FILES ${sim_server_config} DESTINATION bin COMPONENT config)
