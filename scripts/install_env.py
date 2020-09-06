import os
from sys import platform
import requests
import tarfile


##############################################################################
#                               BOOST INSTALL                                #
##############################################################################

boost_version_major = 1
boost_version_minor = 73
boost_version_patch = 0

class BoostInstaller:
  def __init__(self):
    self.boost_libs = ["system", "date_time", "log", "test", "program_options"]
    self.b2_options = "link=static runtime-link=shared -q -j2 threading=multi {}".format(' '.join(list(map(lambda x: '--with-{}'.format(x), self.boost_libs))))

class LinuxBoostInstaller(BoostInstaller):
  def install(self, path):
    commands = [
      "cd " + path,
      "sudo ./bootstrap.sh --prefix=/usr/local",
      "sudo ./b2 {} install".format(self.b2_options)
    ]

    os.system(" && ".join(commands))

class Win32BoostInstaller(BoostInstaller):
  def install(self, path):
    commands = [
      "cd " + path,
      "bootstrap.bat gcc",
      "b2.exe toolset=gcc {} install".format(self.b2_options)
    ]

    os.system(" && ".join(commands))

def MakeBoostInstaller(platform):
  if platform == "linux":
    return LinuxBoostInstaller()
  if platform == "win32":
    return Win32BoostInstaller()

print("Platform: " + platform)

boost_filename = "boost_{}_{}_{}.tar.gz".format(boost_version_major, boost_version_minor, boost_version_patch)
boost_url = "https://sourceforge.net/projects/boost/files/boost/{}.{}.{}/{}/download".format(boost_version_major, boost_version_minor, boost_version_patch, boost_filename)

print("Download boost . . .")

r = requests.get(boost_url, allow_redirects=True)

open(boost_filename, "wb+").write(r.content)

print("Unzip boost . . .")

tar = tarfile.open(boost_filename, "r:gz")
tar.extractall()
tar.close()

print("Install boost . . .")

boost_installer = MakeBoostInstaller(platform)

boost_dir = os.getcwd() + "/boost_{}_{}_{}".format(boost_version_major, boost_version_minor, boost_version_patch)
boost_installer.install(boost_dir)

print("Finish")


##############################################################################
#                            PROTOBUF INSTALL                                #
##############################################################################

class ProtobufInfo:
  def __init__(self, version):
    self.version = version
    self.filename = "protobuf-cpp-{o.version}.tar.gz".format(o=self)

  def get_download_url(self):
    return "https://github.com/protocolbuffers/protobuf/releases/download/v{o.version}/{o.filename}".format(o=self)

  def get_dirname(self):
    return "protobuf-{o.version}".format(o=self)


class LinuxProtobufInstaller:
  def install(self, protobuf_info):
    commands = [
      "mkdir protobuf-build",
      "cd protobuf-build",
      "sudo cmake ../{}/cmake".format(protobuf_info.get_dirname()),
      "sudo make && sudo make check && sudo make install",
      "sudo ldconfig"
    ]

    os.system(" && ".join(commands))

class Win32ProtobufInstaller:
  def install(self, protobuf_info):
    commands = [
      "mkdir protobuf-build",
      "cd protobuf-build",
      "cmake.exe ..\\{}\\cmake -G \"MinGW Makefiles\"".format(protobuf_info.get_dirname()),
      "mingw32-make.exe && mingw32-make.exe check && mingw32-make.exe install"
    ]

    os.system(" && ".join(commands))

def MakeProtobufInstaller():
  if platform == "linux":
    return LinuxProtobufInstaller()
  if platform == "win32":
    return Win32ProtobufInstaller()


protobuf_info = ProtobufInfo("3.13.0")

print("Download protobuf . . .")

r = requests.get(protobuf_info.get_download_url(), allow_redirects=True)

open(protobuf_info.filename, "wb+").write(r.content)


print("Unzip protobuf . .  .")

tar = tarfile.open(protobuf_info.filename, "r:gz")
tar.extractall()
tar.close()

print("Install protobuf . . .")

protobuf_installer = MakeProtobufInstaller()
protobuf_installer.install(protobuf_info)

print("Finish")
