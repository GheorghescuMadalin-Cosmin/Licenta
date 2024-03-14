#######################################################
# project definition                                  #
#######################################################
#   setting the project name, version and language
project(data_visualizations     VERSION 0.1.0
    DESCRIPTION "Project description"
    HOMEPAGE_URL ""
    LANGUAGES CXX)

add_subdirectory(raw_data_visualization)
add_subdirectory(wave_visualizer)
add_subdirectory(signal_monitor)
add_subdirectory(sensor_arduino_visualizer)
add_subdirectory(webcam_visualisez)
add_subdirectory(write_file)
add_subdirectory(read_file)