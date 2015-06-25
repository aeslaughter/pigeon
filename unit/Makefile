###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Optional Environment variables
# MOOSE_DIR        - Root directory of the MOOSE project 
# HERD_TRUNK_DIR   - Location of the HERD repository
# FRAMEWORK_DIR    - Location of the MOOSE framework
#
###############################################################################
MOOSE_DIR          ?= $(shell dirname `pwd`)/../moose
FRAMEWORK_DIR      ?= $(MOOSE_DIR)/framework
###############################################################################
CURRENT_DIR        := $(shell pwd)

# framework
include $(FRAMEWORK_DIR)/build.mk
include $(FRAMEWORK_DIR)/moose.mk

################################## MODULES ####################################
HEAT_CONDUCTION   := yes
NAVIER_STOKES     := yes
MISC              := yes
include           $(MOOSE_DIR)/modules/modules.mk
###############################################################################

# Extra stuff for CPPUNIT
CPPUNIT_DIR 		?= $(PACKAGES_DIR)/cppunit
ADDITIONAL_INCLUDES 	:= -I$(CPPUNIT_DIR)/include
ADDITIONAL_LIBS 	:= -L$(CPPUNIT_DIR)/lib -lcppunit

# dep apps
APPLICATION_DIR    := $(CURRENT_DIR)/..
APPLICATION_NAME   := Tardigrade
include            $(FRAMEWORK_DIR)/app.mk

APPLICATION_DIR    := $(CURRENT_DIR)
APPLICATION_NAME   := Tardigrade-unit
BUILD_EXEC         := yes
DEP_APPS    ?= $(shell $(FRAMEWORK_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))
include $(FRAMEWORK_DIR)/app.mk

# Find all the TARDIGRADE unit test source files and include their dependencies.
Tardigrade-unit_srcfiles := $(shell find $(CURRENT_DIR) -name "*.C")
Tardigrade-unit_deps := $(patsubst %.C, %.$(obj-suffix).d, $(Tardigrade-unit_srcfiles))
-include $(Tardigrade-unit_deps)

###############################################################################
# Additional special case targets should be added here
