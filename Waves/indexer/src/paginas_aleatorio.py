#http://andrenoel.com.br/2008/12/20/random-em-python/

import random

cards = range(126985671)
random.shuffle(cards)
lista = cards[:100000]

arquivo = open('/mnt/hd0/ufam/parser/experimentos/indexer/src/paginasAleatorias.txt', 'w')


for numarquivo in lista:
    arquivo.write(str(numarquivo))
    arquivo.write('\n')

arquivo.close()
