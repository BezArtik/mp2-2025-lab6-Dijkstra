add_library(graph STATIC
	graph/graph.cpp
    graph/graph_generator.cpp
)

target_include_directories(graph 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/graph
)

target_link_libraries(graph 
    PUBLIC
        containers  
        
)

add_library(project::graph ALIAS graph)