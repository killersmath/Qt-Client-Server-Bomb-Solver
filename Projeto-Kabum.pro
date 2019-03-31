#TARGET = Projeto-Kabum
TEMPLATE = subdirs
config += ordered

SUBDIRS = \
          Server \
          Client

Server.subdir = src/Server
Client.subdir = src/Client

