include $(TOPDIR)/rules.mk

PKG_NAME:=mycap
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Capture 80211 frame and send to dbserver by tcp
	DEPENDS:=+libpcap +libpthread
	MAINTAINER:=Sean Shi
endef

define Package/$(PKG_NAME)/description
	If you can't figure out what this program does, you're probably
	brain-dead and need immediate medical attention.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mycap $(1)/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
