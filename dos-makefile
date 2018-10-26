.PHONY: buildnrun run

all:
	dosbox -c "cd C:\DEV\BALLOON" -c "make -f MKTURBOC"
buildnrun:
	dosbox -c "cd C:\DEV\BALLOON" -c "make -f MKTURBOC" -c "balloon.exe"

run:
	dosbox -c "cd C:\DEV\BALLOON" -c "balloon.exe"

clean: preclean
	rm -f *.EXE
	rm -f *.OBJ
	rm -f *.DSK
	rm -f *.ERR
	rm -f *.MAP
	rm -f *.LK1
	rm -f *.SWP
	rm -f *.CFG
	rm -f MAKELOG