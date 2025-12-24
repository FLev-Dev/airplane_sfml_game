cmake_minimum_required (VERSION 3.28)

project ("sfml_airplane")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 3.0.2
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_executable(sfml_airplane 
  src/main.cpp
  
  # Utils
  src/utils/defines.hpp
  src/utils/debug_bounds.hpp
  src/utils/logger.hpp							src/utils/logger.cpp
  src/utils/database_api.hpp					src/utils/database_api.cpp

  # Game objects
  src/Entities/Entity.hpp
  src/Entities/Bullet.hpp
  src/Entities/Enemy.hpp
  src/Entities/Small_stone.hpp
  src/Entities/Big_stone.hpp
  src/Entities/Player.hpp						src/Entities/Player.cpp					

  # Game management
  src/Window/Main_window.hpp					src/Window/Main_window.cpp

  src/Window/Scenes/Game_state.hpp
  src/Window/Scenes/Scene.hpp
  src/Window/Scenes/Login_scene.hpp				src/Window/Scenes/Login_scene.cpp
  src/Window/Scenes/Main_menu.hpp				src/Window/Scenes/Main_menu.cpp
  src/Window/Scenes/Leaderboard_scene.hpp		src/Window/Scenes/Leaderboard_scene.cpp
  src/Window/Scenes/Level_selection.hpp			src/Window/Scenes/Level_selection.cpp
  src/Window/Scenes/Game_scene.hpp				src/Window/Scenes/Game_scene.cpp
  src/Window/Scenes/Victory_scene.hpp			src/Window/Scenes/Victory_scene.cpp
  src/Window/Scenes/Game_over_scene.hpp			src/Window/Scenes/Game_over_scene.cpp

  src/Level/Progress_manager.hpp 				src/Level/Progress_manager.cpp

  # UI elements
  src/UI/Label.hpp								src/UI/Label.cpp	
  src/UI/Button.hpp								src/UI/Button.cpp						
  src/UI/Panel.hpp								src/UI/Panel.cpp
  src/UI/Decorated_panel.hpp					src/UI/Decorated_panel.cpp
)
  
target_include_directories(sfml_airplane PRIVATE src)

# Copy assets directory to output directory after build
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)
if(EXISTS ${ASSETS_SOURCE_DIR})
 set(ASSETS_TARGET_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

 add_custom_command(TARGET sfml_airplane POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${ASSETS_SOURCE_DIR} ${ASSETS_TARGET_DIR}
   COMMENT "Copying assets directory to output directory"
 )
else()
  message(WARNING "Assets directory '${ASSETS_SOURCE_DIR}' does not exist. Skipping copy.")
endif()

# Quill logging library
message(STATUS "Fetching Quill logging library...")
FetchContent_Declare(quill
  GIT_REPOSITORY https://github.com/odygrd/quill.git
  GIT_TAG v10.2.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(quill)

# nlohmann json library
message(STATUS "Fetching nlohmann json library...")
FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(nlohmann_json)

# SQLite3 library
set(SQLITE3_DIR ${CMAKE_SOURCE_DIR}/3rd_party/sqlite3)

add_library(sqlite3 STATIC ${SQLITE3_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${SQLITE3_DIR})
set_target_properties(sqlite3 PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED ON
  COMPILE_DEFINITIONS "SQLITE_ENABLE_FTS5;SQLITE_ENABLE_JSON1"
)

if(MSVC)
  target_compile_options(sqlite3 PRIVATE /W0)
else()
  target_compile_options(sqlite3 PRIVATE -w)
endif()

# Link libraries
target_compile_features(sfml_airplane PRIVATE cxx_std_20)
target_link_libraries(sfml_airplane PRIVATE 
  SFML::Graphics
  quill::quill
  nlohmann_json::nlohmann_json
  sqlite3
)