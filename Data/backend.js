function send_request() {
  var ret = xx(document.getElementById("command").value,
               document.getElementById("address").value,
               document.getElementById("data").value);
  document.getElementById("response").value = ret.html;
}

// xx is EDAs quick communication lib
// response options are html, xml, and status
// it is blocking, since the server is blocking
// fully REST compliant :-)
function xx(verb, loc, dat) {
  var req = new XMLHttpRequest();
  req.open(verb, loc, false);
  req.send(dat);
  while(req.readyState!=4) { }
  var ret = {
    html: req.responseText,
    xml: req.responseXML,
    status: req.status};
  return ret;
}