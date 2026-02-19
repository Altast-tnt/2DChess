cmake_minimum_required(VERSION 3.10)
project(ChessGame)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск обязательных библиотек 
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
find_package(JPEG REQUIRED)  
find_package(PNG REQUIRED)  
find_package(ZLIB REQUIRED) 

# Подключение заголовков 
include_directories(src) 
include_directories(oxygine-framework/oxygine/src)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR})
include_directories(${JPEG_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIRS} ${PNG_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})

# Добавление Oxygine 
add_subdirectory(oxygine-framework)

# Сбор исходников
file(GLOB SOURCES "src/*.cpp")

# Ресурсы
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMENT "Copying game resources..."
)

# Исполняемый файл
add_executable(ChessGame ${SOURCES})
add_dependencies(ChessGame copy_resources)

# Линк библиотек
target_link_libraries(ChessGame 
    oxygine-framework       
    ${SDL2_LIBRARIES}       
    ${JPEG_LIBRARIES}       
    ${PNG_LIBRARIES}        
    ${ZLIB_LIBRARIES}       
    Threads::Threads        
    GL                      
    dl                      
)
