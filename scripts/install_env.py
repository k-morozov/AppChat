import os
from sys import platform
import requests
import tarfile
import subprocess as sub

boost_version_major = 1
boost_version_minor = 73
boost_version_patch = 0

class BoostInstaller:
  def __init__(self):
      self.boost_libs = ["system", "date_time", "test", "log"]
      self.b2_options = "link=static runtime-link=shared"

class LinuxBoostInstaller(BoostInstaller):
  def install(self, path):
    command_1 = "cd " + path
    command_2 = "sudo ./bootstrap.sh --prefix=/usr/local --with-libraries={}".format(','.join(self.boost_libs))
    command_3 = "sudo ./b2 {} install".format(self.b2_options)

    os.system("{} && {} && {}".format(command_1, command_2, command_3))

class Win32BoostInstaller(BoostInstaller):
  def install(self, path):
    boost_root = "C\\:Boost\\"
    boost_includedir = "{}include\\boost_{}_{}_{}\\".format(boost_root, boost_version_major, boost_version_minor, boost_version_patch)
    
    command_1 = "cd " + path
    command_2 = "bootstrap.bat gcc --with-libraries={}".format(','.join(self.boost_libs))
    command_3 = "b2.exe toolset=gcc {} install".format(self.b2_options)
    command_4 = "setx /m BOOST_ROOT {}".format(boost_root)
    command_5 = "setx /m BOOST_INCLUDEDIR {}".format(boost_includedir)

    os.system("{} && {} && {} && {} && {}".format(command_1, command_2, command_3, command_4, command_5))
    

def MakeBoostInstaller(platform):
  if platform == "linux":
    return LinuxBoostInstaller()
  elif platform == "win32":
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
