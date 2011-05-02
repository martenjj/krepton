#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		02-May-11					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

#########################################################################

install(FILES krepton.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kreptonui.rc DESTINATION ${DATA_INSTALL_DIR}/krepton)
install(FILES kreptonedui.rc DESTINATION ${DATA_INSTALL_DIR}/krepton)

kde4_install_icons(${ICON_INSTALL_DIR})
