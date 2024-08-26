set(MODULE_NAME "Server")

qt_add_library(${MODULE_NAME} MODULE)
add_library(Sky::${MODULE_NAME} ALIAS ${MODULE_NAME})

find_package(Qt6 REQUIRED COMPONENTS WebSockets)

target_sources(${MODULE_NAME}
    PRIVATE
        src/WebSocketServer.h src/WebSocketServer.cpp
        src/ServerPlugin.h src/ServerPlugin.cpp
        src/ServerSettings.h src/ServerSettings.cpp
        src/ServerWidget.h src/ServerWidget.cpp src/ServerWidget.ui
        src/ServerPlugin.json
)

target_link_libraries(${MODULE_NAME}
    PRIVATE
        Qt6::Widgets
        Sky::Kernel
        Sky::Model
        Sky::Persistence
        Sky::PluginManager
        Sky::Widget
        Qt::WebSockets
)
