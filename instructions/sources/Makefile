

all: ../FOSDEM_AV_manual.pdf ../FOSDEM_AV_manual.epub ../FOSDEM_Room_setup_checklist.pdf dm-build-timestamp dc-build-timestamp

depend: .depend

.depend: $(wildcard *.tex) makedep.sh
	rm -f .depend
	./makedep.sh > .depend

-include .depend

../FOSDEM_AV_manual.pdf: FOSDEM_AV_manual.tex
	latexmk -pdf -pdflatex="pdflatex -interaction=nonstopmode" FOSDEM_AV_manual.tex
	mv FOSDEM_AV_manual.pdf ..

../FOSDEM_AV_manual.epub: FOSDEM_AV_manual.tex
	mk4ht htlatex FOSDEM_AV_manual.tex 'xhtml,charset=utf-8,pmathml' ' -cunihtf -utf8 -cvalidate'
	ebook-convert FOSDEM_AV_manual.html ../FOSDEM_AV_manual.epub  --language en --no-default-epub-cover

../FOSDEM_Room_setup_checklist.pdf: FOSDEM_Room_setup_checklist.md
	pandoc -f markdown --variable papersize=a4paper -V geometry:margin=1cm -t latex  -o ../FOSDEM_Room_setup_checklist.pdf FOSDEM_Room_setup_checklist.md

dm-build-timestamp: .depend makedep.sh tmpl.my FOSDEM_Devroom_manual.md 
	pandoc -f markdown -t latex --variable papersize=a4paper  --variable classoption=twocolumn  --variable columnsep=1in -V geometry:margin=2cm FOSDEM_Devroom_manual.md -o ../FOSDEM_Devroom_manual.pdf
	touch dm-build-timestamp

dc-build-timestamp: .depend makedep.sh populate-devroom-setup-checklist.sh room_info.json FOSDEM_Room_setup_checklist.md
	bash populate-devroom-setup-checklist.sh room_info.json
	touch dc-build-timestamp

clean:
	rm -f dm-build-timestamp dc-build-timestamp ../checklists/* ../FOSDEM_AV_manual.pdf ../FOSDEM_AV_manual.epub ../FOSDEM_Room_setup_checklist.pdf .depend FOSDEM_AV_manual.4ct FOSDEM_AV_manual.css  FOSDEM_AV_manual.dvi FOSDEM_AV_manual.html FOSDEM_AV_manual.idv FOSDEM_AV_manual.lg FOSDEM_AV_manual.tmp FOSDEM_AV_manual.xref  FOSDEM_AV_manual0x.png
	latexmk -C
