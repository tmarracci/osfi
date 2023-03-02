<?
// vim:foldmethod=marker
/*

  dynamic array and matrix/vector handling
  pick style

*/

/* constants {{{ */
define('SM',"\xff");
define('AM',"\xfe");
define('VM',"\xfd");
define('SVM',"\xfc");
define('SB',"\xfb");
define('TAB',"\x09");
define('NL',"\x0a");
define('CR',"\x0d");
define('DLE',"\x10");
define('NIL',"\x00");
/* }}} */

function _PRINT($txt) { echo str_replace(array(SM,AM,VM,SVM,SB),array('_','^',']','\\','['),$txt); }
function DIM($n) { return MATPARSE(str_repeat(AM,$n-1)); }	 // returns new array
function INMAT($arr) { return max(array_keys($arr)); }		 // returns size of array in Pick terms
function MATBUILD($arr, $delimiter = AM) { return substr(implode($delimiter,$arr),1); }	 // returns string of array
function MATPARSE($str, $delimiter = AM) { return explode($delimiter,$delimiter.$str); }		 // returns array of string
function MAT(&$arr,$val)  /* {{{  */
{ // void. array is replaced. val can be another array (and we copy) or a value (and we fill)
  $size = INMAT($arr);

  if (is_array($val)) {

    for ($i=1; $i<$size; $i++)
      $arr[$i] = $val[$i];

  } else {

    $arr = array_fill(1,$size,$val);
    $arr[0] = '';

  }

} 
/* }}} */
function MATPAD(&$arr,$attr) { /* {{{ */
  $size = INMAT($arr);
  if ($attr > $size)
    $arr = array_merge($arr,array_fill(0,$attr-$size,''));
}
/* }}} */

function __SIDC($rec, $del, &$n, &$st, &$sp, $inc = false) /* {{{ */
/*
  scan for nth del in rec
  starting at st
  do not exceed sp
  if inc is set, incremenet st before starting

  $st is location of delimiter
  $sp is location of next 
  if found, return true
  if not found, return false and update $n  with count remaining
*/
{
  $p = $st;

  if (!$n) return true;

  while ($n-- > 1) {
    if ($inc) $p++;	 // skip last delimiter
    $q = strpos($rec,$del,$p);	 // find next one
    if ($q === false || $q > $sp) {
      $st = $sp; // move to end
//echo "not found: n=$n st=$st sp=$sp\n";
      return false;
    }
    $p = $q; // point to where it was found
    $inc = true; // if continuing then we need to skip past this one
  }

// if we get here, then we found the nth delimiter
// we should find the next one
  if ($inc) $p++;
  $st = $p;
  $q = strpos($rec,$del,$p); // find next delimiter
  if ($q === false || $q > $sp) $q = $sp;
  $sp = $q; // position of end of this field

  return true;
}
/* }}} */

function _EXTRACT($rec, $a, $v = 0, $s = 0) /* {{{ */
{
  $st = 0;
  $sp = strlen($rec);

  if (__SIDC($rec,AM,$a,$st,$sp))
    if (__SIDC($rec,VM,$v,$st,$sp))
      if (__SIDC($rec,SVM,$s,$st,$sp))
        return(substr($rec,$st,$sp-$st));

  return '';
}
/* }}} */

function REPLACE($rec, $str, $a, $v = 0, $s = 0) /* {{{ */
{
  $st = 0;
  $sp = strlen($rec);

  if ($a < 0) {
    if ($s > 0) $str = str_repeat(SVM,$s).$str;
    if ($v > 0) $str = str_repeat(VM,$v).$str;
    if ($sp) $str = AM.$str;
    return $rec.$str;
  }
    
  if (__SIDC($rec,AM,$a,$st,$sp)) {

    if ($v < 0) {

      if ($sp) {
        if ($s > 0) $str = str_repeat(SVM,$s).$str;
        return substr($rec,0,$st).VM.$str.substr($rec,$sp);
      } else 
        $v = 0;

    }

    if (__SIDC($rec,VM,$v,$st,$sp)) {

      if ($s < 0)
        if ($sp)
          return substr($rec,0,$st).SVM.$str.substr($rec,$sp);
        else
          $s = 0;

      if (__SIDC($rec,SVM,$s,$st,$sp))
        return substr($rec,0,$st).$str.substr($rec,$sp);

    } else  {

      if ($s < 0) $s = ($sp-$st > 1 ? 1 : 0);
      else if ($s > 0) $s--;

    }

  } else {

    if ($v < 0) $v = 0; 
    else if ($v) $v--;
 
    if ($s < 0) $s = 0; 
    else if ($s) $s--;

  }

  if ($a)
    return $rec.str_repeat(AM,$a).str_repeat(VM,$v).str_repeat(SVM,$s).$str;

  if ($v || $s)
    return substr($rec,0,$st).str_repeat(VM,$v).str_repeat(SVM,$s).$str.substr($rec,$sp);

  return $str;
}
/* }}} */

