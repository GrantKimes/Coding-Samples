#! /bin/csh

# ECE322 HW2: Junk C Shell script
# Grant Kimes
# 9/30/15

set junkdir = "~/.junk"

# If ~/.junk directory does not exist, make it.
if ( ! -d $junkdir ) then
  echo "Making $junkdir"
  mkdir $junkdir
endif

set args=`getopt lp $*`

foreach i ($args)
  switch ($i)
    ## List contents of junk folder, shift args.
    case "-l":
      echo "Contents of junk:"
      ls $junkdir
      shift args
      breaksw

    ## Purge junk folder, shift args.
    case "-p":
      set junkfiles = `ls ~/.junk`
      foreach filename ($junkfiles)
        rm $junkdir/$filename
      end
      echo "Purged junk folder."
      shift args
      breaksw

    ## Shift args.
    case "--":
      shift args
      breaksw

    default:
      breaksw
  endsw
end

## Iterate through file list. If valid file or directory, move to junk.
foreach file ($args)
  if (-f $file || -d $file) then
    mv $file $junkdir
    echo "Moved $file to junk."
  else
    echo "$file is not a valid file to junk."
  endif
end

