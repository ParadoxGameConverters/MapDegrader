cmake_minimum_required(VERSION 3.5)

project(MapDegrader)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Source)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBDIR /usr/local/lib)
set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} LIBDIR)
set(LD_RUN_PATH ${LD_RUN_PATH} LIBDIR)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=8")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath -Wl,LIBDIR")

add_compile_options("-lstdc++fs")
add_compile_options("-O2")
add_compile_options("-pthread")
add_link_options(-no-pie)

include_directories("../commonItems")
include_directories("/usr/local/include/ImageMagick-7/")
include_directories("${PROJECT_SOURCE_DIR}")
link_directories(${CMAKE_SOURCE_DIR}/Resources /usr/local/lib)

set(MAIN_SOURCES ${MAIN_SOURCES} "${PROJECT_SOURCE_DIR}/MapDegrader.cpp")
set(MAIN_SOURCES ${MAIN_SOURCES} "${PROJECT_SOURCE_DIR}/main.cpp")

set(MAPPERS_COLOR_MAPPER_SOURCES ${MAPPERS_COLOR_MAPPER_SOURCES} "${PROJECT_SOURCE_DIR}/Mappers/ColorMapper/ColorMapper.cpp")
set(MAPPERS_DEFINITIONS_SCRAPER_SOURCES ${MAPPERS_DEFINITIONS_SCRAPER_SOURCES} "${PROJECT_SOURCE_DIR}/Mappers/DefinitionsScraper/DefinitionsScraper.cpp")
set(MAPPERS_LANDED_TITLES_SCRAPER_SOURCES ${MAPPERS_LANDED_TITLES_SCRAPER_SOURCES} "${PROJECT_SOURCE_DIR}/Mappers/LandedTitlesScraper/LandedTitlesScraper.cpp")
set(MAPPERS_LOCALIZATION_SCRAPER_SOURCES ${MAPPERS_LOCALIZATION_SCRAPER_SOURCES} "${PROJECT_SOURCE_DIR}/Mappers/LocalizationScraper/LocalizationScraper.cpp")

set(COMMON_SOURCES "../commonItems/CommonFunctions.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Color.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/LinuxUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Log.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/OSCommonLayer.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Parser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ParserHelpers.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/StringUtils.cpp")

add_executable(MapDegrader 
	${MAIN_SOURCES}
	
	${MAPPERS_COLOR_MAPPER_SOURCES}
	${MAPPERS_DEFINITIONS_SCRAPER_SOURCES}
	${MAPPERS_LANDED_TITLES_SCRAPER_SOURCES}
	${MAPPERS_LOCALIZATION_SCRAPER_SOURCES}
	
	${COMMON_SOURCES}
)

target_link_libraries(MapDegrader LINK_PUBLIC stdc++fs MagickWand-7.Q8 MagickCore-7.Q8 Magick++-7.Q8)
