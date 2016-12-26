LATEX = pdflatex

all: TMVAUsersGuide.pdf

TMVAUsersGuide.pdf: TMVAUsersGuide.tex *.tex plots/*.pdf optiontables/*.tex pymva/*.tex optiontables/pymva/*.tex
	$(LATEX) TMVAUsersGuide.tex
	makeindex TMVAUsersGuide.idx
	$(LATEX) TMVAUsersGuide.tex	
	$(LATEX) TMVAUsersGuide.tex

#TMVAUsersGuide.pdf: TMVAUsersGuide.dvi
#	dvipdf TMVAUsersGuide.dvi

clean: 
	rm -rf TMVAUsersGuide.pdf TMVAUsersGuide.aux TMVAUsersGuide.idx TMVAUsersGuide.ilg TMVAUsersGuide.ind TMVAUsersGuide.log TMVAUsersGuide.out TMVAUsersGuide.toc

