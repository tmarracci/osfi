# osfi
D3 OSFI interface for PHP

Access D3 data using the OSFI interface provided with D3 version 7+

To setup your server, log onto DM and use the NETWORK-SETUP command to create a server.  The easiest and most secure is to make the server listen on localhost and the default port 1598.

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
