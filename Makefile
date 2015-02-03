include $(TOPDIR)/rules.mk

PKG_NAME:=mycap
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
include $(INCLUDE_DIR)/package.mk

define Package/mycap
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=mycap -- prints a snarky message
	DEPENDS:=+libpcap +libpthread
endef

define Package/mycap/description
	If you can't figure out what this program does, you're probably
	brain-dead and need immediate medical attention.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/mycap/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mycap $(1)/bin/
endef

$(eval $(call BuildPackage,mycap))
