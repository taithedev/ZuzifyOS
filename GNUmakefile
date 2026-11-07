SHELL := /bin/bash
.PHONY: help validate linux-iso clean-native
help:
	@printf "ZuzifyOS Linux targets:\n  make validate\n  make linux-iso (Debian 13 root environment)\n"
validate:
	python3 distro/scripts/validate.py
linux-iso: validate
	cd distro && bash scripts/build.sh
clean-native:
	@echo "Native hobby kernel preserved on archive/native-kernel-0.2"
