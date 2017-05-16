class Atomify < Formula
  desc "Live LAMMPS visualizer"
  homepage "http://ovilab.net/atomify"
  url "https://github.com/ovilab/atomify/releases/download/v2.0.7/atomify-2.0.7-macos.tar.gz"
  version "v2.0.7"
  sha256 "f2d9e004c1c805a9d0cd4762916523eeef919089fc9bedf9b13a75ff2de6c6f8"
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
