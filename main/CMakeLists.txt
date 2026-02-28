
if(ESP_PLATFORM)
    idf_component_register( SRCS "main.cpp"
                            REQUIRES driver
                            INCLUDE_DIRS "." "atomic_models" $ENV{CADMIUM})

    target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++2b)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
    target_compile_options(${COMPONENT_LIB} PRIVATE "-frtti")
    target_compile_options(${COMPONENT_LIB} PRIVATE "-fexceptions")
    # target_compile_options(${COMPONENT_LIB} PRIVATE "-DNO_LOGGING")
    # target_compile_options(${COMPONENT_LIB} PRIVATE "-DNO_LOG_STATE")
    # target_compile_options(${COMPONENT_LIB} PRIVATE "-DDEBUG_DELAY")
else()

    # Test executable for generator model
    add_executable(test_generator tests/test_generator_main.cpp)
    target_include_directories(test_generator PRIVATE "." "atomic_models" $ENV{CADMIUM})
    target_compile_options(test_generator PUBLIC -std=gnu++2b)
    #target_compile_definitions(test_generator PRIVATE NO_LOG_STATE)
    #target_compile_definitions(test_generator PRIVATE NO_LOGGING)

    # Test executable for balancer model
    add_executable(test_balancer tests/test_balancer_main.cpp)
    target_include_directories(test_balancer PRIVATE "." "atomic_models" $ENV{CADMIUM})
    target_compile_options(test_balancer PUBLIC -std=gnu++2b)
    #target_compile_definitions(test_balancer PRIVATE NO_LOG_STATE)
    #target_compile_definitions(test_balancer PRIVATE NO_LOGGING)

    # Test executable for server model
    add_executable(test_server tests/test_server_main.cpp)
    target_include_directories(test_server PRIVATE "." "atomic_models" $ENV{CADMIUM})
    target_compile_options(test_server PUBLIC -std=gnu++2b)
    # target_compile_definitions(test_server PRIVATE NO_LOG_STATE)
    # target_compile_definitions(test_server PRIVATE NO_LOGGING)

    # Test executable for dbserver model
    add_executable(test_dbserver tests/test_dbserver_main.cpp)
    target_include_directories(test_dbserver PRIVATE "." "atomic_models" $ENV{CADMIUM})
    target_compile_options(test_dbserver PUBLIC -std=gnu++2b)
    # target_compile_definitions(test_dbserver PRIVATE NO_LOG_STATE)
    # target_compile_definitions(test_dbserver PRIVATE NO_LOGGING)

    # Test executable for LBS coupled model
    add_executable(test_lbs tests/test_lbs_main.cpp)
    target_include_directories(test_lbs PRIVATE "." "atomic_models" "coupled_models" $ENV{CADMIUM})
    target_compile_options(test_lbs PUBLIC -std=gnu++2b)
    # target_compile_definitions(test_lbs PRIVATE NO_LOG_STATE)
    # target_compile_definitions(test_lbs PRIVATE NO_LOGGING)

    # Test executable for TOP  coupled model
    add_executable(test_top tests/test_top_main.cpp)
    target_include_directories(test_top PRIVATE "." "atomic_models" "coupled_models" "Top_model" $ENV{CADMIUM})
    target_compile_options(test_top PUBLIC -std=gnu++2b)
    # target_compile_definitions(test_top PRIVATE NO_LOG_STATE)
    # target_compile_definitions(test_top PRIVATE NO_LOGGING)

endif()
