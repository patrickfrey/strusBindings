#!/bin/bash

ls *.java | sed 's/.java//' | awk '{print "%typemap(\"javapackage\") " $1 ", " $1 "*, " $1 "& \"net.strus.api." $1 "\"" };' > namespace.i

