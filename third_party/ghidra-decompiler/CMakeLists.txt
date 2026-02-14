cmake_minimum_required(VERSION 3.16)
project(ghidra-decompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)

# Suppress warnings from Ghidra code
add_compile_options(-w)
add_compile_definitions(NDEBUG)

set(DECOMP_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# ---- base static library ----
set(BASE_SOURCES
    ${DECOMP_SRC_DIR}/marshal.cc
    ${DECOMP_SRC_DIR}/space.cc
    ${DECOMP_SRC_DIR}/float.cc
    ${DECOMP_SRC_DIR}/address.cc
    ${DECOMP_SRC_DIR}/pcoderaw.cc
    ${DECOMP_SRC_DIR}/translate.cc
    ${DECOMP_SRC_DIR}/opcodes.cc
    ${DECOMP_SRC_DIR}/globalcontext.cc
    ${DECOMP_SRC_DIR}/xml.cc
)

add_library(ghidra_base STATIC ${BASE_SOURCES})
target_include_directories(ghidra_base PUBLIC ${DECOMP_SRC_DIR})

# ---- ghidra decompiler static library ----
set(DECOMPILER_SOURCES
    ${DECOMP_SRC_DIR}/capability.cc
    ${DECOMP_SRC_DIR}/architecture.cc
    ${DECOMP_SRC_DIR}/options.cc
    ${DECOMP_SRC_DIR}/graph.cc
    ${DECOMP_SRC_DIR}/cover.cc
    ${DECOMP_SRC_DIR}/block.cc
    ${DECOMP_SRC_DIR}/cast.cc
    ${DECOMP_SRC_DIR}/typeop.cc
    ${DECOMP_SRC_DIR}/database.cc
    ${DECOMP_SRC_DIR}/cpool.cc
    ${DECOMP_SRC_DIR}/comment.cc
    ${DECOMP_SRC_DIR}/stringmanage.cc
    ${DECOMP_SRC_DIR}/modelrules.cc
    ${DECOMP_SRC_DIR}/fspec.cc
    ${DECOMP_SRC_DIR}/action.cc
    ${DECOMP_SRC_DIR}/loadimage.cc
    ${DECOMP_SRC_DIR}/varnode.cc
    ${DECOMP_SRC_DIR}/op.cc
    ${DECOMP_SRC_DIR}/type.cc
    ${DECOMP_SRC_DIR}/variable.cc
    ${DECOMP_SRC_DIR}/varmap.cc
    ${DECOMP_SRC_DIR}/jumptable.cc
    ${DECOMP_SRC_DIR}/emulate.cc
    ${DECOMP_SRC_DIR}/emulateutil.cc
    ${DECOMP_SRC_DIR}/flow.cc
    ${DECOMP_SRC_DIR}/userop.cc
    ${DECOMP_SRC_DIR}/expression.cc
    ${DECOMP_SRC_DIR}/multiprecision.cc
    ${DECOMP_SRC_DIR}/funcdata.cc
    ${DECOMP_SRC_DIR}/funcdata_block.cc
    ${DECOMP_SRC_DIR}/funcdata_varnode.cc
    ${DECOMP_SRC_DIR}/unionresolve.cc
    ${DECOMP_SRC_DIR}/funcdata_op.cc
    ${DECOMP_SRC_DIR}/pcodeinject.cc
    ${DECOMP_SRC_DIR}/heritage.cc
    ${DECOMP_SRC_DIR}/prefersplit.cc
    ${DECOMP_SRC_DIR}/rangeutil.cc
    ${DECOMP_SRC_DIR}/ruleaction.cc
    ${DECOMP_SRC_DIR}/subflow.cc
    ${DECOMP_SRC_DIR}/transform.cc
    ${DECOMP_SRC_DIR}/blockaction.cc
    ${DECOMP_SRC_DIR}/merge.cc
    ${DECOMP_SRC_DIR}/double.cc
    ${DECOMP_SRC_DIR}/coreaction.cc
    ${DECOMP_SRC_DIR}/condexe.cc
    ${DECOMP_SRC_DIR}/override.cc
    ${DECOMP_SRC_DIR}/dynamic.cc
    ${DECOMP_SRC_DIR}/crc32.cc
    ${DECOMP_SRC_DIR}/prettyprint.cc
    ${DECOMP_SRC_DIR}/printlanguage.cc
    ${DECOMP_SRC_DIR}/printc.cc
    ${DECOMP_SRC_DIR}/printjava.cc
    ${DECOMP_SRC_DIR}/memstate.cc
    ${DECOMP_SRC_DIR}/opbehavior.cc
    ${DECOMP_SRC_DIR}/paramid.cc
    ${DECOMP_SRC_DIR}/string_ghidra.cc
    ${DECOMP_SRC_DIR}/constseq.cc
)

add_library(ghidra_decompiler STATIC ${DECOMPILER_SOURCES})
target_include_directories(ghidra_decompiler PUBLIC ${DECOMP_SRC_DIR})
target_link_libraries(ghidra_decompiler PUBLIC ghidra_base ZLIB::ZLIB)

# ---- sleigh static library ----
set(SLGH_SOURCES
    ${DECOMP_SRC_DIR}/sleigh.cc
    ${DECOMP_SRC_DIR}/sleigh_arch.cc
    ${DECOMP_SRC_DIR}/inject_sleigh.cc
    ${DECOMP_SRC_DIR}/pcodecompile.cc
    ${DECOMP_SRC_DIR}/sleighbase.cc
    ${DECOMP_SRC_DIR}/slghsymbol.cc
    ${DECOMP_SRC_DIR}/slghpatexpress.cc
    ${DECOMP_SRC_DIR}/slghpattern.cc
    ${DECOMP_SRC_DIR}/semantics.cc
    ${DECOMP_SRC_DIR}/context.cc
    ${DECOMP_SRC_DIR}/slaformat.cc
    ${DECOMP_SRC_DIR}/compression.cc
    ${DECOMP_SRC_DIR}/filemanage.cc
    ${DECOMP_SRC_DIR}/pcodeparse.cc
    ${DECOMP_SRC_DIR}/grammar.cc
)

add_library(ghidra_slgh STATIC ${SLGH_SOURCES})
target_include_directories(ghidra_slgh PUBLIC ${DECOMP_SRC_DIR})
target_link_libraries(ghidra_slgh PUBLIC ghidra_base ZLIB::ZLIB)

# ---- libdecomp static library (combines everything) ----
set(LIBDECOMP_SOURCES
    ${DECOMP_SRC_DIR}/libdecomp.cc
)

add_library(libdecomp_static STATIC ${LIBDECOMP_SOURCES})
target_include_directories(libdecomp_static PUBLIC ${DECOMP_SRC_DIR})
target_link_libraries(libdecomp_static PUBLIC ghidra_decompiler ghidra_slgh ZLIB::ZLIB)

# ---- sleighc compiler executable ----
set(SLEIGHC_SOURCES
    ${DECOMP_SRC_DIR}/slgh_compile.cc
    ${DECOMP_SRC_DIR}/slghparse.cc
    ${DECOMP_SRC_DIR}/slghscan.cc
)

add_executable(sleighc ${SLEIGHC_SOURCES})
target_include_directories(sleighc PRIVATE ${DECOMP_SRC_DIR})
target_link_libraries(sleighc PRIVATE ghidra_slgh ZLIB::ZLIB)
