#!/bin/sh

grep '<heading ' xml/*.xml | awk '{print $3}' | sed 's/^id="//' | sed 's/".*//' | sed 's/_/-/' > /tmp/hids.txt
grep '<title ' xml/*.xml | grep id | awk '{print $2}' | sed 's/^id="//' | sed 's/".*//' | sed 's/_/-/' >> /tmp/hids.txt


