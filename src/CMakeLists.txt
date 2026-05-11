add_library(graph STATIC
	graph/graph.cpp
	graph/graph_generator.cpp 
	graph/dijkstra.cpp
)
target_compile_features(graph PUBLIC cxx_std_20)
target_include_directories(graph PUBLIC 
    ${CMAKE_SOURCE_DIR}/include/
)