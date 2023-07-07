include ../.make.conf


all: BRASIS.Nucleo
	@$(MAKE) -C Modulos all

clean:
	@-rm BRASIS.Nucleo
	@-rm *.o
	@$(MAKE) -C Modulos clean

BRASIS.Nucleo: Inicial.o ES.o Brasis.o Debug.o Memoria.o
	@echo -= $@ =-
	@$(LDBIN) $@ -map $@.map $^
	@cp BRASIS.Nucleo $(ROOT)Distro/Temp/BRASIS.Nucleo