function INSERT($rec, $str, $a, $v = 0, $s = 0) /* {{{ */
{
  if ($a < 0 || $v < 0 || $s < 0)
    return REPLACE($rec,$str,$a,$v,$s);

  $st = 0;
  $sp = strlen($rec);
  $term = $s ? SVM : ($v ? VM : AM);
  list($x,$y,$z) = array($a,$v,$s);

  if (__SIDC($rec,AM,$a,$st,$sp)) 
    if (__SIDC($rec,VM,$v,$st,$sp)) 
      if (__SIDC($rec,SVM,$s,$st,$sp)) {

        if ($sp == $st) $term='';
        return substr($rec,0,$st).$str.$term.substr($rec,$st);

      }

// if we get here, perform replace
  if ($v) $v--;
  if ($s) $s--;

  if ($a)
    return $rec.str_repeat(AM,$a).str_repeat(VM,$v).str_repeat(SVM,$s).$str;

  if ($v || $s)
    return substr($rec,0,$st).str_repeat(VM,$v).str_repeat(SVM,$s).$str.$term.substr($rec,$st);

  return $str;
}
/* }}} */

function DELETE($rec, $a, $v = 0, $s = 0) /* {{{ */
{
  $st = 0;
  $sp = strlen($rec);

  $term = $s ? SVM : ($v ? VM : ($a ? AM : SM));

  if (__SIDC($rec,AM,$a,$st,$sp)) 
    if (__SIDC($rec,VM,$v,$st,$sp)) 
      if (__SIDC($rec,SVM,$s,$st,$sp)) {
        $ch  = isset($rec[$sp]) ? $rec[$sp] : substr($rec,$sp,1);
        if ($ch == $term) $sp++;
        return substr($rec,0,$st).substr($rec,$sp);
      }

  return $rec;
}
/* }}} */

function DCOUNT($rec, $ch) /* {{{ */
{
  return $rec == '' ? 0 : _COUNT($rec,$ch)+1;
}
/* }}} */

function _COUNT($rec, $ch) /* {{{ */
{
  return substr_count(strtoupper($rec),strtoupper($ch));
}
/* }}} */

function INDEX($rec, $str, $n) /* {{{  */
// return index into array or false if not found
{
  $p = 0;
  for ($i = 0; $i < $n; $i++) {
    if ($i) $p += strlen($str);
    $q = stripos($rec,$str,$p);
    if ($q === false) return false;
    $p = $q;
  }

  return $p; // position of character
}
/* }}} */

function FIELD($str, $sep, $n) /* {{{  */
{
  $p = INDEX($str,$sep[0],$n-1);

  if ($p !== false) {
    if ($p) $p++; // skip to next char
    $q = stripos($str,$sep,$p);
    if ($q === false)
      return substr($str, $p);
    else
      return substr($str, $p, $q-$p);
  }

  return '';
}
/* }}} */

function LOCATE($str, $rec, &$pos, $a = 0, $v = 0, $st = 1, $sort = null) /* {{{ */
{
  $del = AM;
  if ($a) $del = VM;
  if ($v) $del = SVM;	// choose the delimiter to search for

  $rec = _EXTRACT($rec,$a,$v);	// get the data to search
  $m = DCOUNT($rec,$del);	// and how many elements

  $al = $ar = $dl = $dr = false;	// setup sorting to be false

  if ($sort !== null) {
    $al = strcasecmp($sort,"AL") == 0;	 // ascending left justififed
    $ar = strcasecmp($sort,"AR") == 0;   // ascending right justified
    $dl = strcasecmp($sort,"DL") == 0;   // descending left justified
    $dr = strcasecmp($sort,"DR") == 0;   // descending right justified
  }

  if ($ar || $dr) $slen = strlen($str);	  // if right justified, then shorter is less so we need the length of the string

  for ($pos = $st; $pos <= $m; $pos++) {

    $tmp = FIELD($rec,$del,$pos);
    if ($ar || $dr) {
      $l = strlen($tmp);
      if ($l < $slen) $n = -1;
      else if ($l > $slen) $n = 1;
      else $n = strcasecmp($tmp,$str);
    } else
      $n = strcasecmp($tmp,$str);

    if ($n == 0) return true;	 // found it!
    else if ((($al || $ar) && $n > 0) || (($dl || $dr) && $n < 0))	 // if sorting and out of range, we did not find it
      return false;		// advise not found but $pos has where to insert if we want to
    
  }

  return false; // not found and pos = m+1
}
/* }}} */

