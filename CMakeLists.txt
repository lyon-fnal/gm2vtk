# ======================================================================
#  artg4cadmesh main build file
#
#  . mrb slp   # Run if you have local products to set up
#  . mrb s     # Needed to set up local development products
#  mrb b       # Build
#  mrb t       # Test (if applicable)
#  mrb i       # Install to local products
#  gm2 -c <fhicl_file>  # Run!
#
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(gm2viz)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{CANVAS_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

set_install_root()

# g-2 wants the product name in the library name
set (ART_MAKE_PREPEND_PRODUCT_NAME TRUE)

cet_set_compiler_flags(DIAGS CAUTIOUS
   WERROR
#   EXTRA_FLAGS -pedantic  # Must remove so that vtk includes don't trip pedantic error (extra ;)
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product(art v1_13_01)
find_ups_product(gm2ringsim v3_04_00)
find_ups_product( cetbuildtools v4_08_01 )

# Load the ParaView stuff (ParaView_DIR is a magic cmake variable that tells find_package where to find build products for ParaView Catalyst)
get_filename_component(ParaView_DIR "$ENV{PARAVIEW_FQ_DIR}/lib/cmake/paraview-5.1" REALPATH)
set(USE_CATALYST ON CACHE BOOL "Link the simulation with Catalyst")
find_package(ParaView 5.1 REQUIRED COMPONENTS vtkPVPythonCatalyst vtkFiltersSources)
include_directories(${PARAVIEW_INCLUDE_DIRS})
add_definitions("-DUSE_CATALYST")

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(adapter)
add_subdirectory(simviz)
add_subdirectory(dumbEventDisplay)
add_subdirectory(dumbEventDisplayNew)
add_subdirectory(ups)

# packaging utility
include(UseCPack)
