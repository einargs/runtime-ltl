{ stdenv, fetchTarball, python37 }:

stdenv.mkDerivation {
  name = "spot-2.7.1";
  buildInputs = [
    python37 # should be 3.3 or later
  ];
  src = fetchTarball {
    name = "spot-2.7.1-src";
    url = http://www.lrde.epita.fr/dload/spot/spot-2.7.1.tar.gz;
    sha256 = "16waxdf8b6l50z51xxmf6p2c3jf02c4pdxyy6sld6vd3wam9fkym";
  };
}
