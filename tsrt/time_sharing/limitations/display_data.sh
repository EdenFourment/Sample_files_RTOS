#! /bin/sh

if [ $# -lt 2 ]
then
	echo "usage: $0 <histogram-file> <title> [<output>]"
	exit 1
fi

if [ $# -eq 3 ]
then
	output="$3"
else
	output="| display png:-"
fi

gnuplot <<- EOF
	set terminal png size 800,600 
	set title "$2"
	set output '$output'
	set xlabel "occurence #"
	set ylabel "delay between 2 occurences (microsecond)"
	# Echelle logarithmique en ordonnee
	#set logscale y 10
	# Remplissage par rayures
	#set style fill pattern 2
	# Afficher le contenu du fichier
	plot [] [] "$1" notitle with lines
	# Attendre click de souris
	pause mouse
EOF

#contourne le bug sh: 1: display: not found

if [ $# -eq 3 ]
then
	eog $3
fi
