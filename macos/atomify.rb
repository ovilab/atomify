class Atomify < Formula
  desc "Live LAMMPS visualizer"
  homepage "http://ovilab.net/atomify"
  url "https://github.com/ovilab/atomify/releases/download/v2.0.10/atomify-2.0.10-src.tar.gz"
  version "v2.0.10"
  sha256 "faf07966b1bc8b1dd2fddc8f392b48b1ad55c6240e585d41f80f54dc05fff864"
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
  end

  test do
    assert_equal "2.0.10", shell_output("#{prefix}/atomify.app/Contents/MacOS/atomify --version")
  end
end
