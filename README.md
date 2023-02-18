# osfi
D3 OSFI interface for PHP

Access D3 data using the OSFI interface provided with D3 version 7+

To setup your server, log onto DM and use the NETWORK-SETUP command to create a server.  The easiest and most secure is to make the server listen on localhost and the default port 1598.

Now that your server is listening for OSFI connections, you can create your PHP scripts to access pick/D3 data.

For example:

<?php
include_once('pick.inc'); // provides some basic pick like string and array handling
include_once('osfi.class'); // the base class to use the OSFI communication protocol
include_once('picknfs.class'); // primary class to perform pick/D3 open, read, write, delete, select, readnext, root, key, execute, and call functions

$rp = new PickNFS('localhost',1598); // the host and port specified in NETWORK-SETUP
if ($rp->connect($user, $password, $account, $account_password)) {
  $rp->execute('WHO'); // show the standard who command
} else
  die('access denied');

?>

