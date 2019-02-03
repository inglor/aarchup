execute_process(COMMAND gzip -9 -f -n -k ${CMAKE_CURRENT_SOURCE_DIR}/aarchup.1)
SET(doc_files ../LICENSE.md ../README.md)
INSTALL(FILES ${doc_files} DESTINATION "share/doc/aarchup")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/aarchup.1.gz DESTINATION "share/man/man1")