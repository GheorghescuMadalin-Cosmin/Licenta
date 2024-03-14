#######################################################
# project definition                                  #
#######################################################
#   setting the project name, version and language
project(utilis     
    VERSION 0.1.0
    DESCRIPTION "Project description"
    HOMEPAGE_URL ""
    LANGUAGES CXX)

add_subdirectory(parser/json)
add_subdirectory(parser/xml)