.PHONY: clean

debug:
	cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug -H.
	cmake --build build --target all

release:
	cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -H.
	cmake --build build --target all

clean:
	rm -rf build/
