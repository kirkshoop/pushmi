

add_executable(composition_2 composition_2.cpp)
target_link_libraries(composition_2
  pushmi
  examples
  Threads::Threads)
  