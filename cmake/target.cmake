add_library(${DEPLOY_LIB_NAME} SHARED ${WATER_LEAK_SRC})
target_include_directories(${DEPLOY_LIB_NAME} PUBLIC ${CUDA_INCLUDE_DIR})
target_link_libraries(${DEPLOY_LIB_NAME} PUBLIC ${DEP_LIBS})

add_executable(${MAIN_NAME} ${WATER_LEAK_HEADER} ${WATER_LEAK_MAIN})
target_link_libraries(${MAIN_NAME} PUBLIC ${DEP_LIBS} ${DEPLOY_LIB_NAME})


