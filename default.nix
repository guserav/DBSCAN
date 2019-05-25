with import <nixpkgs> {}; {
  qpidEnv = stdenvNoCC.mkDerivation {
    name = "dbscan";
    buildInputs = [
        gcc8
        glibc.static
        cmake
        valgrind
        doxygen
    ];
  };
}
