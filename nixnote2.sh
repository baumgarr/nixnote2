#! /bin/sh

eval $1

###########################################
# Location variables.  Edit the variables #
# below to your specific installation.    #
# The ones below are examples only.       #
###########################################

NIXNOTE=$(cd `dirname $0` && pwd)

#Do any parameter overrides
while [ -n "$*" ]
do
   eval $1
   shift
done

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NIXNOTE/lib
###################
# Run the program #
###################
./nixnote
