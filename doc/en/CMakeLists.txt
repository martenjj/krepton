#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		30-May-12					#
#########################################################################

project(krepton)

set(doc_SRCS
  index.html
  editor.html)

get_filename_component(LOC "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(doc_DEST "${HTML_INSTALL_DIR}/${LOC}/${CMAKE_PROJECT_NAME}")
message(STATUS "Documentation HTML files will be installed in ${doc_DEST}")

install(FILES ${doc_SRCS} DESTINATION ${doc_DEST})
