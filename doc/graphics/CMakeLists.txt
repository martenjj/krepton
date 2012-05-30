#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		30-May-12					#
#########################################################################

project(krepton)

file(GLOB graphics_SRCS "*.png")

set(LOC "en")
set(graphics_DEST "${HTML_INSTALL_DIR}/${LOC}/${CMAKE_PROJECT_NAME}")
message(STATUS "Documentation graphics will be installed in ${graphics_DEST}")

install(FILES ${graphics_SRCS} DESTINATION ${graphics_DEST})
