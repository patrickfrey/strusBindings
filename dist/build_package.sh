#!/bin/sh

# DEBIAN
PACKAGEID="strusbindings-0.0"

cd pkg/$PACKAGEID
dpkg-buildpackage

