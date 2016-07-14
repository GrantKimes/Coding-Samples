import "io"
import "os1"

manifest
{
  string_size = 64,		// max string size is 64 characters / 16 words
  ptn_bits = selector 10 : 22,
  pn_bits = selector 11 : 11,
  poffset_bits = selector 11 : 0
}

manifest
{
  // indexes in PCB for a process
  pid = 0x401,
  pstate = 0x402,
  pbuff_index = 0x403,
  pbuff = 0x404,
  pregisters = 0x420,
  pflags = 0x421,
  ppc = 0x425,
  pfp = 0x426,
  psp = 0x427,

  // states for a process (in pstate)
  pinit = 0,
  prunning = 1,
  prunnable = 2,
  pwaiting = 3,
  psleeping = 4
}

static
{
  next_pid = 1
}

let str_comp(string, actual) be
{
  // return 1 if strings are the same, 0 otherwise
  let index = 0;
  while index < string_size do
  {
    test (byte index of string) = (byte index of actual) \/ (byte index of string) = (byte index of actual)-('a'-'A') then
    {
      test (byte index of string) = 0  then
        resultis 1	// strings are equal
      else index +:= 1;
    }
    else test (byte index of actual) = 0 /\ (byte index of string) = ' ' then
      resultis 1
    else resultis 0;	// not equal
  }
}

let help() be
{
  out("Commands are:\n");
  out("\texit \t\t\t- Exit program\n");
  out("\techo string \t\t- Prints 'string' in echo formation\n");
  out("\twhat time is it \t- Prints time\n");
  out("\trun filename.exe \t- Runs executable program 'filename'\n");
  out("\thelp \t\t\t- Lists commands\n");
}

let exit() be
{
  out("Goodbye.\n");
  finish;
}

let echo(s) be
{
  let i, q = 5;	// place in string to start after "echo "
  out("Echo... \n");
  for j = 4 to 0 by -2 do
  {
    for k = 0 to j do
    {
      i := q;
      while true do 
      {
        if (byte i of s = 0) then
          break;
        outch(byte i of s);
        i +:= 1;
      }
      out("\t\t");
    }
    out("\n");
  }
}

let what_time_is_it() be
{
  let t = seconds(), v = vec 7;
  datetime(t, v);
  out("Date and time: %d/%d/%d, %d:%d\n", v!1, v!2, v!0, v!4, v!5);
}

let print_page(va, n, s) be
{
  out(">: Print VAs: %s\n", s);
  for i = 0 to n-1 do
  {
    out("0x%x: 0x%x\n", va + i, va ! i);
  }
  out("...\n");
}

let get_page() be
{
  // p is the physical page number (follow pointer at address of infopage ! freelistptr)
  let p = !(infopage ! ifp_freelistptr);

  infopage ! ifp_freelistptr +:= 1;	// increments address of free list
  infopage ! ifp_freelistnum -:= 1;	// decrements number of pages left in free list

  // check if ran out of free pages
  if infopage ! ifp_freelistptr = infopage ! ifp_freelistempty then
  { out("Ran out of free pages\n");
    finish; } 

  // don't use physical page 1
  if p = 1 then 
    resultis get_page();
  resultis p;
}

let return_page(pg_num) be
{
  let page = pg_num << 11;
  out(">: Return page %d: \n", pg_num);
  debug 733;
  assembly
  {
	clrpp	[<page>]
  }
  infopage ! ifp_freelistptr -:= 1;
  ! (infopage ! ifp_freelistptr) := pg_num;
  infopage ! ifp_freelistnum +:= 1;
}

let create_process() be
{
    // make a new PCB, copy system info from original process
  let new_pgdir = (get_page() << 11) bitor 1,
  v = newvec (32);

  ptspec_va ! spec_p1e := new_pgdir;			// put new process page directory in VM
  p1_va ! 0x200 := pgdir_va ! 0x200;   			// copy system code page table
  p1_va ! 0x300 := pgdir_va ! 0x300; 			// copy system spec page table
  p1_va ! 0x2FF := pgdir_va ! 0x2FF;		   	// copy system stack page table

  p1_va ! pid := next_pid;	// set process id to next pid
  next_pid +:= 1;

  p1_va ! pstate := pinit;  	// set process state to initialize state

  // initialize registers in PCB (pc, fp, sp) for running user program
  p1_va ! ppc := 0x400;
  p1_va ! pfp := 0x80000000;
  p1_va ! psp := 0x80000000;
  p1_va ! pregisters := 40;
  assembly	// initialize flags register 
  {
	getsr	r1, $flags
	cbit	r1, $ip
	cbit	r1, $sys
	load	r2, 0x2000 + <pflags>
	loadh	r2, 0xC000
	store	r1, [r2]
  }

  // set buffer index to 0, set buffer to the address of a vector
  p1_va ! pbuff_index := 0;
  p1_va ! pbuff := v;

  // to add a character to the process character buffer
  //byte (pgdir_va ! pbuff_index) of (pgdir_va ! pbuff) := 'a';
  //pgdir_va ! pbuff_index +:= 1;

  proclist_va ! (p1_va ! pid) := new_pgdir;	// add new pcb (phys page addr) to the process list

  //out("New process: id=%d, pgdir=0x%x\n", p1_va ! pid, new_pgdir);
  resultis new_pgdir;
}

