file(GLOB HEADERS ./include/*.h)
file(GLOB SOURCES ./source/*.cpp)

add_library(Chart STATIC ${HEADERS} ${SOURCES})

target_include_directories(Chart PUBLIC ./include)

target_link_libraries(Chart PRIVATE
    Qt::Charts
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

target_link_libraries(Profiler PUBLIC Qt::Core Qt::Widgets)
