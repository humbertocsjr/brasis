include ../../.make.conf


all: BRASIS.Imagem

BRASIS.Imagem: MinixFS.Modulo Disquete.Modulo ISADMA.Modulo PS2.Modulo Modulos.cfg
	@echo -= $@ =-
	@ar -rf $@ *.Modulo Modulos.cfg
	@cp $@ $(ROOT)Distro/Temp/$@

clean:
	@-rm *.Modulo
	@-rm *.Imagem
