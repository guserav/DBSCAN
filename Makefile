.PHONY: clean

StudienArbeit:
	cmake -Bbuild -H.
	cmake --build build --target all

clean:
	rm -rf build/