let switch_process() be
{
  let newproc;

  // move current process registers saved on stack to its pcb
  assembly
  {
	load	r0, 21
	load	r1, sp
	add	r1, 6	// add 6 to the stack because of parameters pushed for function call
	load	r2, 0x800+<pregisters>
	loadh	r2, 0xC000
	move	r1, r2
  }

  // find a process in the proc list that is runnable
  while true do
  {
    test (infopage ! ifp_curproc) + 1 = proclist_max then
      infopage ! ifp_curproc := 0
    else
      infopage ! ifp_curproc +:= 1;

    newproc := proclist_va ! (infopage ! ifp_curproc);
    test (newproc bitand 1) = 0 then
      loop
    else
    {
      ptspec_va ! spec_p1e := newproc;
      if (p1_va ! pstate) = prunnable then
      {
        ptspec_va ! 1 := newproc;	// set pgdir_va
        break;
      }
    }
  }

    // move new process's pcb to the register positions on stack
  assembly
  {
	load	r0, 21
	load	r1, sp
	add	r1, 6
	load	r2, 0x800+<pregisters>
	loadh	r2, 0xC000
	move	r2, r1

    // set pdbr to new process
	load	r1, [<newproc>]
	sub	r1, 1
	setsr	r1, $pdbr
  }
}

let run(filename) be
{
  let r, addr, num_bytes, avail, pos, pgnum,

  new_pgdir = create_process();		// get physical page of new process
  ptspec_va ! spec_p3e := new_pgdir;	// p3_va accesses new pcb

  out(">: Creating new process (%d) to run '%s': \n", p3_va ! pid, filename);

  // run .exe file onto tape
  r := devctl(DC_TAPE_LOAD, 1, filename, 'R');
  if r < 0 then
  { out(">: Error %d for loading tape '%s'\n", r, filename);
    return; }

  // set page table and first page in virtual memory for user code
  p3_va ! 0x0 := (get_page() << 11) bitor 1;		// first entry in new pcb is user code pt
  ptspec_va ! spec_p1e := p3_va ! 0x0;			// p1_va now accesses user code pt
  p1_va ! 0x0 := (get_page() << 11) bitor 1;		// first entry in user code pt is new page
  ptspec_va ! spec_p2e := p1_va ! 0x0;			// p2_va now accesses the first user code page
  addr := p2_va + 0x400;				// address in first user code page to start loading file

  // set page table and first page in virtual memory for user stack
  p3_va ! 0x1FF := (get_page() << 11) bitor 1;		// set page table
  ptspec_va ! spec_p4e := p3_va ! 0x1FF;
  p4_va ! 0x7FF := (get_page() << 11) bitor 1;		// set page

  // initialize variables for first tape read of user code
  num_bytes := 0;				// number of bytes read from tape
  avail := 8; 					// available blocks
  pgnum := 1;					// current page number in user code pt

  // reading from tape into virtual address of user code pages
  while true do
  {
    // read 128 (0x80) words from tape to virtual address addr
    r := devctl(DC_TAPE_READ, 1, addr);
    if r < 0 then
    { out(">: Error %d while reading tape '%s'\n", r, filename);
      return; }

    num_bytes +:= r;		// total number of bytes read so far
    avail -:= 1;		// 1 less available block
    addr +:= 128;		// increment addr to next block
    if r < 128 then break;	// if didn't read a full block, last one

    if avail = 0 then 		// there's no more blocks to read into, so get another page
    {
      p1_va ! pgnum := (get_page() << 11) bitor 1;	// sets new page in pt
      ptspec_va ! spec_p2e := p1_va ! pgnum;		// p2_va accesses new user code page
      addr := p2_va;					// now read in file to this VA (new user code page)
      pgnum +:= 1;					// increment to next page entry in page table
      avail := 16;					// 16 more blocks available
    }
  } 
  r := devctl(DC_TAPE_UNLOAD, 1);
  //out(">: %d bytes, %d page(s) loaded \n", num_bytes, pgnum);

  p3_va ! pstate := prunnable;	// can now be run when timer interrupt is called
}

