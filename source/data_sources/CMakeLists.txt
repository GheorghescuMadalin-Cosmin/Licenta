#######################################################
# project definition                                  #
#######################################################
#   setting the project name, version and language
project(data_sources     VERSION 0.1.0
    DESCRIPTION "Project description"
    HOMEPAGE_URL ""
    LANGUAGES CXX)
    
add_subdirectory(sensor_arduino)
add_subdirectory(web_cam)