if (!function_exists('hex2bin')) { /* {{{ */
function hex2bin($txt) {

  return pack('H*',$txt);

}
} /* }}} */

function _DATE() /* {{{ */
{
  $now = new DateTime(); // so we can adjust by current server timezone
  return intval((time() + $now->getOffset()) / 86400) + 732;
}
/* }}} */

function _TIME()  /* {{{ */
{
  $now = new DateTime();
  return (time() + $now->getOffset()) % 86400;
}
/* }}} */

function FORMAT($str, $just, $fill = null, $length = null) /* {{{ */
{

  if ($fill === null) { // parse just, fill, and length from just

    $mask = strtoupper($just);
    $just = $mask[0];
    $fill = $mask[1]; // always
    $length = intval(substr($mask,2));

  }

  if ($length) {

    if ($just == 'L') {
      if ($fill == '#' || $fill == ' ') $str = sprintf("%-{$length}s",$str);
      else if ($fill == '%' || $fill == '0') $str = sprintf("%-0{$length}s",$str);
      if (strlen($str) > $length) $str = substr($str,0,$length); // truncate if too long
    } else {
      if ($fill == '#' || $fill == ' ') $str = sprintf("%{$length}s",$str);
      if ($fill == '%' || $fill == '0') $str = sprintf("%0{$length}s",$str);
      if (strlen($str) > $length) $str = substr($str,strlen($str) - $length); // chop if too long
    }

  }
         
  return $str;
}
/* }}} */

