include ../../.make.conf


all: BRASIS.Imagem

BRASIS.Imagem: MinixFS.Modulo
	@echo -= $@ =-
	@find *.Modulo | cpio -oH cpio > $@ 2> /dev/null
	@cp $@ $(ROOT)Distro/Temp/$@

clean:
	@-rm *.Modulo
	@-rm *.Imagem
