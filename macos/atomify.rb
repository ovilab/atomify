class Atomify < Formula
  desc "Live LAMMPS visualizer"
  homepage "http://ovilab.net/atomify"
  url "https://github.com/ovilab/atomify/releases/download/v2.0.9/atomify-2.0.9-src.tar.gz"
  version "v2.0.9"
  sha256 "2349fceee2be7afbc665b3b2a14097eed472c565eb3a0352714e8f7b0353be9c"
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
    assert_equal "2.0.9", shell_output("#{prefix}/atomify.app/Contents/MacOS/atomify --version")
  end
end
