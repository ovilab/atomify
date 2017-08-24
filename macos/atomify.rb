class Atomify < Formula
  desc "Live LAMMPS visualizer"
  homepage "http://ovilab.net/atomify"
  url "https://github.com/ovilab/atomify/releases/download/v2.1.0/atomify-2.1.0-src.tar.gz"
  version "v2.1.0"
  sha256 "103e03bc82499cb3d499fe88e41f72a2b5c335fbeb7831797e1c328d0ae8ebc3"
  head "https://github.com/ovilab/atomify.git", :using => :git, :branch => :dev
  depends_on "qt"
  depends_on "gcc"

  def install
    gcc_suffix = Formula["gcc"].version.to_s.slice(/\d/)
    gcc_binary = "#{Formula["gcc"].bin}/g++-"+gcc_suffix

    system "python", "configure.py", "brew", gcc_binary

    system "#{Formula["qt"].bin}/qmake", "PREFIX=#{prefix}", "QMAKE_CXXFLAGS+=-fopenmp", "QMAKE_LFLAGS+=-fopenmp", "QMAKE_CXX="+gcc_binary, "QMAKE_LINK="+gcc_binary, "-spec", "#{Formula["qt"].bin}/../mkspecs/macx-g++"
    system "make", "-j#{ENV.make_jobs}"
    system "make", "install"
  end

  def post_install
    ln_sf "#{prefix}/atomify.app/Contents/MacOS/atomify", "#{HOMEBREW_PREFIX}/bin/atomify"
    ln_sf "#{prefix}/atomify.app/Contents/MacOS/atomify", "#{HOMEBREW_PREFIX}/bin/lmp_atomify"
  end

  test do
    assert_equal "2.1.0", shell_output("#{prefix}/atomify.app/Contents/MacOS/atomify --version")
  end
end
