######################################
#
# control-button-board
#
######################################

# where to find the source code - locally in this case
CONTROL_BUTTON_BOARD_SITE_METHOD = local
CONTROL_BUTTON_BOARD_SITE = $($(PKG)_PKGDIR)/

# even though this is a local build, we still need a version number
# bump this number if you need to force a rebuild
CONTROL_BUTTON_BOARD_VERSION = 1

# dependencies (list of other buildroot packages, separated by space)
CONTROL_BUTTON_BOARD_DEPENDENCIES =

# LV2 bundles that this package generates (space separated list)
CONTROL_BUTTON_BOARD_BUNDLES = control-button-board.lv2

# call make with the current arguments and path. "$(@D)" is the build directory.
CONTROL_BUTTON_BOARD_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/source


# build command
define CONTROL_BUTTON_BOARD_BUILD_CMDS
	$(CONTROL_BUTTON_BOARD_TARGET_MAKE)
endef

# install command
define CONTROL_BUTTON_BOARD_INSTALL_TARGET_CMDS
	$(CONTROL_BUTTON_BOARD_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
endef


# import everything else from the buildroot generic package
$(eval $(generic-package))
