add_custom_target(SVlangUnitTests)
#set_target_properties(SVlangUnitTests PROPERTIES FOLDER "SVlang tests")

function(add_svlang_unittest test_dirname)
    add_unittest(SVlangUnitTests ${test_dirname} ${ARGN})
endfunction()

add_subdirectory(Lex)
