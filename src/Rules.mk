n = src

include .make/template.mk

CHILD_MODULES := solver problems

$(d)/%: %
	@echo
