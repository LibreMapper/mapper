project(Cove)

find_package(Qt6 COMPONENTS Core Widgets Concurrent REQUIRED GLOBAL)

set(CMAKE_CXX_STANDARD 17)

add_library(cove-potrace STATIC
    potrace/curve.c
    potrace/potracelib.c
    potrace/trace.c
)
target_include_directories(cove-potrace INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(cove-vectorizer STATIC
    libvectorizer/AlphaGetter.cpp
    libvectorizer/Concurrency.cpp
    libvectorizer/FIRFilter.cpp
    libvectorizer/KohonenMap.cpp
    libvectorizer/MapColor.cpp
    libvectorizer/Morphology.cpp
    libvectorizer/ParallelImageProcessing.cpp
    libvectorizer/PatternGetter.cpp
    libvectorizer/Polygons.cpp
    libvectorizer/ProgressObserver.h
    libvectorizer/Vectorizer.cpp
)
target_include_directories(cove-vectorizer
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/libvectorizer"
  INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(cove-vectorizer PRIVATE cove-potrace)
target_link_libraries(cove-vectorizer PUBLIC Qt::Gui Qt::Concurrent)

set(Cove_SRCS
    app/classificationconfigform.cpp
    app/colorseditform.cpp
    app/coverunner.cpp
    app/mainform.cpp
    app/ImageView.cpp
    app/PolygonsView.cpp
    app/Settings.cpp
    app/UIProgressDialog.cpp
    app/vectorizationconfigform.cpp
)

add_library(cove STATIC ${Cove_SRCS})
target_compile_definitions(cove INTERFACE WITH_COVE)
target_include_directories(cove
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/app"
    "${Mapper_SOURCE_DIR}/src"
  INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
if(${CMAKE_VERSION} VERSION_LESS 3.8)
	target_include_directories(cove PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endif()
target_link_libraries(cove cove-vectorizer Qt::Widgets)
set_target_properties(cove
  PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

add_subdirectory("tests")
