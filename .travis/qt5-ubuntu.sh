#  The MIT License (MIT)
#
#  Copyright (c) 2016 LRFLEW
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.

QT5_MAJOR=9
QT5_MINOR=0
QT5_PATCH=0-201704251215 # beta 

QT5_LINUX_VERSION=Linux-RHEL_7_2
QT5_LINUX_VERSION_2=linux-Rhel7.2

if (( $# < 2 )); then
  echo "Usage: <destination> <component> [component...]"
  (( "$#" == 0 ))
  exit $?;
fi

for x in ${@:2}; do
  if [[ $x = extra-* ]]; then
    DOWNLOAD_URL="https://download.qt.io/online/qtsdkrepository/linux_x64/desktop/qt5_5${QT5_MAJOR}/qt.5${QT5_MAJOR}.qt${x/#extra-/}.gcc_64/5.${QT5_MAJOR}.${QT5_MINOR}-${QT5_PATCH}qt${x/#extra-/}-${QT5_LINUX_VERSION}-GCC-${QT5_LINUX_VERSION}-X86_64.7z"
  else
    DOWNLOAD_URL="https://download.qt.io/online/qtsdkrepository/linux_x64/desktop/qt5_5${QT5_MAJOR}/qt.5${QT5_MAJOR}.gcc_64/5.${QT5_MAJOR}.${QT5_MINOR}-${QT5_PATCH}qt${x}-${QT5_LINUX_VERSION}-GCC-${QT5_LINUX_VERSION}-X86_64.7z"
  fi

  DOWNLOAD_CHECK=$(curl -f ${DOWNLOAD_URL}.sha1 2>/dev/null)
  if [[ -z $DOWNLOAD_CHECK ]]; then
    echo ${DOWNLOAD_URL}
    echo "ERROR: Unknown package $x"
    exit 1
  fi

  curl -L -o /tmp/qt5.7z $DOWNLOAD_URL
  DOWNLOAD_HASH=$(shasum -a 1 /tmp/qt5.7z)
  if [[ $DOWNLOAD_HASH != $DOWNLOAD_HASH ]]; then
    echo "ERROR: Hash missmatch for $x" 1>&2
    exit 1
  fi

  7z x -aoa "-o$1" /tmp/qt5.7z 1>&2
  rm /tmp/qt5.7z
done

DOWNLOAD_URL="https://download.qt.io/online/qtsdkrepository/linux_x64/desktop/qt5_5${QT5_MAJOR}/qt.5${QT5_MAJOR}.gcc_64/5.${QT5_MAJOR}.${QT5_MINOR}-${QT5_PATCH}icu-${QT5_LINUX_VERSION_2}-x64.7z"
curl -L -o /tmp/qt5.7z $DOWNLOAD_URL
7z x -aoa "-o$1" /tmp/qt5.7z 1>&2
rm /tmp/qt5.7z

# Minimal Qt Configuration File
echo "[Paths]" > $1/5.${QT5_MAJOR}/gcc_64/bin/qt.conf
echo "Prefix=.." >> $1/5.${QT5_MAJOR}/gcc_64/bin/qt.conf

# Why does Qt default to Enterprise Licence?
sed -i 's/^[[:space:]]*QT_EDITION[[:space:]]*=.*$/QT_EDITION = OpenSource/' $1/5.${QT5_MAJOR}/gcc_64/mkspecs/qconfig.pri

echo $1/5.${QT5_MAJOR}/gcc_64/bin
