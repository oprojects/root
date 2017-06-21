############################################################################
# CMakeLists.txt file for parallel TMVA tests.
# @author Omar Zapata
############################################################################

#---hsimple------------------------------------------------------------------------------------
ROOT_EXECUTABLE(ptmva_testdl ptmva_testdl.C LIBRARIES RIO Tree Hist TMVA)
ROOT_ADD_TEST(test-ptmva_testdl COMMAND ptmva_testdl)

ROOT_EXECUTABLE(ptmva_testcv ptmva_testcv.C LIBRARIES RIO Tree Hist TMVA PTMVA Thread Core ${TBB_LIBRARIES})
ROOT_ADD_TEST(test-ptmva_testcv COMMAND ptmva_testcv)

ROOT_EXECUTABLE(ptmva_testfactory ptmva_testfactory.C LIBRARIES RIO Tree Hist TMVA PTMVA Thread Core ${TBB_LIBRARIES})
ROOT_ADD_TEST(test-ptmva_testfactory COMMAND ptmva_testfactory)

