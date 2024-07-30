add_library(rep_eval STATIC Abstracts.h Abstracts.cpp PeerCollection.fwd.h PeerCollection.cpp PeerCollection.h types.h Simulation.cpp Simulation.h SimulationReporter.cpp SimulationReporter.h Abstracts.fwd.h custom_sorting.h NetworkAnalyser.cpp NetworkAnalyser.h SimulationReporter.fwd.h)
target_include_directories(rep_eval PUBLIC ./)
target_link_libraries(rep_eval Eigen3::Eigen)
set(LIB_HEADER FILES_MATCHING PATTERN "*.h")