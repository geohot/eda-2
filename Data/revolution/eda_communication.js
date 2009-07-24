// eda_communication.js -- July 23, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

// xx is EDAs quick communication lib
// returns a JSON object
// it is blocking, since the server is blocking
// fully REST compliant :-)
function xx(verb, loc, dat) {
  var req = new XMLHttpRequest();
  req.open(verb, loc, false);
  req.send(dat);
  while(req.readyState!=4) { }
  window.console.log(req.responseText);
  return eval('('+req.responseText+')');
}
