_LNG = $(LANGUAGE)
_INX_PATH = .
STAMP = stampinf -f $@ -a $(_BUILDARCH) -k $(KMDF_VERSION_MAJOR).$(KMDF_VERSION_MINOR)

$(OBJ_PATH)\$(O)\$(INF_NAME).inf: $(_INX_PATH)\$(INF_NAME).inx
    copy $(_INX_PATH)\$(@B).inx $@
    $(STAMP)
