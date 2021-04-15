#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		15-Apr-21					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

include(ECMInstallIcons)

install(FILES krepton.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES kreptonui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/krepton)
install(FILES kreptonedui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/krepton)

set(krepton_ICONS
  16-apps-krepton.png
  32-apps-krepton.png
)

ecm_install_icons(ICONS ${krepton_ICONS} DESTINATION ${KDE_INSTALL_ICONDIR})