let page_fault_handler(intcode, address, info) be
{
  let pt, 
  ptn = ptn_bits from address, 
  pn = pn_bits from address;

  out(">: Page fault handler at: 0x%x\n", address);
  debug 39;

  // check if there is a valid page table setup in pgdir for this address
  pt := pgdir_va ! ptn;
  if ~(pt bitand 1) then
    pgdir_va ! ptn := (get_page() << 11) bitor 1;

  // make a new page in the page table of the virtual address of page fault
  ptspec_va ! spec_p1e := (pgdir_va ! ptn) bitor 1;
  p1_va ! pn := (get_page() << 11) bitor 1;

  ireturn;
}

let halt_handler(intcode, address, info) be
{
  // halt instruction reached in user code
  //out(">: Halt handler\n");
  assembly
  {
	// moves stack to appropriate place for exit syscall
	load	sp, fp
	pop	fp
	add	sp, 21
	syscall	0
  }
}

let timer_handler(intcode, address, info) be
{
  let newproc;
  //out(".");
  pgdir_va ! pstate := prunnable;

  switch_process();
  pgdir_va ! pstate := prunning;

  assembly
  {
	load	r1, 5000
	setsr	r1, $timer
  }
  ireturn;
}

let intrfault_handler(intcode, address, info) be
{
  // error in an interrupt without a handler setup
  out(">: Fatal interrupt fault code %d, at address 0x%x\n", intcode, address);
  finish;
}

let set_int_vec() be
{
  // set addresses of handlers in interrupt vector page
  intvec_va ! 2 := [page_fault_handler];
  intvec_va ! 4 := [halt_handler];
  intvec_va ! 7 := [timer_handler];
  intvec_va ! 13:= [intrfault_handler]; 
}

let sys_trivial() be
{
  out(">: TRIVIALLLLLLLLLLLLLLLLLLLL\n");
  ireturn;
}

let sys_exit() be
{
  let origproc;

  // exit user code and allows system to receive commands again
  out(">: Process %d exited normally.\n", pgdir_va ! pid);

  proclist_va ! (pgdir_va ! pid) := 0;	// remove process from proc list
  next_pid -:= 1;			// process #pid is now gone

  ptspec_va ! 1 := proclist_va ! 0;	// set pgdir_va back to process 0, system shell
  origproc := proclist_va ! 0;
  pgdir_va ! pstate := prunning;

    // move original process's pcb to the register positions on stack to be restored in ireturn
  assembly
  {
	load	r0, 21
	load	r1, sp
	add	r1, 2		// accounts for parameters pushed to stack
	load	r2, 0x800+<pregisters>
	loadh	r2, 0xC000
	move	r2, r1

    // set pdbr back to original process
	load	r1, [<origproc>]
	sub	r1, 1		// removes bitor 1 for a valid entry
	setsr	r1, $pdbr
  }
  ireturn;
}

let set_callgates() be
{
  // setup syscall page, loading addresses of syscalls into call gate page
  assembly
  {
	load	r1, 2
	setsr	r1, $cglen	// set max number of syscalls
  }
  pgates_va ! 0 := [sys_exit];
  pgates_va ! 1 := [sys_trivial];
}

let get_command() be
{
  // get input from the user, and determine what command to run
  let string = vec string_size;
  let c, 
  index = 0;

  out(">: (%d)Input a command: ", pgdir_va ! pid);
  while index < string_size do
  {
    c := inch();
    if c = '\n' then break;
    byte index of string := c;
    index +:= 1;
  }
  byte index of string := 0;

  test str_comp(string, "help") then
    help()
  else test str_comp(string, "exit") then
    exit()
  else test str_comp(string, "echo") then
    echo(string)
  else test str_comp(string, "what time is it") then
    what_time_is_it()
  else test str_comp(string, "run") then
    run(string+1)
  //else test str_comp(string, "list") then
    //list_processes()
  else 
    out(">: Incorrect input; type 'help' for command list\n");

  resultis string;
}

let start() be
{
  manifest { heapsize = 1024 }
  let heap = vec heapsize;
  init(heap, heapsize);
  
  set_callgates();
  set_int_vec();

  pgdir_va ! pstate := prunning;	// set original process to running
  pgdir_va ! pid := 0;			// set original process to ID 0
  proclist_va ! 0 := ptspec_va ! 1;	// put physical pgdir in proc list
  
  assembly	// turn off interrupt processing flag
  {
	getsr	r1, $flags
	cbit	r1, $ip
	setsr	r1, $flags
  }

  while true do
  {
    get_command();
    out("\n");
  }
}
