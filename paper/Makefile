LATEXPARAM=
texmaster=Thesis

all: build 

build: 
	pdflatex ${LATEXPARAM} ${texmaster}
	bibtex ${texmaster}
	pdflatex ${LATEXPARAM} ${texmaster}
	pdflatex ${LATEXPARAM} ${texmaster} # Second run for resolving references
	rm *.aux *.bbl *.blg *.lof *.log *.lot *.out *.toc 

clean: clean-with-pdf

clean-without-pdf:
	rm -rf *.ps *.dvi *.log *.tmp *.out *.aux *.bbl *.blg *.toc *.lof *.lot *~

clean-with-pdf:
	rm -rf *.pdf *.ps *.dvi *.log *.tmp *.out *.aux *.bbl *.blg *.toc *.lof *.lot *~
