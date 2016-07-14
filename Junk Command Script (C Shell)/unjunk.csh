#!/bin/csh

# Grant Kimes
# ECE322 HW2: unjunk script
# 9/30/15

set junkdir = "~/.junk"
set unjunkdir = `pwd`

if (! -d $junkdir) then
  echo "$junkdir does not exist."
  exit 1
endif

set args = `getopt d: $*`

foreach i ($args)
  switch($i)
    
    ## If -d <dir> is given, set that to $unjunkdir.
    case "-d":
      shift args
      if (! -d $args[1]) then
        echo "Making new unjunkdir $args[1]."
        mkdir $args[1]
      endif
      set unjunkdir = $args[1]
      shift args
      breaksw

    ## If end of arguments, shift and break.
    case "--":
      shift args
      break
  endsw
end

## Iterate through given filenames, and unjunk if it's a valid file or directory in junkdir.
foreach i ($args)
  if (-f $junkdir/$i || -d $junkdir/$i) then
    echo "Unjunking $i to $unjunkdir"
    mv $junkdir/$i $unjunkdir
  else
    echo "$i does not exist in junk folder."
  endif
end
