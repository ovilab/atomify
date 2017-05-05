class Atomify < Formula
  desc "Live LAMMPS visualizer"
  homepage "http://ovilab.net/atomify"
  url "https://github.com/ovilab/atomify/releases/download/v2.0.6/atomify.tar.gz"
  version "2.0"
  sha256 "10ecc1209b51142ba13868477b0f53107c1c2b44eae75312c7a2f35d225ffd33"
  head "https://github.com/ovilab/atomify.git", :using => :git, :branch => :dev
  depends_on "qt"

  def install
    system "python", "configure.py"
    system "qmake", "atomify.pro", "PREFIX=#{prefix}"
    system "make", "-j#{ENV.make_jobs}"
    system "make", "install"
  end

  def post_install
    ln_sf "#{prefix}/atomify.app/Contents/MacOS/atomify", "#{HOMEBREW_PREFIX}/bin/atomify"
  end

  test do
    assert_equal "2.0", shell_output("#{prefix}/atomify.app/Contents/MacOS/atomify --version")
  end
end
