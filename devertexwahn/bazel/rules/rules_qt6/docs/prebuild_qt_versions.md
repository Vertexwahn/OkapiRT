# Prebuilt Qt versions

As mentioned in the [README.md](../README.md) a prebuilt version of Qt is fetched from [vertexwahn.de](https://vertexwahn.de/).

This document explains how the archive of the prebuilt version is generated.

## Windows x64

*Powershell*

```shell
python3 -m pip install aqtinstall
aqt list-qt windows desktop --arch 6.4.0
aqt install-qt windows desktop 6.4.0 win64_msvc2019_64 -m all
```

*Bash using Ubuntu to prepare Qt for Windows build*

```shell
cd ~
mkdir Qt
pip3 install aqtinstall
cd $HOME/Qt
aqt install-qt windows desktop 6.4.0 win64_msvc2019_64 -m all
tar cf - "6.4.0" | xz -z - > qt_windows_x86_64.tar.xz
```

## Ubuntu x64

```shell
cd ~
mkdir Qt
pip3 install aqtinstall
cd $HOME/Qt
aqt list-qt linux desktop --arch 6.4.0
python3 -m aqt install --outputdir $HOME/Qt 6.4.0 linux desktop
tar cf - "6.4.0" | xz -z - > qt_linux_x86_64.tar.xz
```

## macOS 12 Intel

```shell
pip install aqtinstall
aqt list-qt mac desktop --arch 6.2.4
aqt install-qt mac desktop 6.2.4 clang_64
# This is currently not done - but needs also to be done to get it working:
cd 6.2.4/include
ln -s ../lib/QtCore.framework/Headers QtCore
ln -s ../lib/QtWidgets.framework/Headers QtWidgets
ln -s ../lib/QtGui.framework/Headers QtGui
# zip it
mv 6.2.4.zip qt_6.2.4_mac_desktop_clang_64.zip

brew install qt@6 # install qt 6.2.3_1 monterey
cd .../external/qt_6.2.4_mac_desktop_clang_64/include
```