function _OCONV($str, $mask) /* {{{ */
{
  $mask = strtoupper($mask);
  switch($mask[0]) {
    case 'D': /* {{{ */
// D, DD, DW, DWA, DM, DMA, DY[n], D[n][sep]
      if ($str == '') return $str;
      $now = new DateTime(); // so we can adjust timezone (used by date function)
      $dt = ($str - 732) * 86400 - $now->getOffset();
      switch($mask) {
        case 'D': return date('j M Y',$dt);
        case 'DD': return date('j',$dt);
        case 'DW': return date('N',$dt);
        case 'DWA': return date('l',$dt);
        case 'DM': return date('n',$dt);
        case 'DMA': return date('F',$dt);
        case 'DQ': return intval(((date('n',$dt) - 1) / 3)) + 1;
        case 'DY': return date('Y',$dt);
        case 'DJ': return date('z',$dt) + 1;
        default: // choose format based on number of digits requested in the year (2 or 4)
          if (preg_match('/^D[24]Y$/',$mask)) 
            return date($mask[1] == '2' ? 'y' : 'Y',$dt);
          else if (preg_match('/^D[24]$/',$mask))
            return date($mask[1] == '2' ? 'j M y' : 'j M Y',$dt);
          else if (preg_match('/^D[24].{1}/',$mask))  // allows any separator character
            return date($mask[1] == '2' ? "m{$mask[2]}d{$mask[2]}y" : "m{$mask[2]}d{$mask[2]}Y",$dt);

        break;

      }
      break;
/* }}} */

    case 'G': /* {{{ */
// G[n]sep[m]
      $n = 0; $sep = ''; $m = 0; 
      $state = 0; // state machine to parse mask
      $mlen = strlen($mask);
      for ($i=1; $i<$mlen; $i++)
        switch($state) {
          case 0: // looking for skip
            if (is_numeric($mask[$i])) list($state,$n) = array(1,$mask[$i]);
            else list($state,$sep) = array(2,$mask[$i]);
            break;

          case 1: // accum skip 
            if (is_numeric($mask[$i])) $n = $n*10 + $mask[$i];
            else list($state,$sep) = array(2,$mask[$i]);
            break;

          case 2: // get length
            if (is_numeric($mask[$i])) $m = $m*10 + $mask[$i];
            else $state = 99; // done
            break;
        }

      if ($sep != '') 
        return implode($sep,array_slice(explode($sep,$str),$n,$m));

      return $str;
      break;
/* }}} */

    case 'M': /* {{{ */
// MCT, MCL, MCU, MCA, MCN, MCAN, MC/A, MC/N, MC/AN, MCDX, MCXD
      switch($mask[1]) {
        case 'C': /* {{{ */
          switch($mask[2]) {
            case 'D': return $mask[3]=='X' ? dechex($str) : ''; // MCDX
            case 'X': return $mask[3]=='D' ? hexdec($str) : ''; // MCXD
            case 'T': return ucwords(strtolower($str));
            case 'U': return strtoupper($str);
            case 'L': return strtolower($str);
            case 'A': 
              if ($mask[3] == 'N')
                return preg_replace('/[^A-Za-z0-9]/','',$str);
              else
                return preg_replace('/[^A-Za-z]/','',$str);
              
            case 'N': return preg_replace('/[^0-9]/','',$str);
            case '/':
              switch($mask[3]) {
                case 'A':
                  if ($mask[4] == 'N')
                    return preg_replace('/[A-Za-z0-9]/','',$str);
                  else
                    return preg_replace('/[A-Za-z]/','',$str);

                case 'N': return preg_replace('/[0-9]/','',$str);
                  break;

              }
              break;

          }
          break;
/* }}} */

        case 'R': case 'L': case 'D': /* {{{ */
// M[RLD][precision][scale][fill][length] (only #% fill are supported)
          $just = $mask[1];	 // parse mask into specifics
          $precision = 0; $scale = 0; $fill = ' '; $length = 0; $comma = true; $supp = false; $dollar = false;
          $n = 2; $slen = strlen($mask);
          if (is_numeric($mask[$n])) list($precision,$n) = array($mask[$n],$n+1);
          $scale = $precision; // by default, scale is precision
          if (is_numeric($mask[$n])) list($scale,$n) = array($mask[$n],$n+1);
          if ($mask[$n] == 'Z') list($supp,$n) = array(true,$n+1); // suppress zero
          if ($mask[$n] == ',') list($comma,$n) = array(true,$n+1); // sep thousands with comma
          if ($mask[$n] == '$') list($dollar,$n) = array(true,$n+1); // prepend with $
          if (preg_match('/[#%]/',$mask[$n])) list($fill,$n) = array($mask[$n],$n+1); // specify fill character % or # only
          for ($i=$n; $i<$slen; $i++)
            if (is_numeric($mask[$i])) $length = $length * 10 + $mask[$i]; // get the length of the field
            else break;

// apply mask to str
          if ($scale) $str /= pow(10,$scale);
          if ($supp && $str == 0) return '';
          $res = ($dollar ? '$' : '') . number_format($str,$precision,'.',$comma ? ',' : '');
          return FORMAT($res, $just, $fill, $length);

          break;
/* }}} */

        case 'T': /* {{{ */
// MTS, MTH
          if ($str == '') return $str;
          $now = new DateTime(); // so we can adjust timezone used by date function
          switch($mask[2]) {
            case 'S': 
              if ($str >= 86400)
                return sprintf('%d:%02d:%02d',intval($str/3600),intval(($str - intval($str/3600)*3600)/60),$str % 60);
              else
                return date('H:i:s',$str - $now->getOffset());

            case 'H': 
              if ($str >= 86400)
                return sprintf('%d:%02dPM',intval($str/3600),intval(($str - intval($str/3600)*3600)/60));
              else
                return date('h:iA',$str - $now->getOffset());

            break;
          }
          break;
/* }}} */

        case 'X':  return bin2hex($str); // MX

      }
      break;
/* }}} */

  }

  $rp = get_pick();
  if (isset($rp) && is_object($rp))
    return $rp->oconv($str, $mask);

  return '';
}
/* }}} */

function _ICONV($str, $mask) /* {{{ */
{

  $mask = strtoupper($mask);
  if ($mask[0] == 'D') {	// convert to pick julian date

    if ($str == '') return $str;    
    $now = new DateTime(); // so we can adjust by current server timezone
    $tm = strtotime($str);
    return intval(($tm + $now->getOffset()) / 86400) + 732;
    
  } else if (preg_match('/^MT(H|S)$/',$mask)) {	// convert to time (what about more than 24 hours?)

    if ($str == '') return $str;
    $now = new DateTime();
    $tm = strtotime($str);
    return ($tm + $now->getOffset()) % 86400;

  } else if (preg_match('/^M(D|R|L)[0-9]{1}$/',$mask)) {

    if ($str == '') return $str;
    return round($str * pow(10,$mask[2]),0);

  } else if ($mask == 'MX') {

    return hex2bin($str);

  }

  $rp = get_pick();
  if (isset($rp) && is_object($rp))
    return $rp->iconv($str, $mask);

  return '';
}
/* }}} */

function get_pick() { /* {{{ */
/*
 function to return pointer to a picknfs object
 developer decides how to do this

  for example, you could keep a global variable
  that was previously declared (or not)
  and connect it now

  requires picknfs.class to be included 
*/
  global $rp;

  if (!isset($rp)) $rp = new PickNFS('localhost',1598);
  if (!$rp->connected) 
    $rp->connect() or die("could not connect to PickNFS");

  return $rp;
}
/* }}} */
