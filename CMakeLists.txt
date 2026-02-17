cmake_minimum_required(VERSION 3.10)
project(ChessGame)

# Поиск обязательных библиотек 
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
find_package(JPEG REQUIRED)  
find_package(PNG REQUIRED)  
find_package(ZLIB REQUIRED) 

# Подключение заголовков
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${JPEG_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIR})

# Подключение Oxygine
add_subdirectory(oxygine-framework)
include_directories(oxygine-framework/oxygine/src)

# .cpp файлы в папке src
file(GLOB SOURCES "src/*.cpp")

# Настройки для копирования ресурсов (папка data)
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Создание исполняемого файла
add_executable(ChessGame ${SOURCES})

# Линк проекта с движком
target_link_libraries(ChessGame 
    oxygine-framework       # Сам движок
    ${SDL2_LIBRARIES}       # Библиотека SDL2
    SDL2                    
    ${JPEG_LIBRARIES}       # Библиотека JPEG
    ${PNG_LIBRARIES}        # Библиотека PNG
    ${ZLIB_LIBRARIES}       # Библиотека ZLIB
    Threads::Threads        # Потоки
    GL                      # OpenGL
    dl                      # Динамическая загрузка (нужна для Linux)
)

# Настройки компилятора (C++11 или новее)
set_property(TARGET ChessGame PROPERTY CXX_STANDARD 11)
