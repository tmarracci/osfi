# osfi
D3 OSFI interface for PHP

Access D3 data using the OSFI interface provided with D3 version 7+.  The system was developed for and tested on D3/Linux 10.2 and follows functionality as spelled out in the documentation found at https://www3.rocketsoftware.com/rocketd3/support/documentation/d3nt/102/refman/index.htm

To setup your server, log onto DM and use the NETWORK-SETUP command to create a server.  The easiest and most secure is to make the server listen on localhost and the default port 1598.
Before you can start, you need to provide a file for unsupported OSFI functionality.  As an example, create this file in DM called PICKNFS and size it bearing in mind the workload you expect your server to accommodate.  Add a trigger as follows to execute the picknfs.trigger program (code provided herein) on updates:

<pre>
U DICT PICKNFS PICKNFS
008 CALLX DM,BP, PICKNFS.TRIGGER
</pre>

In DM,BP, (or any file path of your choice and change 008 above accordingly) add the included PICKNFS.TRIGGER basic program and compile it.  Note if you compile this with flash, then any subroutines you call must also be flashed.

Now that your server is listening for OSFI connections, you can create your PHP scripts to access pick/D3 data.

For example:

<code>
&lt;?php
include_once('pick.inc'); // provides some basic pick like string and array handling
include_once('osfi.class'); // the base class to use the OSFI communication protocol
include_once('picknfs.class'); // primary class to perform pick/D3 open, read, write, delete, select, readnext, root, key, execute, and call functions

$rp = new PickNFS('localhost',1598); // the host and port specified in NETWORK-SETUP
if ($rp->connect($user, $password, $account, $account_password)) {
  $rp->execute('WHO'); // show the standard who command
} else
  die('access denied');

?&gt;

</code>

Files and functions.

The pick.inc file contains support for pick like functions in the PHP environment space.

<code>
__PRINT($txt) will output the specified text and replace Pick/D3 delimiters 
function DIM($n) create a blank pick array of dimension $n.  Pick arrays start with 1
function INMAT($arr) the size of the array $arr in pick dimensions
function MATBUILD($arr) construct a FE delimited string from the pick array $arr
function MATPARSE($str) output a pick array (starting at 1) of the string delimited by FE
function MAT(&$arr,$val) fill the pick array $arr with $val
function MATPAD(&$arr,$attr) insure the array $arr has at least $attr fields
_function _EXTRACT($rec, $a, $v = 0, $s = 0) equivalent to rec<a> (or rec<a,v> or rec<a,v,s>)
_function REPLACE($rec, $str, $a, $v = 0, $s = 0) equivalent to rec<a,v,s> = str
function INSERT($rec, $str, $a, $v = 0, $s = 0) equivalent to ins str before rec<a,v,s>
function DELETE($rec, $a, $v = 0, $s = 0) equivalent to del rec<a,v,s>
function DCOUNT($rec, $ch) return number of fields in rec delimited by ch
function _COUNT($rec, $ch) return number of ch in rec
function INDEX($rec, $str, $n) return nth position of str in rec. note a PHP string starts at 0 so return value 0 does not mean not found
  instead use === false to determine if str is not found
  for example, INDEX('abc','z',1) === false
  
function FIELD($str, $sep, $n) return the nth field of str delimited by sep
function LOCATE($str, $rec, &$pos, $a = 0, $v = 0, $st = 1, $sort = null) find str in the dynamic array rec
function _DATE() returns the current date in Pick/D3 julian date (ie 0 = Dec 31, 1967)
function _TIME() returns number of seconds since midnight
function FORMAT($str, $just, $fill = null, $length = null) return str, left or right justfied, on a field of length fills
function _OCONV($str, $mask) perform output conversion on str. supported masks include:
  D (or various kinds), MTH, MTS, MCx (of various forms), MR/ML with scaling, precision, and formatting, and MX
  any other mask is sent to OSFI to be handled by the connected Pick/D3 virtual machine
function _ICONV($str, $mask) perform input conversion on str. supported masks include:
  D, MT, MR/ML, and MX. all other masks are sent to virtual machine for processing
  
  </code>
  
osfi.class is the parent class used by picknfs to process the OSFI protocol.  However, 4 methods are available to perform pick array read/write. That is to say, the array must be a pick formatted array starting at index 1 (see array functions above to build pick style arrays)
  
