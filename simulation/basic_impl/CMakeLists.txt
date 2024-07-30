add_subdirectory(json)
# include headers so they are nicely listed, as described in https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake#13703725
add_library(basic_impl Peers.h Peers.cpp Peers.fwd.h ${LIB_HEADER} PeerFactory.cpp PeerFactory.h PeerFactory.fwd.h CalculationStrategy.cpp CalculationStrategy.h ResultPeers.cpp ResultPeers.h ResultPeers.fwd.h ConfigurableTrainUser.cpp ConfigurableTrainUser.h RateHelper.h RateHelper.cpp JSONReporter.cpp JSONReporter.h json/json.cpp ConcreteSimpleExecution.cpp ConcreteSimpleExecutionCycle.cpp ConcreteSimpleExecution.h ConcreteSimpleExecutionCycle.h BaselinePeers.cpp BaselinePeers.h ThreatModels.cpp ThreatModels.h)
target_include_directories(basic_impl PUBLIC ./)


target_link_libraries(basic_impl rep_eval Eigen3::Eigen nlohmann_json)