cmake_minimum_required(VERSION 3.15)
project(sistema_nba C)

set(CMAKE_C_STANDARD 11)

# Modulos compartilhados (todos os .c, menos os que tem main proprio).
set(MODULOS
        leitor.c
        lista.c
        lista_comp.c
        avl.c
        hash.c
        skiplist.c
        bloom.c
        operacoes.c
)

# ---------- Programas principais ----------
add_executable(sistema          main.c             ${MODULOS})
add_executable(benchmark        benchmark.c        ${MODULOS})
add_executable(otimizacao_bench otimizacao_bench.c ${MODULOS})
add_executable(restricoes       restricoes.c       ${MODULOS})

target_link_libraries(sistema m)
target_link_libraries(benchmark m)
target_link_libraries(otimizacao_bench m)
target_link_libraries(restricoes m)

# ---------- Testes de cada estrutura (nao precisam do CSV) ----------
# Cada um demonstra inserir/buscar/remover de uma estrutura, isoladamente.
add_executable(lista_teste    lista_teste.c    lista.c)
add_executable(avl_teste      avl_teste.c      avl.c)
add_executable(hash_teste     hash_teste.c     hash.c)
add_executable(skiplist_teste skiplist_teste.c skiplist.c)
add_executable(bloom_teste    bloom_teste.c    bloom.c)
