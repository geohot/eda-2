function send_request() {
  
  var ret = xx(document.getElementById("command").value,
               document.getElementById("address").value,
               document.getElementById("data").value);
  document.getElementById("response").value = "";
  if(ret.html.substr(0, 38) == '<?xml version="1.0" encoding="UTF-8"?>') {
    var xml = ret.html.substr(38);
    //document.getElementById("response").value = xml;
    //alert(ret.xml.documentElement);
    if(ret.xml == null)
      document.getElementById("response").value = xml;
    else
      document.getElementById("response").value = view_xml(ret.xml.documentElement, 0);

  } else
    document.getElementById("response").value = ret.html;
}

function send_reg_request() {
  /*var registers = ["R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","SP","LR","PC","CPSR"];
  var out = "";
  for(r in registers) {
    var ret = xx("EVAL", "[`"+registers[r]+"`]", "");
    out += registers[r]+": "+ret.html+"<br/>";
  }
  document.getElementById("registers").innerHTML=out;*/
  var out = "";
  var registers = xx("READ","/State","").xml.documentElement.getElementsByTagName("registers")[0];
  for(r in registers.childNodes) {
    if(registers.childNodes[r].childNodes != null)
      out += registers.childNodes[r].nodeName+": "+registers.childNodes[r].childNodes[0].nodeValue+"<br/>";
  }
  document.getElementById("registers").innerHTML=out;
}

function send_step_request() {
  var ret2 = xx("READ", "/Address/[`PC`]-4/Instruction/Parsed", "");

  var ret = xx("STEP", "/Address/[`PC`]-4", "");
  document.getElementById("response").value = view_xml(ret.xml.documentElement, 0);

  document.getElementById("rendered").innerHTML += '<span class="address">'+ret.xml.getElementsByTagName("owner")[0].childNodes[0].nodeValue+": "+'</span>';
  document.getElementById("rendered").innerHTML += ParsedInstructionToHTML(ret2.xml.documentElement);

  send_reg_request();
}

function view_xml(element, depth) {
  if(element.nodeName == null) return "";
  /*if(element.nodeName == "ParsedInstruction") {
    document.getElementById("rendered").innerHTML += ParsedInstructionToHTML(element);
  }*/
  var i;
  var ret = "";
  for(i = 0; i < depth; i++) ret += " ";

  ret += "<" + element.nodeName + ">";
  if(element.childNodes.length > 0 && element.childNodes[0].nodeName != "#text") {
    ret += "\n";
    for(i in element.childNodes) {
      ret += view_xml(element.childNodes[i], depth+2);
    }
    for(i = 0; i < depth; i++) ret += " ";
  } else if(element.childNodes.length > 0) {
    ret += element.childNodes[0].nodeValue;
  }
  
  ret += "</" + element.nodeName + ">\n";
  return ret;
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

function ParsedInstructionToHTML(pi_element) {
  //console.debug(pi_element);
  var ret = '<span class="opblock">';
  var first_formatting = true;
  for(i in pi_element.childNodes) {
    if(pi_element.childNodes[i].nodeName == "atom") {
      if(pi_element.childNodes[i].childNodes[1].childNodes[0] != null) {
        ret += '<span class="'+
          pi_element.childNodes[i].childNodes[0].childNodes[0].nodeValue+
          '">'+
          pi_element.childNodes[i].childNodes[1].childNodes[0].nodeValue+
          '</span>';
      }
    } else if(pi_element.childNodes[i].nodeName == "formatting") {
      if(first_formatting == true) {
        ret+='</span>';
        first_formatting = false;
      }
      ret += pi_element.childNodes[i].childNodes[0].nodeValue;
    }
  }
  if(first_formatting == true) {
    ret+='</span>';
    first_formatting = false;
  }
  return ret+"<br/>";
}