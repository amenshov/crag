#Specify the version being used aswell as the language
cmake_minimum_required(VERSION 2.8.12)
cmake_policy(VERSION 2.8.12)

include("../cmake/common.cmake")

crag_library(Alphabet
  Alphabet
  AlphabetBisonGrammar
  AlphabetFlex
  WordBisonGrammar
  WordFlex
)

target_link_libraries(Alphabet
  PUBLIC Elt
  PRIVATE crag_general
)

target_compile_definitions(Alphabet PRIVATE "YY_NO_UNISTD_H")

crag_main(mainParser Alphabet Elt)