<code>
function matread(&$arr, $fd, $id) read a record and store it as an array
function matreadu(&$arr, $fd, $id, &$locked) read a record and lock it (if not already locked) and store in array. if locked by another user, locked will return true
function matwrite($arr, $fd, $id) write the record in array arr and if locked, unlock it
function matwriteu($arr, $fd, $id) write the record in array and if locked, remain locked
  
function get_pick() use this function (this is a global function, not a method) to access a global osfi object. modify as needed for your particular situation.  The provided version stores the user settings in a $_SERVER[CREDENTIALS] variable elsewhere and this function will make a connection via a global osfi object.  Your mileage may vary.
  
</code>

picknfs.class is the heart of the matter.  This contains the data access methods you will use to access data in your program
  
<code>
  function __construct($host, $port) When specifying a new PickNFS object, provide the host and port to connect to
  function connect($user, $pass, $acct, $apsw) connect to the database. return true if ok, false if it fails
    or connect() to make a connection without logging in.  Provides access but at reduced security due to ignoring credentials
  function open($dict, $fname, &$fd)  open a file (specify the full path account,file, for best results) and set a handle to it. returns true or false
  function close($fd)
  function clearfile($fd)
  function read(&$rec, $fd, $id) read a record and store the dynamic array in rec. returns true/false depending on whether item is found
  function readu(&$rec, $fd, $id, &$locked) read a record and lock it. if already locked, returns false and sets locked to true
  function readv(&$rec, $fd, $id, $attr) read a single attribute from a file (note: this is not efficient since OSFI does not provide support for readv)
  function readvu(&$rec, $fd, $id, $attr, &$locked) read a single attribute and lock. if already locked, return false and set locked to true
  function write($rec, $fd, $id) write a dynamic array record to a file. If the record was locked, it will be unlocked
  function writeu($rec, $fd, $id) write a record and leave it locked
  function writev($rec, $fd, $id, $attr) write a single attribute and release any locks
  function writevu($rec, $fd, $id, $attr) write a single attribute and leave it locked
  function delete_item($fd, $id) delete a record from a file
  function release($fd, $id) release the lock set with readu
  
  function execute($tcl, &$cap = null, &$ret = null, $passlist = 0, $passitems = null, &$rtnlist = null, &$count = null)
     execute a TCL statement in the virtual machine.  all arguments are optional except the tcl statement to execute
     $cap - if provided, store the output in cap. 
     $ret - if provided, store the error number if there is one
            if cap and ret are both null, output will be printed 
     $passlist - to pass a list of items from a select list, set to the list variable+1.  
     $passitems - dynamic array (FE delimited) of item id's to pass to the tcl statement
     $rtnlist - a select variable in which to store the resulting list, if one is created
     $count - number of items selected by the tcl statement
 
  function system($val) the pick/d3 system function
  function iconv($str, $conv) handle iconv masks that _iconv() does not support
  function oconv($str, $conv) handle oconv masks that _oconv() does not support
  function call($sub,$args = null) 
    call a subroutine. With this handler, the subroutine need not be compiled with flash.  For most consistent results, provide a full path
    to the subroutine (eg. dm,bp, subname)
    if the subroutine takes arguments, pass them in an array.  Arguments to be passed by reference must be preceeded with &
  
    for example, assume the subroutine called ADD is in the DM,BP, file:
  
  001 SUBROUTINE (X,Y,S)
  002 S = X+Y
  003 RETURN
  004 END
  
  use $rp->call('dm,bp, add',array(1,2,&$sum)) to call it and return the result in $sum
  
  function lock($lock, $val)
    $rp->lock(1,10) - lock execution lock #10. return true if we can, false if already locked
    $rp->lock(0,10) - unlock execution lock #10
  
  
  function select($fd, &$sel) select the file referenced by $fd and return a select list variable $sel
  function readnext(&$id, &$sel, &$val = null)  readnext id from the select variable $sel. If $val is set, then it is assumed select is from a previous execute and return a multivalued set.  $val will have the value mark counter of the next item
  function clearselect($sel) if a select variable does not readnext to completion, clear it's use to avoid memory or workspace issues
  
  function root($file, $corr, &$sel) access the index on the file name $file and correlative $corr (must be an A correlative in order to work) and return an index handle sel.  returns true or false
  function key($mode, $root, &$key, &$id, &$val = null)  access the index referenced by $root
  function closeroot($sel) close an index root when done with it
  
  
</code>

See LICENSE file.  No warranty, express or implied, is provided.  Use at your own risk.
