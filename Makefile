
.PHONY: all lib clean

all: 
	cd ranlib; $(MAKE) all
	cd general; $(MAKE) all
	cd Elt; $(MAKE) all
	cd Alphabet; $(MAKE) all
	cd Graph; $(MAKE) all
	cd Group; $(MAKE) all
	cd SbgpFG; $(MAKE) all
	cd FreeGroup; $(MAKE) all
	cd HigmanGroup; $(MAKE) all
	cd BraidGroup; $(MAKE) all
	cd CryptoKL; $(MAKE) all
	cd CryptoAAG; $(MAKE) all
	cd CryptoAE; $(MAKE) all
	cd CryptoShftConj; $(MAKE) all
	cd CryptoTripleDecomposition; $(MAKE) all
	cd Equation; $(MAKE) all
	cd Maps; $(MAKE) all
	cd Examples; $(MAKE) all
	cd Graphics; $(MAKE) all 
	cd StringSimilarity; $(MAKE) all
	cd TheGrigorchukGroup; $(MAKE) all
lib:
	cd ranlib; $(MAKE) lib
	cd general; $(MAKE) lib
	cd Elt; $(MAKE) lib
	cd Alphabet; $(MAKE) lib
	cd Graph; $(MAKE) lib
	cd Group; $(MAKE) all
	cd SbgpFG; $(MAKE) lib
	cd FreeGroup; $(MAKE) lib
	cd HigmanGroup; $(MAKE) all
	cd BraidGroup; $(MAKE) lib
	cd CryptoKL; $(MAKE) lib
	cd CryptoShftConj; $(MAKE) lib
	cd CryptoAAG; $(MAKE) lib
	cd CryptoAE; $(MAKE) lib
	cd CryptoTripleDecomposition; $(MAKE) lib
	cd Equation; $(MAKE) lib
	cd Examples; $(MAKE) lib
	cd Graphics; $(MAKE) lib 
	cd StringSimilarity; $(MAKE) lib
	cd Maps; $(MAKE) lib
	cd TheGrigorchukGroup; $(MAKE) lib
clean:
	rm -f -R Release
	cd ranlib; $(MAKE) clean
	cd general; $(MAKE) clean
	cd Elt; $(MAKE) clean
	cd Alphabet; $(MAKE) clean
	cd Graph; $(MAKE) clean
	cd Group; $(MAKE) clean
	cd SbgpFG; $(MAKE) clean
	cd FreeGroup; $(MAKE) clean
	cd HigmanGroup; $(MAKE) all
	cd BraidGroup; $(MAKE) clean
	cd CryptoKL; $(MAKE) clean
	cd CryptoAAG; $(MAKE) clean
	cd CryptoAE; $(MAKE) clean
	cd CryptoShftConj; $(MAKE) clean
	cd CryptoTripleDecomposition; $(MAKE) clean
	cd Equation; $(MAKE) clean
	cd Examples; $(MAKE) clean
	cd Experiments; $(MAKE) clean
	cd Maps; $(MAKE) clean
	cd Examples; $(MAKE) clean
	cd Graphics; $(MAKE) clean
	cd StringSimilarity; $(MAKE) clean
	cd TheGrigorchukGroup; $(MAKE) clean
	cd python; $(MAKE) clean
