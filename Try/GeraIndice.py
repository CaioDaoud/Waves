import sys
import subprocess


if (len(sys.argv) != 5):
	print "\n\n python geraIndice.py <pathColecao> <pathBase> <pathIndice> <Nome do Indice>\n\n"
	exit(0)
	
print "== Processando ==="

pathCol  = sys.argv[1]
pathCol_ = sys.argv[2]
pathInd  = sys.argv[3]
indName  = sys.argv[4]
indice = pathInd+indName



comando = "mkdir "+pathCol_+" "+pathInd
print comando
subprocess.call(comando, shell=True)


comando = "./GeraArqDocBase "+pathCol+" "+pathCol_
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "./geraParcial "+pathCol_+" "+indice+" "+indice+" 1" #(1 == BM25)
print "\n==>"+comando
nDocs = subprocess.check_output(comando, shell=True).split("Salvos")[1].split()[0]
print nDocs+" Documentos"

comando = "ls "+pathInd+"*.temp*"
print "\n==>"+comando
nTemp = len( (subprocess.check_output(comando, shell=True)).split() ) 

print str(nTemp)+" Temporarios"

comando = "./mergeParcial "+indice+ " " +str(nTemp)
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "./geraIndiceFrequencia "+indice
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "./geraIdf "+indice+" "+nDocs+" 1" 
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "./geraNorma "+indice+" "+nDocs+" 2"
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "cp "+indice+"idx "+indice+"[comp]idx"
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "./geraSkiplistBlockMaxFIX "+indice+" 3"
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "cp "+indice+"idx "+indice+"[nComp]idx"
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "cp "+indice+"[comp]idx "+indice+"idx"
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "./estimaDistribuicao "+indice+" 3 > "+indice+"Distribuicao"
print "\n==>"+comando
subprocess.call(comando, shell=True)

comando = "cp "+indice+"[nComp]idx "+indice+"idx"
print "\n==>"+comando
subprocess.call(comando, shell=True)
