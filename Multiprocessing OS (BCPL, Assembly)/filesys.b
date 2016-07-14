import "io"

manifest
{ // indexes for file system struct
  fs_discnum = 0,
  fs_numblocks = 1,
  fs_rootstart = 2,
  fs_rootsize = 3,
  fs_ffblock = 4,
  fs_rootdir = 5,
  fs_discname = 6,
  fs_lastupdate = 7,
  sizeof_fs = 8
}

manifest 
{ // indexes for superblock data
  sb_discname = 0,
  sb_namesize = 31,
  sb_rootstart = 8,
  sb_rootsize = 9,
  sb_ffblock = 10,
  sb_lastupdate = 11
}

manifest
{ // indexes for root directory entry data
  rde_name = 0,
  rde_namesize = 22,
  rde_status = byte 23,
	rdes_free = 0,
	rdes_exists = 1,
  rde_ffblock = 6,
  rde_length = 7,
  sizeof_rde = 8
}

manifest
{ // indexes for file information data
  fi_status = 0,
  fi_number = 1,
  fi_ffblock = 2,
  fi_length = 3,
  sizeof_fi = 4
}

manifest
{
  blocks_per_disc = 6000,
  words_per_block = 128
}

let ins(addr, num_chars) be
{
  let index = 0, c;
  while index < num_chars do
  {
    c := inch();
    if c = '\n' then
      break;
    byte index of addr := c;
    index +:= 1;
  }
  byte index of addr := 0;
  resultis addr;
}

let string_compare(string, actual) be
{
  // return 1 if strings are the same, 0 otherwise
  let index = 0;
  let string_size = strlen(actual);
  while index < string_size do
  {
    test (byte index of string) = (byte index of actual) \/ (byte index of string) = (byte index of actual) then
    {
      test (byte index of string) = 0  then
        resultis 1      // strings are equal
      else index +:= 1;
    }
    else test (byte index of actual) = 0 /\ (byte index of string) = ' ' then
      resultis 1
    else resultis 0;    // not equal
  }
}

let clear_vec(arr_addr, size) be
{
  for i = 0 to size-1 do
    arr_addr ! i := 0;
}

let copy_string(addr, dest) be 
{
  let n = strlen(addr)/4 + 1;
  for i = 0 to n-1 do
    dest ! i := addr ! i;
}

let create_superblock(fs) be
{    // write metadata about file system from memory to disc
  let r,
  sb = vec(words_per_block);
  clear_vec(sb, words_per_block);
  
  copy_string(fs ! fs_discname, sb + sb_discname); // + because want the address, not following the ptr

  sb ! sb_rootstart := fs ! fs_rootstart;
  sb ! sb_rootsize := fs ! fs_rootsize;
  sb ! sb_ffblock := fs ! fs_ffblock;
  
  r := devctl(DC_DISC_WRITE, fs ! fs_discnum, 0, 1, sb);
  if r < 0 then
    out("Error %d writing superblock to disc.\n", r);
  resultis r;
}

let read_superblock(fs) be
{
  let r,
  sb = vec(words_per_block);
  clear_vec(sb, words_per_block);
  
  r := devctl(DC_DISC_READ, fs ! fs_discnum, 0, 1, sb);
  if r < 0 then 
  { out("Error %d reading superblock from disc.\n", r);
    resultis r; }
  fs ! fs_discname := sb + sb_discname; // + because want the address 
  fs ! fs_rootstart := sb ! sb_rootstart;
  fs ! fs_rootsize := sb ! sb_rootsize;
  fs ! fs_ffblock := sb ! sb_ffblock;
  resultis r;
}

let create_rootdir(fs) be
{
  let r = devctl(DC_DISC_WRITE, fs ! fs_discnum, fs ! fs_rootstart, fs ! fs_rootsize, fs ! fs_rootdir);
  if r < 0 then
    out("Error %d writing root directory to disc.\n", r);
  resultis r;
}

let read_rootdir(fs) be
{
  let rd = newvec(fs ! fs_rootsize * words_per_block);
  let r = devctl(DC_DISC_READ, fs ! fs_discnum, fs ! fs_rootstart, fs ! fs_rootsize, fs ! fs_rootdir);
  if r < 0 then
  { out("Error %d reading root directory from disc.\n", r);
    freevec(rd); }
  fs ! fs_rootdir := rd;
  resultis r;
}

let format(fs) be
{
  let x, r, n, s,
  name = newvec(sb_namesize/4 - 1),
  rootdirarr;

  out("Disc number to load?: ");
  n := inno();
  x := devctl(DC_DISC_CHECK, n);
  if (x < 0) then
    out("Error %d checking disc num %d\n", x, n);
  fs ! fs_discnum := n;
  fs ! fs_numblocks := x;

  out("Name of disc?: ");
  s := ins(name, sb_namesize);
  fs ! fs_discname := s;
  fs ! fs_rootstart := 1;

  out("Number of blocks for root directory?: ");
  n := inno();
  fs ! fs_rootsize := n;	// set root dir to size of 3 blocks
  fs ! fs_ffblock := 1 + n;

  rootdirarr := newvec(n * words_per_block);
  clear_vec(rootdirarr, n * words_per_block);
  fs ! fs_rootdir := rootdirarr;

  r := create_superblock(fs);
  if r > 0 then
    r := create_rootdir(fs);
  if r < 0 then
  {
    freevec(name);
    freevec(fs ! fs_rootdir);
    fs ! fs_discnum := 0;
  }
}

let load(fs) be
{
  let r, n;
  out("Disc number to load?: ");
  n := inno();
  r := devctl(DC_DISC_CHECK, n);
  if r < 0 then
    out("Error %d loading disc.\n", r);
  fs ! fs_discnum := n;
  fs ! fs_numblocks := r;

  r := read_superblock(fs);
  if r > 0 then
    r := read_rootdir(fs);
  if r < 0 then
  {
    freevec(fs ! fs_discname);
    fs ! fs_discnum := 0;
    return;
  }
  out("Loaded disc %d labeled '%s', first free block is %d.\n", fs ! fs_discnum, fs ! fs_discname, fs ! fs_ffblock);
}

let num_blocks(num_words) be { resultis (num_words/words_per_block + 1) } 


let start() be
{
  manifest { heapsize = 1000 }
  let x, command = vec(16);
  let heap = vec(heapsize);
  let fs = vec(sizeof_fs);
  init(heap, heapsize);

  out(">: Type 'format' or 'load': ");
  command := ins(command, 16*4);
  test string_compare(command, "format") then
    format(fs)
  else test string_compare(command, "load") then
    load(fs)
  else


  return;
}
