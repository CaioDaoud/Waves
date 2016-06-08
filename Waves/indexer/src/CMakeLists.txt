aux_source_directory( utils UTILS )
aux_source_directory( . SRC )
aux_source_directory( ../../libs-iw/indexer/include AUXINCLUDE )
add_library( indexer ${UTILS} ${SRC} ${AUXINCLUDE})
target_link_libraries( indexer )

add_executable(geraParcial run/geraParcial.cc)
target_link_libraries (geraParcial indexer)

#add_executable(geraParcialEcommerce run/geraParcialEcommerce.cc)
#target_link_libraries (geraParcialEcommerce indexer)

add_executable(mergeParcial run/mergeParcial.cc)
target_link_libraries (mergeParcial indexer)

add_executable(geraIndiceFrequencia run/geraIndiceFrequencia.cc)
target_link_libraries (geraIndiceFrequencia indexer)

add_executable(geraSkiplistBlockMaxFIX run/geraSkiplistBlockMaxFIX.cc)
target_link_libraries (geraSkiplistBlockMaxFIX indexer)

add_executable(splitIndex run/splitIndex.cc)
target_link_libraries (splitIndex indexer)

add_executable(estimaDistribuicao run/estimaDistribuicao.cc)
target_link_libraries (estimaDistribuicao indexer)

add_executable(geraNorma run/geraNorma.cc)
target_link_libraries (geraNorma indexer)

add_executable(geraIdf run/geraIdf.cc)
target_link_libraries (geraIdf indexer)

#add_executable(geraSkiplistBlockMax run/geraSkiplistBlockMax.cc)
#target_link_libraries (geraSkiplistBlockMax indexer)

#add_executable(geraIndiceTermo run/geraIndiceTermo.cc)
#target_link_libraries (geraIndiceTermo indexer)

#add_executable(geraIndiceFracao run/geraIndiceFracao.cc)
#target_link_libraries (geraIndiceFracao indexer)

#add_executable(splitTerms run/splitTerms.cc)
#target_link_libraries (splitTerms indexer)

#add_executable(geraSkiplistFrequencia run/geraSkiplistFrequencia.cc)
#target_link_libraries (geraSkiplistFrequencia indexer)

#add_executable(geraSkiplistFrequenciaVetor run/geraSkiplistFrequenciaVetor.cc)
#target_link_libraries (geraSkiplistFrequenciaVetor indexer)

#add_executable(geraSkiplistPosicional run/geraSkiplistPosicional.cc)
#target_link_libraries (geraSkiplistPosicional indexer)

#add_executable(tamanhoMedio run/tamanhoMedio.cc)
#target_link_libraries (tamanhoMedio indexer)

#add_executable(leLista run/leLista.cc)
#target_link_libraries (leLista indexer)

#add_executable(leListaArquivo run/leListaArquivo.cc)
#target_link_libraries (leListaArquivo indexer)

#add_executable(varreLista run/varreLista.cc)
#target_link_libraries (varreLista indexer)

#add_executable(leListaVetor run/leListaVetor.cc)
#target_link_libraries (leListaVetor indexer)

#add_executable(converteWT10G run/converteWT10G.cc)
#target_link_libraries (converteWT10G indexer)

#add_executable(leDoc run/leDoc.cc)
#target_link_libraries (leDoc indexer)

#add_executable(mergeListas run/mergeListas.cc)
#target_link_libraries (mergeListas indexer)

#add_executable(geraWholeDoc run/geraWholeDoc.cc)
#target_link_libraries (geraWholeDoc indexer)

#add_executable(mergeListasBloco run/mergeListasBloco.cc)
#target_link_libraries (mergeListasBloco indexer)

#add_executable(mergeVetorFeature run/mergeVetorFeature.cc)
#target_link_libraries (mergeVetorFeature indexer)

set( ${PROJECT_NAME}_TO_INSTALL indexer )
include ( ${${PROJECT_NAME}_SOURCE_DIR}/install.cmake )

