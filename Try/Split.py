import sys, subprocess



if (len(sys.argv) != 4):
	print "\n\n python Split2.py <pathIndice> <Nome do Indice.> <% (1 - 99)>\n\n"
	exit(0)
	
print "== Processando ==="

pathInd  = sys.argv[1]
indName  = sys.argv[2]
div = sys.argv[3]
indice = pathInd+indName


arq = open(indice+"Distribuicao", 'r')
texto = arq.readlines()


for linha in texto :
	if '['+div in str(linha) :
		
		peso = linha.split('score : ')[1].split(' ')[0]
		print( div+"% = Threshold: "+ peso)

		
		comando = "cp "+indice+"[comp]idx "+indice+"idx"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)

		comando = "./splitIndex "+indice+" "+pathInd+"fir_"+div+". "+pathInd+"sec_"+div+". "+peso+" 3"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
		
		comando = "cp "+indice+"[nComp]idx "+indice+"idx"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)


		comando = "cp "+pathInd+"fir_"+div+".idx "+pathInd+"fir_"+div+".[comp]idx"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
	
		comando = "cp "+pathInd+"sec_"+div+".idx "+pathInd+"sec_"+div+".[comp]idx"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
		
		comando = "./geraSkiplistBlockMaxFIX "+pathInd+"fir_"+div+". 3"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
		
		comando = "./geraSkiplistBlockMaxFIX "+pathInd+"sec_"+div+". 3"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
		
		comando = "cp "+pathInd+"fir_"+div+".idx "+pathInd+"fir_"+div+".[nComp]idx"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
	
		comando = "cp "+pathInd+"sec_"+div+".idx "+pathInd+"sec_"+div+".[nComp]idx"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
		
		comando = "cp "+pathInd+"sec_"+div+".max_score   "+pathInd+"fir_"+div+".min_score"
		print "\n==>"+comando
		subprocess.call(comando, shell=True)
		
		break
			
arq.close()
	
	
